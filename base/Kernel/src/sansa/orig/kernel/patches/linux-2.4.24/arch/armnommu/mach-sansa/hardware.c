/*
 * hardware.c - special hardware routines for Sansa
 *
 * Copyright (c) 2003-2008 Sebastian Duell, Bernard Leach, Rockbox
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/system.h>
#include "pp5020.h"
#include "as3514.h"

static struct sysinfo_t sansa_sys_info;
static int sansa_sys_info_set;

void sansa_set_sys_info(void);
int sansa_i2c_readbyte(unsigned int dev_addr, int addr);


unsigned sansa_get_hw_version(void)
{
   return 0x60000;
	if (!sansa_sys_info_set) {
		sansa_set_sys_info();
	}

	if (sansa_sys_info_set > 0) {
		return system_rev;
	}

	return 0x0;
}

struct sysinfo_t *sansa_get_sysinfo(void)
{
	if (!sansa_sys_info_set) {
		sansa_set_sys_info();
	}

	if (sansa_sys_info_set > 0) {
		return &sansa_sys_info;
	}

	return 0x0;
}

int sansa_is_pp5022(void) {
	return (inl(0x70000000) << 8) >> 24 == '2';
}

void sansa_set_sys_info(void)
{
#if 0
	if (!sansa_sys_info_set) {
		unsigned sysinfo_tag = SYSINFO_TAG;
		struct sysinfo_t ** sysinfo_ptr = SYSINFO_PTR;

		if (sansa_is_pp5022()) {
			sysinfo_tag = SYSINFO_TAG_PP5022;
			sysinfo_ptr = SYSINFO_PTR_PP5022;
		}

		if (*(unsigned *)sysinfo_tag == *(unsigned *)"IsyS" 
				&& (*(struct sysinfo_t **)sysinfo_ptr)->IsyS ==  *(unsigned *)"IsyS" ) {
			memcpy(&sansa_sys_info, *sysinfo_ptr, sizeof(struct sysinfo_t));
			sansa_sys_info_set = 1;
			/* magic length based on newer sansa nano sysinfo */
			if (sansa_sys_info.len == 0xf8) {
				system_rev = sansa_sys_info.sdram_zero2;
			} else {
				system_rev = sansa_sys_info.boardHwSwInterfaceRev;
			}
		}
		else {
			sansa_sys_info_set = -1;
		}
	}
#endif
}

void sansa_hard_reset(void)
{
   sansa_i2c_lock();
   CACHE_CTL &= ~CACHE_CTL_VECT_REMAP;

   sansa_i2c_send(AS3514_I2C_ADDR, DCDC15, 0x0); /* backlight off */

   /* Magic used by the c200 OF: 0x23066000
    Magic used by the c200 BL: 0x23066b7b
    In both cases, the OF executes these 2 commands from iram. */
   STRAP_OPT_A = 0x23066b7b;
   DEV_RS = DEV_SYSTEM;
}

void sansa_power_off(void)
{
   char byte;
   
   sansa_i2c_lock();
   /* Mask them on both cores */
   CPU_INT_CLR = -1;
   COP_INT_CLR = -1;
   
   /* Send shutdown command to PMU */
   byte = i2c_readbyte(AS3514_I2C_ADDR, SYSTEM);
   byte &= ~0x1;   
   sansa_i2c_send(AS3514_I2C_ADDR, SYSTEM, byte);
   
   /* Halt everything and wait for device to power off */
   while (1)
   {
      CPU_CTL = PROC_SLEEP;
      COP_CTL = PROC_SLEEP;
   }
}

void
sansa_init_cache(void)
{
	unsigned i;
	/* cache init mode? */
	outl(0x4, 0x6000C000);

	/* PP5002 has 8KB cache */
	for (i = 0xf0004000; i < 0xf0006000; i += 16) {
		outl(0x0, i);
	}

	outl(0x0, 0xf000f040);
	outl(0x3fc0, 0xf000f044);

	/* enable cache */
	outl(0x1, 0x6000C000);

	for (i = 0x10000000; i < 0x10002000; i += 16) {
		inb(i);
	}
}

extern void (*pm_power_off)(void);

void
sansa_set_cpu_speed(void)
{
   DEV_RS2 |= 0x20;
   DEV_RS = 0x3bfffef8;
   DEV_RS2 = -1;
   DEV_RS = 0;
   DEV_RS2 = 0;
   outl(inl(0x6000a000) | 0x80000000, 0x6000a000); /* Init DMA controller? */

   
   DEV_INIT2 |= 1 << 30; /* enable PLL power */

   CLOCK_SOURCE = 0x10007772;  /* source #1: 24MHz, #2, #3, #4: PLL */
   DEV_TIMING1  = 0x00000303;
   PLL_CONTROL  = 0x8a121403;  /* (20/3 * 24MHz) / 2 */
   udelay(250);
   while (!(PLL_STATUS & 0x80000000)); /* wait for relock */

   DEV_TIMING1 = 0x00000808;
   CLOCK_SOURCE = (CLOCK_SOURCE & ~0xf0000000) | 0x20000000;  /* select source #2 */

   pm_power_off = sansa_power_off;
}


#define SANSA_I2C_CTRL	(sansa_i2c_base+0x00)
#define SANSA_I2C_ADDR	(sansa_i2c_base+0x04)
#define SANSA_I2C_DATA0	(sansa_i2c_base+0x0c)
#define SANSA_I2C_DATA1	(sansa_i2c_base+0x10)
#define SANSA_I2C_DATA2	(sansa_i2c_base+0x14)
#define SANSA_I2C_DATA3	(sansa_i2c_base+0x18)
#define SANSA_I2C_STATUS	(sansa_i2c_base+0x1c)

