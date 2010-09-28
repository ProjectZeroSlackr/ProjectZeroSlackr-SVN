/*
 * fb.c - Frame-buffer driver for Sansa
 *
 * Copyright (c) 2008 Sebastian Duell (Sebastian.Duell@SansaLinux.org)
 *
 * Skeleton from iPodLinux, routines mostly from Rockbox
 */

#include <linux/config.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <video/fbcon.h>
#include <video/fbcon-cfb16.h>

#include "pp5020.h"
#include "as3514.h"

#define SANSA_LCD_WIDTH	176
#define SANSA_LCD_HEIGHT	220

#define LCD_WIDTH	SANSA_LCD_WIDTH
#define LCD_HEIGHT	SANSA_LCD_HEIGHT

/* Reverse Flag */
#define R_DISP_CONTROL_NORMAL 0x0004
#define R_DISP_CONTROL_REV    0x0000
static unsigned short r_disp_control_rev = R_DISP_CONTROL_NORMAL;

/* Flipping */
#define R_DRV_OUTPUT_CONTROL_NORMAL  0x101b
#define R_DRV_OUTPUT_CONTROL_FLIPPED 0x131b
static unsigned short r_drv_output_control  = R_DRV_OUTPUT_CONTROL_NORMAL;

#define GPIOG_OUTPUT_VAL (*(volatile unsigned long *)(0x6000d0a8))
#define GPIO_CLEAR_BITWISE(port, mask) do { *(&port + (0x800/sizeof(long))) = mask << 8; } while(0)
#define GPIO_SET_BITWISE(port, mask) do { *(&port + (0x800/sizeof(long))) = (mask << 8) | mask; } while(0)

#define LCD_CLOCK_GPIO GPIOB_OUTPUT_VAL
#define LCD_CLOCK_PIN 5

#define LCD_DATA_OUT_GPIO GPIOB_OUTPUT_VAL
#define LCD_DATA_OUT_PIN 7

#define LCD_CS_GPIO GPIOD_OUTPUT_VAL
#define LCD_CS_PIN 6


#define LCD_REG_0 (*(volatile unsigned long *)(0xc2000000))
#define LCD_REG_1 (*(volatile unsigned long *)(0xc2000004))
#define LCD_REG_2 (*(volatile unsigned long *)(0xc2000008))
#define LCD_REG_3 (*(volatile unsigned long *)(0xc200000c))
#define LCD_REG_4 (*(volatile unsigned long *)(0xc2000010))
#define LCD_REG_5 (*(volatile unsigned long *)(0xc2000014))
#define LCD_REG_6 (*(volatile unsigned long *)(0xc2000018))
#define LCD_REG_7 (*(volatile unsigned long *)(0xc200001c))
#define LCD_REG_8 (*(volatile unsigned long *)(0xc2000020))
#define LCD_REG_9 (*(volatile unsigned long *)(0xc2000024))
#define LCD_FB_BASE_REG (*(volatile unsigned long *)(0xc2000028))

/* Taken from HD66789 datasheet and seems similar enough.
Definitely a Renesas chip though with a perfect register index
match but at least one bit seems to be set that that datasheet
doesn't show. It says T.B.D. on the regmap anyway. */
#define R_START_OSC             0x00
#define R_DRV_OUTPUT_CONTROL    0x01
#define R_DRV_WAVEFORM_CONTROL  0x02
#define R_ENTRY_MODE            0x03
#define R_COMPARE_REG1          0x04
#define R_COMPARE_REG2          0x05
#define R_DISP_CONTROL1         0x07
#define R_DISP_CONTROL2         0x08
#define R_DISP_CONTROL3         0x09
#define R_FRAME_CYCLE_CONTROL   0x0b
#define R_EXT_DISP_INTF_CONTROL 0x0c
#define R_POWER_CONTROL1        0x10
#define R_POWER_CONTROL2        0x11
#define R_POWER_CONTROL3        0x12
#define R_POWER_CONTROL4        0x13
#define R_RAM_ADDR_SET          0x21
#define R_RAM_READ_DATA         0x21
#define R_RAM_WRITE_DATA        0x22
#define R_RAM_WRITE_DATA_MASK1  0x23
#define R_RAM_WRITE_DATA_MASK2  0x24
#define R_GAMMA_FINE_ADJ_POS1   0x30
#define R_GAMMA_FINE_ADJ_POS2   0x31
#define R_GAMMA_FINE_ADJ_POS3   0x32
#define R_GAMMA_GRAD_ADJ_POS    0x33
#define R_GAMMA_FINE_ADJ_NEG1   0x34
#define R_GAMMA_FINE_ADJ_NEG2   0x35
#define R_GAMMA_FINE_ADJ_NEG3   0x36
#define R_GAMMA_GRAD_ADJ_NEG    0x37
#define R_GAMMA_AMP_ADJ_POS     0x38
#define R_GAMMA_AMP_ADJ_NEG     0x39
#define R_GATE_SCAN_START_POS   0x40
#define R_VERT_SCROLL_CONTROL   0x41
#define R_1ST_SCR_DRIVE_POS     0x42
#define R_2ND_SCR_DRIVE_POS     0x43
#define R_HORIZ_RAM_ADDR_POS    0x44
#define R_VERT_RAM_ADDR_POS     0x45


