/*
 *  gamelib - blit.c - Blitter for the iPod.
 *  
 *	Based on videocop.c by Adam Johnston <agjohnst@uiuc.edu>
 *	and lcd-ipodvideo from the rockbox project www.rockbox.org
 *
 *  This file contains functions to manually controll the blitting to the iPod
 *  LCD-display. Has been tested to work with iPod 2G and iPod Mini.
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
 
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/file.h>
#include <linux/kd.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "blit.h"

#ifdef IPOD

#include "ipodhw.h"

#define outl(a,b) (*(volatile unsigned int *)  (b) = (a))
#define inl(a)    (*(volatile unsigned int *)  (a))
#define outw(a,b) (*(volatile unsigned short *)(b) = (a))
#define inw(a)    (*(volatile unsigned short *)(a))

#define RGB565TOGRAY(a)(((((((a) & 0xF800) >> 8) +       \
                           (((a) & 0x07e0) >> 3)) >> 1) + \
                           (((a) & 0x001f) << 3)) >> 1)


static unsigned int ipod_rtc      = 0x60005010;
static unsigned int lcd_base      = 0x70008a0c;
static unsigned int lcd_busy_mask = 0x80000000;

static unsigned int lcd_width  = 220;
static unsigned int lcd_height = 176;

//hardware identification number
unsigned int hw_version = -1;
int lcd_type;

static unsigned long flags = 0;

/**
 * Sets up some variables for different kinds of iPods.
 *
 * @author Benjamin Eriksson & Mattias Pierre
 */
static void blitter_setup(void)
{
	hw_info info;
	get_ipod_hw_info(&info);
	
	hw_version    = info.version;
	flags         = info.flags;
	lcd_base      = info.lcd_base;
	lcd_busy_mask = info.lcd_busy_mask;
	lcd_width     = info.lcd_width;
	lcd_height    = info.lcd_height;
	ipod_rtc      = info.ipod_rtc;
	
	lcd_type      = (flags & LCD_TYPE_2)?1:0;
}

/**
 * Get current usec counter.
 * 
 * @return The time in micro seconds
 */
static int blitter_timer_get_current(void)
{
	return inl(ipod_rtc);
}

/**
 * Checks if 'usecs' of microseconds has past since 'clock_start'.
 * 
 * @param clock_start The time in microseconds we want to compare to.
 * @param usecs       Minimum timediff.
 * 
 * @return Wherever the time diff is larger or not.
 */
static int blitter_timer_check(int clock_start, int usecs)
{
	unsigned long clock;
	clock = inl(ipod_rtc);
	
	return (clock - clock_start) >= usecs;
}

/**
 * Wait for LCD with timeout.  TODO: Comment me!
 */
static void blitter_lcd_wait_write(void)
{
	if ((inl(lcd_base) & lcd_busy_mask) != 0) {
		int start = blitter_timer_get_current();
			
		do {
			if ((inl(lcd_base) & lcd_busy_mask) == 0)
				break;
		} while (blitter_timer_check(start, 1000) == 0);
	}
}

/**
 * Send data to lcd. TODO: Comment me!
 */
static void blitter_lcd_send_data(int data_lo, int data_hi)
{
	blitter_lcd_wait_write();
	if (hw_version == 0x7) {
		outl((inl(0x70003000) & ~0x1f00000) | 0x1700000, 0x70003000);
		outl(data_hi | (data_lo << 8) | 0x760000, 0x70003008);
	}
	else {
		outl(data_lo, lcd_base + LCD_DATA);
		blitter_lcd_wait_write();
		outl(data_hi, lcd_base + LCD_DATA);
	}
}

/**
 * Prepare LCD to command. TODO: Comment me!
 */
