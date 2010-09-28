/*
 * irq.c - irq processing for Sansa
 *
 * Copyright (c) 2003, Bernard Leach (leachbj@bouncycastle.org)
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <asm/mach/irq.h>
#include <asm/irq.h>
#include <asm/timex.h>

#include "pp5020.h"

/* PP5002 functions */
static void pp5002_unmask_irq(unsigned int irq)
{
	/* unmask the interrupt */
	outl((1 << irq), 0xcf001024);
	/* route it to IRQ not FIQ */
	outl(inl(0xcf00102c) & ~(1 << irq), 0xcf00102c);
}

static void pp5002_mask_irq(unsigned int irq)
{
	/* mask the interrupt */
	outl((1 << irq), 0xcf001028);
}

static void pp5002_mask_ack_irq(unsigned int irq)
{

	/* there is no general IRQ ack, we have to do it at the source */
	switch (irq) {
	case PP5002_IDE_IRQ:
		/* clear FIFO interrupt status */
		outl(0xff, 0xc0003020);
		outl(inl(0xc0003024) | (1<<4) | (1<<5), 0xc0003024);
		break;

	case PP5002_TIMER1_IRQ:
		inl(PP5002_TIMER1_ACK);
		break;
	}

	pp5002_mask_irq(irq);
}

/* PP5020 functions */
static void pp5020_unmask_irq(unsigned int irq)
{
	switch (irq) {
	case PP5020_IDE_IRQ:
		outl(inl(0xc3000028) | (1<<5), 0xc3000028);
	}

	if (irq < 32) {
		outl((1 << irq), 0x60004024);
	}
	else {
		/* hi interrupt enable */
		outl(0x40000000, 0x60004024);

		outl((1 << (irq - 32)), 0x60004124);
	}
}

static void pp5020_mask_irq(unsigned int irq)
{
	/* mask the interrupt */
	if (irq < 32) {
		outl((1 << irq), 0x60004028);
	}
	else {
		outl((1 << (irq - 32)), 0x60004128);
	}
}

static void pp5020_mask_ack_irq(unsigned int irq)
{
	/* there is no general IRQ ack, we have to do it at the source */
	switch (irq) {
	case PP5020_TIMER1_IRQ:
		inl(PP5020_TIMER1_ACK);
		break;

	case PP5020_IDE_IRQ:
		outl(inl(0xc3000028) & ~((1<<4) | (1<<5)), 0xc3000028);
		break;
	}

	pp5020_mask_irq(irq);
}

int sansa_init_irq(void)
{
	int irq;


	/* disable all interrupts */
/*		outl(-1, 0x60001138);
		outl(-1, 0x60001128);
		outl(-1, 0x6000111c);

		outl(-1, 0x60001038);
		outl(-1, 0x60001028);
		outl(-1, 0x6000101c);
*/
   COP_HI_INT_CLR      = -1;
   CPU_HI_INT_CLR      = -1;
   HI_INT_FORCED_CLR   = -1;
   
   COP_INT_CLR         = -1;
   CPU_INT_CLR         = -1;
   INT_FORCED_CLR      = -1;
   
   GPIOA_INT_EN        = 0;
   GPIOB_INT_EN        = 0;
   GPIOC_INT_EN        = 0;
   GPIOD_INT_EN        = 0;
   GPIOE_INT_EN        = 0;
   GPIOF_INT_EN        = 0;
   GPIOG_INT_EN        = 0;
   GPIOH_INT_EN        = 0;
   GPIOI_INT_EN        = 0;
   GPIOJ_INT_EN        = 0;
   GPIOK_INT_EN        = 0;
   GPIOL_INT_EN        = 0;

	/* clear all interrupts */
	for ( irq = 0; irq < NR_IRQS; irq++ ) {

			if (!PP5020_VALID_IRQ(irq)) continue;

		irq_desc[irq].valid     = 1;
		irq_desc[irq].probe_ok  = 1;
		irq_desc[irq].mask_ack  = pp5020_mask_ack_irq;
		irq_desc[irq].mask      = pp5020_mask_irq;
		irq_desc[irq].unmask    = pp5020_unmask_irq;
	}

	/*
	 * since the interrupt vectors at 0x0 are now installed
	 * we can wake up the COP safely
	 */
	outl(0x0, 0x60007004);

   return 0;
}

