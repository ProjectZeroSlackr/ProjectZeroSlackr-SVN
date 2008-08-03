/*
 * Last updated: Aug 1, 2008
 * ~Keripo
 *
 * atari_ipod_hotdog.h - header for iPodLinux specific, optimized port code
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

#include "config.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <linux/kd.h>
#include <sys/ioctl.h>

/* Atari800 includes */
#include "input.h"
#include "colours.h"
#include "monitor.h"
#include "platform.h"
#include "ui.h"
#include "screen.h"
#include "pokeysnd.h"
#include "gtia.h"
#include "antic.h"
#include "devices.h"
#include "cpu.h"
#include "memory.h"
#include "pia.h"
#include "log.h"
#include "util.h"

// Pixel manipulation
#define get_R_from_RGB565(p) \
	(((p) & 0xf800) >> 8)

#define get_G_from_RGB565(p) \
	(((p) & 0x07e0) >> 3)

#define get_B_from_RGB565(p) \
	(((p) & 0x001f) << 3)

#define RGB565(r, g, b) \
	((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) << 0)

// Blend eight pixel values - p1 gets weighted four times
#define get_avg_R_from_8_RGB565(p1, p2, p3, p4, p5) \
	(((get_R_from_RGB565(p1) << 2) + \
		get_R_from_RGB565(p2) + get_R_from_RGB565(p3) + get_R_from_RGB565(p4) + get_R_from_RGB565(p5) \
	) >> 3)

#define get_avg_G_from_8_RGB565(p1, p2, p3, p4, p5) \
	(((get_G_from_RGB565(p1) << 2) + \
		get_G_from_RGB565(p2) + get_G_from_RGB565(p3) + get_G_from_RGB565(p4) + get_G_from_RGB565(p5) \
	) >> 3)

#define get_avg_B_from_8_RGB565(p1, p2, p3, p4, p5) \
	(((get_B_from_RGB565(p1) << 2) + \
		get_B_from_RGB565(p2) + get_B_from_RGB565(p3) + get_B_from_RGB565(p4) + get_B_from_RGB565(p5) \
	) >> 3)

#define blend_pixels_8_RGB565(p1, p2, p3, p4, p5) \
	RGB565( \
		get_avg_R_from_8_RGB565(p1, p2, p3, p4, p5), \
		get_avg_G_from_8_RGB565(p1, p2, p3, p4, p5), \
		get_avg_B_from_8_RGB565(p1, p2, p3, p4, p5) \
	)

// Input

#define KEY_MENU	50 // Up
#define KEY_PLAY	32 // Down
#define KEY_REWIND	17 // Left
#define KEY_FORWARD	33 // Right
#define KEY_ACTION	28 // Select
#define KEY_HOLD	35 // Exit
#define SCROLL_L	38 // Counter-clockwise
#define SCROLL_R	19 // Clockwise

#define TOUCH_U		 0 // North
#define TOUCH_UR	 1 // North-East
#define TOUCH_R		 2 // East
#define TOUCH_DR	 3 // South-East
#define TOUCH_D		 4 // South
#define TOUCH_DL	 5 // South-West
#define TOUCH_L		 6 // West
#define TOUCH_UL	 7 // North-West

#define KEY_NULL	-1 // No key event
#define TOUCH_NULL	-1 // No wheel touch event

#define KEYCODE(a)	(a & 0x7f) // Use to get keycode of scancode.
#define KEYSTATE(a)	(a & 0x80) // Check if key is pressed or lifted

// COP

#define outl(a, b) \
	(*(volatile unsigned int *)(b) = (a)) \

#define inl(a) \
	(*(volatile unsigned int *)(a)) \


#define COP_HANDLER			0x4001501C
#define COP_STATUS			0x40015020
#define COP_CONTROL			0x60007004

#define COP_RUNNING			(1 << 0)
#define COP_LINE_REQ		(1 << 1)
#define COP_LINE_REQ_CLEAR	(2 << 1)

#define ipod_cop_clear_frameready() \
	outl(inl(COP_STATUS) &~ COP_LINE_REQ_CLEAR, COP_STATUS)

void ipod_cop_update_screen() {
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}

// Scaling

#define FULLSCREEN	0
#define SCALED		1
#define UNCROPPED	2

// hotdog

extern void HD_LCD_Init();
extern void HD_LCD_GetInfo (int *hw_ver, int *lcd_width, int *lcd_height, int *lcd_type);
extern void HD_LCD_Update (void *fb, int x, int y, int w, int h);
extern void HD_LCD_Quit();