static void blitter_lcd_prepare_cmd(int cmd)
{
	blitter_lcd_wait_write();
	if (hw_version == 0x7) {
		outl((inl(0x70003000) & ~0x1f00000) | 0x1700000, 0x70003000);
		outl(cmd | 0x740000, 0x70003008);
	}
	else {
		outl(0x0, lcd_base + LCD_CMD);
		blitter_lcd_wait_write();
		outl(cmd, lcd_base + LCD_CMD);
	}
}

/**
 * TODO: Comment me!
 */
static void lcd_send_lo(int v)
{
	blitter_lcd_wait_write();
	outl(v | 0x80000000, 0x70008a0c);
}

/**
 * TODO: Comment me!
 */

static void lcd_send_hi(int v)
{
	blitter_lcd_wait_write();
	outl(v | 0x81000000, 0x70008a0c);
}

/**
 * TODO: Comment me!
 */
static void blitter_lcd_cmd_data(int cmd, int data)
{
	if (lcd_type == 0)
	{
		lcd_send_lo(cmd);
		lcd_send_lo(data);
	}
	else
	{
		lcd_send_lo(0);
		lcd_send_lo(cmd);
		lcd_send_hi((data >> 8) & 0xff);
		lcd_send_hi( data       & 0xff);
	}
}

/**
 * TODO: Comment me!
 */
static inline void blitter_lcd_cmd_and_data(int cmd, int data_lo, int data_hi)
{
	blitter_lcd_prepare_cmd(cmd);
	blitter_lcd_send_data(data_lo, data_hi);
}


/**
 * Blitter optimized for ipod nano.
 * 
 */
static void blitter_ipod_update_nano(unsigned short * addr)
{
	
	while ((inl(0x70008a20) & 0x4000000) == 0);
	outl(0x0, 0x70008a24);
	
	blitter_lcd_cmd_data(0x44, 0xaf00);
	blitter_lcd_cmd_data(0x45, 0x8300);
	blitter_lcd_cmd_data(0x21, 0x0);
	
	lcd_send_lo(0x0);
	lcd_send_lo(0x22);
	
	outl(0x10000080, 0x70008a20);
	outl(0xC001B57F, 0x70008a24);
	outl(0x34000000, 0x70008a20);
	
	for (int i = 0; i < 11616; i++, addr += 2)
	{
		while ((inl(0x70008a20) & 0x1000000) == 0);
		outl(addr[0] | (addr[1] << 16), 0x70008b00);
	}

	while ((inl(0x70008a20) & 0x4000000) == 0);
	outl(0x0, 0x70008a24);
}

/**
 * Combined blitter for iPod nano, Photo and Color.
 *
 * @param addr fullscreen framebuffer to blit.
 */
