/*
 * torr_cpu.c
 *
 * CAL interface and non-generated code for torrigion core
 */

/* $Id: torr_cpu.c,v 1.6 2000/04/23 20:15:13 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include "cal.h"
#include "emu68k.h"
#include "tool.h"
#include "ui.h"

#ifdef CPU_TORR68K

void cal_torr68k_reset(cal_cpu cpu);
void cal_torr68k_run(cal_cpu cpu);
void cal_torr68k_runfor(cal_cpu cpu, int cycles);
void cal_torr68k_irq(cal_cpu cpu, int irqno);
void cal_torr68k_nmi(cal_cpu cpu);
void cal_torr68k_setzpage(cal_cpu cpu, void *page0);
int cal_torr68k_timeleft(cal_cpu cpu);
void cal_torr68k_setmmu8(cal_cpu cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl);
void cal_torr68k_setmmu16(cal_cpu cpu, int shift, int mask, memread16_t *rtbl, memwrite16_t *wtbl);

void cal_torr68k_init(cal_cpu *cpu)
{
    (*cpu)->data.d_junk68k = calloc(1, sizeof(struct emu68k_context));
    if (!(*cpu)->data.d_junk68k) {
	printf("Insufficient memory to create CPU.\n");
	free(*cpu);
	*cpu = NULL;
	return;
    }
    (*cpu)->reset = cal_torr68k_reset;
    (*cpu)->run = cal_torr68k_run;
    (*cpu)->runfor = cal_torr68k_runfor;
    (*cpu)->irq = cal_torr68k_irq;
    (*cpu)->nmi = cal_torr68k_nmi;
    (*cpu)->setzpage = cal_torr68k_setzpage;
    (*cpu)->timeleft = cal_torr68k_timeleft;
    (*cpu)->setmmu8 = cal_torr68k_setmmu8;
    (*cpu)->setmmu16 = cal_torr68k_setmmu16;
    (*cpu)->data.d_junk68k->cpu = *cpu;
}

void cal_torr68k_reset(cal_cpu cpu)
{
    emu68k_reset(cpu->data.d_junk68k);
}

typedef void (*m68k_instr_handler)(struct emu68k_context *, unsigned short);

void cal_torr68k_run(cal_cpu cpu)
{
    unsigned short opword;
    struct emu68k_context *context;
    extern m68k_instr_handler torrigion_decode[0x10000];
/*     static int trace = 0; */

    context = cpu->data.d_junk68k;
#if 0
    if (!trace) {
	dt68000_disassemble(0x0206, 0x03d4);
	system_flags = F_UNIMPL;
    }
#endif
    opword = context->read16table[(context->pc >> context->memshift) & context->memmask](context->cpu, context->pc);
    context->pc += 2;
    if (torrigion_decode[opword]) {
#if 0
	if ((opword & 0xffc0) == 0x4e80) {
	    trace = 1;
	}
	if ((opword & 0xff00) == 0x6100) {
	    trace = 1;
	}
	if ((opword & 0xffff) == 0x4e71) {
	    trace = 0;
	}
	if (trace) {
	    deb_printf("torrigion: executing opword 0x%04hx from 0x%06lx.\n", opword, context->pc - 2);
	}
#endif
	if (opword == 0x60fe) {
	    deb_printf("torrigion: stopping execution at infinite loop at 0x%06x.\n", context->pc - 2);
	    system_flags |= F_UNIMPL;
	}
/* 	if (opword == 0x3036) { */
/* 	    deb_printf("torrigion: 0x3036!.\n"); */
/* 	} */
/* 	deb_printf("torrigion: executing opword 0x%04hx from 0x%06lx.\n", opword, context->pc - 2); */
/* 	deb_printf("torrigion: executing opword 0x%04hx\n", opword); */
	torrigion_decode[opword](cpu->data.d_junk68k, opword);
    } else {
	printf("torrigion: unimplemented opword 0x%04hx at 0x%06lx.\n", opword, context->pc);
	system_flags |= F_UNIMPL;
    }
}

