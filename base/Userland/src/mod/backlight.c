/*
 * Last updated: July 26, 2008
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

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

// Backlight

#define BACKLIGHT_OFF	0
#define BACKLIGHT_ON	1

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOSET_BACKLIGHT	_IOW('F', 0x25, int)
 
static int ipod_ioctl(int request, int *arg)
{
	int fd;
	fd = open("/dev/fb0", O_NONBLOCK);
	if (fd < 0) fd = open("/dev/fb/0", O_NONBLOCK);
	if (fd < 0) return -1;
	if (ioctl(fd, request, arg) < 0) {
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

static void ipod_set_backlight(int backlight)
{
	ipod_ioctl(FBIOSET_BACKLIGHT, (int *)(long)backlight);
}


// Brightness - will only work on iPod nanos and videos

// Default brightness level when backlight is turned on
#define BRIGHTNESS_DEFAULT 16

/* Standard GPIO addresses + 0x800 allow atomic port manipulation on PP502x.
 * Bits 8..15 of the written word define which bits are changed, bits 0..7
 * define the value of those bits. */

#define GPIOD_OUTPUT_VAL \
	(*(volatile unsigned long *)(0x6000d02c))

#define GPIO_SET_BITWISE(port, mask) \
	do { *(&port + (0x800/sizeof(long))) = (mask << 8) | mask; } while(0)

#define GPIO_CLEAR_BITWISE(port, mask) \
	do { *(&port + (0x800/sizeof(long))) = mask << 8; } while(0)

#define IRQ_STATUS	0x80

static inline int disable_interrupt_save(int mask)
{
	/* Set I and/or F disable bit and return old cpsr value */
	int cpsr, tmp;
	asm volatile (
		"mrs     %1, cpsr   \n"
		"orr     %0, %1, %2 \n"
		"msr     cpsr_c, %0 \n"
		: "=&r"(tmp), "=&r"(cpsr)
		: "r"(mask)
		);
	return cpsr;
}

static inline void restore_interrupt(int cpsr)
{
	/* Set cpsr_c from value returned by disable_interrupt_save or set_interrupt_status */
	asm volatile ("msr cpsr_c, %0" : : "r"(cpsr));
}

#define disable_irq_save() \
	disable_interrupt_save(IRQ_STATUS)

#define restore_irq(cpsr) \
	restore_interrupt(cpsr)

#define USEC_TIMER \
	(*(volatile unsigned long *)(0x60005010))

#define TIME_AFTER(a,b) \
	((long)(b) - (long)(a) < 0)

#define TIME_BEFORE(a,b) \
	TIME_AFTER(b,a)

static inline void udelay(unsigned usecs)
{
	unsigned stop = USEC_TIMER + usecs;
	while (TIME_BEFORE(USEC_TIMER, stop));
}

static long ipod_generation() 
{
	static long gen = 0;
	if (gen == 0) {
		int i;
		char cpuinfo[256];
		char *ptr;
		FILE *file;
		
		if ((file = fopen("/proc/cpuinfo", "r")) != NULL) {
			while (fgets(cpuinfo, sizeof(cpuinfo), file) != NULL)
				if (strncmp(cpuinfo, "Revision", 8) == 0)
					break;
			fclose(file);
		}
		for (i = 0; !isspace(cpuinfo[i]); i++);
		for (; isspace(cpuinfo[i]); i++);
		ptr = cpuinfo + i + 2;
		
		gen = strtol (ptr, NULL, 16);
	}
	return gen;
}

static void ipod_set_brightness(int val)
{
	int oldlevel;
	int brightness_current = BRIGHTNESS_DEFAULT;
	if (brightness_current < val) {
		do {
			oldlevel = disable_irq_save();
			GPIO_CLEAR_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			udelay(10);
			GPIO_SET_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			restore_irq(oldlevel);
			udelay(10);
		} while (++brightness_current < val);
	} else if (brightness_current > val) {
		do {
			oldlevel = disable_irq_save();
			GPIO_CLEAR_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			udelay(200);
			GPIO_SET_BITWISE(GPIOD_OUTPUT_VAL, 0x80);
			restore_irq(oldlevel);
			udelay(10);
		} while (--brightness_current > val);
	}
}


// Main

static void print_help()
{
	printf("Usage:\n");
	printf("  backlight on\n");
	printf("  backlight off\n");
	printf("  backlight brightness N\n");
	printf("N should be an integer from 1 to 32\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_help();
		exit(1);
	}
	if (strcmp(argv[1], "on") == 0) {
		ipod_set_backlight(BACKLIGHT_ON);
		printf("Backlight turned on.\n");
		exit(0);
	} else if (strcmp(argv[1], "off") == 0) {
		ipod_set_backlight(BACKLIGHT_OFF);
		printf("Backlight turned off.\n");
		exit(0);
	} else if (argc == 3 && strcmp(argv[1], "brightness")  == 0) {
		int hw_ver = ipod_generation() >> 16;
		if (hw_ver == 0xC || hw_ver == 0xB) { // iPod nano or iPod video
			int brightness = atoi(argv[2]);
			// 32 is the max brightness level
			if (!brightness || brightness < 1 || brightness > 32) {
				printf("Please specify a valid brightness level between 1 and 32.\n");
				print_help();
				exit(1);
			} else {
				// Turn on backlight or brightness won't have any effect
				ipod_set_backlight(BACKLIGHT_ON);
				printf("Backlight turned on.\n");
				ipod_set_brightness(brightness);
				printf("Backlight brightness level set to %i.\n", brightness);
				exit(0);
			}
		} else {
			printf("Sorry, backlight brightness levels can only be changed for iPod nanos and videos.\n");
			exit(1);
		}
	} else {
		print_help();
		exit(1);
	}
}
