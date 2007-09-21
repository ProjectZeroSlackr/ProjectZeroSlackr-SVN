/*
 *  gamelib - ipodhw.c - Hardware probing routines.
 *
 *  This file contains functions to find out the properties of the iPod
 *  currently in use, such as the resolution.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ipodhw.h"

static int version = -1;

/**
 * Finds the hw-ID for the current iPod.
 *
 * 10000: iPod 1G
 * 20000: iPod 2G
 * 30000: iPod 3G
 * 40000: iPod Mini 1G
 * 50000: iPod 4G
 * 50013: iPod 4G
 * 60000: iPod Photo / Color
 * 60005: iPod Photo / Color Rev.2
 * 70000: iPod Mini 2G
 * c0005: iPod Nano
 * ?????: iPod Video
 *
 * NOTE: These numbers are subject to change. This is not a complete list.
 */
static int get_ipod_hw_type(void)
{
	if (version < 0)
	{
		int i;
		char cpuinfo[512];
		char * ptr;
		FILE * file;

		if ((file = fopen("/proc/cpuinfo", "r")) != NULL)
		{
			while (fgets(cpuinfo, sizeof(cpuinfo), file) != NULL)
				if (strncmp(cpuinfo, "Revision", 8) == 0)
					break;
			
			fclose(file);
		}
		else
		{
			return 0;
		}
		
		for (i = 0; !isspace(cpuinfo[i]); i++);
		for (; isspace(cpuinfo[i]); i++);
		
		ptr     = cpuinfo + i + 2;
		version = strtol(ptr, NULL, 16);
	}
	
	return version;
}

/**
 * Finds out the hardware specs. of the current iPod.
 */
void get_ipod_hw_info(hw_info * info)
{
	info->flags = 0;
	switch (info->version = (get_ipod_hw_type() >> 16))
	{
		case 0xb: /* iPod Video */
			info->lcd_base      = IPOD_VIDEO_LCD_BASE;
			info->lcd_busy_mask = IPOD_VIDEO_LCD_BUSY_MASK;
			info->lcd_width     = IPOD_VIDEO_LCD_WIDTH;
			info->lcd_height    = IPOD_VIDEO_LCD_HEIGHT;
			info->ipod_rtc      = IPOD_PP5020_RTC;
			info->ipod_type     = "iPod Video";
			info->flags        |= LCD_COLOR | LCD_VIDEO;
			
			break;
		
		case 0xc: /* iPod Nano */
			info->lcd_base      = IPOD_NANO_LCD_BASE;
			info->lcd_busy_mask = IPOD_NANO_LCD_BUSY_MASK;
			info->lcd_width     = IPOD_NANO_LCD_WIDTH;
			info->lcd_height    = IPOD_NANO_LCD_HEIGHT;
			info->ipod_rtc      = IPOD_PP5020_RTC;
			info->ipod_type     = "iPod Nano";
			info->flags        |= LCD_COLOR | LCD_TYPE_2;
			
			
			break;
		
		case 0x6: /* iPod Photo / Color */
			info->lcd_base      = IPOD_COLOR_LCD_BASE;
			info->lcd_busy_mask = IPOD_COL_LCD_BUSY_MASK;
			info->lcd_width     = IPOD_COL_LCD_WIDTH;
			info->lcd_height    = IPOD_COL_LCD_HEIGHT;
			info->ipod_rtc      = IPOD_PP5020_RTC;
			info->ipod_type     = "iPod Color";
			info->flags        |= LCD_COLOR;
			
			if(get_ipod_hw_type() == 0x60000)
				info->flags &= ~LCD_TYPE_2; // it's type 1, remove the flag
			else
			{
				int gpio_a01 = (inl(0x6000D030) & 0x02) >> 1;
				int gpio_a04 = (inl(0x6000D030) & 0x10) >> 4;
				
				if (((gpio_a01 << 1) | gpio_a04) == 0 ||
				    ((gpio_a01 << 1) | gpio_a04) == 2)
				{
					info->flags &= ~LCD_TYPE_2; // it's type 1, remove the flag
				} else {
					info->flags |= LCD_TYPE_2; // it's type 2
				}
			}

			break;
		
		case 0x5: /* iPod 4G (gray)*/
			info->lcd_base      = IPOD_PP5020_LCD_BASE;
			info->lcd_busy_mask = IPOD_STD_LCD_BUSY_MASK;
			info->lcd_width     = IPOD_STD_LCD_WIDTH;
			info->lcd_height    = IPOD_STD_LCD_HEIGHT;
			info->ipod_rtc      = IPOD_PP5020_RTC;
			info->ipod_type     = "iPod 4G";
			info->flags        &= ~LCD_COLOR;
			break;
		 
		case 0x7: /* iPod Mini 2G */
		
		case 0x4: /* iPod Mini 1G */
			info->lcd_width     = IPOD_MINI_LCD_WIDTH;
			info->lcd_height    = IPOD_MINI_LCD_HEIGHT;
			info->lcd_base      = IPOD_PP5020_LCD_BASE;
			info->lcd_busy_mask = IPOD_STD_LCD_BUSY_MASK;
			info->ipod_rtc      = IPOD_PP5020_RTC;
			info->ipod_type     = "iPod Mini";
			info->flags        &= ~LCD_COLOR;
			break;
		
		case 0x3: /* iPod 3G */
		
		case 0x2: /* iPod 2G */
		
		case 0x1: /* iPod 1G */
			info->lcd_width     = IPOD_STD_LCD_WIDTH;
			info->lcd_height    = IPOD_STD_LCD_HEIGHT;
			info->lcd_base      = IPOD_PP5002_LCD_BASE;
			info->ipod_rtc      = IPOD_PP5002_RTC;
			info->lcd_busy_mask = IPOD_STD_LCD_BUSY_MASK;
			info->ipod_type     = "iPod";
			info->flags        &= ~LCD_COLOR;
		break;
	}
}
