// KERIPO MOD
#include "_mods.h"

/*
 *  iDoom - i_video.h
 *
 *  Copyright 2005 Benjamin Eriksson & Mattias Pierre.
 * 
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

// NOTE: This file has been substantially rewritten for the iPod, including
//       but not restrected to:
//
//       * Special adaption for the grayscale versions such as converting
//         to grayscale and dithering to simulate more colors.
//       * Trimming of code to speed everything up.
//
//       Theese changes has been made by Benjamin Eriksson and  Mattias Pierre.
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "doomtype.h"
#include "doomdef.h"

#include "blit.h"
#include "ipodhw.h"
#include "keyHandler.h"

// create 16 bit 5/6/5 format pixel from RGB triplet
#define RGB2PIXEL565(r,g,b) \
	((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

// Calculate the mean value for the three variables.
#define MEAN3VALUE(x,y,z) \
	((((x+y)>>1)+z)>>1)

// Floor value to nearest multiple of 4.
#define FLOOR4(a) \
	( (a) & ~0x3 )

// Width and height of the LCD-screen.
static int video_w, video_h;

// The current pallete.
static uint16_t * palette = 0;

// If FPSCOUNT is set, an estimation of FPS will be written to a logfile.
#ifdef FPSCOUNT
static int frameCount = 0;
FILE * fpsFile;
static struct timeval prevTime;
#endif // FPSCOUNT

// Scaling tables. TODO: Comment this.
static uint16_t * xtable  = 0;
static uint16_t * ytable1 = 0;
static uint16_t * ytable2 = 0;

// This is the buffer that will hold the frame to be blitted by the blitter.
static uint16_t * doomFB16bpp = 0;

// This struct will contain info about the current iPod hardware.
hw_info info;

/**
 * Shutdowns the graphic and frees some memory.
 * 
 * @author Mattias Pierre & Benjamin Eriksson
 */
void I_ShutdownGraphics(void)
{
	blitter_finish();
#ifdef FPSCOUNT
	fclose(fpsFile);
#endif // FPSCOUNT
	// Deallocating framebuffer..
	free(doomFB16bpp);
	
	// ...color palette...
	free(palette);
	free(xtable);
	free(ytable1);
	free(ytable2);
}

/**
 * Called at the beginning of each frame.
 * No longer used, should be removed later.
 * 
 * @author id software
 */
void I_StartFrame (void){}

/**
 * Called at the beginning of the main doom loop.
 * 
 * @author Mattias Pierre
 */
void I_StartTic (void)
{
	event_t event;
	int button;
	
	while ((button = getKey()) != -1)
		if (getEventFromButton(button, &event))
			D_PostEvent(&event);
}

/**
 * Called at the beginning of each frame.
 * No longer used, should be removed later.
 * 
 * @author id software
 */
void I_UpdateNoBlit (void){}

#ifdef FPSCOUNT

/**
 * Updates the 'frame per second' counter.
 * TODO: This Doesn't work very well. Why? I can't figure out... FIX!
 * 
 * @author Mattias Pierre & Benjamin Eriksson
 */
static void updateFPS(void)
{
	if ((frameCount++ % 100) == 0)
	{
		struct timeval newTime;
		int fps, diff;
		long sdiff, udiff;
		
		gettimeofday(&newTime, 0);
		sdiff = newTime.tv_sec  - prevTime.tv_sec;
		udiff = newTime.tv_usec - prevTime.tv_usec;
		
		if (udiff < 0)
		{
			udiff += 1000000;
			sdiff++;
		}
		
		fps  = frameCount * 1000000 / ((sdiff * 1000000) + udiff);
		
		fprintf(fpsFile, "FPS %d", fps);
		
		frameCount = 1;
		fflush(fpsFile);
		gettimeofday(&prevTime, 0);
	}
}
#endif // FPSCOUNT

/**
 * Writes whatever is in in the array 'screens' to the color display
 * in the correct resulution.
 * This function is _not_ called by the system, it calls @see I_FinishUpdate
 * instead.
 * 
 * @author Benjamin Eriksson & Mattias Pierre
 */
