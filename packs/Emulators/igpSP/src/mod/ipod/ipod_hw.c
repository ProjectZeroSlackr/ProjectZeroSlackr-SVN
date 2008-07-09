/*
 * Last updated: July 9, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * CPU speed code borrowed from ithread
 * Brightness code borrowed from Rockbox
 * Backlight and contrast code borrowed from iBoy
 */

#include "ipod_common.h"
#include "ipod_hw.h"

extern int IPOD_LCD_TYPE;

int ipod_cpu_speed;
//int ipod_contrast;
int ipod_brightness;
int ipod_backlight;

static int cpu_speed_current = -1;
//static int contrast_current = -1;
static int brightness_current = -1;
static int backlight_current = -1;


/* CPU speed control */

// See ipod_common.h for postscalar constants
static void ipod_set_cpu_speed(int postscalar)
{
	outl(inl(0x70000020) | (1 << 30), 0x70000020);
	RUN_SET(0x2);
	outl(0xaa020008 | (postscalar << 8), CLOCK_SCALER);
	int x;
	for (x = 0; x < 10000; x++);
	RUN_SET(0x7);
}

// Note: Do NOT keep at 81MHz too long (i.e. over 15-30min) as iPod get hot
// After a certain temperature, igpSP gets screwy and iPL stops functioning
void ipod_update_cpu_speed()
{
	if (cpu_speed_current != ipod_cpu_speed) {
		cpu_speed_current = ipod_cpu_speed;
		switch (cpu_speed_current) {
			case 0: // Underclock
				ipod_set_cpu_speed(CPU_66MHz);
				break;
			case 1: // Normal
				ipod_set_cpu_speed(CPU_75MHz);
				break;
			case 2: // Overclock
				ipod_set_cpu_speed(CPU_78MHz);
				break;
			case 3: // Max overclock - Unstable!
				ipod_set_cpu_speed(CPU_81MHz);
				break;
		}
	}
}


/* Contrast control (monochrome iPods only) */

static int ipod_ioctl(int request, int *arg)
{
	int fd;
	fd = open("/dev/fb0", O_NONBLOCK);
	if (fd < 0) fd = open("/dev/fb/0", O_NONBLOCK);
	if (fd < 0) return -1;
	if (ioctl(fd, request, arg) < 0) {
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

// No support for monochrome iPods - see ipod_video.c
/*
static int ipod_get_contrast()
{
	int contrast;
	ipod_ioctl(FBIOGET_CONTRAST, &contrast);
	return contrast;
}

static void ipod_set_contrast(int contrast)
{
	ipod_ioctl(FBIOSET_CONTRAST, (int*)(long)contrast);
}

void ipod_update_contrast()
{
	if (IPOD_LCD_TYPE == 2 || IPOD_LCD_TYPE == 3) { // monochromes (1-4G & minis)
		if (contrast_current != ipod_contrast) {
			contrast_current = ipod_contrast;
			ipod_set_contrast(contrast_current);
		}
	}
}
*/


/* Brightness control (iPod nano and videos only) */
static void ipod_set_brightness(int val)
{
	int oldlevel;
	if (brightness_current < val) {
		do {
			oldlevel = disable_irq_save();
			GPIO_CLEAR_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			udelay(10);
			GPIO_SET_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			restore_irq(oldlevel);
			udelay(10);
		} while (++brightness_current < val);
	} else if (brightness_current > val) {
		do {
			oldlevel = disable_irq_save();
			GPIO_CLEAR_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			udelay(200);
			GPIO_SET_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			restore_irq(oldlevel);
			udelay(10);
		} while (--brightness_current > val);
	}
}

void ipod_update_brightness()
{
	if (IPOD_LCD_TYPE == 1 || IPOD_LCD_TYPE == 5) { // iPod nano or video
		if (brightness_current != ipod_brightness) {
			ipod_set_brightness(ipod_brightness);
		}
	}
}


/* Backlight control */

// Can't figure out how this works so unused by igpSP
/*
static int ipod_get_backlight()
{
	int backlight = 0;
	if (ipod_ioctl(FBIOGET_BACKLIGHT, &backlight) < 0)
		return -1;
	return backlight;
}
*/

static void ipod_set_backlight(int backlight)
{
	ipod_ioctl(FBIOSET_BACKLIGHT, (int *)(long)backlight);
}

void ipod_update_backlight()
{
	if (backlight_current != ipod_backlight) {
		backlight_current = ipod_backlight;
		if (backlight_current == 0) {
			ipod_set_backlight(BACKLIGHT_OFF);
		} else {
			ipod_set_backlight(BACKLIGHT_ON);
			// Turning on backlight sets brightness level to default, thus need to re-sync
			brightness_current = BRIGHTNESS_DEFAULT;
			ipod_update_brightness();
		}
	}
}


/* Overall */

void ipod_init_hw()
{
	ipod_update_cpu_speed();
	ipod_update_backlight();
	// ipod_update_brightness() called by ipod_update_backlight()
}

void ipod_exit_hw()
{
	ipod_set_backlight(CPU_75MHz); // iPodLinux default
	ipod_set_backlight(BACKLIGHT_ON);
}