void blitter_ipod_update_photo_working(unsigned short * addr)
{
	int sy, sx;
    int startY = sy = 0;
    int startX = sx = 0;
    int diff   = 0;
    int height = lcd_height;
    int width  = lcd_width;
    int rect1, rect2, rect3, rect4;
    
    
    // Calculate the drawing region.
    if (hw_version != 0x6)
    {
        rect1 = 0;
        rect2 = 0;
        rect3 = width  -1;
        rect4 = height -1;
    }
    else
    {
        rect1 = 0;
        rect2 = lcd_width - 1;
        rect3 = height - 1;
        rect4 = rect2 - width  + 1;
    }
    
    if (lcd_type == 0)
    {
        blitter_lcd_cmd_data(0x12, rect1 & 0xff);
        blitter_lcd_cmd_data(0x13, rect2 & 0xff);
        blitter_lcd_cmd_data(0x15, rect3 & 0xff);
        blitter_lcd_cmd_data(0x16, rect4 & 0xff);
    }
    else
    {
        if (rect3 < rect1)
        {
            int t;
            t = rect1;
            rect1 = rect3;
            rect3 = t;
        }
        
        if (rect4 < rect2)
        {
            int t;
            t = rect2;
            rect2 = rect4;
            rect4 = t;
        }
 
        /* max horiz << 8 | start horiz */
        blitter_lcd_cmd_data(0x44, (rect3 << 8) | rect1);
        /* max vert << 8 | start vert */
        blitter_lcd_cmd_data(0x45, (rect4 << 8) | rect2);
        
        
        /* position cursor (set AD0-AD15) */
        /* start vert << 8 | start horiz */
        blitter_lcd_cmd_data(0x21, (rect2 << 8) | rect1);
        
        /* start drawing */
        lcd_send_lo(0x0);
        lcd_send_lo(0x22);
    }
    
    addr += startY * 220 + startX;

    while (height > 0) {
        int x, y;
        int h, pixels_to_write;

        pixels_to_write = (width * height) * 2;

        /* calculate how much we can do in one go */
        h = height;
        if (pixels_to_write > 64000) {
            h = (64000/2) / width;
            pixels_to_write = (width * h) * 2;
        }

        outl(0x10000080, 0x70008a20);
        outl((pixels_to_write - 1) | 0xc0010000, 0x70008a24);
        outl(0x34000000, 0x70008a20);

        /* for each row */
        for (y = 0x0; y < h; y++)
        {
            /* for each column */
            for (x = 0; x < width; x += 2)
            {
                unsigned two_pixels;
                
                two_pixels = addr[0] | (addr[1] << 16);
                addr += 2;
                
                while ((inl(0x70008a20) & 0x1000000) == 0);
                
                /*if (lcd_type != 0)
                {
                    unsigned t = (two_pixels & ~0xFF0000) >> 8;
                    two_pixels = two_pixels & ~0xFF00;
                    two_pixels = t | (two_pixels << 8);
                }*/

                /* output 2 pixels */
                outl(two_pixels, 0x70008b00);
            }

            addr += diff;
            //addr += lcd_width - width;
                }

        while ((inl(0x70008a20) & 0x4000000) == 0);

        outl(0x0, 0x70008a24);

        height = height - h;
    }
}



/*void blitter_ipod_update_color(unsigned short * addr)
{
	int height = 176;
	
	blitter_lcd_cmd_data(0x44, 44800);
	blitter_lcd_cmd_data(0x45, 219);
	blitter_lcd_cmd_data(0x21, 56064);
	
	lcd_send_lo(0x0);
	lcd_send_lo(0x22);
	
	while (height > 0) {
		int x, y;
		int h, pixels_to_write;

		pixels_to_write = (220 * height) << 1;

		// calculate how much we can do in one go
		if (pixels_to_write > 64000) {
			h = 145;
			pixels_to_write = 63800;
		} else {
			h = 176;
		}

		outl(0x10000080, 0x70008a20);
		outl((pixels_to_write - 1) | 0xc0010000, 0x70008a24);
		outl(0x34000000, 0x70008a20);

		for (y = 0; y < h; y++){
			for (x = 0; x < 220; x += 2) {
				while ((inl(0x70008a20) & 0x1000000) == 0);
				
				outl(addr[0] | (addr[1] << 16), 0x70008b00);
				addr += 2;
			}
		}
		while ((inl(0x70008a20) & 0x4000000) == 0);
		outl(0x0, 0x70008a24);
		height = height - h;
	}
}*/
void blitter_ipod_update_photo(unsigned short * addr)
{
	if(lcd_type == 0) {
		blitter_lcd_cmd_data(0x12,   0);
		blitter_lcd_cmd_data(0x13, 219);
		blitter_lcd_cmd_data(0x15, 175);
		blitter_lcd_cmd_data(0x16,   0);
	} else {
		blitter_lcd_cmd_data(0x44, 44800);
		blitter_lcd_cmd_data(0x45, 219);
		blitter_lcd_cmd_data(0x21, 56064);
		
		lcd_send_lo(0x0);
		lcd_send_lo(0x22);
	}
	
	outl(0x10000080, 0x70008a20);
	outl(0xc001f937, 0x70008a24);
	outl(0x34000000, 0x70008a20);

	for (int i=0; i<31900; i++, addr+=2){
		while ((inl(0x70008a20) & 0x1000000) == 0);
		outl(addr[0] | (addr[1] << 16), 0x70008b00);
	}
	
	while ((inl(0x70008a20) & 0x4000000) == 0);
	outl(0x0, 0x70008a24);
	
	// second wave
	outl(0x10000080, 0x70008a20);
	outl(0xc0013547, 0x70008a24);
	outl(0x34000000, 0x70008a20);
	
	for(int i=0; i<6820; i++, addr+=2) {
		while ((inl(0x70008a20) & 0x1000000) == 0);
		outl(addr[0] | (addr[1] << 16), 0x70008b00);
	}
	
	while ((inl(0x70008a20) & 0x4000000) == 0);
	outl(0x0, 0x70008a24);
}

