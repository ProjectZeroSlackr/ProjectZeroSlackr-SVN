/*
 * Last updated: Nov 13, 2008
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

// Small program for testing iPod input (wheel touches).
// Not part of igpSP - compile this separately with arm-uclinux-elf-gcc
// Note that this will probably not work for 3Gs and older
// as they lack touch wheel functionality.

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>

#include "ipod_input.h"

#define inl(a) \
	(*(volatile unsigned int *)(a)) \

static int IPOD_HW_VER;

// Borrowed from hotdog
static long iPod_GetGeneration() 
{
	static long gen = 0;
	if (gen == 0) {
		int i;
		char cpuinfo[256];
		char *ptr;
		FILE *file;
		
		if ((file = fopen("/proc/cpuinfo", "r")) != NULL) {
			while (fgets(cpuinfo, sizeof(cpuinfo), file) != NULL)
				if (strncmp(cpuinfo, "Revision", 8) == 0)
					break;
			fclose(file);
		}
		for (i = 0; !isspace(cpuinfo[i]); i++);
		for (; isspace(cpuinfo[i]); i++);
		ptr = cpuinfo + i + 2;
		
		gen = strtol (ptr, NULL, 16);
	}
	return gen;
}

static int ipod_get_keytouch()
{
	int touch;
	
	touch = 0xff;
	if (IPOD_HW_VER != 0x4 && IPOD_HW_VER != 0x3) { // Not mini 1G or 3G
		int in, st;
		in = inl(0x7000C140);
		st = ((in & 0xff000000) >> 24);
		
		touch = 0xff;
		if (st == 0xc0)
			touch = (in & 0x007F0000 ) >> 16;
	}
	
	if (touch != 0xff) {
		touch += 6;
		touch /= 12;
		if(touch > 7)
			touch = 0;
		return touch;
	} else {
		return TOUCH_NULL;
	}
}

int main()
{
	int input;
	char *keyname;
	int quit = 0;
	char *model = "Unknown";
	
	IPOD_HW_VER = iPod_GetGeneration() >> 16;
	
	#define IPOD_SANSA		0x0
	#define IPOD_VIDEO		0xB
	#define IPOD_NANO		0xC
	#define IPOD_PHOTO		0x6
	#define IPOD_MINI_2G	0x7
	#define IPOD_MINI_1G	0x4
	#define IPOD_MONO_4G	0x5
	#define IPOD_MONO_3G	0x3
	#define IPOD_MONO_2G	0x2
	#define IPOD_MONO_1G	0x1
	
	switch (IPOD_HW_VER) {
		case 0x0: // Sansa e200
			model = "Sansa e200";
			quit = 1;
			break;
		case 0xB: // video
			model = "5/5.5G iPod video";
			break;
		case 0xC: // nano
			model = "iPod nano 1G";
			break;
		case 0x6: // photo, color
			if (iPod_GetGeneration() == 0x60000)
				model = "iPod photo";
			else
				model = "iPod colour";
			break;
		case 0x7: // mini2g
			model = "iPod mini 2G";
			break;
		case 0x4: // mini1g
			model = "iPod mini 1G";
			break;
		case 0x5: // 4g
			model = "4G iPod mono";
			break;
		case 0x3: // 3g
			model = "3G iPod mono";
			quit = 1;
			break;
		case 0x2: // 2g
			model = "2G iPod mono";
			quit = 1;
			break;
		case 0x1: // 1g
			model = "1G iPod mono";
			quit = 1;
			break;
	}
	fprintf(stderr, "iPod model: %s\n", model);
	if (quit) {
		fprintf(stderr, "Sorry, this iPod model does not have touch wheel support./n");
		exit(1);
	}
	
	for (;;) { // Infinite loop
		input = ipod_get_keytouch();
		keyname = NULL;
		if (input != TOUCH_NULL) {
			switch(input) {
				case TOUCH_U:
					keyname = "up";
					break;
				case TOUCH_UR:
					keyname = "up-right";
					break;
				case TOUCH_R:
					keyname = "right";
					break;
				case TOUCH_DR:
					keyname = "down-right";
					break;
				case TOUCH_D:
					keyname = "down";
					break;
				case TOUCH_DL:
					keyname = "down-left";
					break;
				case TOUCH_L:
					keyname = "left";
					break;
				case TOUCH_UL:
					keyname = "up-left";
					break;
				default:
					break;
			}
			if (keyname)
					fprintf(stderr, "Touch %i: %s\n", input, keyname);
		}
	}
}