#define DEFAULT_BRIGHTNESS_SETTING  6


#define P2_M1(p2)  ((1 << (p2))-1)

/* align up or down to nearest 2^p2 */
#define ALIGN_DOWN_P2(n, p2) ((n) & ~P2_M1(p2))
#define ALIGN_UP_P2(n, p2)   ALIGN_DOWN_P2((n) + P2_M1(p2),p2)

#define uintptr_t   unsigned long
#define CACHEALIGN_BITS (5) /* 2^5 = 32 bytes */

#define CACHEALIGN_UP(x)  ((typeof (x))ALIGN_UP_P2((uintptr_t)(x), CACHEALIGN_BITS))
/* Aligns at least to the greater of size x or CACHEALIGN_SIZE */
#define CACHEALIGN_AT_LEAST_ATTR(x)   __attribute__((aligned(CACHEALIGN_UP(x))))

#define UNCACHED_BASE_ADDR 0x10000000
#define UNCACHED_ADDR(a) ((typeof (a))((uintptr_t)(a) | UNCACHED_BASE_ADDR))

/* allow for 2bpp */
//#define sansa_scr sansa_scrDriver
typedef unsigned short fb_data;

static fb_data sansa_scr[SANSA_LCD_HEIGHT][SANSA_LCD_WIDTH];
static /*volatile*/ fb_data sansa_scrDriver[SANSA_LCD_HEIGHT][SANSA_LCD_WIDTH]   CACHEALIGN_AT_LEAST_ATTR(16);

static int power_on   = 0; /* Is the power turned on?   */
static int display_on = 0; /* Is the display turned on? */


static void lcd_bus_idle(void)
{
   LCD_CLOCK_GPIO |= (1 << LCD_CLOCK_PIN);
   LCD_DATA_OUT_GPIO |= (1 << LCD_DATA_OUT_PIN);
}

static void lcd_send_byte(unsigned char byte)
{
   
   int i;
   
   for (i = 7; i >=0 ; i--)
   {
      LCD_CLOCK_GPIO &= ~(1 << LCD_CLOCK_PIN);
      if ((byte >> i) & 1)
      {
         LCD_DATA_OUT_GPIO |= (1 << LCD_DATA_OUT_PIN);
      } else {
         LCD_DATA_OUT_GPIO &= ~(1 << LCD_DATA_OUT_PIN);
      }
      udelay(1);
      LCD_CLOCK_GPIO |= (1 << LCD_CLOCK_PIN);
      udelay(1);
      lcd_bus_idle();
      udelay(3);
   }
}

static void lcd_send_msg(unsigned char cmd, unsigned int data)
{
   lcd_bus_idle();
   udelay(1);
   LCD_CS_GPIO &= ~(1 << LCD_CS_PIN);
   udelay(10);
   lcd_send_byte(cmd);
   lcd_send_byte((unsigned char)(data >> 8));
   lcd_send_byte((unsigned char)(data & 0xff));
   LCD_CS_GPIO |= (1 << LCD_CS_PIN);
   udelay(1);
   lcd_bus_idle();
}

static void lcd_write_reg(unsigned int reg, unsigned int data)
{
   lcd_send_msg(0x70, reg);
   lcd_send_msg(0x72, data);
}

static inline unsigned long phys_fb_address(typeof (sansa_scr) fb)
{
   if ((unsigned long)fb < 0x10000000)
   {
      return (unsigned long)fb + 0x10000000;
   }
   else
   {
      return (unsigned long)fb;
   }
}

static void lcd_init_gpio(void)
{
   GPIOB_ENABLE |= (1<<7);
   GPIOB_ENABLE |= (1<<5);
   GPIOB_OUTPUT_EN |= (1<<7);
   GPIOB_OUTPUT_EN |= (1<<5);
   GPIOD_ENABLE |= (1<<6);
   GPIOD_OUTPUT_EN |= (1<<6);
}

#define HZ 1000000
#define sleep udelay

