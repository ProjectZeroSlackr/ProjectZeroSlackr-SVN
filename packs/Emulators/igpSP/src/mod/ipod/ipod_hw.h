/*
 * Last updated: May 31, 2008
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

// Postscalar values: CPU speed = (24 / 8) * postscalar
// 81MHz is the MAXIMUM the iPod will go without screwing up
// 75MHz is iPodLinux's default, 66MHz is Apple OS's default
#define CPU_33MHz	11
#define CPU_45MHz	15
#define CPU_66MHz	22 // Underclock
#define CPU_75MHz	25 // Normal
#define CPU_78MHz	26 // Overclocked
#define CPU_81MHz	27 // Max Overclock - Unstable!

#define BACKLIGHT_OFF	0
#define BACKLIGHT_ON	1

#define CLOCK_SCALER	0x60006034
#define CLOCK_POLICY	0x60006020
#define RUN_CLK(x) (0x20000000 | ((x) <<  4))
#define RUN_GET(x) ((inl(CLOCK_POLICY) & 0x0fffff8f) | RUN_CLK(x))
#define RUN_SET(x) outl(RUN_GET(x), CLOCK_POLICY)

