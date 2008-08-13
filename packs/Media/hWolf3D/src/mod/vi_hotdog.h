/*
 * Last updated: Aug 11, 2008
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

#include "hotdog.h"
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Pixel code

#define get_R_from_RGB565(p) \
	(((p) & 0xf800) >> 8)

#define get_G_from_RGB565(p) \
	(((p) & 0x07e0) >> 3)

#define get_B_from_RGB565(p) \
	(((p) & 0x001f) << 3)

#define RGB565(r, g, b) \
	((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) << 0)

#define get_avg_R_from_4_RGB565(p1, p2, p3) \
	(((get_R_from_RGB565(p1) << 1) + get_R_from_RGB565(p2) + get_R_from_RGB565(p3)) >> 2)

#define get_avg_G_from_4_RGB565(p1, p2, p3) \
	(((get_G_from_RGB565(p1) << 1) + get_G_from_RGB565(p2) + get_G_from_RGB565(p3)) >> 2)

#define get_avg_B_from_4_RGB565(p1, p2, p3) \
	(((get_B_from_RGB565(p1) << 1) + get_B_from_RGB565(p2) + get_B_from_RGB565(p3)) >> 2)

#define blend_pixels_4_RGB565(p1, p2, p3) \
	RGB565( \
		get_avg_R_from_4_RGB565(p1, p2, p3), \
		get_avg_G_from_4_RGB565(p1, p2, p3), \
		get_avg_B_from_4_RGB565(p1, p2, p3) \
	)

// Input code

#define KEY_MENU	50 // Up
#define KEY_PLAY	32 // Down
#define KEY_REWIND	17 // Left
#define KEY_FORWARD	33 // Right
#define KEY_ACTION	28 // Select
#define KEY_HOLD	35 // Exit
#define SCROLL_L	38 // Counter-clockwise
#define SCROLL_R	19 // Clockwise

#define KEY_NULL	-1 // No key event
#define KEYCODE(a)	(a & 0x7f) // Use to get keycode of scancode.
#define KEYSTATE(a)	(a & 0x80) // Check if key is pressed or lifted

#define outl(a, b) \
	(*(volatile unsigned int *)(b) = (a))

#define inl(a) \
	(*(volatile unsigned int *)(a))