/* Run the powerup sequence for the driver IC */
static void lcd_power_on(void)
{
    /* Clear standby bit */
    lcd_write_reg(R_POWER_CONTROL1, 0x0000);

    /** Power ON Sequence **/
    lcd_write_reg(R_START_OSC, 0x0001);
    /* 10ms or more for oscillation circuit to stabilize */
    sleep(HZ/50);

    /* SAP2-0=100, BT2-0=100, AP2-0=100, DK=1, SLP=0, STB=0 */
    lcd_write_reg(R_POWER_CONTROL1, 0x4444);
    /* DC12-10=000, DC2-0=000, VC2-0=001 */
    lcd_write_reg(R_POWER_CONTROL2, 0x0001);
    /* PON=0, VRH3-0=0011 */
    lcd_write_reg(R_POWER_CONTROL3, 0x0003);
    /* VCOMG=0, VDV4-0=10001, VCM3-0=11001 */
    lcd_write_reg(R_POWER_CONTROL4, 0x1119);
    /* PON=1, VRH3-0=0011 */
    lcd_write_reg(R_POWER_CONTROL3, 0x0013);
    sleep(HZ/25);

    /* SAP2-0=100, BT2-0=100, AP2-0=100, DK=0, SLP=0, STB=0 */
    lcd_write_reg(R_POWER_CONTROL1, 0x4440);
    /* VCOMG=1, VDV4-0=10001, VCM3-0=11001 */
    lcd_write_reg(R_POWER_CONTROL4, 0x3119);
    sleep(HZ/6);

    /* VSPL=0, HSPL=0, DPL=1, EPL=0, SM=0, GS=x, SS=x, NL4-0=11011 */
    lcd_write_reg(R_DRV_OUTPUT_CONTROL, r_drv_output_control);
    /* FLD=0, FLD0=1, B/C=1, EOR=1, NW5-0=000000 */
    lcd_write_reg(R_DRV_WAVEFORM_CONTROL, 0x0700);
    /* TRI=0, DFM1-0=11, BGR=0, HWM=1, ID1-0=10, AM=0, LG2-0=000
     * AM: horizontal update direction
     * ID1-0: H decrement, V increment
     */
    lcd_write_reg(R_ENTRY_MODE, 0x6020);
    lcd_write_reg(R_COMPARE_REG1, 0x0000);
    lcd_write_reg(R_COMPARE_REG2, 0x0000);
    /* FP3-0=0010, BP3-0=0010 */
    lcd_write_reg(R_DISP_CONTROL2, 0x0202);
    /* PTG1-0=00 (normal scan), ISC3-0=0000 (ignored) */
    lcd_write_reg(R_DISP_CONTROL3, 0x0000);
    /* NO2-0=01, SDT1-0=00, EQ1-0=01, DIV1-0=00, RTN3-0=0000 */
    lcd_write_reg(R_FRAME_CYCLE_CONTROL, 0x4400);
    /* RM=1, DM1-0=01, RIM1-0=00 */
    lcd_write_reg(R_EXT_DISP_INTF_CONTROL, 0x0110);
    /* SCN4-0=00000 - G1 if GS=0, G240 if GS=1 */
    lcd_write_reg(R_GATE_SCAN_START_POS, 0x0000);
    /* VL7-0=00000000 (0 lines) */
    lcd_write_reg(R_VERT_SCROLL_CONTROL, 0x0000);
    /* SE17-10=219, SS17-10=0 - 220 gates */
    lcd_write_reg(R_1ST_SCR_DRIVE_POS, (219 << 8));
    /* SE27-10=0, SS27-10=0 - no second screen */
    lcd_write_reg(R_2ND_SCR_DRIVE_POS, 0x0000);
    /* HEA=175, HSA=0 = H window from 0-175 */
    lcd_write_reg(R_HORIZ_RAM_ADDR_POS, (175 << 8));
    /* VEA=219, VSA=0 = V window from 0-219 */
    lcd_write_reg(R_VERT_RAM_ADDR_POS, (219 << 8));
    /* PKP12-10=000, PKP02-00=000 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_POS1, 0x0000);
    /* PKP32-30=111, PKP22-20=100 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_POS2, 0x0704);
    /* PKP52-50=001, PKP42-40=111 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_POS3, 0x0107);
    /* PRP12-10=111, PRP02-00=100 */
    lcd_write_reg(R_GAMMA_GRAD_ADJ_POS, 0x0704);
    /* PKN12-10=001, PKN02-00=111 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_NEG1, 0x0107);
    /* PKN32-30=000, PKN22-20=010 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_NEG2, 0x0002);
    /* PKN52-50=111, PKN42-40=111 */
    lcd_write_reg(R_GAMMA_FINE_ADJ_NEG3, 0x0707);
    /* PRN12-10=101, PRN02-00=011 */
    lcd_write_reg(R_GAMMA_GRAD_ADJ_NEG, 0x0503);
    /* VRP14-10=00000, VRP03-00=0000 */
    lcd_write_reg(R_GAMMA_AMP_ADJ_POS, 0x0000);
    /* WRN14-10=00000, VRN03-00=0000 */
    lcd_write_reg(R_GAMMA_AMP_ADJ_NEG, 0x0000);
    /* AD15-0=175 (upper right corner) */
    lcd_write_reg(R_RAM_ADDR_SET, 175);
    /* RM=1, DM1-0=01, RIM1-0=00 */
    lcd_write_reg(R_EXT_DISP_INTF_CONTROL, 0x0110);

    power_on = 1;
}