void cal_torr68k_runfor(cal_cpu cpu, int cycles)
{
/*     cpu->data.d_emuz80->cycles_left += cycles; */
}

#endif

#if (defined CPU_TORR68K) || (defined CPU_JUNK68K)
void m68k_push_long(struct emu68k_context *context, unsigned long data);

void cal_torr68k_irq(cal_cpu cpu, int irqno)
{
/*     emuz80_IRQ(cpu->data.d_emuz80); */
    struct emu68k_context *context;
    unsigned short new_sr;
    unsigned short tmp_sp;
    unsigned short tmp1;
    unsigned short tmp2;

    context = cpu->data.d_junk68k;

/*     deb_printf("torr_68k: SR = 0x%04hx.\n", context->flags); */
    if ((context->flags & 0x0700) < (irqno << 8)) {
	/* FIXME: push PC, push SR, adjust SR (lose trace, gain supervisor),
	   switch stacks if nessecary, load appropriate PC address */
	new_sr = context->flags;
	new_sr &= ~0x8000;
	new_sr |= 0x2000;
	new_sr &= ~0x0700;
	new_sr |= (irqno << 8);

	if ((new_sr ^ context->flags) & 0x2000) {
	    /* FIXME: switch stacks */
	    deb_printf("BUG: stack switch on IRQ not implemented\n");
	}
	m68k_push_long(context, context->pc);
	context->regs_a[7] -= 2;
	context->write16table[(context->regs_a[7] >> context->memshift) & context->memmask](cpu, context->regs_a[7], context->flags);
	context->flags = new_sr;

/* 	tmp_sp = (0x24 + irqno) << 2; */
	tmp_sp = 0x78;
	tmp1 = context->read16table[(tmp_sp >> context->memshift) & context->memmask](context->cpu, tmp_sp);
	tmp_sp += 2;
	tmp2 = context->read16table[(tmp_sp >> context->memshift) & context->memmask](context->cpu, tmp_sp);
	context->pc = (tmp1 << 16) | tmp2;

/* 	deb_printf("torr_68k: IRQ %d.\n", irqno); */
    }
}
#endif

#ifdef CPU_TORR68K

void cal_torr68k_nmi(cal_cpu cpu)
{
/*     emuz80_NMI(cpu->data.d_emuz80); */
}

void cal_torr68k_setzpage(cal_cpu cpu, void *page0)
{
}

int cal_torr68k_timeleft(cal_cpu cpu)
{
/*     return cpu->data.d_emuz80->cycles_left; */
    return 0;
}

void cal_torr68k_setmmu8(cal_cpu cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl)
{
    cpu->data.d_junk68k->memshift = shift;
    cpu->data.d_junk68k->memmask = mask;
    cpu->data.d_junk68k->read8table = rtbl;
    cpu->data.d_junk68k->write8table = wtbl;
}

void cal_torr68k_setmmu16(cal_cpu cpu, int shift, int mask, memread16_t *rtbl, memwrite16_t *wtbl)
{
    cpu->data.d_junk68k->read16table = rtbl;
    cpu->data.d_junk68k->write16table = wtbl;
}

#endif

/*
 * $Log: torr_cpu.c,v $
 * Revision 1.6  2000/04/23 20:15:13  nyef
 * made junk68k (emu68k) core use the torr68k code for IRQ generation
 *
 * Revision 1.5  1999/09/08 01:02:11  nyef
 * added some more disabled debugging code
 * hacked up the irq emulation a bit (will need to be fixed later)
 *
 * Revision 1.4  1999/09/04 15:37:57  nyef
 * added IRQ emulation (possibly buggy)
 * added some (disabled) debugging code
 *
 * Revision 1.3  1999/08/28 03:31:03  nyef
 * commented out some debug info that was getting in the way
 *
 * Revision 1.2  1999/08/23 01:19:24  nyef
 * fixed some include file problems
 *
 * Revision 1.1  1999/08/15 02:28:05  nyef
 * Initial revision
 *
 */
