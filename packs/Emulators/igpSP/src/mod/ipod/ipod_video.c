/*
 * Last updated: Jun 20, 2008
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
 * Scaling code based on Zaphod's old code
 * Hotdog code based on hotdog demos (hdpong)
 * RGB565 code based on various resources
 * (see ipod_video.h)
 */

#include "ipod_common.h"
#include "ipod_video.h"

// From gpSP's common.h
// typedef unsigned short int u16;
// typedef unsigned long u32;

typedef void (*ipod_update_screen_type)(); 

extern u16 *screen;
extern uint32 WIDTH, HEIGHT;

int ipod_scale_type;
int ipod_smooth_type;
int IPOD_HW_VER, IPOD_LCD_TYPE;
uint32 IPOD_WIDTH, IPOD_HEIGHT;

uint16 *ipod_screen; //RGB565
uint8 *ipod_screen_mono; //Y'UV - Y only

static ipod_update_screen_type ipod_update_screen_funct = 0;
static int monochrome = -1;
static int scale_type_current = -1;
static int smooth_type_current = -1;
static int scale_x[320], scale_y[240]; // Max iPod screen dimensions (iPod video)
static int scale_x_offset, scale_y_offset;
static int scale_x_max, scale_y_max;

// Yes, a LOT, LOT of code duplication.
// Unfortunately no way of #define macro'ing them
// and doing checks inside the loops will make things slow ; /

// Normal, no bound checks
static void ipod_update_screen_default()
{
	int x, y, p_ipod, p_src;
	for (y = 0; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			ipod_screen[p_ipod] = screen[p_src];
		}
	}
}