/* Run the display on sequence for the driver IC */
static void lcd_display_on(void)
{
    if (!power_on)
    {
        /* Power has been turned off so full reinit is needed */
        lcd_power_on();
    }
    else
    {
        /* Restore what we fiddled with when turning display off */
        /* PON=1, VRH3-0=0011 */
        lcd_write_reg(R_POWER_CONTROL3, 0x0013);
        /* NO2-0=01, SDT1-0=00, EQ1-0=01, DIV1-0=00, RTN3-0=0000 */
        lcd_write_reg(R_FRAME_CYCLE_CONTROL, 0x4400);
        /* VCOMG=1, VDV4-0=10001, VCM3-0=11001 */
        lcd_write_reg(R_POWER_CONTROL4, 0x3119);
    }

    /* SAP2-0=100, BT2-0=111, AP2-0=100, DK=1, SLP=0, STB=0 */
    lcd_write_reg(R_POWER_CONTROL1, 0x4740);

    sleep(HZ/25);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=0, DTE=0, CL=0,
       REV=x, D1-0=01 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0041 | r_disp_control_rev);

    sleep(HZ/30);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=1, DTE=0, CL=0,
       REV=x, D1-0=01 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0061 | r_disp_control_rev);
    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=1, DTE=0, CL=0,
       REV=x, D1-0=11 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0063 | r_disp_control_rev);

    sleep(HZ/30);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=1, DTE=1, CL=0,
       REV=x, D1-0=11 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0073 | r_disp_control_rev);

    /* Go into write data mode */
    lcd_send_msg(0x70, R_RAM_WRITE_DATA);

    /* tell that we're on now */
    display_on = 1;
}

/* Turn off visible display operations */
static void lcd_display_off(void)
{
    /* block drawing operations and changing of first */
    display_on = 0;

    /* NO2-0=01, SDT1-0=00, EQ1-0=00, DIV1-0=00, RTN3-0=0000 */
    lcd_write_reg(R_FRAME_CYCLE_CONTROL, 0x4000);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=1, DTE=1, CL=0,
       REV=x, D1-0=10 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0072 | r_disp_control_rev);

    sleep(HZ/25);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=1, GON=1, DTE=0, CL=0,
       REV=x, D1-0=10 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0062 | r_disp_control_rev);

    sleep(HZ/25);

    /* PT1-0=00, VLE2-1=00, SPT=0, IB6(??)=0, GON=0, DTE=0, CL=0,
       REV=0, D1-0=00 */
    lcd_write_reg(R_DISP_CONTROL1, 0x0000);
    /* SAP2-0=000, BT2-0=000, AP2-0=000, DK=0, SLP=0, STBY=0 */
    lcd_write_reg(R_POWER_CONTROL1, 0x0000);
    /* PON=0, VRH3-0=0011 */
    lcd_write_reg(R_POWER_CONTROL3, 0x0003);
    /* VCOMG=0, VDV4-0=10001, VCM4-0=11001 */
    lcd_write_reg(R_POWER_CONTROL4, 0x1119);
}

void lcd_init_device(void)
{
/* All this is magic worked out by MrH */

/* Stop any DMA which is in progress */
    LCD_REG_6 &= ~1;
    udelay(100000);

/* Init GPIO ports */
    lcd_init_gpio();
/* Controller init */
    GPO32_ENABLE |= (1 << 28);
    GPO32_VAL &= ~(1 << 28);
    DEV_INIT1 = ( (DEV_INIT1 & 0x03ffffff) | (0x15 << 26) );
    outl(((inl(0x70000014) & (0x0fffffff)) | (0x5 << 28)), 0x70000014);
    outl((inl(0x70000020) & ~(0x3 << 10)), 0x70000020);
    DEV_EN |= DEV_LCD; /* Enable controller */
    outl(0x6, 0x600060d0);
    DEV_RS |= DEV_LCD; /* Reset controller */
    outl((inl(0x70000020) & ~(1 << 14)), 0x70000020);
    lcd_bus_idle();
    DEV_RS &=~DEV_LCD; /* Clear reset */
    udelay(1000);

    LCD_REG_0 = (LCD_REG_0 & (0x00ffffff)) | (0x22 << 24);
    LCD_REG_0 = (LCD_REG_0 & (0xff00ffff)) | (0x14 << 16);
    LCD_REG_0 = (LCD_REG_0 & (0xffffc0ff)) | (0x3 << 8);
    LCD_REG_0 = (LCD_REG_0 & (0xffffffc0)) | (0xa);

    LCD_REG_1 &= 0x00ffffff;
    LCD_REG_1 &= 0xff00ffff;
    LCD_REG_1 = (LCD_REG_1 & 0xffff03ff) | (0x2 << 10);
    LCD_REG_1 = (LCD_REG_1 & 0xfffffc00) | (0xdd);

    LCD_REG_2 |= (1 << 5);
    LCD_REG_2 |= (1 << 6);
    LCD_REG_2 = (LCD_REG_2 & 0xfffffcff) | (0x2 << 8);

    LCD_REG_7 &= (0xf800ffff);
    LCD_REG_7 &= (0xfffff800);

    LCD_REG_8 = (LCD_REG_8 & (0xf800ffff)) | (0xb0 << 16);
    LCD_REG_8 = (LCD_REG_8 & (0xfffff800)) | (0xdc); /* X-Y Geometry? */

    LCD_REG_5 |= 0xc;
    LCD_REG_5 = (LCD_REG_5 & ~(0x70)) | (0x3 << 4);
    LCD_REG_5 |= 2;

    LCD_REG_6 &= ~(1 << 15);
    LCD_REG_6 |= (0xe00);
    LCD_REG_6 = (LCD_REG_6 & (0xffffff1f)) | (0x4 << 5);
    LCD_REG_6 |= (1 << 4);

    LCD_REG_5 &= ~(1 << 7);
    LCD_FB_BASE_REG = ((unsigned long)&sansa_scrDriver[0][0] & 0x0fffffff) | 0x10000000; //0x11000000;//phys_fb_address(lcd_driver_framebuffer);

    udelay(100000);

/* LCD init */
    /* Pull RESET low, then high to reset driver IC */
    GPO32_VAL &= ~(1 << 28);
    udelay(10000);
    GPO32_VAL |= (1 << 28);
    udelay(10000);

    lcd_display_on();
    LCD_REG_6 |= 1; /* Start DMA */
}