/**
 * Blits the data pointed to by 'addr' on the LCD in the rectangle
 * defined by [sx, sy, mx, my]. The data pointed to by 'addr' should consist of
 * bytes that store 4 pixels per byte, 2 bits per pixels.
 *
 * addr - A pointer to the bitmap data to be blitted on the LCD.
 * sx   - The start x-coord of the blit-rect.
 * sy   - The start y-coord of the blit-rect.
 * mx   - The end x-coord of the blit-rect.
 * my   - The end y-coord of the blit-rect.
 */
void blitter_blit_buffer_2bpp(unsigned char * img_data, int sx, int sy, int mx, int my)
{
	unsigned short  y;
	unsigned short  cursor_pos;
	unsigned int    width;
	unsigned short  diff = 0;
	
	// Truncate the height of the data if it falls outside the LCD-area.
	if (my > lcd_height)
		my = lcd_height;

	// Truncate the width of the data if it falls outside the LCD-area.
	// Save the amount of data being cropped so one can skip that redundant
	// information when blitting.
	if (mx > lcd_width)
	{
		diff = (lcd_width - mx) / 4;
		mx   = lcd_width;
	}
	
	// The width of the blit-rect in bytes.
	width = (mx - sx) / 8; 
	
	// Set the cursor position to where the blit-rect starts.
	// NOTE: 0x20 = 32, 32*4 = 256.
	cursor_pos = sx + (sy * 0x20);
	
	// Blit the data to the screen. This is done, as one would expect, first
	// horizontally, then vertically. 
	for (y = sy; y <= my; y++)
	{
		unsigned char x;
		blitter_lcd_cmd_and_data(0x11, cursor_pos >> 8, cursor_pos & 0xff);
		blitter_lcd_prepare_cmd(0x12);
		
		// This is the horizontal plot.
		for (x = 0; x < width; x++)
		{
			// Send two bytes at once (?).
			blitter_lcd_send_data(*(img_data+1), *img_data);
			img_data += 2;
		}
		
		// Skip any data that got truncated before.
		img_data += diff;
		
		// Skip to the next row.
		cursor_pos += 0x20;
	}
}

uint8_t  * fbdith;

/**
 * Goes into graphics mode and sets up the blitter.
 *
 * @author Benjamin Eriksson & Mattias Pierre
 */
void blitter_init(void)
{
	int fd = open("/dev/console", O_NONBLOCK);
	
	if (fd < 0)
	{
		fprintf(stderr, "Graphics mode could not be set!\n");
		exit(-1);
	}
	
	ioctl(fd, KDSETMODE, KD_GRAPHICS);
	close(fd);
	
	blitter_setup();

	switch(hw_version) {
		case 0xb: case 0xc: case 0x6://color
			break;
		default: //grayscale
			fbdith = malloc((lcd_width*lcd_height)>>2);
	}
	//could it be someting else?
	lcd_width = lcd_width & ~0x1;
}

