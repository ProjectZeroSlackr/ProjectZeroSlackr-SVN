/*
 * Last updated: Jun 12, 2008
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

// Brightness

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


// Backlight and contrast

#define BACKLIGHT_OFF	0
#define BACKLIGHT_ON	1

#define FBIOGET_CONTRAST	_IOR('F', 0x22, int)
#define FBIOSET_CONTRAST	_IOW('F', 0x23, int)

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOSET_BACKLIGHT	_IOW('F', 0x25, int)


// CPU speed

// Postscalar values: CPU speed = (24 / 8) * postscalar
// 81MHz is the MAXIMUM the iPod will go without screwing up
// 75MHz is iPodLinux's default, 66MHz is Apple OS's default
#define CPU_33MHz	11
#define CPU_45MHz	15
#define CPU_66MHz	22 // Underclock
#define CPU_75MHz	25 // Normal
#define CPU_78MHz	26 // Overclocked
#define CPU_81MHz	27 // Max Overclock - Unstable!

#define CLOCK_SCALER	0x60006034
#define CLOCK_POLICY	0x60006020
#define RUN_CLK(x) (0x20000000 | ((x) <<  4))
#define RUN_GET(x) ((inl(CLOCK_POLICY) & 0x0fffff8f) | RUN_CLK(x))
#define RUN_SET(x) outl(RUN_GET(x), CLOCK_POLICY)