/* initialise the LCD */
static void init_lcd(void)
{
   sansa_i2c_init();
   lcd_init_device();
}

extern void lcd_copy_buffer_rect(fb_data *dst, const fb_data *src,
                                 int width, int height);

static void sansa_update_display(struct display *p, int x, int y, int width, int height)
{
#if 0
    fb_data *dst, *src;

    if (!display_on)
        return;

    if (x + width > LCD_WIDTH)
        width = LCD_WIDTH - x; /* Clip right */
    if (x < 0)
        width += x, x = 0; /* Clip left */
    if (width <= 0)
        return; /* nothing left to do */

    if (y + height > LCD_HEIGHT)
        height = LCD_HEIGHT - y; /* Clip bottom */
    if (y < 0)
        height += y, y = 0; /* Clip top */
    if (height <= 0)
        return; /* nothing left to do */

    /* TODO: It may be faster to swap the addresses of lcd_driver_framebuffer
     * and lcd_framebuffer */
    dst = UNCACHED_ADDR(&sansa_scrDriver[x][y]);
    src = &sansa_scr[x][y];

    /* Copy part of the Rockbox framebuffer to the second framebuffer */
    if (width < LCD_WIDTH)
    {
        /* Not full width - do line-by-line */
        lcd_copy_buffer_rect(dst, src, width, height);
    }
    else
    {
        /* Full width - copy as one line */
        lcd_copy_buffer_rect(dst, src, LCD_WIDTH*height, 1);
    }
#endif
    if(display_on)
    {
       fb_data *dst, *src;
       dst = UNCACHED_ADDR(&sansa_scrDriver[0][0]);
       src = &sansa_scr[0][0];

       lcd_copy_buffer_rect(dst, src, LCD_WIDTH*LCD_HEIGHT, 1);
    }
}

void lcd_sleep(void)
{
   LCD_REG_6 &= ~1;
   sleep(HZ/50);
   
   if (power_on)
   {
      /* Turn off display */
      if (display_on)
         lcd_display_off();
      
      power_on = 0;
   }
   
   /* Set standby mode */
   /* SAP2-0=000, BT2-0=000, AP2-0=000, DK=0, SLP=0, STB=1 */
   lcd_write_reg(R_POWER_CONTROL1, 0x0001);
}

void lcd_enable(int on)
{
   if (on)
   {
      if(!display_on)
      {
//         DEV_EN |= DEV_LCD; /* Enable LCD controller */
         lcd_display_on();  /* Turn on display */
//         sansa_update_display(NULL, 0, 0, 0, 0);      /* Resync display */
         LCD_REG_6 |= 1;    /* Restart DMA */
//         sleep(HZ/50);      /* Wait for a frame to be written */
      }
   }
   else
   {
      LCD_REG_6 &= ~1;    /* Disable DMA */
      sleep(HZ/50);       /* Wait for dma end (assuming 50Hz) */
      lcd_display_off();  /* Turn off display */
//      DEV_EN &= ~DEV_LCD; /* Disable LCD controller */
   }
}

static int backlightStatus = 1;

