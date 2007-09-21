/*
 * cpu6502.c
 *
 * CAL interface and non-generated code for the cpu6502 core
 */

/* $Id: cpu6502.c,v 1.5 2001/03/05 00:04:22 nyef Exp $ */

#include "cpu6502.h"

#include <stdlib.h>
#include <stdio.h>
#include "cal.h"
#include "tool.h"
#include "ui.h"

#include "cpu6502int.h"

struct cpu6502_context *cpu6502_create(void)
{
    return calloc(1, sizeof(struct cpu6502_context));
}

void cpu6502_setuserdata(struct cpu6502_context *context, void *userdata)
{
    context->cpu = userdata;
}

void cpu6502_setmemhandlers(struct cpu6502_context *context, memread8_t readfunc, memwrite8_t writefunc)
{
    context->readfunc = readfunc;
    context->writefunc = writefunc;
}

void cpu6502_setzpage(struct cpu6502_context *context, void *page0)
{
    context->zpage = page0;
}

void cpu6502_reset(struct cpu6502_context *context)
{
    context->reg_a = 0x00;
    context->reg_x = 0x00;
    context->reg_y = 0x00;
    context->reg_s = 0xff;

    SET_FLAGS(context, 0x00); /* FIXME: wrong */

    context->pc = context->readfunc(context->cpu, VECTOR_RESET_LO);
    context->pc |= context->readfunc(context->cpu, VECTOR_RESET_HI) << 8;
}

void cpu6502_step(struct cpu6502_context *context)
{
    u8 opcode;

    opcode = OPFETCH(context);
    cpu6502_decode[opcode](context);
}

void cpu6502_run(struct cpu6502_context *context)
{
	context->cycles_left-=cpu_exec(context->cycles_left);
	if (context->cycles_left<0) context->cycles_left=0;
/*    while (context->cycles_left > 0) {
	cpu6502_step(context);

	if (system_flags & F_UNIMPL) {
	    return;
	}
    }*/
}

void cpu6502_runfor(struct cpu6502_context *context, int cycles)
{
    context->cycles_left += cycles;
}

void cpu6502_event_delay_callback(void *context, int cycles)
{
    cpu6502_runfor(context, cycles);
    cpu6502_run(context);
}

void cpu6502_irq(struct cpu6502_context *context)
{
    if (context->flags & FLAG_I) {
	return; /* interrupts disabled */
    }

    PUSH_BYTE(context, context->pc >> 8);
    PUSH_BYTE(context, context->pc & 0xff);
    PUSH_BYTE(context, (GET_FLAGS(context)) & ~FLAG_B);

    context->flags |= FLAG_I;
    context->flags &= ~FLAG_D;
    
    context->pc = context->readfunc(context->cpu, VECTOR_IRQ_LO);
    context->pc |= context->readfunc(context->cpu, VECTOR_IRQ_HI) << 8;

    context->cycles_left -= 7;
    
}

void cpu6502_nmi(struct cpu6502_context *context)
{
    PUSH_BYTE(context, context->pc >> 8);
    PUSH_BYTE(context, context->pc & 0xff);
    PUSH_BYTE(context, (GET_FLAGS(context)) & ~FLAG_B);

    context->pc = context->readfunc(context->cpu, VECTOR_NMI_LO);
    context->pc |= context->readfunc(context->cpu, VECTOR_NMI_HI) << 8;

    context->cycles_left -= 7;
}

#ifdef LAZY_FLAG_EVALUATION
u8 cpu6502_get_flags(struct cpu6502_context *context)
{
    u8 retval;

    retval = context->flags;
    retval &= ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
    retval |= context->flag_n & FLAG_N;
    retval |= (!context->flag_z) << 1;
    retval |= context->flag_c;
    retval |= context->flag_v >> 1;

    return retval;
}

void cpu6502_set_flags(struct cpu6502_context *context, u8 flags)
{
    context->flags = flags & ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
    context->flag_n = flags;
    context->flag_z = !(flags & FLAG_Z);
    context->flag_c = flags & FLAG_C;
    context->flag_v = (flags & FLAG_V) << 1;
}
#endif

void cpu6502_op_0(struct cpu6502_context *context)
{
    context->pc -= 1;
    deb_printf("unimplemented opcode 0x%02x.\n", OPFETCH(context));
    system_flags |= F_UNIMPL;
}

/* CAL interface code */

