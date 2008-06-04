/*
 * Last updated: May 31, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo, Various
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

#include "ipod_common.h"
#include "ipod_hw.h"

int ipod_contrast;
int ipod_cpu_speed;

static int ipod_backlight = -1;
static int contrast_current = -1;
static int cpu_speed_current = -1;

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

// Can't figure out how this works so unused by igpSP
/*
static int ipod_get_backlight()
{
	int backlight = 0;
	if (ipod_ioctl(_IOR('F', 0x24, int), &backlight) < 0)
		return -1;
	return backlight;
}
*/

static void ipod_set_backlight(int backlight)
{
	ipod_ioctl(_IOW('F', 0x25, int), (int *)(long)backlight);
}

// Toggle is used instead of setting as I can't seem to get it to work otherwise >_<
void ipod_toggle_backlight()
{
	if (ipod_backlight == 1) {
		ipod_backlight = 0;
		ipod_set_backlight(BACKLIGHT_OFF);
	} else {
		ipod_backlight = 1;
		ipod_set_backlight(BACKLIGHT_ON);
	}
}

static int ipod_get_contrast()
{
	int contrast;
	ipod_ioctl(_IOR('F', 0x22, int), &contrast);
	return contrast;
}

static void ipod_set_contrast(int contrast)
{
	ipod_ioctl(_IOW('F', 0x23, int), (int*)(long)contrast);
}

void ipod_update_contrast()
{
	if (contrast_current != ipod_contrast) {
		contrast_current = ipod_contrast;
		ipod_set_contrast(contrast_current);
	}
}

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

void ipod_init_hw()
{
	ipod_backlight = 1; // For book-keeping purposes only
	ipod_cpu_speed = 2; // Default overclocked - place this into some config file
	ipod_contrast = ipod_get_contrast();
	ipod_set_backlight(BACKLIGHT_ON);
	ipod_update_cpu_speed();
}

void ipod_exit_hw()
{
	ipod_set_cpu_speed(CPU_75MHz); // Normal iPL speed
	ipod_set_backlight(BACKLIGHT_ON);
}