static void set_backlight(int on)
{
   if(on != backlightStatus)
   {
      if(on)
      {
         lcd_enable(1);
         sansa_i2c_lock();
         sansa_i2c_send(AS3514_I2C_ADDR, DCDC15, DEFAULT_BRIGHTNESS_SETTING);
         sansa_i2c_unlock();
         GPIO_SET_BITWISE(GPIOG_OUTPUT_VAL, 0x80);
      }
      else
      {
         GPIO_CLEAR_BITWISE(GPIOG_OUTPUT_VAL, 0x80);
         sansa_i2c_lock();
         sansa_i2c_send(AS3514_I2C_ADDR, DCDC15, 0x0);
         sansa_i2c_unlock();
         lcd_enable(0);
         lcd_sleep();
      }
      backlightStatus = on;
   }
}

struct sansafb_info {
	/*
	 *  Choose _one_ of the two alternatives:
	 *
	 *    1. Use the generic frame buffer operations (fbgen_*).
	 */
	struct fb_info_gen gen;

#if 0
	/*
	 *    2. Provide your own frame buffer operations.
	 */
	struct fb_info info;
#endif

	/* Here starts the frame buffer device dependent part */
	/* You can use this to store e.g. the board number if you support */
	/* multiple boards */
};


struct sansafb_par {
	/*
	 *  The hardware specific data in this structure uniquely defines a video
	 *  mode.
	 *
	 *  If your hardware supports only one video mode, you can leave it empty.
	 */
};


void sansa_fb_setup(struct display *p)
{
	fbcon_cfb16.setup(p);
}

void sansa_fb_bmove(struct display *p, int sy, int sx, int dy, int dx,
		     int height, int width)
{
	fbcon_cfb16.bmove(p, sy, sx, dy, dx, height, width);
	sansa_update_display(p, 0, 0, SANSA_LCD_WIDTH/8, SANSA_LCD_HEIGHT/fontheight(p));
}

void sansa_fb_clear(struct vc_data *conp, struct display *p, int sy, int sx,
		     int height, int width)
{
	fbcon_cfb16.clear(conp, p, sy, sx, height, width);
	sansa_update_display(p, sx, sy, sx+width, sy+height);
}

void sansa_fb_putc(struct vc_data *conp, struct display *p, int c, int yy,
		    int xx)
{
	fbcon_cfb16.putc(conp, p, c, yy, xx);
	sansa_update_display(p, xx, yy, xx+1, yy+1);
}

void sansa_fb_putcs(struct vc_data *conp, struct display *p, 
		     const unsigned short *s, int count, int yy, int xx)
{
	fbcon_cfb16.putcs(conp, p, s, count, yy, xx);
	sansa_update_display(p, xx, yy, xx+count, yy+1);
}

void sansa_fb_revc(struct display *p, int xx, int yy)
{
	fbcon_cfb16.revc(p, xx, yy);
	sansa_update_display(p, xx, yy, xx+1, yy+1);
}


/*
 *  `switch' for the low level operations
 */

struct display_switch fbcon_sansa = {
	setup:		sansa_fb_setup,
	bmove:		sansa_fb_bmove,
	clear:		sansa_fb_clear,
	putc:		sansa_fb_putc,
	putcs:		sansa_fb_putcs,
	revc:		sansa_fb_revc,
	fontwidthmask:	FONTWIDTH(8)
};

static struct sansafb_info fb_info;
static struct sansafb_par current_par;
static int current_par_valid = 0;
static struct display disp;

static struct fb_var_screeninfo default_var;

int sansafb_init(void);
int sansafb_setup(char*);

/* ------------------- chipset specific functions -------------------------- */

static void sansa_get_par(struct sansafb_par *, const struct fb_info *);
static int sansa_encode_var(struct fb_var_screeninfo *, struct sansafb_par *, const struct fb_info *);

static void sansa_detect(void)
{
	/*
	 *  This function should detect the current video mode settings and store
	 *  it as the default video mode
	 */

	struct sansafb_par par;

	sansa_get_par(&par, NULL);
	sansa_encode_var(&default_var, &par, NULL);
}

