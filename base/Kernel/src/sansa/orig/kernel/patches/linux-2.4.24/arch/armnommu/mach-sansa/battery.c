/*
 * battery.c - battery-status for Sansa
 *
 * Copyright (c) 2008 Sebastian Duell (Sebastian.Duell@SansaLinux.org)
 *
 * Routines mostly from Rockbox and iPodLinux
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>

#include <linux/proc_fs.h>

#include "pp5020.h"
#include "as3514.h"

#define NUM_ADC_CHANNELS 13
#define ADC_BVDD         0  /* Battery voltage of 4V LiIo accumulator */

extern sansa_i2c_send(unsigned int addr, int data0, int data1);
extern int sansa_i2c_readbytes(unsigned int dev_addr, int addr, int len, unsigned char *data);

/* Read 10-bit channel data */
unsigned short adc_read(int channel)
{
   unsigned short data = 0;
   
   if ((unsigned)channel < NUM_ADC_CHANNELS)
   {
      /* Select channel */   
      sansa_i2c_lock();
      if (sansa_i2c_send( AS3514_I2C_ADDR, ADC_0, (channel << 4)) >= 0)
      {
         unsigned char buf[2];
         
         /* Read data */
         if (sansa_i2c_readbytes( AS3514_I2C_ADDR, ADC_0, 2, buf) >= 0)
         {
            data = (((buf[0] & 0x3) << 8) | buf[1]);
         }
      }
      sansa_i2c_unlock();
   }
   
   return data;
}

/* voltages (millivolt) of 0%, 10%, ... 100% when charging disabled */
const unsigned short percent_to_volt_discharge[11] =
{
   /* Sansa Li Ion 750mAH, took from battery benchs */
     3300, 3390, 3480, 3570, 3660, 3750, 3840, 3930, 4020, 4110, 4200 
};


static int voltage_to_percent(int voltage, const short* table)
{
   if (voltage <= table[0])
      return 0;
   else
      if (voltage >= table[10])
         return 100;
      else {
         /* search nearest value */
         int i = 0;
         while ((i < 10) && (table[i+1] < voltage))
            i++;
         /* interpolate linear between the smaller and greater value */
         return (i * 10) /* Tens digit, 10% per entry */
            + (((voltage - table[i]) * 10)
            / (table[i+1] - table[i])); /* Ones digit: interpolated */
      }
}

/* ADC should read 0x3ff=5.12V */
#define BATTERY_SCALE_FACTOR 5125       
/* full-scale ADC readout (2^10) in millivolt */

/* Returns battery voltage from ADC [millivolts] */
unsigned int battery_adc_voltage(void)
{
   return (adc_read(ADC_BVDD) * BATTERY_SCALE_FACTOR) >> 10;
}

static int ipod_apm_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	char *p;

	static char driver_version[] = "1.16";      /* no spaces */

	unsigned short  ac_line_status = 0xff;
	unsigned short  battery_status = 0;
	unsigned short  battery_flag   = 0xff;
	int             percentage     = -1;
	int             time_units     = -1;
	char            *units         = "?";

   percentage = voltage_to_percent(battery_adc_voltage(), percent_to_volt_discharge);
   time_units = ((percentage * 512) / 100);

   if(GPIOB_INPUT_VAL & 0x10)
      battery_status = 0x3;	// charging 
   
	units = "ipods";

        /* Arguments, with symbols from linux/apm_bios.h.  Information is
           from the Get Power Status (0x0a) call unless otherwise noted.

           0) Linux driver version (this will change if format changes)
           1) APM BIOS Version.  Usually 1.0, 1.1 or 1.2.
           2) APM flags from APM Installation Check (0x00):
              bit 0: APM_16_BIT_SUPPORT
              bit 1: APM_32_BIT_SUPPORT
              bit 2: APM_IDLE_SLOWS_CLOCK
              bit 3: APM_BIOS_DISABLED
              bit 4: APM_BIOS_DISENGAGED
           3) AC line status
              0x00: Off-line
              0x01: On-line
              0x02: On backup power (BIOS >= 1.1 only)
              0xff: Unknown
           4) Battery status
              0x00: High
              0x01: Low
              0x02: Critical
              0x03: Charging
              0x04: Selected battery not present (BIOS >= 1.2 only)
              0xff: Unknown
           5) Battery flag
              bit 0: High
              bit 1: Low
              bit 2: Critical
              bit 3: Charging
              bit 7: No system battery
              0xff: Unknown
           6) Remaining battery life (percentage of charge):
              0-100: valid
              -1: Unknown
           7) Remaining battery life (time units):
              Number of remaining minutes or seconds
              -1: Unknown
           8) min = minutes; sec = seconds */

	p = page;
	p += sprintf(p, "%s %d.%d 0x%02x 0x%02x 0x%02x 0x%02x %d%% %d %s\n",
		driver_version,
		1, // (apm_info.bios.version >> 8) & 0xff,
		0, // apm_info.bios.version & 0xff,
		0, // apm_info.bios.flags,
		ac_line_status,
		battery_status,
		battery_flag,
		percentage,
		time_units,
		units);

	return p - page;
}

static int __init battery_init(void)
{
   sansa_i2c_init();
   create_proc_read_entry("apm", 0, NULL, ipod_apm_read, NULL);
   return 0;
}

static void __exit battery_exit(void)
{
   remove_proc_entry("apm", NULL);
}

module_init(battery_init);
module_exit(battery_exit);

MODULE_AUTHOR("Sebastian Duell (Sebastian.Duell@SansaLinux.org)");
MODULE_DESCRIPTION("Battery-status for Sansa");
MODULE_LICENSE("GPL");