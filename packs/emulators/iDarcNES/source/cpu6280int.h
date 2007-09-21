/*
 * cpu6280int.h
 *
 * Internal definitions for the cpu6280 core
 */

/* $Id: cpu6280int.h,v 1.9 2000/09/09 18:11:05 nyef Exp $ */

#ifndef CPU6280INT_H
#define CPU6280INT_H

#include "types.h"
#include "cal.h"

#define LAZY_FLAG_EVALUATION

#define VECTOR_RESET_LO 0xfffe
#define VECTOR_RESET_HI 0xffff

#define VECTOR_NMI_LO 0xfffc
#define VECTOR_NMI_HI 0xfffd

#define VECTOR_TIMER_LO 0xfffa
#define VECTOR_TIMER_HI 0xfffb

#define VECTOR_IRQ1_LO 0xfff8
#define VECTOR_IRQ1_HI 0xfff9

#define VECTOR_IRQ2_LO 0xfff6
#define VECTOR_IRQ2_HI 0xfff7

#define VECTOR_BRK_LO VECTOR_IRQ2_LO
#define VECTOR_BRK_HI VECTOR_IRQ2_HI

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_T 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

#ifndef LAZY_FLAG_EVALUATION
#define EVAL_FLAGNZ(context, data) (context)->flags &= ~(FLAG_N|FLAG_Z); if (!(data)) { (context)->flags |= FLAG_Z; } (context)->flags |= (data) & FLAG_N
#define GET_FLAGS(context) (context)->flags
#define SET_FLAGS(context, data) (context)->flags = (data)
#define STATUS_ZERO(context) ((context)->flags & FLAG_Z)
#define STATUS_NEGATIVE(context) ((context)->flags & FLAG_N)
#define STATUS_CARRY(context) ((context)->flags & FLAG_C)
#define STATUS_OVERFLOW(context) ((context)->flags & FLAG_V)
#define SET_FLAG_C(context, data) (context)->flags &= ~FLAG_C; (context)->flags |= (data)
#define SET_FLAG_V(context, data) (context)->flags &= ~FLAG_V; (context)->flags |= (data) >> 1
#else
#define EVAL_FLAGNZ(context, data) (context)->flag_n = data; (context)->flag_z = data
u8 cpu6280_get_flags(struct cpu6280_context *context);
#define GET_FLAGS(context) cpu6280_get_flags(context)
void cpu6280_set_flags(struct cpu6280_context *context, u8 flags);
#define SET_FLAGS(context, data) cpu6280_set_flags(context, data)
#define STATUS_ZERO(context) !((context)->flag_z)
#define STATUS_NEGATIVE(context) (context)->flag_n & FLAG_N
#define STATUS_CARRY(context) (context)->flag_c
#define STATUS_OVERFLOW(context) (context)->flag_v
#define SET_FLAG_C(context, data) (context)->flag_c = (data)
#define SET_FLAG_V(context, data) (context)->flag_v = (data)
#endif

typedef void (*ophandler_t)(struct cpu6280_context *);

extern ophandler_t cpu6280_decode[0x100];

#define OPFETCH(context) context->readfunc(context->cpu, context->pc++)

#define PUSH_BYTE(context, data) context->zpage[0x100 | context->reg_s--] = data
#define POP_BYTE(context) context->zpage[0x100 | ++context->reg_s]

struct cpu6280_context {
    cal_cpu cpu;
    int cycles_left;
    u8 *zpage; /* also stack */

    u16 pc;
    
    u8 reg_a;
    u8 reg_x;
    u8 reg_y;
    u8 reg_s;
    u8 flags;

    u8 mpr[8];

#ifdef LAZY_FLAG_EVALUATION
    u8 flag_n;
    u8 flag_z;
    u8 flag_v;
    u8 flag_c;
#endif
    
    memread8_t readfunc;
    memwrite8_t writefunc;
};

void cal_cpu6280_init(cal_cpu *cpu);
void cpu6280_step(struct cpu6280_context *context);
void cpu6280_op_0(struct cpu6280_context *context); /* unimplemented opcode */

#endif /* CPU6280INT_H */

/*
 * $Log: cpu6280int.h,v $
 * Revision 1.9  2000/09/09 18:11:05  nyef
 * fixed problem with non-lazy mode and operator precedence
 *
 * Revision 1.8  2000/09/09 16:01:29  nyef
 * converted to use lazy flag evaluation
 *
 * Revision 1.7  2000/01/23 23:03:13  nyef
 * added remaining interrupt vectors
 *
 * Revision 1.6  2000/01/22 17:29:38  nyef
 * added macros for stack access
 *
 * Revision 1.5  2000/01/21 03:19:29  nyef
 * added macro for NZ flag evaluation (EVAL_FLAGNZ)
 * added macro for opcode fetches (OPFETCH)
 *
 * Revision 1.4  2000/01/17 02:52:16  nyef
 * added #defines for the processor flags
 *
 * Revision 1.3  2000/01/17 01:05:30  nyef
 * added opcode handler table
 *
 * Revision 1.2  2000/01/16 22:01:36  nyef
 * added program counter and reset vector
 *
 * Revision 1.1  2000/01/16 21:32:24  nyef
 * Initial revision
 *
 */