static int sansa_encode_fix(struct fb_fix_screeninfo *fix, struct sansafb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  This function should fill in the 'fix' structure based on the values
	 *  in the `par' structure.
	 */

	memset(fix, 0x0, sizeof(*fix));

	strcpy(fix->id, "Sansa");
	/* required for mmap() */
	fix->smem_start = sansa_scr;
	fix->smem_len = SANSA_LCD_HEIGHT * SANSA_LCD_WIDTH * 2;

	fix->type= FB_TYPE_PACKED_PIXELS;

	fix->visual = FB_VISUAL_TRUECOLOR; //FB_VISUAL_DIRECTCOLOR; //FB_VISUAL_PSEUDOCOLOR;	/* fixed visual */
	fix->line_length = SANSA_LCD_WIDTH * 2;	/* cfb2 default */

	fix->xpanstep = 0;	/* no hardware panning */
	fix->ypanstep = 0;	/* no hardware panning */
	fix->ywrapstep = 0;	/* */

	fix->accel = FB_ACCEL_NONE;

	return 0;
}

static int sansa_decode_var(struct fb_var_screeninfo *var, struct sansafb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  Get the video params out of 'var'. If a value doesn't fit, round it up,
	 *  if it's too big, return -EINVAL.
	 *
	 *  Suggestion: Round up in the following order: bits_per_pixel, xres,
	 *  yres, xres_virtual, yres_virtual, xoffset, yoffset, grayscale,
	 *  bitfields, horizontal timing, vertical timing.
	 */

	if ( var->xres > SANSA_LCD_WIDTH ||
		var->yres > SANSA_LCD_HEIGHT ||
		var->xres_virtual != var->xres ||
		var->yres_virtual != var->yres ||
		var->xoffset != 0 ||
		var->yoffset != 0 ) {
		return -EINVAL;
	}

	if ( var->bits_per_pixel != 16 ) {
		return -EINVAL;
	}

	return 0;
}

static int sansa_encode_var(struct fb_var_screeninfo *var, struct sansafb_par *par,
			  const struct fb_info *info)
{
	/*
	 *  Fill the 'var' structure based on the values in 'par' and maybe other
	 *  values read out of the hardware.
	 */

	var->xres = SANSA_LCD_WIDTH;
	var->yres = SANSA_LCD_HEIGHT;
	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;
	var->xoffset = 0;
	var->yoffset = 0;

	var->bits_per_pixel = 16;

	return 0;
}

static void sansa_get_par(struct sansafb_par *par, const struct fb_info *info)
{
	/*
	 *  Fill the hardware's 'par' structure.
	 */

	if ( current_par_valid ) {
		*par = current_par;
	}
	else {
		/* ... */
	}
}

static void sansa_set_par(struct sansafb_par *par, const struct fb_info *info)
{
	/*
	 *  Set the hardware according to 'par'.
	 */

	current_par = *par;
	current_par_valid = 1;

	/* ... */
}

static int sansa_getcolreg(unsigned regno, unsigned *red, unsigned *green,
			 unsigned *blue, unsigned *transp,
			 const struct fb_info *info)
{
	/*
	 *  Read a single color register and split it into colors/transparent.
	 *  The return values must have a 16 bit magnitude.
	 *  Return != 0 for invalid regno.
	 */

	/* ... */
	return 0;
}

static int sansa_setcolreg(unsigned regno, unsigned red, unsigned green,
			 unsigned blue, unsigned transp,
			 const struct fb_info *info)
{
	/*
	 *  Set a single color register. The values supplied have a 16 bit
	 *  magnitude.
	 *  Return != 0 for invalid regno.
	 */

	/* ... */
	return 0;
}

static int sansa_pan_display(struct fb_var_screeninfo *var,
			   struct sansafb_par *par, const struct fb_info *info)
{
	/*
	 *  Pan (or wrap, depending on the `vmode' field) the display using the
	 *  `xoffset' and `yoffset' fields of the `var' structure.
	 *  If the values don't fit, return -EINVAL.
	 */

	/* ... */
	return -EINVAL;
}

static int sansa_blank(int blank_mode, const struct fb_info *info)
{
	static int backlight_on = -1;
#if 0
	switch (blank_mode) {
	case VESA_NO_BLANKING:
		/* printk(KERN_ERR "VESA_NO_BLANKING\n"); */

		/* start oscillation
		 * wait 10ms
		 * cancel standby
		 * turn on LCD power
		 */
		lcd_cmd_and_data(0x0, 0x0, 0x1);
		udelay(10000);
		lcd_cmd_and_data(0x3, 0x15, 0x0);
		lcd_cmd_and_data(0x3, 0x15, 0xc);

		if (backlight_on != -1) {
			set_backlight(backlight_on);
		}
		backlight_on = -1;
		break;

	case VESA_VSYNC_SUSPEND:
	case VESA_HSYNC_SUSPEND:
		/* printk(KERN_ERR "VESA_XSYNC_BLANKING\n"); */
		if (backlight_on == -1) {
			backlight_on = get_backlight();
			set_backlight(0);
		}

		/* go to SLP = 1 */
		/* 10101 00001100 */
		lcd_cmd_and_data(0x3, 0x15, 0x0);
		lcd_cmd_and_data(0x3, 0x15, 0x2);
		break;

	case VESA_POWERDOWN:
		/* printk(KERN_ERR "VESA_POWERDOWN\n"); */
		if (backlight_on == -1) {
			backlight_on = get_backlight();
			set_backlight(0);
		}

		/* got to standby */
		lcd_cmd_and_data(0x3, 0x15, 0x1);
		break;

	default:
		/* printk(KERN_ERR "unknown blank value %d\n", blank_mode); */
		return -EINVAL;
	}
#endif
	return 0;
}

static void sansa_set_disp(const void *par, struct display *disp,
			 struct fb_info_gen *info)
{
	/*
	 *  Fill in a pointer with the virtual address of the mapped frame buffer.
	 *  Fill in a pointer to appropriate low level text console operations (and
	 *  optionally a pointer to help data) for the video mode `par' of your
	 *  video hardware. These can be generic software routines, or hardware
	 *  accelerated routines specifically tailored for your hardware.
	 *  If you don't have any appropriate operations, you must fill in a
	 *  pointer to dummy operations, and there will be no text output.
	 */

