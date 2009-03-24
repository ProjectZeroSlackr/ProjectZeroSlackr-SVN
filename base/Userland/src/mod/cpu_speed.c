/*
 * Last updated: Aug 8, 2008
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
 
#include <stdio.h>
#include <stdlib.h>

#define outl(a, b)		(*(volatile unsigned int *)(b) = (a))
#define inl(a)			(*(volatile unsigned int *)(a))

#define CLOCK_SCALER	0x60006034
#define CLOCK_POLICY	0x60006020
#define RUN_CLK(x)		(0x20000000 | ((x) <<  4))
#define RUN_GET(x)		((inl(CLOCK_POLICY) & 0x0fffff8f) | RUN_CLK(x))
#define RUN_SET(x)		outl(RUN_GET(x), CLOCK_POLICY)



static void ipod_set_cpu_speed(int speed)
{
	int postscalar = speed / 3;
	outl(inl(0x70000020) | (1 << 30), 0x70000020);
	RUN_SET(0x2);
	outl(0xaa020008 | (postscalar << 8), CLOCK_SCALER);
	int x;
	for (x = 0; x < 10000; x++);
	RUN_SET(0x7);
}

static void print_help()
{
	printf("Usage: cpu_speed [SPEED]\n");
	printf("SPEED should be a multiple of 3 between 45 and 81 inclusively.\n");
	printf("The Apple OS default is 66MHz, ZeroSlackr's default is 75MHz.\n");
	printf("To prevent your iPod from overheating, avoid extended usage at 78MHz or higher.\n");
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		print_help();
		exit(1);
	}
	int speed = atoi(argv[1]);
	// 45 MHz is already very slow whereas 81 MHz seems to be the maximum without iPodLinux becoming unstable
	if (!speed || speed < 45 || speed > 81) {
		printf("Please specify a valid CPU speed in MHz between 45 and 81.\n");
		print_help();
		exit(1);
	} else {
		ipod_set_cpu_speed(speed);
		printf("CPU speed set to %iMHz\n", speed);
		exit(0);
	}
}
