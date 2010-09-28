/*
 * arch.c - architecture definition for Sansa
 *
 * Copyright (c) 2003, Bernard Leach (leachbj@bouncycastle.org)
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/init.h>

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <asm/arch/irq.h>
#include <asm/mach/arch.h>

static void __init
sansa_fixup(struct machine_desc *desc, struct param_struct *params,
	char **cmdline, struct meminfo *mi)
{
}

MACHINE_START(SANSA, "Sansa")
	MAINTAINER("Sebastian Duell")
	BOOT_MEM(0x10000000, 0x60000000, 0x00000000)
	INITIRQ(sansa_init_irq)
	FIXUP(sansa_fixup)
MACHINE_END