	disp->screen_base = sansa_scr;
	disp->dispsw = &fbcon_sansa;
}


/* ------------ Interfaces to hardware functions ------------ */


struct fbgen_hwswitch sansa_switch = {
	detect:		sansa_detect,
	encode_fix:	sansa_encode_fix,
	decode_var:	sansa_decode_var,
	encode_var:	sansa_encode_var,
	get_par:	sansa_get_par,
	set_par:	sansa_set_par,
	getcolreg:	sansa_getcolreg,
	setcolreg:	sansa_setcolreg,
	pan_display:	sansa_pan_display,
	blank:		sansa_blank,
	set_disp:	sansa_set_disp,
};


/* ------------------------------------------------------------------------- */


/*
 *  Frame buffer operations
 */

static int sansa_fp_open(const struct fb_info *info, int user)
{
	return 0;
}

#define FBIOGET_CONTRAST	_IOR('F', 0x22, int)
#define FBIOPUT_CONTRAST	_IOW('F', 0x23, int)

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOPUT_BACKLIGHT	_IOW('F', 0x25, int)

#define SANSA_MIN_CONTRAST 0
#define SANSA_MAX_CONTRAST 0x7f

static int sansa_fb_ioctl(struct inode *inode, struct file *file, u_int cmd,
	u_long arg, int con, struct fb_info *info)

{
	int val;

	switch (cmd) {
	case FBIOGET_CONTRAST:
		val = SANSA_MAX_CONTRAST;
		break;

	case FBIOPUT_CONTRAST:
		break;

	case FBIOGET_BACKLIGHT:
		val = backlightStatus;
		if (put_user(val, (int *)arg))
			return -EFAULT;
		break;

	case FBIOPUT_BACKLIGHT:
		val = (int)arg;
		set_backlight(val);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}


/*
 *  In most cases the `generic' routines (fbgen_*) should be satisfactory.
 *  However, you're free to fill in your own replacements.
 */

static struct fb_ops sansafb_ops = {
	owner:		THIS_MODULE,
	fb_open:	sansa_fp_open,
	fb_get_fix:	fbgen_get_fix,
	fb_get_var:	fbgen_get_var,
	fb_set_var:	fbgen_set_var,
	fb_get_cmap:	fbgen_get_cmap,
	fb_set_cmap:	fbgen_set_cmap,
	fb_pan_display:	fbgen_pan_display,
	fb_ioctl:	sansa_fb_ioctl,
};


/* ------------ Hardware Independent Functions ------------ */


/*
 *  Initialization
 */

int __init sansafb_init(void)
{
   int i,j;

   fb_info.gen.fbhw = &sansa_switch;

	fb_info.gen.fbhw->detect();

	strcpy(fb_info.gen.info.modename, "Sansa");

	fb_info.gen.info.changevar = NULL;
	fb_info.gen.info.node = -1;
	fb_info.gen.info.fbops = &sansafb_ops;
	fb_info.gen.info.disp = &disp;
	fb_info.gen.info.switch_con = &fbgen_switch;
	fb_info.gen.info.updatevar = &fbgen_update_var;
	fb_info.gen.info.blank = &fbgen_blank;
	fb_info.gen.info.flags = FBINFO_FLAG_DEFAULT;

	/* This should give a reasonable default video mode */
	fbgen_get_var(&disp.var, -1, &fb_info.gen.info);
	fbgen_do_set_var(&disp.var, 1, &fb_info.gen);
	fbgen_set_disp(-1, &fb_info.gen);
	fbgen_install_cmap(0, &fb_info.gen);

	if ( register_framebuffer(&fb_info.gen.info) < 0 ) {
		return -EINVAL;
	}

	init_lcd();

	printk(KERN_INFO "fb%d: %s frame buffer device\n", GET_FB_IDX(fb_info.gen.info.node), fb_info.gen.info.modename);

	/* uncomment this if your driver cannot be unloaded */
	/* MOD_INC_USE_COUNT; */
	return 0;
}


/*
 *  Cleanup
 */

void sansafb_cleanup(struct fb_info *info)
{
	/*
	 *  If your driver supports multiple boards, you should unregister and
	 *  clean up all instances.
	 */

	unregister_framebuffer(info);
	/* ... */
}


/*
 *  Setup
 */

int __init sansafb_setup(char *options)
{
	/* Parse user speficied options (`video=sansafb:') */
	return 0;
}



/* ------------------------------------------------------------------------- */


/*
 *  Modularization
 */

#ifdef MODULE
MODULE_LICENSE("GPL");
int init_module(void)
{
	return sansafb_init();
}

void cleanup_module(void)
{
	sansafb_cleanup(void);
}
#endif /* MODULE */

