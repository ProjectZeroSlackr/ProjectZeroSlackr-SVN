/*
 *  gamelib - ipodhw.h- Hardware probing routines.
 *
 *  Copyright 2005 Benjamin Eriksson & Mattias Pierre.
 * 
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the names of Mattias Pierre and Benjamin Eriksson nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 *  DAMAGE.
 */

#ifndef __IPODHW_H__
#define __IPODHW_H__

/* ===== Constant definitions ===== */

// Busy mask
#define IPOD_STD_LCD_BUSY_MASK  0x8000
#define IPOD_COL_LCD_BUSY_MASK  0x80000000
#define IPOD_NANO_LCD_BUSY_MASK IPOD_COL_LCD_BUSY_MASK
#define IPOD_VIDEO_LCD_BUSY_MASK IPOD_NANO_LCD_BUSY_MASK

// Screen Sizes
#define IPOD_STD_LCD_WIDTH    160
#define IPOD_STD_LCD_HEIGHT   128
#define IPOD_MINI_LCD_WIDTH   138
#define IPOD_MINI_LCD_HEIGHT  110
#define IPOD_COL_LCD_WIDTH    220
#define IPOD_COL_LCD_HEIGHT   176
#define IPOD_NANO_LCD_WIDTH   176
#define IPOD_NANO_LCD_HEIGHT  132
#define IPOD_VIDEO_LCD_WIDTH  320
#define IPOD_VIDEO_LCD_HEIGHT 240

// Other stuff
#define IPOD_PP5002_RTC      0xcf001110
#define IPOD_PP5020_RTC      0x60005010
#define IPOD_PP5002_LCD_BASE 0xc0001000
#define IPOD_PP5020_LCD_BASE 0x70003000
#define IPOD_COLOR_LCD_BASE  0x70008a0c
#define IPOD_NANO_LCD_BASE   IPOD_COLOR_LCD_BASE
#define IPOD_VIDEO_LCD_BASE  IPOD_NANO_LCD_BASE

#define LCD_CMD  0x8
#define LCD_DATA 0x10

/* ===== Flags ===== */

#define LCD_COLOR  0x00000001 // = 00000001_b
#define LCD_GRAY  ~LCD_COLOR
#define LCD_TYPE_2 0x00000002 // = 00000010_b
#define LCD_VIDEO  0x00000004 // = 00000100_b

typedef struct
{
	unsigned int version;
	unsigned int flags; /* display type etc. */
	unsigned int lcd_base;
	unsigned int lcd_busy_mask;
	unsigned int lcd_width;
	unsigned int lcd_height;
	unsigned int ipod_rtc;
	char *       ipod_type;
} hw_info;

void get_ipod_hw_info(hw_info * info);

#endif // __IPODHW_H__