void I_FinishUpdate (void)
{
	uint8_t paletteIndex;
	int x, y;
	
	if(video_h >= SCREENHEIGHT)
	{
		for (y = 0; y < video_h; y++)
		{
			for (x = 0; x < video_w; x++)
			{
				paletteIndex = screens[0][((y*SCREENHEIGHT) / video_h) * SCREENWIDTH + x];
				doomFB16bpp[y * video_w + x] = palette[paletteIndex];
			}
		}
		
	} else {
		for (y = 0; y < video_h; y++)
		{
			for (x = 0; x < video_w; x++)
			{
				paletteIndex = screens[0][ytable1[y] +xtable[x]];
				doomFB16bpp[x + ytable2[y]] = palette[paletteIndex];
			}
		}
	}
	// Blit the framebuffer to the LCD.
	blitter_blit_fullscreen(doomFB16bpp);
	
#ifdef FPSCOUNT
	updateFPS();
#endif //FPSCOUNT
}

/**
 * Generates data tables that is used in the resize functions.
 * 
 * @author Benjamin Eriksson
 */
static void genscalexytable(void)
{
	// shall we use uint8_t instead of ints?
	int y = video_h-1;
	int x = video_w-1;
	int i = 1 + (x>y?x:y);
	xtable  = malloc(sizeof(int)*video_w );
	ytable1 = malloc(sizeof(int)*video_h );
	ytable2 = malloc(sizeof(int)*video_h );

	while(i--){
		if(y>=0){
			ytable1[y] = ((y*SCREENHEIGHT) / video_h) * SCREENWIDTH;
			ytable2[y] = y*video_w;
			y--;
		}
		if(x>=0){
			xtable[x] = (x*SCREENWIDTH) / video_w;
			x--;
		}
	}
}

/**
 * Copies the screen in the original resulution.
 * @param scr Where to save the copy.
 *
 * @author id software
 */
void I_ReadScreen (byte* scr)
{
	memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

/**
 * Copies the 256 element palette.
 * @param pal The palette to duplicate.
 * 
 * @author Mattias Pierre & Benjamin Eriksson
 */
void I_SetPalette (byte* pal) //pal = palette
{
	get_ipod_hw_info(&info);
	
	for (int i = 0; i < 256; ++i )
	{
		uint8_t r = gammatable[usegamma][*pal++];
		uint8_t g = gammatable[usegamma][*pal++];
		uint8_t b = gammatable[usegamma][*pal++];
			
		if(info.flags & LCD_TYPE_2) {
			// Flip bytes on it for lcd type 2
			unsigned int tmp = RGB2PIXEL565(r, g, b);
			palette[i] = ((tmp & 0xff00) >> 8) |
			             ((tmp & 0x00ff) << 8) ;
		} else {
			palette[i] = RGB2PIXEL565(r, g, b);
		}
	}
}

/**
 * Called before the main loop. It sets up the graphics, input handler etc.
 * 
 * @author Benjamin Eriksson & Mattias Pierre
 */
void I_InitGraphics(void)
{
	static int firsttime = 1;
	
	if (!firsttime)
		return;
	firsttime = 0;
	
#ifdef FPSCOUNT
	fpsFile = fopen("FPSCount", "w");
	gettimeofday(&prevTime, 0);
#endif // FPSCOUNT
	
	printf("iPod port by: Benjamin Eriksson & Mattias Pierre\n");
	printf("Modified by Keripo for ZeroSlackr\n");
	sleep(1);
	
	startKeyHandler();
	get_ipod_hw_info(&info);
	// KERIPO MOD
	//parseKeyMapFile("keys.key");
	parseKeyMapFile(KEYS_FILE);
	
	// NOTE: The blitter can only blit widths that are multiples of 4.
	// TODO: Fix this.
	video_w = FLOOR4(info.lcd_width);
	video_h = info.lcd_height;

	doomFB16bpp = calloc((video_w*video_h), sizeof(uint16_t));
	
	blitter_init();
	// Alloc memory for the palette
	// TODO: This could be allocated on the stack.
	palette = malloc( sizeof(uint16_t) * 256 );

	// Set up the screen displays
	screens[0] = malloc (SCREENWIDTH * SCREENHEIGHT * sizeof(uint8_t));
	
	if (screens[0] == NULL)
		I_Error("Couldn't allocate screen memory");
	
	genscalexytable();
}

