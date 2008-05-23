/*
 * Last updated: May 22, 2008
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

// FIXME: Current screen blitting code does NOT work on older generations
// Re-write the entire ipod_screen part to use hd_surface/hotdog stuff
// (May also be able to use hotdog stuff to to scaling at expense of speed)

#include "ipod_common.h"
#include "hotdog.h"

// typedef unsigned short int u16;
// typedef unsigned long u32;

#define CLEAR	0x000000 // Black

extern void HD_LCD_Init();
extern void HD_LCD_Quit();
extern void HD_LCD_GetInfo (int *hw_ver, int *lcd_width, int *lcd_height, int *lcd_type);
extern void HD_LCD_Update (void *fb, int x, int y, int w, int h);

extern u16 *screen;
extern uint32 WIDTH, HEIGHT;

u16 *ipod_screen;
uint32 IPOD_WIDTH, IPOD_HEIGHT;

int ipod_scale_type;
static int scale_type_current = -1;
static int scale_x[320]; // iPod video dimensions
static int scale_y[240];

// For scale to width
static u32 MAX_PIXEL;
static u32 num, den;

void ipod_update_scale_type()
{
	if (scale_type_current != ipod_scale_type) {
		scale_type_current = ipod_scale_type;
		int i;
		switch (scale_type_current) {
			case 0: // Unscaled
				for (i = 0; i < IPOD_WIDTH; ipod_cop_operation(i++))
					ipod_cop_operation(scale_x[i] = i);
				for (i = 0; i < IPOD_HEIGHT; ipod_cop_operation(i++))
					ipod_cop_operation(scale_y[i] = i * WIDTH);
				break;		
			case 1: // Full-screen
				for (i = 0; i < IPOD_WIDTH; ipod_cop_operation(i++))
					ipod_cop_operation(scale_x[i] = i * WIDTH / IPOD_WIDTH);
				for (i = 0; i < IPOD_HEIGHT; ipod_cop_operation(i++))
					ipod_cop_operation(scale_y[i] = (i * HEIGHT / IPOD_HEIGHT) * WIDTH);
				break;
			case 2: // Scale to height - width gets stretched
				for (i = 0; i < IPOD_WIDTH; ipod_cop_operation(i++))
					ipod_cop_operation(scale_x[i] = i * num / den);
				for (i = 0; i < IPOD_HEIGHT; ipod_cop_operation(i++))
					ipod_cop_operation(scale_y[i] = (i * HEIGHT / IPOD_HEIGHT) * WIDTH);
				break;
			case 3: // Scale to width - height gets shortened (slightly faster benchmark than others due to using clear pixel)
				for (i = 0; i < IPOD_WIDTH; ipod_cop_operation(i++))
					ipod_cop_operation(scale_x[i] = i * WIDTH / IPOD_WIDTH);
				for (i = 0; i < IPOD_HEIGHT; ipod_cop_operation(i++))
					ipod_cop_operation(scale_y[i] = (i * WIDTH / IPOD_WIDTH) * WIDTH);
				break;
		}
	}
}

void ipod_clear_screen(u16 color)
{
	u16 *buffer_screen;
	ipod_cop_operation(buffer_screen = screen);
	int x, y;
	for (y = 0; y < HEIGHT; ipod_cop_operation(y++)) {
		for (x = 0; x < WIDTH; ipod_cop_operation(x++)) {
			ipod_cop_operation(*buffer_screen = color);
			ipod_cop_operation(buffer_screen++);
		}	
	}
}

void ipod_update_screen()
{
	int x, y;
	u32 p;
	u16 *buffer_screen;
	ipod_cop_operation(buffer_screen = ipod_screen);
	
	if (scale_type_current == 3) { // Make sure not to go outside screen boundaries
		for (y = 0; y < IPOD_HEIGHT; ipod_cop_operation(y++)) {
			for (x = 0; x < IPOD_WIDTH; ipod_cop_operation(x++)) {
				ipod_cop_operation(p = scale_x[x] + scale_y[y]);
				if (p < MAX_PIXEL) {
					ipod_cop_operation(*buffer_screen = screen[p]);
				} else {
					ipod_cop_operation(*buffer_screen = CLEAR);
				}
				ipod_cop_operation(buffer_screen++);
			}	
		}
	} else {
		for (y = 0; y < IPOD_HEIGHT; ipod_cop_operation(y++)) {
			for (x = 0; x < IPOD_WIDTH; ipod_cop_operation(x++)) {
				ipod_cop_operation(p = scale_x[x] + scale_y[y]);
				ipod_cop_operation(*buffer_screen = screen[p]);
				ipod_cop_operation(buffer_screen++);
			}
		}
	}
	HD_LCD_Update(ipod_screen, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
}

void ipod_init_video()
{
	HD_LCD_Init();
	HD_LCD_GetInfo (0, &IPOD_WIDTH, &IPOD_HEIGHT, 0);
	ipod_screen = (uint16 *)(malloc(IPOD_WIDTH * IPOD_HEIGHT * 2));
	
	// For scale to width
	ipod_cop_operation(
		MAX_PIXEL =	(IPOD_WIDTH * WIDTH / IPOD_WIDTH)
		+ ((IPOD_HEIGHT * HEIGHT / IPOD_HEIGHT) * WIDTH) - WIDTH
		);
	ipod_cop_operation(num = WIDTH * HEIGHT * IPOD_WIDTH);
	ipod_cop_operation(den = IPOD_WIDTH * IPOD_HEIGHT * WIDTH);
	
	ipod_scale_type = 3; // Default scale width - place this into some config file
	ipod_update_scale_type();
}

void ipod_exit_video()
{
	HD_LCD_Quit();
}