// Four pixel blending
static void ipod_update_screen_default_smooth_1()
{
	int x, y, p_ipod, p_src;
	uint16 p1, p2, p3;
	for (y = 0; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
}

// Eight pixel blending
static void ipod_update_screen_default_smooth_2()
{
	int x, y, p_ipod, p_src;
	uint16 p1, p2, p3, p4, p5;
	// To stay in bounds
	for (y = 0; y < 2; y++) {
		for (x = 0; x < IPOD_WIDTH * 2; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
	for (y = 2; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src - 1]; // Pixel to the right
			p4 = screen[p_src + WIDTH]; // Pixel underneath
			p5 = screen[p_src - WIDTH]; // Pixel above
			ipod_screen[p_ipod] = blend_pixels_8_RGB565(p1, p2, p3, p4, p5);
		}
	}
}

// For 5G and photo (not sure if works on photo though)
static void ipod_update_screen_unscaled_big()
{
	int x, y, sx, sy, p_ipod, p_src;
	for (y = scale_y_offset, sy = 0; y <= scale_y_max; y++, sy++) {
		for (x = scale_x_offset, sx = 0; x <= scale_x_max; x++, sx++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[sx] + scale_y[sy];
			ipod_screen[p_ipod] = screen[p_src];
		}
	}
}

// Four pixel blending
static void ipod_update_screen_unscaled_big_smooth_1()
{
	int x, y, sx, sy, p_ipod, p_src;
	uint16 p1, p2, p3;
	for (y = scale_y_offset, sy = 0; y <= scale_y_max; y++, sy++) {
		for (x = scale_x_offset, sx = 0; x <= scale_x_max; x++, sx++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[sx] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
}

// Eight pixel blending
static void ipod_update_screen_unscaled_big_smooth_2()
{
	int x, y, sx, sy, p_ipod, p_src;
	uint16 p1, p2, p3, p4, p5;
	// To stay in bounds
	for (y = scale_y_offset, sy = 0; y < scale_y_offset + 2; y++, sy++) {
		for (x = scale_x_offset, sx = 0; x <= scale_x_max; x++, sx++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[sx] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p4 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
	for (y = scale_y_offset + 2, sy = 2; y <= scale_y_max; y++, sy++) {
		for (x = scale_x_offset, sx = 0; x <= scale_x_max; x++, sx++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[sx] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src - 1]; // Pixel to the right
			p4 = screen[p_src + WIDTH]; // Pixel underneath
			p5 = screen[p_src - WIDTH]; // Pixel above
			ipod_screen[p_ipod] = blend_pixels_8_RGB565(p1, p2, p3, p4, p5);
		}
	}
}

// Doesn't fill full screen
static void ipod_update_screen_width()
{
	int x, y, sy, p_ipod, p_src;
	for (y = scale_y_offset, sy = 0; y <= scale_y_max; y++, sy++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[sy];
			ipod_screen[p_ipod] = screen[p_src];
		}
	}
}

// Four pixel blending
static void ipod_update_screen_width_smooth_1()
{
	int x, y, sy, p_ipod, p_src;
	uint16 p1, p2, p3;
	for (y = scale_y_offset, sy = 0; y <= scale_y_max; y++, sy++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
}

// Eight pixel blending
static void ipod_update_screen_width_smooth_2()
{
	int x, y, sy, p_ipod, p_src;
	uint16 p1, p2, p3, p4, p5;
	// To stay in bounds
	for (y = scale_y_offset, sy = 0; y <= scale_y_offset + 2; y++, sy++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src + WIDTH]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
	for (y = scale_y_offset + 2, sy = 2; y <= scale_y_max; y++, sy++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[sy];
			p1 = screen[p_src]; // Pixel
			p2 = screen[p_src + 1]; // Pixel to the right
			p3 = screen[p_src - 1]; // Pixel to the right
			p4 = screen[p_src + WIDTH]; // Pixel underneath
			p5 = screen[p_src - WIDTH]; // Pixel above
			ipod_screen[p_ipod] = blend_pixels_8_RGB565(p1, p2, p3, p4, p5);
		}
	}
}

// Monochrome iPods - will be slightly slower due to extra conversion but
// I don't want to duplicate code just for this; besides, not many people use monochromes
static void ipod_update_screen_convert_to_mono()
{
	int i, max;
	max = IPOD_WIDTH * IPOD_HEIGHT;
	for (i = 0; i < max; i++) { // Linear - don't worry about co-ordinates
		ipod_screen_mono[i] = convert_pixel_RGB565_to_Y(ipod_screen[i]);
	}
}

void ipod_update_screen()
{
	ipod_update_screen_funct();
	if (monochrome) {
		ipod_update_screen_convert_to_mono();
		HD_LCD_Update(ipod_screen_mono, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
	} else {
		HD_LCD_Update(ipod_screen, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
	}
}

void ipod_clear_screen()
{
	int i, max;
	max = IPOD_WIDTH * IPOD_HEIGHT;
	for (i = 0; i < max; i++) { // Linear - don't worry about co-ordinates
		ipod_screen[i] = CLEAR;
	}
}

void ipod_update_scale_type()
{
	if (scale_type_current != ipod_scale_type ||
		smooth_type_current != ipod_smooth_type) {
		scale_type_current = ipod_scale_type;
		smooth_type_current = ipod_smooth_type;
		int i;
		switch (scale_type_current) {
			case 0: // Unscaled
				scale_x_offset = (IPOD_WIDTH - WIDTH) / 2;
				scale_y_offset = (IPOD_HEIGHT - HEIGHT) / 2;
				
				if (IPOD_HEIGHT > HEIGHT || IPOD_WIDTH > WIDTH) { // iPod video or photo
					scale_x_max = IPOD_WIDTH - scale_x_offset;
					scale_y_max = IPOD_HEIGHT - scale_y_offset;
					
					for (i = 0; i < IPOD_WIDTH; i++)
						scale_x[i] = i;
					for (i = 0; i < IPOD_HEIGHT; i++)
						scale_y[i] = i * WIDTH;
					
					switch(ipod_smooth_type) {
						case 0:
							ipod_update_screen_funct = ipod_update_screen_unscaled_big;
							break;
						case 1:
							ipod_update_screen_funct = ipod_update_screen_unscaled_big_smooth_1;
							break;
						case 2:
							ipod_update_screen_funct = ipod_update_screen_unscaled_big_smooth_2;
							break;
					}
				} else {
					scale_x_max = IGNORE;
					scale_y_max = IGNORE;
					
					for (i = 0; i < IPOD_WIDTH; i++)
						scale_x[i] = i - scale_x_offset; // - negative = positive
					for (i = 0; i < IPOD_HEIGHT; i++)
						scale_y[i] = (i - scale_y_offset) * WIDTH;
					
					switch(ipod_smooth_type) {
						case 0:
							ipod_update_screen_funct = ipod_update_screen_default;
							break;
						case 1:
							ipod_update_screen_funct = ipod_update_screen_default_smooth_1;
							break;
						case 2:
							ipod_update_screen_funct = ipod_update_screen_default_smooth_2;
							break;
					}
				}
				break;
			case 1: // Full-screen
				scale_x_offset = IGNORE;
				scale_y_offset = IGNORE;
				scale_x_max = IGNORE;
				scale_y_max = IGNORE;
				
				for (i = 0; i < IPOD_WIDTH; i++)
					scale_x[i] = i * WIDTH / IPOD_WIDTH;
				for (i = 0; i < IPOD_HEIGHT; i++)
					scale_y[i] = (i * HEIGHT / IPOD_HEIGHT) * WIDTH;
				
				switch(ipod_smooth_type) {
					case 0:
						ipod_update_screen_funct = ipod_update_screen_default;
						break;
					case 1:
						ipod_update_screen_funct = ipod_update_screen_default_smooth_1;
						break;
					case 2:
						ipod_update_screen_funct = ipod_update_screen_default_smooth_2;
						break;
				}
				break;
			case 2: // Scale to height - width gets stretched
				scale_x_offset = (IPOD_WIDTH - WIDTH * IPOD_HEIGHT / HEIGHT) / 2;
				scale_y_offset = IGNORE;
				scale_x_max = IGNORE;
				scale_y_max = IGNORE;
				
				for (i = 0; i < IPOD_WIDTH; i++)
					scale_x[i] = (i - scale_x_offset) * HEIGHT / IPOD_HEIGHT;
				for (i = 0; i < IPOD_HEIGHT; i++)
					scale_y[i] = (i * HEIGHT / IPOD_HEIGHT) * WIDTH;
				
				switch(ipod_smooth_type) {
					case 0:
						ipod_update_screen_funct = ipod_update_screen_default;
						break;
					case 1:
						ipod_update_screen_funct = ipod_update_screen_default_smooth_1;
						break;
					case 2:
						ipod_update_screen_funct = ipod_update_screen_default_smooth_2;
						break;
				}
				break;
			case 3: // Scale to width - height gets shortened
				scale_x_offset = IGNORE;
				scale_y_offset = (IPOD_HEIGHT - HEIGHT * IPOD_WIDTH / WIDTH) / 2;
				scale_x_max = IGNORE;
				scale_y_max = IPOD_HEIGHT - scale_y_offset;
				
				for (i = 0; i < IPOD_WIDTH; i++)
					scale_x[i] = i * WIDTH / IPOD_WIDTH;
				for (i = 0; i < IPOD_HEIGHT; i++)
					scale_y[i] = (i * WIDTH / IPOD_WIDTH) * WIDTH;
				
				switch(ipod_smooth_type) {
					case 0:
						ipod_update_screen_funct = ipod_update_screen_width;
						break;
					case 1:
						ipod_update_screen_funct = ipod_update_screen_width_smooth_1;
						break;
					case 2:
						ipod_update_screen_funct = ipod_update_screen_width_smooth_2;
						break;
				}
				break;
		}
		// Note that this sometimes screws up due to syncing issues
		// Screwy pixels get cleared when the menu is exited though
		ipod_clear_screen(CLEAR);
	}
}

void ipod_init_video()
{
	HD_LCD_Init();
	HD_LCD_GetInfo(&IPOD_HW_VER, &IPOD_WIDTH, &IPOD_HEIGHT, &IPOD_LCD_TYPE);
	
	ipod_screen = malloc(IPOD_WIDTH * IPOD_HEIGHT * 2);
	if (IPOD_LCD_TYPE == 2 || IPOD_LCD_TYPE == 3) { // monochromes (1-4G & minis)
		monochrome = 1;
		ipod_screen_mono = malloc(IPOD_WIDTH * IPOD_HEIGHT * 2);
	} else {
		monochrome = 0;
		ipod_screen_mono = NULL;
	}
	
	ipod_update_scale_type();
}

void ipod_exit_video()
{
	free(screen);
	free(ipod_screen);
	HD_LCD_Quit();
}
