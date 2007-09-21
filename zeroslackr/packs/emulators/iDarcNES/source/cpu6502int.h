/*
 * cpu6502int.h
 *
 * Internal definitions for the cpu6502 core
 */

/* $Id: cpu6502int.h,v 1.2 2000/07/15 22:18:53 nyef Exp $ */

#ifndef CPU6502INT_H
#define CPU6502INT_H

#include "types.h"
#include "cal.h"

#define LAZY_FLAG_EVALUATION

#define VECTOR_RESET_LO 0xfffc
#define VECTOR_RESET_HI 0xfffd

#define VECTOR_NMI_LO 0xfffa
#define VECTOR_NMI_HI 0xfffb

#define VECTOR_IRQ_LO 0xfffe
#define VECTOR_IRQ_HI 0xffff

#define VECTOR_BRK_LO VECTOR_IRQ_LO
#define VECTOR_BRK_HI VECTOR_IRQ_HI

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80

#ifndef LAZY_FLAG_EVALUATION
#define EVAL_FLAGNZ(context, data) (context)->flags &= ~(FLAG_N|FLAG_Z); if (!(data)) { (context)->flags |= FLAG_Z; } (context)->flags |= (data) & FLAG_N
#define GET_FLAGS(context) (context)->flags
#define SET_FLAGS(context, data) (context)->flags = (data)
#define STATUS_ZERO(context) (context)->flags & FLAG_Z
#define STATUS_NEGATIVE(context) (context)->flags & FLAG_N
#define STATUS_CARRY(context) (context)->flags & FLAG_C
#define STATUS_OVERFLOW(context) (context)->flags & FLAG_V
#define SET_FLAG_C(context, data) (context)->flags &= ~FLAG_C; (context)->flags |= (data)
#define SET_FLAG_V(context, data) (context)->flags &= ~FLAG_V; (context)->flags |= (data) >> 1
#else
#define EVAL_FLAGNZ(context, data) (context)->flag_n = data; (context)->flag_z = data
u8 cpu6502_get_flags(struct cpu6502_context *context);
#define GET_FLAGS(context) cpu6502_get_flags(context)
void cpu6502_set_flags(struct cpu6502_context *context, u8 flags);
#define SET_FLAGS(context, data) cpu6502_set_flags(context, data)
#define STATUS_ZERO(context) !((context)->flag_z)
#define STATUS_NEGATIVE(context) (context)->flag_n & FLAG_N
#define STATUS_CARRY(context) (context)->flag_c
#define STATUS_OVERFLOW(context) (context)->flag_v
#define SET_FLAG_C(context, data) (context)->flag_c = (data)
#define SET_FLAG_V(context, data) (context)->flag_v = (data)
#endif

typedef void (*ophandler_t)(struct cpu6502_context *);

extern ophandler_t cpu6502_decode[0x100];

#define OPFETCH(context) context->readfunc(context->cpu, context->pc++)

#define PUSH_BYTE(context, data) context->zpage[0x100 | context->reg_s--] = data
#define POP_BYTE(context) context->zpage[0x100 | ++context->reg_s]

struct cpu6502_context {
    cal_cpu cpu;
    int cycles_left;
    u8 *zpage; /* also stack */

    u16 pc;
    
    u8 reg_a;
    u8 reg_x;
    u8 reg_y;
    u8 reg_s;
    u8 flags;

#ifdef LAZY_FLAG_EVALUATION
    u8 flag_n;
    u8 flag_z;
    u8 flag_v;
    u8 flag_c;
#endif
    
    memread8_t readfunc;
    memwrite8_t writefunc;
};

void cal_cpu6502_init(cal_cpu *cpu);
void cpu6502_step(struct cpu6502_context *context);
void cpu6502_op_0(struct cpu6502_context *context); /* unimplemented opcode */

#endif /* CPU6502INT_H */

/*
 * $Log: cpu6502int.h,v $
 * Revision 1.2  2000/07/15 22:18:53  nyef
 * converted to use lazy flag evaluation
 *
 * Revision 1.1  2000/05/01 00:35:36  nyef
 * Initial revision
 *
 */
