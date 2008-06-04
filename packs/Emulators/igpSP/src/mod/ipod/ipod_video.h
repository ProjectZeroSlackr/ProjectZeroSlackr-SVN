/*
 * Last updated: June 4, 2008
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

#define CLEAR	0x000000	// Black pixel
#define IGNORE	0			// Not part of scaling checks

// http://objectmix.com/dotnet/101346-help-display-16-bit-bitmap.html
//   For the RGB565 case:
//   byte red = (color & 0xF800) >> 8;
//   byte green = (color & 0x07E0) >> 3;
//   byte blue = (color & 0x001F) << 3;
// (note byte = uint8)

// http://www.fourcc.org/fccyvrgb.php
//   Y = 0.299R + 0.587G + 0.114B
// About the same as (1R + 2G + 1B) / 4
// Thanks to BleuLlama (Scott Lawrence) for the help and testing

// http://www.gamedev.net/reference/articles/article1369.asp
//  #define RGB565(r, g, b) ((r >> 3) << 11)| ((g >> 2) << 5)| ((b >> 3) << 0)

#define get_R_from_RGB565(p) \
	(((p) & 0xf800) >> 8)

#define get_G_from_RGB565(p) \
	(((p) & 0x07e0) >> 3)

#define get_B_from_RGB565(p) \
	(((p) & 0x001f) << 3)

#define RGB565(r, g, b) \
	((r >> 3) << 11)| ((g >> 2) << 5)| ((b >> 3) << 0)


// Get Y (luminosity) value of YUV format from RGB565 format for monochrome iPods
#define get_Y_from_RGB565(p) \
	(get_R_from_RGB565(p) + get_G_from_RGB565(p) << 1 + get_B_from_RGB565(p)) >> 2


// For blending two pixels for smooth screen
#define get_avg_R_from_2_RGB565(p1, p2) \
	((get_R_from_RGB565(p1) + get_R_from_RGB565(p2)) >> 1)

#define get_avg_G_from_2_RGB565(p1, p2) \
	((get_G_from_RGB565(p1) + get_G_from_RGB565(p2)) >> 1)

#define get_avg_B_from_2_RGB565(p1, p2) \
	((get_B_from_RGB565(p1) + get_B_from_RGB565(p2)) >> 1)

#define blend_pixels_2_RGB565(p1, p2) \
	RGB565( \
		get_avg_R_from_2_RGB565(p1, p2), \
		get_avg_G_from_2_RGB565(p1, p2), \
		get_avg_B_from_2_RGB565(p1, p2) \
	)


// For blending four pixels for even smoother screen
#define get_avg_R_from_4_RGB565(p1, p2, p3, p4) \
	((get_R_from_RGB565(p1) + get_R_from_RGB565(p2) + get_R_from_RGB565(p3) + get_R_from_RGB565(p4)) >> 2)

#define get_avg_G_from_4_RGB565(p1, p2, p3, p4) \
	((get_G_from_RGB565(p1) + get_G_from_RGB565(p2) + get_G_from_RGB565(p3) + get_G_from_RGB565(p4)) >> 2)

#define get_avg_B_from_4_RGB565(p1, p2, p3, p4) \
	((get_B_from_RGB565(p1) + get_B_from_RGB565(p2) + get_B_from_RGB565(p3) + get_B_from_RGB565(p4)) >> 2)

#define blend_pixels_4_RGB565(p1, p2, p3, p4) \
	RGB565( \
		get_avg_R_from_4_RGB565(p1, p2, p3, p4), \
		get_avg_G_from_4_RGB565(p1, p2, p3, p4), \
		get_avg_B_from_4_RGB565(p1, p2, p3, p4) \
	)