/* SANSA_I2C_CTRL bit definitions */
#define SANSA_I2C_SEND	0x80

/* SANSA_I2C_STATUS bit definitions */
#define SANSA_I2C_BUSY	(1<<6)

#define POLL_TIMEOUT (HZ)

static unsigned sansa_i2c_base;
struct semaphore i2c_lock_mutex;

static int
sansa_i2c_wait_not_busy(void)
{
	unsigned long timeout;

	timeout = jiffies + POLL_TIMEOUT;
	while (time_before(jiffies, timeout)) {
		if (!(inb(SANSA_I2C_STATUS) & SANSA_I2C_BUSY)) {
			return 0;
		}
		yield();
	}
	return -ETIMEDOUT;
}

void
sansa_i2c_init(void)
{
	static int i2c_init = 0;

	if (i2c_init == 0) 
   {
		i2c_init = 1;
      init_MUTEX(&i2c_lock_mutex);

    DEV_EN |= DEV_I2C;  /* Enable I2C */
    DEV_RS |= DEV_I2C;  /* Start I2C Reset */
    DEV_RS &=~DEV_I2C;  /* End I2C Reset */

    outl(0x0, 0x600060a4);
    outl(0x23, 0x600060a4);

    sansa_i2c_base = 0x7000c000;

    i2c_readbyte(0x8, 0);
	}
}

static int
sansa_i2c_read_byte(unsigned int addr, unsigned int *data)
{
	if (sansa_i2c_wait_not_busy() < 0) {
		return -ETIMEDOUT;
	}

	// clear top 15 bits, left shift 1, or in 0x1 for a read
	outb(((addr << 17) >> 16) | 0x1, SANSA_I2C_ADDR);

	outb(inb(SANSA_I2C_CTRL) | 0x20, SANSA_I2C_CTRL);

	outb(inb(SANSA_I2C_CTRL) | SANSA_I2C_SEND, SANSA_I2C_CTRL);

	if (sansa_i2c_wait_not_busy() < 0) {
		return -ETIMEDOUT;
	}

	if (data) {
		*data = inb(SANSA_I2C_DATA0);
	}

	return 0;
}

int sansa_i2c_readbytes(unsigned int dev_addr, int addr, int len, unsigned char *data) {
   unsigned int temp;
   int i;
   sansa_i2c_send_byte(dev_addr, addr);
   for (i = 0; i < len; i++) {
      sansa_i2c_read_byte(dev_addr, &temp);
//      i2c_readbyte(dev_addr, &temp);
      data[i] = temp;
   }
   return i;
}

int
sansa_i2c_send_bytes(unsigned int addr, unsigned int len, unsigned char *data)
{
	int data_addr;
	int i;

	if (len < 1 || len > 4) {
		return -EINVAL;
	}

	if (sansa_i2c_wait_not_busy() < 0) {
		return -ETIMEDOUT;
	}

	// clear top 15 bits, left shift 1
	outb((addr << 17) >> 16, SANSA_I2C_ADDR);

	outb(inb(SANSA_I2C_CTRL) & ~0x20, SANSA_I2C_CTRL);

	data_addr = SANSA_I2C_DATA0;
	for ( i = 0; i < len; i++ ) {
		outb(*data++, data_addr);

		data_addr += 4;
	}

	outb((inb(SANSA_I2C_CTRL) & ~0x26) | ((len-1) << 1), SANSA_I2C_CTRL);

	outb(inb(SANSA_I2C_CTRL) | SANSA_I2C_SEND, SANSA_I2C_CTRL);

	return 0x0;
}

int
sansa_i2c_send(unsigned int addr, int data0, int data1)
{
	unsigned char data[2];
											
	data[0] = data0;
	data[1] = data1;

	return sansa_i2c_send_bytes(addr, 2, data);
}

int
sansa_i2c_send_byte(unsigned int addr, int data0)
{
	unsigned char data[1];

	data[0] = data0;

	return sansa_i2c_send_bytes(addr, 1, data);
}

int
i2c_readbyte(unsigned int dev_addr, int addr)
{
	int data;

	sansa_i2c_send_byte(dev_addr, addr);
	sansa_i2c_read_byte(dev_addr, &data);

   return data;
}

void sansa_i2c_lock()
{
   down(&i2c_lock_mutex);
}

void sansa_i2c_unlock()
{
   up(&i2c_lock_mutex);
}


void
sansa_serial_init(void)
{
}

/* put our ptr in on chip ram to avoid caching problems */
static sansa_dma_handler_t * sansa_dma_handler = DMA_HANDLER;

static sansa_cop_handler_t * sansa_cop_handler = COP_HANDLER;


void sansa_handle_cop(void)
{
	if (*sansa_cop_handler != 0) {
		(*sansa_cop_handler)();
	}
}

void sansa_set_handle_cop(sansa_cop_handler_t new_handler)
{
	*sansa_cop_handler = new_handler;
}


void sansa_set_process_dma(sansa_dma_handler_t new_handler)
{
	*sansa_dma_handler = new_handler;
}

void sansa_handle_dma(void)
{
	if (*sansa_dma_handler != 0) {
		(*sansa_dma_handler)();
	}
}

EXPORT_SYMBOL(sansa_get_hw_version);
EXPORT_SYMBOL(sansa_get_sysinfo);
EXPORT_SYMBOL(sansa_is_pp5022);
EXPORT_SYMBOL(sansa_i2c_init);
EXPORT_SYMBOL(sansa_i2c_send_bytes);
EXPORT_SYMBOL(sansa_i2c_send);
EXPORT_SYMBOL(sansa_i2c_send_byte);
EXPORT_SYMBOL(sansa_serial_init);
EXPORT_SYMBOL(sansa_set_process_dma);
EXPORT_SYMBOL(sansa_set_handle_cop);