uint8_t col16bppTo2bpp(uint16_t col, uint8_t dith)
{
	uint8_t gray = RGB565TOGRAY(col);
	
	     if (gray <  21) return 3;
	else if (gray <  42) return 3 - dith; // Dithered (3 | 2)
	else if (gray <  63) return 2;
	else if (gray <  84) return 2 - dith; // Dithered (2 | 1)
	else if (gray < 105) return 1;
	else if (gray < 126) return 1 - dith; // Dithered (1 | 0)
	else                 return 0;
}

/**
 * Dithers an area from fb to fbdith;
 */
void blitter_blit_2bpp_dither(void* fb, int sx, int sy, int mx, int my)
{
	unsigned short  y;
	unsigned short  cursor_pos;
	unsigned int    width;
	unsigned short  diff = 0;
	
	// Truncate the height of the data if it falls outside the LCD-area.
	if (my > lcd_height)
		my = lcd_height;

	// Truncate the width of the data if it falls outside the LCD-area.
	// Save the amount of data being cropped so one can skip that redundant
	// information when blitting.
	if (mx > lcd_width)
	{
		diff = (lcd_width - mx) / 4;
		mx   = lcd_width;
	}
	
	// The width of the blit-rect in bytes.
	width = (mx - sx); 
	
	// Set the cursor position to where the blit-rect starts.
	// NOTE: 0x20 = 32, 32*4 = 256
	cursor_pos = sx + (sy * 0x20);
	
	// Blit the data to the screen. This is done, as one would expect, first
	// horizontally, then vertically. 
	for (y = sy; y <= my; y++)
	{
		unsigned char x;
		blitter_lcd_cmd_and_data(0x11, cursor_pos >> 8, cursor_pos & 0xff);
		blitter_lcd_prepare_cmd(0x12);
		
		// This is the horizontal plot.
		for (x = sx; x < width; x+=8)
		{
			// Declare variables in the innermost scope (doctors orders).
			uint16_t pos     = x + width * y;
			uint8_t  pxByte1 = 0;
			uint8_t  pxByte2 = 0;
			uint8_t  dith    = (y & 1);
			
			pxByte1 |= col16bppTo2bpp(((uint16_t*)fb)[pos  ], dith)   ; // First pixel
			pxByte1 |= col16bppTo2bpp(((uint16_t*)fb)[pos+1],!dith)<<2; // Second pixel
			pxByte1 |= col16bppTo2bpp(((uint16_t*)fb)[pos+2], dith)<<4; // Third pixel
			pxByte1 |= col16bppTo2bpp(((uint16_t*)fb)[pos+3],!dith)<<6; // Fourth pixel
			
			pxByte2 |= col16bppTo2bpp(((uint16_t*)fb)[pos+4], dith)   ; // Fifth pixel
			pxByte2 |= col16bppTo2bpp(((uint16_t*)fb)[pos+5],!dith)<<2; // Sixth pixel
			pxByte2 |= col16bppTo2bpp(((uint16_t*)fb)[pos+6], dith)<<4; // Guess what?
			pxByte2 |= col16bppTo2bpp(((uint16_t*)fb)[pos+7],!dith)<<6; // ..... 
			
			// Send eight pixels to the LCD.

			// Send two bytes at once.
			blitter_lcd_send_data(pxByte2, pxByte1);
		}
		
		// Skip to the next row.
		cursor_pos += 0x20;
	}
}

/**
 * blitter_bcm_write32 - writes 32 bits to the broadcom chip.
 *
 * @param address where to write the 32 bits
 * @param value   data to write
 */
static inline void blitter_bcm_write32(unsigned address, unsigned value)
{
	/* write out destination address as two 16bit values */
	outw(address, 0x30010000);
	outw((address >> 16), 0x30010000);

	/* wait for it to be write ready */
	while ((inw(0x30030000) & 0x2) == 0);

	/* write out the value low 16, high 16 */
	outw(value, 0x30000000);
	outw((value >> 16), 0x30000000);
}