void cal_cpu6502_reset(cal_cpu cpu);
void cal_cpu6502_run(cal_cpu cpu);
void cal_cpu6502_runfor(cal_cpu cpu, int cycles);
void cal_cpu6502_burn(cal_cpu cpu, int cycles);
void cal_cpu6502_irq(cal_cpu cpu, int irqno);
void cal_cpu6502_nmi(cal_cpu cpu);
void cal_cpu6502_setzpage(cal_cpu cpu, void *page0);
int cal_cpu6502_timeleft(cal_cpu cpu);
void cal_cpu6502_setmmu8(cal_cpu cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl);
void cal_cpu6502_init(cal_cpu *cpu)
{
    (*cpu)->data.d_cpu6502 =  calloc(1, sizeof(struct cpu6502_context));
    memset((char*)(*cpu)->data.d_cpu6502,0,sizeof(struct cpu6502_context));
    if (!(*cpu)->data.d_cpu6502) {
	printf("Insufficient memory to create CPU.\n");
	free(*cpu);
	*cpu = NULL;
	return;
    }
    (*cpu)->reset = cal_cpu6502_reset;
    (*cpu)->run = cal_cpu6502_run;
    (*cpu)->runfor = cal_cpu6502_runfor;
    (*cpu)->burn = cal_cpu6502_burn;
    (*cpu)->irq = cal_cpu6502_irq;
    (*cpu)->nmi = cal_cpu6502_nmi;
    (*cpu)->setzpage = cal_cpu6502_setzpage;
    (*cpu)->timeleft = cal_cpu6502_timeleft;
    (*cpu)->setmmu8 = cal_cpu6502_setmmu8;
    (*cpu)->data.d_cpu6502->cpu = *cpu;
    
}

void cal_cpu6502_reset(cal_cpu cpu)
{
//    cpu6502_reset(cpu->data.d_cpu6502);
      reset_cpu();
}

void cal_cpu6502_run(cal_cpu cpu)
{
//    cpu6502_run(cpu->data.d_cpu6502);
//f (cpu->data.d_cpu6502->cycles_left>0)
	cpu->data.d_cpu6502->cycles_left-=cpu_exec(cpu->data.d_cpu6502->cycles_left);
	if (cpu->data.d_cpu6502->cycles_left<0) cpu->data.d_cpu6502->cycles_left=0;
}

void cal_cpu6502_runfor(cal_cpu cpu, int cycles)
{
//    cpu6502_runfor(cpu->data.d_cpu6502, cycles);
	cpu->data.d_cpu6502->cycles_left+=cycles;
}
void cal_cpu6502_burn(cal_cpu cpu, int cycles)
{
//   cpu_release();
   cpu_burn(cycles);
//   if (cpu->data.d_cpu6502->cycles_left>=512)
//	cpu->data.d_cpu6502->cycles_left-=512;
  /*lse
	cpu->data.d_cpu6502->cycles_left=0;*/
}
void cal_cpu6502_irq(cal_cpu cpu, int irqno)
{
//    cpu6502_irq(cpu->data.d_cpu6502);
    cpu_pending_irq();
    cpu->data.d_cpu6502->cycles_left-=7;
	if (cpu->data.d_cpu6502->cycles_left<0) cpu->data.d_cpu6502->cycles_left=0;
}

void cal_cpu6502_nmi(cal_cpu cpu)
{
//    cpu6502_nmi(cpu->data.d_cpu6502);
    cpu_nmi();
    cpu->data.d_cpu6502->cycles_left-=7;
	if (cpu->data.d_cpu6502->cycles_left<0) cpu->data.d_cpu6502->cycles_left=0;
}

void cal_cpu6502_setzpage(cal_cpu cpu, void *page0)
{
//   cpu6502_setzpage(cpu->data.d_cpu6502, page0);
    init_cpu(page0);  
}

int cal_cpu6502_timeleft(cal_cpu cpu)
{
    return cpu->data.d_cpu6502->cycles_left;
}
memread8_t readfunc;
memwrite8_t writefunc;
extern u32 lraddr;
extern int updatelr;
u8 Rd6502(u32 Addr)
{
	cal_cpu cpu;
	u32 bleh=readfunc(cpu,Addr);
	if (updatelr==1) lraddr=bleh;
	return bleh;
}
void Wr6502(u32 Addr, u8 Value)
{
	cal_cpu cpu;
	writefunc(cpu,Addr,Value);
}

void cal_cpu6502_setmmu8(cal_cpu cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl)
{
    cpu6502_setmemhandlers(cpu->data.d_cpu6502, *rtbl, *wtbl);
    readfunc=rtbl[0];
    writefunc=wtbl[0];
}

void Cdebug_opcode() {};
void panic() {};
/*
 * $Log: cpu6502.c,v $
 * Revision 1.5  2001/03/05 00:04:22  nyef
 * added inclusion of new cpu6502.h
 *
 * Revision 1.4  2001/03/04 23:22:26  nyef
 * reduced cal interface code to wrappers around cpu6502_*() functions
 *
 * Revision 1.3  2000/09/26 06:30:36  nyef
 * added cycle timings for IRQ and NMI
 *
 * Revision 1.2  2000/07/15 22:18:49  nyef
 * converted to use lazy flag evaluation
 *
 * Revision 1.1  2000/05/01 00:35:33  nyef
 * Initial revision
 *
 */
