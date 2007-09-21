/*
 * cpu6502.h
 *
 * cpu6502 core interface
 */

/* $Id: cpu6502.h,v 1.1 2001/03/05 00:04:09 nyef Exp $ */

#ifndef CPU6502_H
#define CPU6502_H

#include "cal.h"

struct cpu6502_context;

struct cpu6502_context *cpu6502_create(void);
void cpu6502_setuserdata(struct cpu6502_context *context, void *userdata);
void cpu6502_setmemhandlers(struct cpu6502_context *context, memread8_t readfunc, memwrite8_t writefunc);
void cpu6502_setzpage(struct cpu6502_context *context, void *page0);
void cpu6502_reset(struct cpu6502_context *context);
void cpu6502_step(struct cpu6502_context *context);
void cpu6502_run(struct cpu6502_context *context);
void cpu6502_runfor(struct cpu6502_context *context, int cycles);
void cpu6502_event_delay_callback(void *context, int cycles);
void cpu6502_irq(struct cpu6502_context *context);
void cpu6502_nmi(struct cpu6502_context *context);

#endif /* CPU6502_H */

/*
 * $Log: cpu6502.h,v $
 * Revision 1.1  2001/03/05 00:04:09  nyef
 * Initial revision
 *
 */