/**
 * blitter_bcm_read32 - Read 32 bits from the broadcom chip.
 *
 * @param  address where to read the data
 * @return         the data
 */
static unsigned blitter_bcm_read32(unsigned address) {
	while ((inw(0x30020000) & 1) == 0);

	/* write out destination address as two 16bit values */
	outw(address, 0x30020000);
	outw((address >> 16), 0x30020000);

	/* wait for it to be read ready */
	while ((inw(0x30030000) & 0x10) == 0);

	/* read the value */
	return inw(0x30000000) | inw(0x30000000) << 16;
}

void blitter_ipod_update_video(unsigned short * addr)
{
	unsigned int data;
	static int finishup_needed = 0;
	
	if(finishup_needed) {
		do {
			/* This function takes about 14ms to execute - so we yield() */
			//TODO: fix this stuff
			//yield();
			data = blitter_bcm_read32(0x1F8);
		} while (data == 0xFFFA0005 || data == 0xFFFF);

		blitter_bcm_read32(0x1FC);
	} else {
		finishup_needed = 1;
	}
	
	blitter_bcm_write32(0x1F8, 0xFFFA0005);
	blitter_bcm_write32(0xE0000, 0x34);
	blitter_bcm_write32(0xE0004, 0);
	blitter_bcm_write32(0xE0008, 0);
	blitter_bcm_write32(0xE000C, 319);
	blitter_bcm_write32(0xE0010, 239);
	blitter_bcm_write32(0xE0014, 153600);
	blitter_bcm_write32(0xE0018, 153600);
	blitter_bcm_write32(0xE001C, 0);

	outw(0x20, 0x30010000);
	outw(0x0e, 0x30010000);

	while ((inw(0x30030000) & 0x2) == 0);

	register int count = 76800;
	while(count--) {
//    for(int i=0; i<240; i++) {
//        for(int j=0; j<320; j+=2) {
			outw(*(addr++), 0x30000000);
			outw(*(addr++), 0x30000000);
//        }
	}
	outw(0x31, 0x30030000); 
	blitter_bcm_read32(0x1FC);
}

void blitter_blit_fullscreen( void* fb )
{
	switch (hw_version)
	{
		//Color iPods
		case 0xb: //Video
			blitter_ipod_update_video(fb); //assumes it's a fullscreen refresh
			break;
		case 0xc://Nano
			blitter_ipod_update_nano(fb);
			break;
		case 0x6://Photo/Color
			blitter_ipod_update_photo_working(fb);
			break;
		//Grayscale iPods
		default:
			blitter_blit_partial( fb, 0, 0, lcd_width, lcd_height );
	}
}

/**
 * TODO: we don't need to update small parts, so this should be removed.
 * 
 * @author Benjamin Eriksson & Mattias Pierre
 */
void blitter_blit_partial(void * fb, uint16_t startx, uint16_t starty,
                                     uint16_t width,  uint16_t height)
{
	switch (hw_version)
	{
		//Color iPods
		case 0xb: //Video
			//removed
			break;
		case 0xc://Nano
		case 0x6://Photo/Color
			//removed
			break;
		//Grayscale iPods
		default:
			blitter_blit_2bpp_dither(fb, startx, starty, width, height);
	}
}


/**
 * Goes back to text-mode.
 * 
 * @author Benjamin Eriksson & Mattias Pierre
 */
void blitter_finish(void)
{
/*	switch(hw_version)
	{
		case 0xc:
			//TODO: check if this is nessesary
			while ((inl(0x70008a20) & 0x4000000) == 0);
			outl(0x0, 0x70008a24);
			break;
	}*/
	int fd = open("/dev/console", O_NONBLOCK);	
	
	if (fd < 0)
	{
		fprintf(stderr, "Text mode could not be set!\n");
		exit(-1);
	}
	
	ioctl(fd, KDSETMODE, KD_TEXT);
	close(fd);
}

#endif


