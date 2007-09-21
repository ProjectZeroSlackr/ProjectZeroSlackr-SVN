/*
 * emu68k.h
 *
 * emulation of the 68000
 */

/* $Id: emu68k.h,v 1.7 2000/03/18 19:45:12 nyef Exp $ */

#ifndef EMU68K_H
#define EMU68K_H

#include "types.h"

struct emu68k_context {
    u32 regs_a[8];
    u32 regs_d[8];
    u32 other_sp;
    u32 pc;
    u16 flags;
    u8 flag_n;
    u8 flag_c;
    u8 flag_x;
    u32 flag_z;
    u32 flag_v;
    int cycles_left;
    memread8_t *read8table;
    memwrite8_t *write8table;
    memread16_t *read16table;
    memwrite16_t *write16table;
    int memshift;
    u32 memmask;
    cal_cpu cpu;
};

void emu68k_step(struct emu68k_context *context);
void emu68k_run(struct emu68k_context *context);
void emu68k_reset(struct emu68k_context *context);

u16 emu68k_get_flags(struct emu68k_context *context);
void emu68k_set_flags(struct emu68k_context *context, u16 flags);

#endif /* EMU68K_H */

/*
 * $Log: emu68k.h,v $
 * Revision 1.7  2000/03/18 19:45:12  nyef
 * added "V" flag cache
 *
 * Revision 1.6  2000/03/18 18:48:52  nyef
 * added "C" and "X" flag caches
 *
 * Revision 1.5  2000/03/18 18:19:56  nyef
 * added an "N" flag cache
 *
 * Revision 1.4  2000/03/18 18:12:50  nyef
 * added an interface for getting/setting the CPU flags
 *
 * Revision 1.3  2000/01/14 04:40:16  nyef
 * added prototype for emu68k_run() (for cycle-counted operation)
 *
 * Revision 1.2  1999/12/12 17:38:48  nyef
 * added support for cycle counting
 *
 * Revision 1.1  1999/04/17 22:24:58  nyef
 * Initial revision
 *
 */
