/*
 *  gamelib - blit.h
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
 
#ifndef __BLIT_H__
#define __BLIT_H__

#include <stdint.h>

long blit_ipod_get_hw_version(void);
void blitter_init(void);
void blitter_blit_fullscreen(void * fb);
void blitter_blit_partial   (void * fb, uint16_t startx, uint16_t starty,
                                        uint16_t width,  uint16_t height);

//void blitter_blit_buffer_2bpp(unsigned char * img_data, int sx, int sy, int mx, int my);
//void blitter_ipod_update_photo(unsigned short * addr, int sx, int sy, int mx, int my);
void blitter_finish(void);

#endif // __BLIT_H__
