/*
 * emu68k.c
 *
 * emulation of the 68000
 */

/* $Id: emu68k.c,v 1.58 2001/02/25 22:05:30 nyef Exp $ */

#include <stdlib.h>
#include "cal.h"
#include "ui.h"
#include "emu68k.h"
#include "tool.h"
#include "types.h"

/*  #define USE_UNTESTED */

/*
 * Instruction handler interface
 */

typedef void (*m68k_instr_handler)(struct emu68k_context *, u16);

struct m68k_instr {
    u16 mask;
    u16 pattern;
    m68k_instr_handler handler;
    char *name;
};

struct m68k_instr *m68k_instrs[16];

/*
 * Register and memory access
 */

#define WRITE_D_REG_BYTE(context, reg, data) {context->regs_d[reg] &= ~0xff; context->regs_d[reg] |= data;}
#define WRITE_D_REG_WORD(context, reg, data) {context->regs_d[reg] &= ~0xffff; context->regs_d[reg] |= data;}
#define WRITE_D_REG_LONG(context, reg, data) {context->regs_d[reg] = data;}

u8 emu68k_read_8(struct emu68k_context *context, u32 addr)
{
    context->cycles_left -= 4;
    
    return context->read8table[(addr >> context->memshift)
			      & context->memmask](context->cpu, addr);
}

u16 emu68k_read_16(struct emu68k_context *context, u32 addr)
{
    /* FIXME: BUS error check goes here */

    context->cycles_left -= 4;
    
    return context->read16table[(addr >> context->memshift)
			       & context->memmask](context->cpu, addr);
}

u32 emu68k_read_32(struct emu68k_context *context, u32 addr)
{
    u16 tmp1;
    u16 tmp2;
    
    /* FIXME: BUS error check goes here */

    /* FIXME: check if this is the correct order */
    tmp1 = emu68k_read_16(context, addr);
    tmp2 = emu68k_read_16(context, addr + 2);

    return (tmp1 << 16) | tmp2;
}

void emu68k_write_8(struct emu68k_context *context, u32 addr, u8 data)
{
    context->cycles_left -= 4;
    
    context->write8table[(addr >> context->memshift)
			& context->memmask](context->cpu, addr, data);
}

void emu68k_write_16(struct emu68k_context *context, u32 addr, u16 data)
{
    /* FIXME: BUS error check goes here */
    
    context->cycles_left -= 4;
    
    context->write16table[(addr >> context->memshift)
			& context->memmask](context->cpu, addr, data);
}

void emu68k_write_32(struct emu68k_context *context, u32 addr, u32 data)
{
    /* FIXME: BUS error check goes here */

    /* FIXME: check if this is the correct order */
    emu68k_write_16(context, addr, data >> 16);
    emu68k_write_16(context, addr + 2, data & 0xffff);
}

/*
 * Main loop and support code
 */

u16 emu68k_opfetch_16(struct emu68k_context *context)
{
    u16 retval;

    retval = emu68k_read_16(context, context->pc);
    context->pc += 2;

    return retval;
}

u32 emu68k_opfetch_32(struct emu68k_context *context)
{
    u32 retval;

    retval = emu68k_read_32(context, context->pc);
    context->pc += 4;

    return retval;
}

void emu68k_step(struct emu68k_context *context)
{
    u16 opword;
    struct m68k_instr *cur_instr;
    
    if (context->pc & 1) {
	/* FIXME: bad things happen here */
    }
    opword = emu68k_opfetch_16(context);
    cur_instr = m68k_instrs[opword >> 12];
    while ((opword & cur_instr->mask) != cur_instr->pattern) {
	cur_instr++;
    }
    cur_instr->handler(context, opword);
}

void emu68k_run(struct emu68k_context *context)
{
    while (context->cycles_left > 0) {
	emu68k_step(context);

	if (system_flags & F_UNIMPL) {
	    return;
	}
    }
}

void emu68k_reset(struct emu68k_context *context)
{
    context->regs_a[7] = emu68k_read_32(context, 0);
    
    context->pc = emu68k_read_32(context, 4);

#if 0
    context->flags = 0x2000; /* FIXME: may be wrong */
#else
    emu68k_set_flags(context, 0x2000); /* FIXME: may be wrong */
#endif
}

/*
 * CAL interface
 */

void cal_torr68k_irq(cal_cpu cpu, int irqno);

void cal_junk68k_reset(cal_cpu cpu)
{
    emu68k_reset(cpu->data.d_junk68k);
}

void cal_junk68k_run(cal_cpu cpu)
{
/*      emu68k_step(cpu->data.d_junk68k); */
    emu68k_run(cpu->data.d_junk68k);
}

void cal_junk68k_runfor(cal_cpu cpu, int cycles)
{
    cpu->data.d_junk68k->cycles_left += cycles;
}

void cal_junk68k_irq(cal_cpu cpu, int irqno)
{
/*     emuz80_IRQ(cpu->data.d_emuz80); */
}

void cal_junk68k_nmi(cal_cpu cpu)
{
/*     emuz80_NMI(cpu->data.d_emuz80); */
}

void cal_junk68k_setzpage(cal_cpu cpu, void *page0)
{
}

int cal_junk68k_timeleft(cal_cpu cpu)
{
/*     return cpu->data.d_emuz80->cycles_left; */
    return 0;
}

void cal_junk68k_setmmu8(cal_cpu cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl)
{
    cpu->data.d_junk68k->memshift = shift;
    cpu->data.d_junk68k->memmask = mask;
    cpu->data.d_junk68k->read8table = rtbl;
    cpu->data.d_junk68k->write8table = wtbl;
}

void cal_junk68k_setmmu16(cal_cpu cpu, int shift, int mask, memread16_t *rtbl, memwrite16_t *wtbl)
{
    cpu->data.d_junk68k->read16table = rtbl;
    cpu->data.d_junk68k->write16table = wtbl;
}

void cal_junk68k_init(cal_cpu *cpu)
{
    (*cpu)->data.d_junk68k = calloc(1, sizeof(struct emu68k_context));
    if (!(*cpu)->data.d_junk68k) {
	deb_printf("Insufficient memory to create CPU.\n");
	free(*cpu);
	*cpu = NULL;
	return;
    }
    (*cpu)->reset = cal_junk68k_reset;
    (*cpu)->run = cal_junk68k_run;
    (*cpu)->runfor = cal_junk68k_runfor;
    (*cpu)->irq = cal_torr68k_irq;
    (*cpu)->nmi = cal_junk68k_nmi;
    (*cpu)->setzpage = cal_junk68k_setzpage;
    (*cpu)->timeleft = cal_junk68k_timeleft;
    (*cpu)->setmmu8 = cal_junk68k_setmmu8;
    (*cpu)->setmmu16 = cal_junk68k_setmmu16;
    (*cpu)->data.d_junk68k->cpu = *cpu;
/*     emuz80_init((*cpu)->data.d_junk68k); */
}

/*
 * Condition flag code and macros
 */

#define FLAG_NZ_byte(context, data) {context->flag_z = data; context->flag_n = data; context->flag_c = 0; context->flag_v = 0;}
#define FLAG_NZ_word(context, data) {context->flag_z = data; context->flag_n = data >> 8; context->flag_c = 0; context->flag_v = 0;}
#define FLAG_NZ_long(context, data) {context->flag_z = data; context->flag_n = data >> 24; context->flag_c = 0; context->flag_v = 0;}

/* FIXME: I don't understand these next few macros */

#define FLAG_VC_ADD_byte(context, dest, src1, src2) {context->flag_c = (dest < src1); context->flag_v = (((dest) ^ (src1)) & ((dest) ^ (src2)) & 0x80);}
#define FLAG_VC_ADD_word(context, dest, src1, src2) {context->flag_c = (dest < src1); context->flag_v = (((dest) ^ (src1)) & ((dest) ^ (src2)) & 0x8000);}
#define FLAG_VC_ADD_long(context, dest, src1, src2) {context->flag_c = (dest < src1); context->flag_v = (((dest) ^ (src1)) & ((dest) ^ (src2)) & 0x80000000);}

#define FLAG_VC_SUB_byte(context, dest, src1, src2) {context->flag_c = (src1 > src2); context->flag_v = (((dest) ^ (src1)) & ((src1) ^ (src2)) & 0x80);}
#define FLAG_VC_SUB_word(context, dest, src1, src2) {context->flag_c = (src1 > src2); context->flag_v = (((dest) ^ (src1)) & ((src1) ^ (src2)) & 0x8000);}
#define FLAG_VC_SUB_long(context, dest, src1, src2) {context->flag_c = (src1 > src2); context->flag_v = (((dest) ^ (src1)) & ((src1) ^ (src2)) & 0x80000000);}

#define FLAG_X(context) {context->flag_x = context->flag_c;}

u16 emu68k_get_flags(struct emu68k_context *context)
{
    u16 retval;

    retval = context->flags & ~0x001f;

    if (!context->flag_z) {
	retval |= 0x0004;
    }

    if (context->flag_n & 0x80) {
	retval |= 0x0008;
    }
    
    if (context->flag_c) {
	retval |= 0x0001;
    }

    if (context->flag_x) {
	retval |= 0x0010;
    }

    if (context->flag_v) {
	retval |= 0x0002;
    }
    
    return retval;
}

void emu68k_set_flags(struct emu68k_context *context, u16 flags)
{
    context->flags = flags & ~0x001f;

    context->flag_z = !(flags & 0x04);
    context->flag_n = flags << 4;
    context->flag_x = flags & 0x10;
    context->flag_c = flags & 0x01;
    context->flag_v = flags & 0x02;
}

/*
 * Addressing modes
 */

#define AMT_DATA      0x1
#define AMT_MEMORY    0x2
#define AMT_CONTROL   0x4
#define AMT_ALTERABLE 0x8

#define AMT_DA (AMT_DATA | AMT_ALTERABLE)
#define AMT_MA (AMT_MEMORY | AMT_ALTERABLE)
#define AMT_CA (AMT_CONTROL | AMT_ALTERABLE)

#define AMODE_IS(amode, am_type) ((amode.type & am_type) == am_type)

struct m68k_amode {
    u8 type;
    u32 data;
    u32 flags;
    int reg;
    struct emu68k_context *context;
    struct m68k_amode_api *api;
};

typedef void (*m68k_amode_resolver)(struct m68k_amode *, struct emu68k_context *, int);
typedef u8 (*m68k_amode_read_byte)(struct m68k_amode *);
typedef u16 (*m68k_amode_read_word)(struct m68k_amode *);
typedef u32 (*m68k_amode_read_long)(struct m68k_amode *);
typedef void (*m68k_amode_write_byte)(struct m68k_amode *, u8);
typedef void (*m68k_amode_write_word)(struct m68k_amode *, u16);
typedef void (*m68k_amode_write_long)(struct m68k_amode *, u32);
typedef void (*m68k_amode_after)(struct m68k_amode *);

struct m68k_amode_api {
    m68k_amode_resolver resolve;
    m68k_amode_read_byte readbyte;
    m68k_amode_read_word readword;
    m68k_amode_read_long readlong;
    m68k_amode_write_byte writebyte;
    m68k_amode_write_word writeword;
    m68k_amode_write_long writelong;
    m68k_amode_after after;
};

#define AMS_BOGUS 0
#define AMS_BYTE 1
#define AMS_WORD 2
#define AMS_LONG 3

void m68k_amode_none_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    deb_printf("m68k: unimplemented amode (api: 0x%08x).\n", amode->api);
    system_flags |= F_UNIMPL;
}

u8 m68k_control_amode_readbyte(struct m68k_amode *amode)
{
    return emu68k_read_8(amode->context, amode->data);
}

u16 m68k_control_amode_readword(struct m68k_amode *amode)
{
    return emu68k_read_16(amode->context, amode->data);
}

u32 m68k_control_amode_readlong(struct m68k_amode *amode)
{
    return emu68k_read_32(amode->context, amode->data);
}

void m68k_control_amode_writebyte(struct m68k_amode *amode, u8 data)
{
    emu68k_write_8(amode->context, amode->data, data);
}

void m68k_control_amode_writeword(struct m68k_amode *amode, u16 data)
{
    emu68k_write_16(amode->context, amode->data, data);
}

void m68k_control_amode_writelong(struct m68k_amode *amode, u32 data)
{
    emu68k_write_32(amode->context, amode->data, data);
}

void m68k_control_amode_after(struct m68k_amode *amode)
{
}

#define CONTROL_AMODE_API \
    m68k_control_amode_readbyte, \
    m68k_control_amode_readword, \
    m68k_control_amode_readlong, \
    m68k_control_amode_writebyte, \
    m68k_control_amode_writeword, \
    m68k_control_amode_writelong, \
    m68k_control_amode_after

void m68k_amode_0_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;
}

u8 m68k_amode_0_readbyte(struct m68k_amode *amode)
{
    return (u8)amode->context->regs_d[amode->reg];
}

u16 m68k_amode_0_readword(struct m68k_amode *amode)
{
    return (u16)amode->context->regs_d[amode->reg];
}

u32 m68k_amode_0_readlong(struct m68k_amode *amode)
{
    return amode->context->regs_d[amode->reg];
}

void m68k_amode_0_writebyte(struct m68k_amode *amode, u8 data)
{
    WRITE_D_REG_BYTE(amode->context, amode->reg, data);
}

void m68k_amode_0_writeword(struct m68k_amode *amode, u16 data)
{
    WRITE_D_REG_WORD(amode->context, amode->reg, data);
}

void m68k_amode_0_writelong(struct m68k_amode *amode, u32 data)
{
    WRITE_D_REG_LONG(amode->context, amode->reg, data);
}

void m68k_amode_0_after(struct m68k_amode *amode)
{
}

struct m68k_amode_api m68k_amode_api_0 = {
    m68k_amode_0_resolve,
    m68k_amode_0_readbyte,
    m68k_amode_0_readword,
    m68k_amode_0_readlong,
    m68k_amode_0_writebyte,
    m68k_amode_0_writeword,
    m68k_amode_0_writelong,
    m68k_amode_0_after,
};

void m68k_amode_1_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;
}

u8 m68k_amode_1_readbyte(struct m68k_amode *amode)
{
    /* FIXME: signal exception */
    return 0;
}

u16 m68k_amode_1_readword(struct m68k_amode *amode)
{
    return (u16)amode->context->regs_a[amode->reg];
}

u32 m68k_amode_1_readlong(struct m68k_amode *amode)
{
    return amode->context->regs_a[amode->reg];
}

void m68k_amode_1_writebyte(struct m68k_amode *amode, u8 data)
{
    /* FIXME: signal exception */
}

void m68k_amode_1_writeword(struct m68k_amode *amode, u16 data)
{
    amode->context->regs_a[amode->reg] &= ~0xffff;
    amode->context->regs_a[amode->reg] |= data;
}

void m68k_amode_1_writelong(struct m68k_amode *amode, u32 data)
{
    amode->context->regs_a[amode->reg] = data;
}

void m68k_amode_1_after(struct m68k_amode *amode)
{
}

struct m68k_amode_api m68k_amode_api_1 = {
    m68k_amode_1_resolve,
    m68k_amode_1_readbyte,
    m68k_amode_1_readword,
    m68k_amode_1_readlong,
    m68k_amode_1_writebyte,
    m68k_amode_1_writeword,
    m68k_amode_1_writelong,
    m68k_amode_1_after,
};

void m68k_amode_2_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;
    amode->data = context->regs_a[amode->reg];
}

struct m68k_amode_api m68k_amode_api_2 = {
    m68k_amode_2_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_3_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;
    if (size == AMS_BYTE) {
	if (amode->reg != 7) {
	    amode->flags = 1;
	} else {
	    amode->flags = 2;
	}
    } else if (size == AMS_WORD) {
	amode->flags = 2;
    } else if (size == AMS_LONG) {
	amode->flags = 4;
    } else {
	deb_printf("m68k: BUG: bogus size passed to amode 3.\n");
    }
    amode->data = context->regs_a[amode->reg];
}

u8 m68k_amode_3_readbyte(struct m68k_amode *amode)
{
    return emu68k_read_8(amode->context, amode->data);
}

u16 m68k_amode_3_readword(struct m68k_amode *amode)
{
    return emu68k_read_16(amode->context, amode->data);
}

u32 m68k_amode_3_readlong(struct m68k_amode *amode)
{
    return emu68k_read_32(amode->context, amode->data);
}

void m68k_amode_3_writebyte(struct m68k_amode *amode, u8 data)
{
    emu68k_write_8(amode->context, amode->data, data);
}

void m68k_amode_3_writeword(struct m68k_amode *amode, u16 data)
{
    emu68k_write_16(amode->context, amode->data, data);
}

void m68k_amode_3_writelong(struct m68k_amode *amode, u32 data)
{
    emu68k_write_32(amode->context, amode->data, data);
}

void m68k_amode_3_after(struct m68k_amode *amode)
{
    amode->context->regs_a[amode->reg] += amode->flags;
    amode->data += amode->flags;
}

struct m68k_amode_api m68k_amode_api_3 = {
    m68k_amode_3_resolve,
    m68k_amode_3_readbyte,
    m68k_amode_3_readword,
    m68k_amode_3_readlong,
    m68k_amode_3_writebyte,
    m68k_amode_3_writeword,
    m68k_amode_3_writelong,
    m68k_amode_3_after,
};

void m68k_amode_4_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    /* FIXME: this may not do the right thing for MOVE.L A7, -(A7) */
    amode->context = context;
    if (size == AMS_BYTE) {
	if (amode->reg != 7) {
	    amode->flags = 1;
	} else {
	    amode->flags = 2;
	}
    } else if (size == AMS_WORD) {
	amode->flags = 2;
    } else if (size == AMS_LONG) {
	amode->flags = 4;
    } else {
	deb_printf("m68k: BUG: bogus size passed to amode 4.\n");
    }
    amode->data = context->regs_a[amode->reg] - amode->flags;
}

u8 m68k_amode_4_readbyte(struct m68k_amode *amode)
{
    amode->context->cycles_left -= 2;
    
    return emu68k_read_8(amode->context, amode->data);
}

u16 m68k_amode_4_readword(struct m68k_amode *amode)
{
    amode->context->cycles_left -= 2;
    
    return emu68k_read_16(amode->context, amode->data);
}

u32 m68k_amode_4_readlong(struct m68k_amode *amode)
{
    amode->context->cycles_left -= 2;
    
    return emu68k_read_32(amode->context, amode->data);
}

void m68k_amode_4_writebyte(struct m68k_amode *amode, u8 data)
{
    emu68k_write_8(amode->context, amode->data, data);
}

void m68k_amode_4_writeword(struct m68k_amode *amode, u16 data)
{
    emu68k_write_16(amode->context, amode->data, data);
}

void m68k_amode_4_writelong(struct m68k_amode *amode, u32 data)
{
    emu68k_write_32(amode->context, amode->data, data);
}

void m68k_amode_4_after(struct m68k_amode *amode)
{
    amode->context->regs_a[amode->reg] -= amode->flags;
    amode->data = amode->context->regs_a[amode->reg] - amode->flags;
}

struct m68k_amode_api m68k_amode_api_4 = {
    m68k_amode_4_resolve,
    m68k_amode_4_readbyte,
    m68k_amode_4_readword,
    m68k_amode_4_readlong,
    m68k_amode_4_writebyte,
    m68k_amode_4_writeword,
    m68k_amode_4_writelong,
    m68k_amode_4_after,
};

void m68k_amode_5_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    s16 data;
    
    amode->context = context;
    amode->data = context->regs_a[amode->reg];

    data = emu68k_opfetch_16(context);

    amode->data += (signed long)data;
}

struct m68k_amode_api m68k_amode_api_5 = {
    m68k_amode_5_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_6_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    s16 data;

    amode->context = context;
    amode->data = context->regs_a[amode->reg];

    data = emu68k_opfetch_16(context);

    amode->data += (s8)data;

    if (data & 0x0800) {
	if (data & 0x8000) {
	    /* A.L */
	    amode->data += amode->context->regs_a[(data >> 12) & 7];
	} else {
	    /* D.L */
	    amode->data += amode->context->regs_d[(data >> 12) & 7];
	}
    } else {
	if (data & 0x8000) {
	    /* A.W */
	    amode->data += (s16)amode->context->regs_a[(data >> 12) & 7];
	} else {
	    /* D.W */
	    amode->data += (s16)amode->context->regs_d[(data >> 12) & 7];
	}
    }
    
    amode->context->cycles_left -= 2;
}

struct m68k_amode_api m68k_amode_api_6 = {
    m68k_amode_6_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_7_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    s16 tmp;
    
    amode->context = context;

    tmp = emu68k_opfetch_16(context);

    amode->data = tmp; /* needed for sign extension */
}

struct m68k_amode_api m68k_amode_api_7 = {
    m68k_amode_7_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_8_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;

    amode->data = emu68k_opfetch_32(context);
}

struct m68k_amode_api m68k_amode_api_8 = {
    m68k_amode_8_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_9_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    s16 data;
    
    amode->context = context;
    amode->data = context->pc; /* FIXME: may be too small by 2 */

    data = emu68k_opfetch_16(context);

    amode->data += (signed long)data;
}

struct m68k_amode_api m68k_amode_api_9 = {
    m68k_amode_9_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_10_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    s16 data;

    amode->context = context;
    amode->data = context->pc;

    data = emu68k_opfetch_16(context);

    amode->data += (s8)data;

    if (data & 0x0800) {
	if (data & 0x8000) {
	    /* A.L */
	    amode->data += amode->context->regs_a[(data >> 12) & 7];
	} else {
	    /* D.L */
	    amode->data += amode->context->regs_d[(data >> 12) & 7];
	}
    } else {
	if (data & 0x8000) {
	    /* A.W */
	    amode->data += (s16)amode->context->regs_a[(data >> 12) & 7];
	} else {
	    /* D.W */
	    amode->data += (s16)amode->context->regs_d[(data >> 12) & 7];
	}
    }
    
    context->cycles_left -= 2;
}

struct m68k_amode_api m68k_amode_api_10 = {
    m68k_amode_10_resolve,
    CONTROL_AMODE_API
};

void m68k_amode_11_resolve(struct m68k_amode *amode, struct emu68k_context *context, int size)
{
    amode->context = context;

    if ((size == AMS_BYTE) || (size == AMS_WORD)) {
	amode->data = emu68k_opfetch_16(context);
    } else if (size == AMS_LONG) {
	amode->data = emu68k_opfetch_32(context);
    } else {
	deb_printf("m68k: BUG: amode 11 reports illegal size %d.\n", size);
	amode->data = 0;
    }
}

u8 m68k_amode_11_readbyte(struct m68k_amode *amode)
{
    return amode->data & 0xff;
}

u16 m68k_amode_11_readword(struct m68k_amode *amode)
{
    return amode->data;
}

u32 m68k_amode_11_readlong(struct m68k_amode *amode)
{
    return amode->data;
}

void m68k_amode_11_writebyte(struct m68k_amode *amode, u8 data)
{
    deb_printf("m68k: CPU BUG: Write to non-alterable.\n");
    abort();
}

void m68k_amode_11_writeword(struct m68k_amode *amode, u16 data)
{
    deb_printf("m68k: CPU BUG: Write to non-alterable.\n");
    abort();
}

void m68k_amode_11_writelong(struct m68k_amode *amode, u32 data)
{
    deb_printf("m68k: CPU BUG: Write to non-alterable.\n");
    abort();
}

void m68k_amode_11_after(struct m68k_amode *amode)
{
}

struct m68k_amode_api m68k_amode_api_11 = {
    m68k_amode_11_resolve,
    m68k_amode_11_readbyte,
    m68k_amode_11_readword,
    m68k_amode_11_readlong,
    m68k_amode_11_writebyte,
    m68k_amode_11_writeword,
    m68k_amode_11_writelong,
    m68k_amode_11_after,
};

struct m68k_amode m68k_amodes[] = {
    {0x09, 0, 0, 0, NULL, &m68k_amode_api_0},  /* Dn */
    {0x08, 0, 0, 0, NULL, &m68k_amode_api_1},  /* An */
    {0x0f, 0, 0, 0, NULL, &m68k_amode_api_2},  /* (An) */
    {0x0b, 0, 0, 0, NULL, &m68k_amode_api_3},  /* (An)+ */
    {0x0b, 0, 0, 0, NULL, &m68k_amode_api_4},  /* -(An) */
    {0x0f, 0, 0, 0, NULL, &m68k_amode_api_5},  /* d(An) */
    {0x0f, 0, 0, 0, NULL, &m68k_amode_api_6},  /* d(An,Ri) */
    {0x0f, 0, 0, 0, NULL, &m68k_amode_api_7},  /* xxxx */
    {0x0f, 0, 0, 0, NULL, &m68k_amode_api_8},  /* xxxxxxxx */
    {0x07, 0, 0, 0, NULL, &m68k_amode_api_9},  /* d */
    {0x07, 0, 0, 0, NULL, &m68k_amode_api_10}, /* d(Ri) */
    {0x03, 0, 0, 0, NULL, &m68k_amode_api_11}, /* #xxxx */
    {0x00, 0, 0, 0, NULL, NULL},               /* <invalid> */
};

struct m68k_amode m68k_get_src_amode(u16 opword)
{
    struct m68k_amode retval;
    
    if ((opword & 0x38) == 0x38) {
	if ((opword & 7) > 4) {
	    retval = m68k_amodes[12];
	} else {
	    retval = m68k_amodes[7 + (opword & 7)];
	}
    } else {
	retval = m68k_amodes[(opword & 0x38) >> 3];
    }
    retval.reg = opword & 7;
    return retval;
}

struct m68k_amode m68k_get_dest_amode(u16 opword)
{
    struct m68k_amode retval;
    
    if ((opword & 0x1c0) == 0x1c0) {
	if ((opword & 0xe00) > 0x800) {
	    retval = m68k_amodes[12];
	} else {
	    retval = m68k_amodes[7 + ((opword & 0xe00) >> 9)];
	}
    } else {
	retval = m68k_amodes[(opword & 0x1c0) >> 6];
    }
    retval.reg = (opword & 0xe00) >> 9;
    return retval;
}

/*
 * Stack operations
 */

/* FIXME: Why do the word stack ops use long data? */

void m68k_push_word(struct emu68k_context *context, u32 data)
{
    context->regs_a[7] -= 2;
    emu68k_write_16(context, context->regs_a[7], data);
}

u32 m68k_pop_word(struct emu68k_context *context)
{
    u16 retval;

    retval = emu68k_read_16(context, context->regs_a[7]);
    context->regs_a[7] += 2;
    
    return retval;
}

void m68k_push_long(struct emu68k_context *context, u32 data)
{
    context->regs_a[7] -= 4;
    emu68k_write_32(context, context->regs_a[7], data);
}

u32 m68k_pop_long(struct emu68k_context *context)
{
    u32 retval;

    retval = emu68k_read_32(context, context->regs_a[7]);
    context->regs_a[7] += 4;
    
    return retval;
}

/*
 * Opcode handlers
 */

#define BEGIN_OPHANDLER(instr) \
void m68k_do_##instr(struct emu68k_context *context, u16 opword)

void m68k_do_unimp(struct emu68k_context *context, u16 opword)
{
    deb_printf("emu68k: unimplemented opcode 0x%04hx at 0x%06x.\n", opword, context->pc);
    system_flags |= F_UNIMPL;
    /* FIXME: signal invalid opcode */
}

void m68k_do_alinetrap(struct emu68k_context *context, u16 opword)
{
    deb_printf("emu68k: A-Line trap 0x%04hx at 0x%06x.\n", opword, context->pc);
    /* FIXME: signal a-line trap */
}

void m68k_do_flinetrap(struct emu68k_context *context, u16 opword)
{
    deb_printf("emu68k: F-Line trap 0x%04hx at 0x%06x.\n", opword, context->pc);
    /* FIXME: signal f-line trap */
}

#define AMODE_CHECK_DA(side) \
    side = m68k_get_##side##_amode(opword); \
    if (!AMODE_IS(side, AMT_DA)) { m68k_do_unimp(context, opword); return; }

#define AMODE_CHECK_MA(side) \
    side = m68k_get_##side##_amode(opword); \
    if (!AMODE_IS(side, AMT_MA)) { m68k_do_unimp(context, opword); return; }

#define AMODE_CHECK_DATA(side) \
    side = m68k_get_##side##_amode(opword); \
    if (!AMODE_IS(side, AMT_DA)) { m68k_do_unimp(context, opword); return; }

#define AMODE_CHECK_ANY(side) \
    side = m68k_get_##side##_amode(opword); \
    if (!side.type) { m68k_do_unimp(context, opword); return; }


#define ALU_FLAG_VC_TEST_NONE(size)
#define ALU_FLAG_VC_TEST_CMP(size) \
    FLAG_VC_SUB_##size(context, data3, data2, data);
#define ALU_FLAG_VC_TEST_SUB(size) \
    FLAG_VC_SUB_##size(context, data3, data2, data); FLAG_X(context);
#define ALU_FLAG_VC_TEST_ADD(size) \
    FLAG_VC_ADD_##size(context, data3, data2, data); FLAG_X(context);


#define ALU_IMMED_FUNCTION(instr, size1, size2, size3, size4, op, vc_test, cycle_penalty, amode) \
BEGIN_OPHANDLER(instr##_##size1) { \
    size2 data; size2 data2; size2 data3; struct m68k_amode src; \
    AMODE_CHECK_##amode(src); \
    data2 = emu68k_opfetch_##size4(context); \
    src.api->resolve(&src, context, AMS_##size3); \
    data = src.api->read##size1(&src); \
    src.api->after(&src); \
    data3 = data op data2; \
    ALU_IMMED_WRITE_##amode(size1); \
    FLAG_NZ_##size1(context, data3); \
    ALU_FLAG_VC_TEST_##vc_test(size1); \
    ALU_IMMED_CYCLE_ADJUST_##size3(cycle_penalty); }

#define ALU_IMMED_CYCLE_ADJUST_BYTE(cycles)
#define ALU_IMMED_CYCLE_ADJUST_WORD(cycles)
#define ALU_IMMED_CYCLE_ADJUST_LONG(cycles) \
    if (src.api == &m68k_amode_api_0) { \
	context->cycles_left -= cycles; }

#define ALU_IMMED_WRITE_DA(size) \
    src.api->write##size(&src, data3);
#define ALU_IMMED_WRITE_DATA(size)

ALU_IMMED_FUNCTION(SUBI, byte, u8, BYTE, 16, -, SUB, 4, DA);
ALU_IMMED_FUNCTION(SUBI, word, u16, WORD, 16, -, SUB, 4, DA);
ALU_IMMED_FUNCTION(SUBI, long, u32, LONG, 32, -, SUB, 4, DA);

ALU_IMMED_FUNCTION(ADDI, byte, u8, BYTE, 16, +, ADD, 4, DA);
ALU_IMMED_FUNCTION(ADDI, word, u16, WORD, 16, +, ADD, 4, DA);
ALU_IMMED_FUNCTION(ADDI, long, u32, LONG, 32, +, ADD, 4, DA);

ALU_IMMED_FUNCTION(ANDI, byte, u8, BYTE, 16, &, NONE, 2, DA);
ALU_IMMED_FUNCTION(ANDI, word, u16, WORD, 16, &, NONE, 2, DA);
ALU_IMMED_FUNCTION(ANDI, long, u32, LONG, 32, &, NONE, 2, DA);

ALU_IMMED_FUNCTION(ORI, byte, u8, BYTE, 16, |, NONE, 4, DA);
ALU_IMMED_FUNCTION(ORI, word, u16, WORD, 16, |, NONE, 4, DA);
ALU_IMMED_FUNCTION(ORI, long, u32, LONG, 32, |, NONE, 4, DA);

ALU_IMMED_FUNCTION(EORI, byte, u8, BYTE, 16, ^, NONE, 4, DA);
ALU_IMMED_FUNCTION(EORI, word, u16, WORD, 16, ^, NONE, 4, DA);
ALU_IMMED_FUNCTION(EORI, long, u32, LONG, 32, ^, NONE, 4, DA);

ALU_IMMED_FUNCTION(CMPI, byte, u8, BYTE, 16, -, CMP, 2, DATA);
ALU_IMMED_FUNCTION(CMPI, word, u16, WORD, 16, -, CMP, 2, DATA);
ALU_IMMED_FUNCTION(CMPI, long, u32, LONG, 32, -, CMP, 2, DATA);


#define ALU_REG_FUNCTION(instr, size1, size2, size3, op, vc_test, amode, is_rmw) \
BEGIN_OPHANDLER(instr##_reg_##size1) { \
    size2 data; size2 data2; size2 data3; struct m68k_amode src; \
    AMODE_CHECK_##amode(src); \
    src.api->resolve(&src, context, AMS_##size3); \
    data = context->regs_d[(opword >> 9) & 7]; \
    data2 = src.api->read##size1(&src); \
    data3 = data op data2; \
    FLAG_NZ_##size1(context, data3); \
    ALU_REG_WRITE_##is_rmw(size3); \
    src.api->after(&src); \
    ALU_FLAG_VC_TEST_##vc_test(size1); \
    ALU_REG_CYCLE_ADJUST_##size3; }

#define ALU_REG_WRITE_YES(size) \
    WRITE_D_REG_##size(context, (opword >> 9) & 7, data3)
#define ALU_REG_WRITE_NO(size)

#define ALU_REG_CYCLE_ADJUST_BYTE
#define ALU_REG_CYCLE_ADJUST_WORD
#define ALU_REG_CYCLE_ADJUST_LONG \
    if ((src.type == 3) || ((src.type & 0xe) == 8)) { /* reg direct, immed */ \
	context->cycles_left -= 4; } else { context->cycles_left -= 2; }

ALU_REG_FUNCTION(AND, byte, u8, BYTE, &, NONE, DATA, YES);
ALU_REG_FUNCTION(AND, word, u16, WORD, &, NONE, DATA, YES);
ALU_REG_FUNCTION(AND, long, u32, LONG, &, NONE, DATA, YES);

ALU_REG_FUNCTION(OR, byte, u8, BYTE, |, NONE, DATA, YES);
ALU_REG_FUNCTION(OR, word, u16, WORD, |, NONE, DATA, YES);
ALU_REG_FUNCTION(OR, long, u32, LONG, |, NONE, DATA, YES);

ALU_REG_FUNCTION(ADD, byte, u8, BYTE, +, ADD, ANY, YES);
ALU_REG_FUNCTION(ADD, word, u16, WORD, +, ADD, ANY, YES);
ALU_REG_FUNCTION(ADD, long, u32, LONG, +, ADD, ANY, YES);

ALU_REG_FUNCTION(SUB, byte, u8, BYTE, -, SUB, ANY, YES);
ALU_REG_FUNCTION(SUB, word, u16, WORD, -, SUB, ANY, YES);
ALU_REG_FUNCTION(SUB, long, u32, LONG, -, SUB, ANY, YES);

ALU_REG_FUNCTION(CMP, byte, u8, BYTE, -, CMP, DATA, NO);
ALU_REG_FUNCTION(CMP, word, u16, WORD, -, CMP, DATA, NO);
ALU_REG_FUNCTION(CMP, long, u32, LONG, -, CMP, DATA, NO);


#define ALU_MEM_CYCLE_ADJUST_MA(size)
#define ALU_MEM_CYCLE_ADJUST_DA(size) \
    ALU_MEM_CYCLE_ADJUST_DA_##size

#define ALU_MEM_CYCLE_ADJUST_DA_BYTE
#define ALU_MEM_CYCLE_ADJUST_DA_WORD
#define ALU_MEM_CYCLE_ADJUST_DA_LONG \
    if (src.type == 9) { /* Dn */ context->cycles_left -= 4; }

#define ALU_MEM_FUNCTION(instr, size1, size2, size3, op, vc_test, amode) \
BEGIN_OPHANDLER(instr##_mem_##size1) { \
    size2 data; size2 data2; size2 data3; struct m68k_amode src; \
    AMODE_CHECK_##amode(src); \
    src.api->resolve(&src, context, AMS_##size3); \
    data2 = context->regs_d[(opword >> 9) & 7]; \
    data = src.api->read##size1(&src); \
    data3 = data op data2; \
    FLAG_NZ_##size1(context, data3); \
    src.api->write##size1(&src, data3); \
    src.api->after(&src); \
    ALU_FLAG_VC_TEST_##vc_test(size1); \
    ALU_MEM_CYCLE_ADJUST_##amode(size3); }


ALU_MEM_FUNCTION(AND, byte, u8, BYTE, &, NONE, MA);
ALU_MEM_FUNCTION(AND, word, u16, WORD, &, NONE, MA);
ALU_MEM_FUNCTION(AND, long, u32, LONG, &, NONE, MA);

ALU_MEM_FUNCTION(OR, byte, u8, BYTE, |, NONE, MA);
ALU_MEM_FUNCTION(OR, word, u16, WORD, |, NONE, MA);
ALU_MEM_FUNCTION(OR, long, u32, LONG, |, NONE, MA);

ALU_MEM_FUNCTION(ADD, byte, u8, BYTE, +, ADD, MA);
ALU_MEM_FUNCTION(ADD, word, u16, WORD, +, ADD, MA);
ALU_MEM_FUNCTION(ADD, long, u32, LONG, +, ADD, MA);

ALU_MEM_FUNCTION(SUB, byte, u8, BYTE, -, SUB, MA);
ALU_MEM_FUNCTION(SUB, word, u16, WORD, -, SUB, MA);
ALU_MEM_FUNCTION(SUB, long, u32, LONG, -, SUB, MA);

ALU_MEM_FUNCTION(EOR, byte, u8, BYTE, ^, NONE, DA);
ALU_MEM_FUNCTION(EOR, word, u16, WORD, ^, NONE, DA);
ALU_MEM_FUNCTION(EOR, long, u32, LONG, ^, NONE, DA);


void m68k_do_MOVE_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    struct m68k_amode dest;
    u8 data;
    
    src = m68k_get_src_amode(opword);
    dest = m68k_get_dest_amode(opword);

    if (!(AMODE_IS(dest, AMT_DA) && (src.type))) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_BYTE);
    dest.api->resolve(&dest, context, AMS_BYTE);
    data = src.api->readbyte(&src);
    dest.api->writebyte(&dest, data);
    src.api->after(&src);
    dest.api->after(&dest);
    FLAG_NZ_byte(context, data);
}

void m68k_do_MOVE_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    struct m68k_amode dest;
    u16 data;
    
    src = m68k_get_src_amode(opword);
    dest = m68k_get_dest_amode(opword);

    if (!(AMODE_IS(dest, AMT_DA) && (src.type))) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_WORD);
    dest.api->resolve(&dest, context, AMS_WORD);
    data = src.api->readword(&src);
    dest.api->writeword(&dest, data);
    src.api->after(&src);
    dest.api->after(&dest);
    FLAG_NZ_word(context, data);
}

void m68k_do_MOVEA_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    s32 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    data = (s16)src.api->readword(&src);
    src.api->after(&src);
    
    context->regs_a[(opword >> 9) & 7] = data;
}

void m68k_do_MOVE_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    struct m68k_amode dest;
    u32 data;
    
    src = m68k_get_src_amode(opword);
    dest = m68k_get_dest_amode(opword);

    if (!(AMODE_IS(dest, AMT_DA) && (src.type))) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_LONG);
    dest.api->resolve(&dest, context, AMS_LONG);
    data = src.api->readlong(&src);
    dest.api->writelong(&dest, data);
    src.api->after(&src);
    dest.api->after(&dest);
    FLAG_NZ_long(context, data);
}

void m68k_do_MOVEA_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_LONG);
    data = src.api->readlong(&src);
    src.api->after(&src);
    
    context->regs_a[(opword >> 9) & 7] = data;
}

void m68k_do_CLR_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BYTE);
    src.api->writebyte(&src, 0);
    src.api->after(&src);
    FLAG_NZ_byte(context, 0);
}

void m68k_do_CLR_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    src.api->writeword(&src, 0);
    src.api->after(&src);
    FLAG_NZ_word(context, 0);
}

void m68k_do_CLR_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_LONG);
    src.api->writelong(&src, 0);
    src.api->after(&src);
    FLAG_NZ_long(context, 0);

    context->cycles_left -= 2;
}

void m68k_do_TST_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BYTE);
    data = src.api->readbyte(&src);
    src.api->after(&src);
    FLAG_NZ_byte(context, data);
}

void m68k_do_TST_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    src.api->after(&src);
    FLAG_NZ_word(context, data);
}

void m68k_do_TST_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_LONG);
    data = src.api->readlong(&src);
    src.api->after(&src);
    FLAG_NZ_long(context, data);
}

void m68k_do_NOT_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BYTE);
    data = src.api->readbyte(&src);
    data ^= 0xff;
    src.api->writebyte(&src, data);
    src.api->after(&src);
    FLAG_NZ_byte(context, data);
}

void m68k_do_NOT_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    data ^= 0xffff;
    src.api->writeword(&src, data);
    src.api->after(&src);
    FLAG_NZ_word(context, data);
}

void m68k_do_NOT_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_LONG);
    data = src.api->readlong(&src);
    data ^= 0xffffffff;
    src.api->writelong(&src, data);
    src.api->after(&src);
    FLAG_NZ_long(context, data);
    context->cycles_left -= 2;
}

void m68k_do_TAS(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BYTE);
    data = src.api->readbyte(&src);
    src.api->writebyte(&src, data | 0x80);
    src.api->after(&src);
    FLAG_NZ_byte(context, data);
}

void m68k_handle_Bcc(struct emu68k_context *context, u16 opword, int take_branch)
{
    s16 offset;
    
    if (!take_branch) {
	if (!(opword & 0xff)) {
	    offset = emu68k_opfetch_16(context);
	}
	context->cycles_left -= 4;
    } else {
	offset = emu68k_opfetch_16(context);
	context->pc -= 2; /* cheating, but what can you do? */

	if (opword & 0xff) {
	    offset = (s8)opword;
	}

	context->pc += offset;
	context->cycles_left -= 2;
    }
}

void m68k_do_BRA(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, 1);
}

void m68k_do_BHI(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (!context->flag_c) && context->flag_z);
}

void m68k_do_BLS(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (context->flag_c) || (!context->flag_z));
}

void m68k_do_BCC(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, !context->flag_c);
}

void m68k_do_BCS(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, context->flag_c);
}

void m68k_do_BVC(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, !context->flag_v);
}

void m68k_do_BVS(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, context->flag_v);
}

void m68k_do_BNE(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, context->flag_z);
}

void m68k_do_BEQ(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, !context->flag_z);
}

void m68k_do_BGE(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (context->flag_n & 0x80)? context->flag_v: !context->flag_v);
}

void m68k_do_BLT(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (context->flag_n & 0x80)? !context->flag_v: context->flag_v);
}

void m68k_do_BGT(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (!context->flag_z)? 0: (context->flag_n & 0x80)? context->flag_v: !context->flag_v);
}

void m68k_do_BLE(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, (!context->flag_z)? 1: (context->flag_n & 0x80)? !context->flag_v: context->flag_v);
}

void m68k_do_BPL(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, !(context->flag_n & 0x80));
}

void m68k_do_BMI(struct emu68k_context *context, u16 opword)
{
    m68k_handle_Bcc(context, opword, context->flag_n & 0x80);
}

void m68k_do_BSR(struct emu68k_context *context, u16 opword)
{
    s16 offset;

    if (opword & 0xff) {
	offset = ((s8)opword);

	m68k_push_long(context, context->pc);
    } else {
	offset = emu68k_opfetch_16(context);

	m68k_push_long(context, context->pc);
	
	context->pc -= 2;
    }
    
    context->pc += offset;
}

void m68k_do_LEA(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_CONTROL)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BOGUS);
    context->regs_a[(opword >> 9) & 7] = src.data;
    src.api->after(&src);
}

void m68k_do_PEA(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_CONTROL)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BOGUS);
    m68k_push_long(context, src.data);
    src.api->after(&src);
}

void m68k_do_MOVEQ(struct emu68k_context *context, u16 opword)
{
    s32 data;
    
    data = (s8)(opword & 0xff); /* NOTE: this had better work */
    WRITE_D_REG_LONG(context, (opword >> 9) & 7, data);
    FLAG_NZ_long(context, data);
}

void m68k_do_DBF(struct emu68k_context *context, u16 opword)
{
    s16 offset;
    u16 tmp;
    u32 new_pc;

    new_pc = context->pc;
    offset = emu68k_opfetch_16(context);
    new_pc += offset;
    
    tmp = context->regs_d[opword & 7] & 0xffff;
    tmp--;
    WRITE_D_REG_WORD(context, opword & 7, tmp);
    
    if (tmp == 0xffff) {
	emu68k_read_16(context, new_pc); /* Stupid pipeline */
    } else {
	context->pc = new_pc;
    }
    
    context->cycles_left -= 2;
}

void m68k_do_SF(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BYTE);
    src.api->writebyte(&src, 0);
    src.api->after(&src);
}

void m68k_do_BTST_dyn(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 tmp;
    u32 data;

    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DATA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    tmp = context->regs_d[(opword >> 9) & 7];
    
    if (src.api == &m68k_amode_api_0) {
	src.api->resolve(&src, context, AMS_LONG);
	data = src.api->readlong(&src);
	tmp &= 0x1f;
    } else {
	src.api->resolve(&src, context, AMS_BYTE);
	data = src.api->readbyte(&src);
	tmp &= 7;
    }
    src.api->after(&src);
    
    context->flag_z = (data & (1 << tmp));

    context->cycles_left -= 2;
}

void m68k_do_BTST_stat(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 tmp;
    u32 data;

    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DATA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    tmp = emu68k_opfetch_16(context);
    
    if (src.api == &m68k_amode_api_0) {
	src.api->resolve(&src, context, AMS_LONG);
	data = src.api->readlong(&src);
	tmp &= 0x1f;
    } else {
	src.api->resolve(&src, context, AMS_BYTE);
	data = src.api->readbyte(&src);
	tmp &= 7;
    }
    src.api->after(&src);

    context->flag_z = (data & (1 << tmp));
}

void m68k_do_NEG_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data2;
    u8 data3;

    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_BYTE);
    data2 = src.api->readbyte(&src);
    
    data3 = 0 - data2;
    FLAG_NZ_byte(context, data3);
    FLAG_VC_SUB_byte(context, data3, data2, 0);
    FLAG_X(context);
    
    src.api->writebyte(&src, data3);
    src.api->after(&src);
}

void m68k_do_NEG_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data2;
    u16 data3;

    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_WORD);
    data2 = src.api->readword(&src);
    
    data3 = 0 - data2;
    FLAG_NZ_word(context, data3);
    FLAG_VC_SUB_word(context, data3, data2, 0);
    FLAG_X(context);
    
    src.api->writeword(&src, data3);
    src.api->after(&src);
}

void m68k_do_NEG_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data2;
    u32 data3;

    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_LONG);
    data2 = src.api->readlong(&src);
    
    data3 = 0 - data2;
    FLAG_NZ_long(context, data3);
    FLAG_VC_SUB_long(context, data3, data2, 0);
    FLAG_X(context);
    
    src.api->writelong(&src, data3);
    src.api->after(&src);

    context->cycles_left -= 2; /* FIXME: should only be for register mode */
}

void m68k_do_SUBA_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    s16 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    src.api->after(&src);
    context->regs_a[(opword >> 9) & 7] -= data;
    context->cycles_left -= 4;
}

void m68k_do_SUBA_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_LONG);
    data = src.api->readlong(&src);
    src.api->after(&src);
    context->regs_a[(opword >> 9) & 7] -= data;
    context->cycles_left -= 4;
}

void m68k_do_SUBQ_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data;
    u8 data2;
    u8 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data2 = (opword >> 9) & 7;
    if (!data2) {
	data2 = 8;
    }
    src.api->resolve(&src, context, AMS_BYTE);
    data = src.api->readbyte(&src);
    data3 = data - data2;
    FLAG_NZ_byte(context, data3);
    FLAG_VC_SUB_byte(context, data3, data2, data);
    FLAG_X(context);
    src.api->writebyte(&src, data3);
    src.api->after(&src);
}

void m68k_do_SUBQ_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;
    u16 data2;
    u16 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_ALTERABLE)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data2 = (opword >> 9) & 7;
    if (!data2) {
	data2 = 8;
    }
    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    data3 = data - data2;
    FLAG_NZ_word(context, data3);
    FLAG_VC_SUB_word(context, data3, data2, data);
    FLAG_X(context);
    src.api->writeword(&src, data3);
    src.api->after(&src);
}

void m68k_do_SUBQ_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    u32 data2;
    u32 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_ALTERABLE)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data2 = (opword >> 9) & 7;
    if (!data2) {
	data2 = 8;
    }
    src.api->resolve(&src, context, AMS_LONG);
    data = src.api->readlong(&src);
    data3 = data - data2;
    FLAG_NZ_long(context, data3);
    FLAG_VC_SUB_long(context, data3, data2, data);
    FLAG_X(context);
    src.api->writelong(&src, data3);
    src.api->after(&src);
    
    context->cycles_left -= 4;
}

void m68k_do_SUBQ_An(struct emu68k_context *context, u16 opword)
{
    u32 data;
    
    data = (opword >> 9) & 7;
    if (!data) {
	data = 8;
    }
    context->regs_a[opword & 7] -= data;
    context->cycles_left -= 4;
}

void m68k_do_ADDQ_byte(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u8 data;
    u8 data2;
    u8 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    data = (opword >> 9) & 7;
    if (!data) {
	data = 8;
    }
    src.api->resolve(&src, context, AMS_BYTE);
    data2 = src.api->readbyte(&src);
    data3 = data2 + data;
    FLAG_NZ_byte(context, data3);
    FLAG_VC_ADD_byte(context, data3, data2, data);
    FLAG_X(context);
    src.api->writebyte(&src, data3);
    src.api->after(&src);
}

void m68k_do_ADDQ_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;
    u16 data2;
    u16 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_ALTERABLE)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data = (opword >> 9) & 7;
    if (!data) {
	data = 8;
    }
    src.api->resolve(&src, context, AMS_WORD);
    data2 = src.api->readword(&src);
    data3 = data2 + data;
    FLAG_NZ_word(context, data3);
    FLAG_VC_ADD_word(context, data3, data2, data);
    FLAG_X(context);
    src.api->writeword(&src, data3);
    src.api->after(&src);
}

void m68k_do_ADDQ_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    u32 data2;
    u32 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_ALTERABLE)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data = (opword >> 9) & 7;
    if (!data) {
	data = 8;
    }
    src.api->resolve(&src, context, AMS_LONG);
    data2 = src.api->readlong(&src);
    data3 = data2 + data;
    FLAG_NZ_long(context, data3);
    FLAG_VC_ADD_long(context, data3, data2, data);
    FLAG_X(context);
    src.api->writelong(&src, data3);
    src.api->after(&src);
}

void m68k_do_ADDQ_An(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    u32 data2;
    u32 data3;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_ALTERABLE)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    data = (opword >> 9) & 7;
    if (!data) {
	data = 8;
    }
    src.api->resolve(&src, context, AMS_LONG);
    data2 = src.api->readlong(&src);
    data3 = data2 + data;
    src.api->writelong(&src, data3);
    src.api->after(&src);
}

void m68k_do_MOVE_to_CCR(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;
    
    src = m68k_get_src_amode(opword);
    
    if (!AMODE_IS(src, AMT_DATA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    src.api->after(&src);

    data &= 0x001f;
    data |= emu68k_get_flags(context) & 0xffe0;
    emu68k_set_flags(context, data);

    context->cycles_left -= 8;
}

void m68k_do_MOVE_to_SR(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 data;

    src = m68k_get_src_amode(opword);
    
    if (!AMODE_IS(src, AMT_DATA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    if (!(context->flags & 0x2000)) {
	/* FIXME: raise TRAP */
	deb_printf("m68k_do_MOVE_to_SR(): TRAP!\n");
	return;
    }

    src.api->resolve(&src, context, AMS_WORD);
    data = src.api->readword(&src);
    src.api->after(&src);
    emu68k_set_flags(context, data);
    if (!(data & 0x2000)) {
	u32 data2;
	data2 = context->regs_a[7];
	context->regs_a[7] = context->other_sp;
	context->other_sp = data2;
    }
    context->cycles_left -= 8;
}

void m68k_do_MOVE_from_SR(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }

    src.api->resolve(&src, context, AMS_WORD);
    src.api->writeword(&src, emu68k_get_flags(context));
    src.api->after(&src);

    context->cycles_left -= 2;
}

void m68k_do_MOVEM_RM_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u16 reglist;
    u16 data;
    int i;
    
    src = m68k_get_src_amode(opword);

    if (!(AMODE_IS(src, AMT_DA) || (src.api == &m68k_amode_api_4))) {
	m68k_do_unimp(context, opword);
	return;
    }

    reglist = emu68k_opfetch_16(context);
    src.api->resolve(&src, context, AMS_WORD);
    if (src.api == &m68k_amode_api_4) {
	for (i = 0; i < 16; i++) {
	    if (reglist & (1 << i)) {
		if (i & 8) {
		    data = context->regs_d[(7 - i) & 7];
		} else {
		    data = context->regs_a[(7 - i) & 7];
		}
		src.api->writeword(&src, data);
		src.api->after(&src);
	    }
	}
    } else {
	for (i = 0; i < 16; i++) {
	    if (reglist & (1 << i)) {
		if (i & 8) {
		    data = context->regs_a[i & 7];
		} else {
		    data = context->regs_d[i & 7];
		}
		src.api->writeword(&src, data);
		src.api->after(&src);
		if (src.api != &m68k_amode_api_3) {
		    src.data += 2;
		}
	    }
	}
    }
}

void m68k_do_MOVEM_RM_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    u16 reglist;
    int i;

    src = m68k_get_src_amode(opword);

    if (!(AMODE_IS(src, AMT_DA) || (src.api == &m68k_amode_api_4))) {
	m68k_do_unimp(context, opword);
	return;
    }

    reglist = emu68k_opfetch_16(context);
    src.api->resolve(&src, context, AMS_LONG);
    if (src.api == &m68k_amode_api_4) {
	for (i = 0; i < 16; i++) {
	    if (reglist & (1 << i)) {
		if (i & 8) {
		    data = context->regs_d[(7 - i) & 7];
		} else {
		    data = context->regs_a[(7 - i) & 7];
		}
		src.api->writelong(&src, data);
		src.api->after(&src);
	    }
	}
    } else {
	for (i = 0; i < 16; i++) {
	    if (reglist & (1 << i)) {
		if (i & 8) {
		    data = context->regs_a[i & 7];
		} else {
		    data = context->regs_d[i & 7];
		}
		src.api->writelong(&src, data);
		src.api->after(&src);
		if (src.api != &m68k_amode_api_3) {
		    src.data += 4;
		}
	    }
	}
    }
}

void m68k_do_MOVEM_MR_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    s16 data;
    u16 reglist;
    int i;
    
    src = m68k_get_src_amode(opword);

    if (!(AMODE_IS(src, AMT_CONTROL) || (src.api == &m68k_amode_api_3))) {
	m68k_do_unimp(context, opword);
	return;
    }

    reglist = emu68k_opfetch_16(context);
    src.api->resolve(&src, context, AMS_WORD);
    for (i = 0; i < 16; i++) {
	if (reglist & (1 << i)) {
	    data = src.api->readword(&src);
	    if (i & 8) {
		context->regs_a[i & 7] = data;
	    } else {
		context->regs_d[i & 7] = data;
	    }
	    src.api->after(&src);
	    if (src.api != &m68k_amode_api_3) {
		src.data += 2;
	    }
	}
    }
}

void m68k_do_MOVEM_MR_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    u32 data;
    u16 reglist;
    int i;
    
    src = m68k_get_src_amode(opword);

    if (!(AMODE_IS(src, AMT_CONTROL) || (src.api == &m68k_amode_api_3))) {
	m68k_do_unimp(context, opword);
	return;
    }

    reglist = emu68k_opfetch_16(context);
    src.api->resolve(&src, context, AMS_LONG);
    for (i = 0; i < 16; i++) {
	if (reglist & (1 << i)) {
	    data = src.api->readlong(&src);
	    if (i & 8) {
		context->regs_a[i & 7] = data;
	    } else {
		context->regs_d[i & 7] = data;
	    }
	    src.api->after(&src);
	    if (src.api != &m68k_amode_api_3) {
		src.data += 4;
	    }
	}
    }
}

void m68k_do_MOVE_to_USP(struct emu68k_context *context, u16 opword)
{
    if (!context->flags & 0x2000) {
	/* FIXME: raise TRAP */
	deb_printf("m68k_do_MOVE_to_USP(): TRAP!\n");
	return;
    }
    
    context->other_sp = context->regs_a[opword & 7];
}

void m68k_do_MOVE_from_USP(struct emu68k_context *context, u16 opword)
{
    if (!context->flags & 0x2000) {
	/* FIXME: raise TRAP */
	deb_printf("m68k_do_MOVE_from_USP(): TRAP!\n");
	return;
    }
    
    context->regs_a[opword & 7] = context->other_sp;
}

void m68k_do_ADDA_word(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    s32 data;
    s16 data2;
    s32 data3;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = context->regs_a[(opword >> 9) & 7];
    data2 = src.api->readword(&src);
    data3 = data2 + data;
    context->regs_a[(opword >> 9) & 7] = data3;
    
    src.api->after(&src);
    
    context->cycles_left -= 4;
}

void m68k_do_ADDA_long(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    s32 data;
    s32 data2;
    s32 data3;
    
    src = m68k_get_src_amode(opword);

    if (!src.type) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_LONG);
    
    data = context->regs_a[(opword >> 9) & 7];
    data2 = src.api->readlong(&src);
    data3 = data2 + data;
    context->regs_a[(opword >> 9) & 7] = data3;
    
    src.api->after(&src);
    
    context->cycles_left -= 4;
}

void m68k_do_RTS(struct emu68k_context *context, u16 opword)
{
    context->pc = m68k_pop_long(context);
    context->cycles_left -= 4;
}

void m68k_do_RTE(struct emu68k_context *context, u16 opword)
{
    u16 new_flags;
    
    if (!(context->flags & 0x2000)) {
	/* FIXME: raise TRAP */
	deb_printf("m68k_do_MOVE_from_USP(): TRAP!\n");
	return;
    }
    
    new_flags = m68k_pop_word(context);
    context->pc = m68k_pop_long(context);
    if ((new_flags ^ context->flags) & 0x2000) {
	u32 tmp;
	
	tmp = context->regs_a[7];
	context->regs_a[7] = context->other_sp;
	context->other_sp = tmp;
    }
    emu68k_set_flags(context, new_flags);
    context->cycles_left -= 4;
}

void m68k_do_SWAP(struct emu68k_context *context, u16 opword)
{
    int reg;
    u32 data;

    reg = opword & 7;
    data = context->regs_d[reg];
    data = (data << 16) | (data >> 16);
    context->regs_d[reg] = data;

    FLAG_NZ_long(context, data);
}

void m68k_do_RESET(struct emu68k_context *context, u16 opword)
{
    /* FIXME: We should probably do something here. */
    deb_printf("emu68k: RESET.\n");
}

void m68k_do_NOP(struct emu68k_context *context, u16 opword)
{
}

void m68k_do_JMP(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_CONTROL)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BOGUS);
    context->pc = src.data;
    src.api->after(&src);
    context->cycles_left -= 4;
}

void m68k_do_JSR(struct emu68k_context *context, u16 opword)
{
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_CONTROL)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_BOGUS);
    m68k_push_long(context, context->pc);
    context->pc = src.data;
    src.api->after(&src);
    context->cycles_left -= 4;
}

void m68k_do_LINK(struct emu68k_context *context, u16 opword)
{
    s16 tmp;

    tmp = emu68k_opfetch_16(context);

    m68k_push_long(context, context->regs_a[opword & 7]);
    context->regs_a[opword & 7] = context->regs_a[7];
    context->regs_a[7] += tmp; /* FIXME: make sure this sign-extends */
}

void m68k_do_UNLK(struct emu68k_context *context, u16 opword)
{
    context->regs_a[7] = context->regs_a[opword & 7];
    context->regs_a[opword & 7] = m68k_pop_long(context);
}

#define GET_SHIFT_COUNT(count, opword) \
    if (opword & 0x0020) {             \
        count = context->regs_d[(opword >> 9) & 7] & 0x3f; \
    } else {                           \
        data = (opword >> 9) & 7;      \
        if (!data) {                   \
            data = 8;                  \
        }                              \
    }

#define SHIFT_FUNCTION(instr, size1, size2, size3, cycles) \
BEGIN_OPHANDLER(instr##_r_##size1) { \
    size2 data; size2 data2; size2 data3; \
    GET_SHIFT_COUNT(data, opword); \
    data2 = context->regs_d[opword & 7]; \
    instr##_OPERATION(size3); \
    FLAG_NZ_##size1(context, data3); \
    instr##_FLAG_C_CHECK(size3); \
    instr##_FLAG_X_CHECK; \
    WRITE_D_REG_##size3(context, opword & 7, data3); \
    context->cycles_left -= cycles + (data + data); \
}

#define HIGHBIT_BYTE 0x80
#define HIGHBIT_WORD 0x8000
#define HIGHBIT_LONG 0x80000000

#define HIGHBYTE_BYTE 0xff
#define HIGHBYTE_WORD 0xff00
#define HIGHBYTE_LONG 0xff000000

#define NUMBITS_BYTE 8
#define NUMBITS_WORD 16
#define NUMBITS_LONG 32

#define LSL_OPERATION(width) \
    data3 = data2 << data

#define LSL_FLAG_X_CHECK \
    if (data) { FLAG_X(context); }

#define LSL_FLAG_C_CHECK(width) \
    if (data2 & (HIGHBIT_##width >> (data - 1))) { \
        context->flag_c = 1; }

SHIFT_FUNCTION(LSL, byte, u8, BYTE, 2);
SHIFT_FUNCTION(LSL, word, u16, WORD, 2);
SHIFT_FUNCTION(LSL, long, u32, LONG, 4);


#define LSR_OPERATION(width) \
    data3 = data2 >> data

#define LSR_FLAG_X_CHECK LSL_FLAG_X_CHECK

#define LSR_FLAG_C_CHECK(width) \
    if (data2 & (1 << (data - 1))) { \
	context->flag_c = 1; }

SHIFT_FUNCTION(LSR, byte, u8, BYTE, 2);
SHIFT_FUNCTION(LSR, word, u16, WORD, 2);
SHIFT_FUNCTION(LSR, long, u32, LONG, 4);


#define ASR_OPERATION(width) \
    data3 = data2 >> data; \
    if (data2 & HIGHBIT_##width) { \
	data3 |= (HIGHBYTE_##width << (8 - data)); }

#define ASR_FLAG_X_CHECK LSL_FLAG_X_CHECK

#define ASR_FLAG_C_CHECK(width) LSR_FLAG_C_CHECK(width)

SHIFT_FUNCTION(ASR, byte, u8, BYTE, 2);
SHIFT_FUNCTION(ASR, word, u16, WORD, 2);
SHIFT_FUNCTION(ASR, long, u32, LONG, 4);


#define ROL_OPERATION(width) \
    data3 = data2 << data; \
    data3 |= data2 >> (NUMBITS_##width - data);

#define ROL_FLAG_X_CHECK

#define ROL_FLAG_C_CHECK(width) \
    context->flag_c = data3 & 1;

SHIFT_FUNCTION(ROL, byte, u8, BYTE, 2);
SHIFT_FUNCTION(ROL, word, u16, WORD, 2);
SHIFT_FUNCTION(ROL, long, u32, LONG, 4);


#define ROR_OPERATION(width) \
    data3 = data2 >> data; \
    data3 |= data2 << (NUMBITS_##width - data);

#define ROR_FLAG_X_CHECK

#define ROR_FLAG_C_CHECK(width) \
    context->flag_c = data2 & (1 << (data - 1));

SHIFT_FUNCTION(ROR, byte, u8, BYTE, 2);
SHIFT_FUNCTION(ROR, word, u16, WORD, 2);
SHIFT_FUNCTION(ROR, long, u32, LONG, 4);


#define ROXL_OPERATION(width) \
    data3 = data2 << data; \
    data3 |= data2 >> ((NUMBITS_##width + 1) - data); \
    if (context->flag_x) { data3 |= 1 << (data - 1); }

#define ROXL_FLAG_C_CHECK(width) \
    if (data2 & (HIGHBIT_##width >> (data - 1))) { \
	context->flag_c = 1; }

#define ROXL_FLAG_X_CHECK \
    if (data) { FLAG_X(context); \
    } else { context->flag_c = context->flag_x; }

SHIFT_FUNCTION(ROXL, byte, u8, BYTE, 2);
SHIFT_FUNCTION(ROXL, word, u16, WORD, 2);

/* Workaround for x86 masking shift count to 5 bits */
#undef ROXL_OPERATION
#define ROXL_OPERATION(width) \
    data3 = data2 << data; \
    if (data == 1) { data3 |= data2 >> (33 - data); } \
    if (data && context->flag_x) { data3 |= 1 << (data - 1); }

SHIFT_FUNCTION(ROXL, long, u32, LONG, 4);


#define ROXR_OPERATION(width) \
    data3 = data2 >> data; \
    data3 |= data2 << ((NUMBITS_##width + 1) - data); \
    if (context->flag_x) { data3 |= (HIGHBIT_##width >> (data - 1)); }

#define ROXR_FLAG_C_CHECK(width) \
    if (data) { context->flag_x = data2 & (1 << (data - 1)); } \
    context->flag_c = context->flag_x;

#define ROXR_FLAG_X_CHECK

SHIFT_FUNCTION(ROXR, byte, u8, BYTE, 2);
SHIFT_FUNCTION(ROXR, word, u16, WORD, 2);

/* workaround for the x86's mod 32 shift instruction */
#undef ROXR_OPERATION
#define ROXR_OPERATION(width) \
    data3 = data2 >> data; \
    if (data > 1) { data3 |= data2 << (33 - data); } \
    if (context->flag_x) { data3 |= 0x80000000 >> (data - 1); }

SHIFT_FUNCTION(ROXR, long, u32, LONG, 4);


void m68k_do_LSL_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 << data;
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (0x8000 >> (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_LSR_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 >> data;
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (1 << (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_ASR_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 >> data;
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (1 << (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_ROL_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 << data;
    data3 |= data2 >> (16 - data);
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (0x8000 >> (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_ROR_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 >> data;
    data3 |= data2 << (16 - data);
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (1 << (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_ROXL_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 << data;
    data3 |= data2 >> (15 - data);
    if (context->flags & 0x10) {
	data3 |= 1 << (data - 1);
    }
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (0x8000 >> (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

void m68k_do_ROXR_m(struct emu68k_context *context, u16 opword)
{
    u16 data;
    u16 data2;
    u16 data3;
    struct m68k_amode src;
    
    src = m68k_get_src_amode(opword);

    if (!AMODE_IS(src, AMT_DA)) {
	m68k_do_unimp(context, opword);
	return;
    }
    
    src.api->resolve(&src, context, AMS_WORD);
    
    data = 1;
    data2 = src.api->readword(&src);
    data3 = data2 >> data;
    data3 |= data2 << (15 - data);
    if (context->flags & 0x10) {
	data3 |= 0x8000 >> (data - 1);
    }
    
    FLAG_NZ_word(context, data3);
    
    if (data2 & (1 << (data - 1))) {
	context->flags |= 0x11;
    }
    
    src.api->writeword(&src, data3);
    
    src.api->after(&src);
}

/*
 * Decode tables
 */

struct m68k_instr m68k_grp_0[] = {
    {0x0fc0, 0x0000, m68k_do_ORI_byte, "ORI.B"},
    {0x0fc0, 0x0040, m68k_do_ORI_word, "ORI.W"},
    {0x0fc0, 0x0080, m68k_do_ORI_long, "ORI.L"},
    {0x0fc0, 0x0200, m68k_do_ANDI_byte, "ANDI.B"},
    {0x0fc0, 0x0240, m68k_do_ANDI_word, "ANDI.W"},
    {0x0fc0, 0x0280, m68k_do_ANDI_long, "ANDI.L"},
    {0x0fc0, 0x0400, m68k_do_SUBI_byte, "SUBI.B"},
    {0x0fc0, 0x0440, m68k_do_SUBI_word, "SUBI.W"},
    {0x0fc0, 0x0480, m68k_do_SUBI_long, "SUBI.L"},
    {0x0fc0, 0x0600, m68k_do_ADDI_byte, "ADDI.B"},
    {0x0fc0, 0x0640, m68k_do_ADDI_word, "ADDI.W"},
    {0x0fc0, 0x0680, m68k_do_ADDI_long, "ADDI.L"},
    {0x0fc0, 0x0800, m68k_do_BTST_stat, "BTST-S"},
    {0x0fc0, 0x0a00, m68k_do_EORI_byte, "EORI.B"},
    {0x0fc0, 0x0a40, m68k_do_EORI_word, "EORI.W"},
    {0x0fc0, 0x0a80, m68k_do_EORI_long, "EORI.L"},
    {0x0fc0, 0x0c00, m68k_do_CMPI_byte, "CMPI.B"},
    {0x0fc0, 0x0c40, m68k_do_CMPI_word, "CMPI.W"},
    {0x0fc0, 0x0c80, m68k_do_CMPI_long, "CMPI.L"},
    {0x01c0, 0x0100, m68k_do_BTST_dyn, "BTST-D"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_1[] = {
    {0x0000, 0x0000, m68k_do_MOVE_byte, "MOVE.B"},
};

struct m68k_instr m68k_grp_2[] = {
    {0x01c0, 0x0040, m68k_do_MOVEA_long, "MOVEA.L"},
    {0x0000, 0x0000, m68k_do_MOVE_long, "MOVE.L"},
};

struct m68k_instr m68k_grp_3[] = {
    {0x01c0, 0x0040, m68k_do_MOVEA_word, "MOVEA.W"},
    {0x0000, 0x0000, m68k_do_MOVE_word, "MOVE.W"},
};

struct m68k_instr m68k_grp_4[] = {
    {0x0fff, 0x0e70, m68k_do_RESET, "RESET"},
    {0x0fff, 0x0e71, m68k_do_NOP, "NOP"},
    {0x0fff, 0x0e73, m68k_do_RTE, "RTE"},
    {0x0fff, 0x0e75, m68k_do_RTS, "RTS"},
    {0x0ff8, 0x0e50, m68k_do_LINK, "LINK"},
    {0x0ff8, 0x0e58, m68k_do_UNLK, "UNLK"},
    {0x0ff8, 0x0e60, m68k_do_MOVE_to_USP, "MOVE to USP"},
#ifdef USE_UNTESTED
    {0x0ff8, 0x0e68, m68k_do_MOVE_from_USP, "MOVE from USP"},
#endif
    {0x0fc0, 0x00c0, m68k_do_MOVE_from_SR, "MOVE from SR"},
    {0x0fc0, 0x0200, m68k_do_CLR_byte, "CLR.B"},
    {0x0fc0, 0x0240, m68k_do_CLR_word, "CLR.W"},
    {0x0fc0, 0x0280, m68k_do_CLR_long, "CLR.L"},
    {0x0fc0, 0x04c0, m68k_do_MOVE_to_CCR, "MOVE to CCR"},
    {0x0fc0, 0x0600, m68k_do_NOT_byte, "NOT.B"},
    {0x0fc0, 0x0640, m68k_do_NOT_word, "NOT.W"},
    {0x0fc0, 0x0680, m68k_do_NOT_long, "NOT.L"},
    {0x0fc0, 0x06c0, m68k_do_MOVE_to_SR, "MOVE to SR"},
    {0x0fc0, 0x0880, m68k_do_MOVEM_RM_word, "MOVEM.W"},
    {0x0fc0, 0x08c0, m68k_do_MOVEM_RM_long, "MOVEM.L"},
    {0x0fc0, 0x0a00, m68k_do_TST_byte, "TST.B"},
    {0x0fc0, 0x0a40, m68k_do_TST_word, "TST.W"},
    {0x0fc0, 0x0a80, m68k_do_TST_long, "TST.L"},
#ifdef USE_UNTESTED
    {0x0fc0, 0x0ac0, m68k_do_TAS, "TAS"},
#endif
    {0x0fc0, 0x0c80, m68k_do_MOVEM_MR_word, "MOVEM.W"},
    {0x0fc0, 0x0cc0, m68k_do_MOVEM_MR_long, "MOVEM.L"},
    {0x0fc0, 0x0e80, m68k_do_JSR, "JSR"},
    {0x0fc0, 0x0ec0, m68k_do_JMP, "JMP"},
    {0x01c0, 0x01c0, m68k_do_LEA, "LEA"},
    {0x0ff8, 0x0840, m68k_do_SWAP, "SWAP"},
    {0x0fc0, 0x0840, m68k_do_PEA, "PEA"},
    {0x0fc0, 0x0400, m68k_do_NEG_byte, "NEG.B"},
    {0x0fc0, 0x0440, m68k_do_NEG_word, "NEG.W"},
    {0x0fc0, 0x0480, m68k_do_NEG_long, "NEG.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_5[] = {
    /* FIXME: DBcc instructions come first */
    {0x0ff8, 0x01c8, m68k_do_DBF, "DBF"},
#ifdef USE_UNTESTED
    {0x0fc0, 0x01c0, m68k_do_SF, "SF"},
#endif
    {0x01c0, 0x0000, m68k_do_ADDQ_byte, "ADDQ.B"},
    {0x01f8, 0x0048, m68k_do_ADDQ_An, "ADDQ.W"},
    {0x01c0, 0x0040, m68k_do_ADDQ_word, "ADDQ.W"},
    {0x01f8, 0x0088, m68k_do_ADDQ_An, "ADDQ.L"},
    {0x01c0, 0x0080, m68k_do_ADDQ_long, "ADDQ.L"},
    {0x01c0, 0x0100, m68k_do_SUBQ_byte, "SUBQ.B"},
    {0x01f8, 0x0148, m68k_do_SUBQ_An, "SUBQ.W"},
    {0x01c0, 0x0140, m68k_do_SUBQ_word, "SUBQ.W"},
    {0x01f8, 0x0188, m68k_do_SUBQ_An, "SUBQ.L"},
    {0x01c0, 0x0180, m68k_do_SUBQ_long, "SUBQ.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_6[] = {
    {0x0f00, 0x0000, m68k_do_BRA, "BRA"},
    {0x0f00, 0x0100, m68k_do_BSR, "BSR"},
    {0x0f00, 0x0200, m68k_do_BHI, "BHI"},
    {0x0f00, 0x0300, m68k_do_BLS, "BLS"},
    {0x0f00, 0x0400, m68k_do_BCC, "BCC"},
    {0x0f00, 0x0500, m68k_do_BCS, "BCS"},
    {0x0f00, 0x0600, m68k_do_BNE, "BNE"},
    {0x0f00, 0x0700, m68k_do_BEQ, "BEQ"},
    {0x0f00, 0x0800, m68k_do_BVC, "BVC"},
    {0x0f00, 0x0900, m68k_do_BVS, "BVS"},
    {0x0f00, 0x0a00, m68k_do_BPL, "BPL"},
    {0x0f00, 0x0b00, m68k_do_BMI, "BMI"},
    {0x0f00, 0x0c00, m68k_do_BGE, "BGE"},
    {0x0f00, 0x0d00, m68k_do_BLT, "BLT"},
    {0x0f00, 0x0e00, m68k_do_BGT, "BGT"},
    {0x0f00, 0x0f00, m68k_do_BLE, "BLE"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_7[] = {
    {0x0100, 0x0000, m68k_do_MOVEQ, "MOVEQ"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_8[] = {
    {0x01c0, 0x0000, m68k_do_OR_reg_byte, "OR.B"},
    {0x01c0, 0x0040, m68k_do_OR_reg_word, "OR.W"},
    {0x01c0, 0x0080, m68k_do_OR_reg_long, "OR.L"},
    {0x01c0, 0x0100, m68k_do_OR_mem_byte, "OR.B"},
    {0x01c0, 0x0140, m68k_do_OR_mem_word, "OR.W"},
    {0x01c0, 0x0180, m68k_do_OR_mem_long, "OR.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_9[] = {
    {0x01c0, 0x0000, m68k_do_SUB_reg_byte, "SUB.B"},
    {0x01c0, 0x0040, m68k_do_SUB_reg_word, "SUB.W"},
    {0x01c0, 0x0080, m68k_do_SUB_reg_long, "SUB.L"},
    {0x01c0, 0x00c0, m68k_do_SUBA_word, "SUBA.W"},
    {0x01c0, 0x0100, m68k_do_SUB_mem_byte, "SUB.B"},
    {0x01c0, 0x0140, m68k_do_SUB_mem_word, "SUB.W"},
    {0x01c0, 0x0180, m68k_do_SUB_mem_long, "SUB.L"},
    {0x01c0, 0x01c0, m68k_do_SUBA_long, "SUBA.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_a[] = {
    {0x0000, 0x0000, m68k_do_alinetrap, "A-TRAP"},
};

struct m68k_instr m68k_grp_b[] = {
    {0x01c0, 0x0000, m68k_do_CMP_reg_byte, "CMP.B"},
    {0x01c0, 0x0040, m68k_do_CMP_reg_word, "CMP.W"},
    {0x01c0, 0x0080, m68k_do_CMP_reg_long, "CMP.L"},
    {0x01c0, 0x0100, m68k_do_EOR_mem_byte, "EOR.B"},
    {0x01c0, 0x0140, m68k_do_EOR_mem_word, "EOR.W"},
    {0x01c0, 0x0180, m68k_do_EOR_mem_long, "EOR.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_c[] = {
    {0x01c0, 0x0000, m68k_do_AND_reg_byte, "AND.B"},
    {0x01c0, 0x0040, m68k_do_AND_reg_word, "AND.W"},
    {0x01c0, 0x0080, m68k_do_AND_reg_long, "AND.L"},
    {0x01c0, 0x0100, m68k_do_AND_mem_byte, "AND.B"},
    {0x01c0, 0x0140, m68k_do_AND_mem_word, "AND.W"},
    {0x01c0, 0x0180, m68k_do_AND_mem_long, "AND.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_d[] = {
    {0x01c0, 0x0000, m68k_do_ADD_reg_byte, "ADD.B"},
    {0x01c0, 0x0040, m68k_do_ADD_reg_word, "ADD.W"},
    {0x01c0, 0x0080, m68k_do_ADD_reg_long, "ADD.L"},
    {0x01c0, 0x00c0, m68k_do_ADDA_word, "ADDA.W"},
    {0x01c0, 0x0100, m68k_do_ADD_mem_byte, "ADD.B"},
    {0x01c0, 0x0140, m68k_do_ADD_mem_word, "ADD.W"},
    {0x01c0, 0x0180, m68k_do_ADD_mem_long, "ADD.L"},
    {0x01c0, 0x01c0, m68k_do_ADDA_long, "ADDA.L"},
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_e[] = {
    {0x1d8, 0x0000, m68k_do_ASR_r_byte},
    {0x1d8, 0x0040, m68k_do_ASR_r_word},
    {0x1d8, 0x0080, m68k_do_ASR_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x00c0, m68k_do_ASR_m},
#endif
    {0x1d0, 0x0100, m68k_do_LSL_r_byte}, /* also ASL */
    {0x1d0, 0x0140, m68k_do_LSL_r_word}, /* also ASL */
    {0x1d0, 0x0180, m68k_do_LSL_r_long}, /* also ASL */
#ifdef USE_UNTESTED
    {0xdc0, 0x01c0, m68k_do_LSL_m}, /* also ASL */
#endif
    {0x1d8, 0x0008, m68k_do_LSR_r_byte},
    {0x1d8, 0x0048, m68k_do_LSR_r_word},
    {0x1d8, 0x0088, m68k_do_LSR_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x02c0, m68k_do_LSR_m},
#endif
    {0x1d8, 0x0010, m68k_do_ROXR_r_byte},
    {0x1d8, 0x0050, m68k_do_ROXR_r_word},
    {0x1d8, 0x0090, m68k_do_ROXR_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x04c0, m68k_do_ROXR_m},
#endif
    {0x1d8, 0x0110, m68k_do_ROXL_r_byte},
    {0x1d8, 0x0150, m68k_do_ROXL_r_word},
    {0x1d8, 0x0190, m68k_do_ROXL_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x05c0, m68k_do_ROXL_m},
#endif
    {0x1d8, 0x0018, m68k_do_ROR_r_byte},
    {0x1d8, 0x0058, m68k_do_ROR_r_word},
    {0x1d8, 0x0098, m68k_do_ROR_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x06c0, m68k_do_ROR_m},
#endif
    {0x1d8, 0x0118, m68k_do_ROL_r_byte},
    {0x1d8, 0x0158, m68k_do_ROL_r_word},
    {0x1d8, 0x0198, m68k_do_ROL_r_long},
#ifdef USE_UNTESTED
    {0xfc0, 0x07c0, m68k_do_ROL_m},
#endif
    {0x0000, 0x0000, m68k_do_unimp, "<unimp>"},
};

struct m68k_instr m68k_grp_f[] = {
    {0x0000, 0x0000, m68k_do_flinetrap, "F-TRAP"},
};

struct m68k_instr *m68k_instrs[16] = {
    m68k_grp_0, m68k_grp_1, m68k_grp_2, m68k_grp_3,
    m68k_grp_4, m68k_grp_5, m68k_grp_6, m68k_grp_7,
    m68k_grp_8, m68k_grp_9, m68k_grp_a, m68k_grp_b,
    m68k_grp_c, m68k_grp_d, m68k_grp_e, m68k_grp_f,
};

/*
 * $Log: emu68k.c,v $
 * Revision 1.58  2001/02/25 22:05:30  nyef
 * moved CAL interface code in from cal.c
 *
 * Revision 1.57  2000/12/11 18:13:17  nyef
 * rewrote the register and memory ALU instructions with much less code
 * (also, it is to be hoped, fewer bugs)
 *
 * Revision 1.56  2000/12/10 02:40:12  nyef
 * rewrote the 0x0xxx immediate instructions with much less code
 *
 * Revision 1.55  2000/12/10 01:33:34  nyef
 * rewrote the shift/rotate instructions with much less code
 *
 * Revision 1.54  2000/12/10 00:07:21  nyef
 * moved some code around and added a few section comments
 * made all control amodes use the same accessor routines
 *
 * Revision 1.53  2000/12/08 20:49:02  nyef
 * added dummy implementation of the RESET instruction
 *
 * Revision 1.52  2000/06/04 16:52:51  nyef
 * fixed and enabled all shift/rotate immediate, reg instructions
 *
 * Revision 1.51  2000/04/28 00:50:37  nyef
 * added SWAP and NEG instructions
 *
 * Revision 1.50  2000/04/23 18:07:46  nyef
 * added ADDA.L instruction
 *
 * Revision 1.49  2000/04/23 17:39:08  nyef
 * added PEA instruction
 *
 * Revision 1.48  2000/04/23 14:51:06  nyef
 * fixed immediate instructions in the 0x0xxx range (adding EORI)
 *
 * Revision 1.47  2000/04/23 03:07:25  nyef
 * fixed ADD instructions in the 0xdxxx range
 *
 * Revision 1.46  2000/04/23 02:43:27  nyef
 * fixed SUB instructions in the 0x9xxx range
 *
 * Revision 1.45  2000/04/23 01:48:48  nyef
 * fixed and enabled all AND and OR instructions in banks 0x8000 and 0x9000
 *
 * Revision 1.44  2000/04/22 22:52:58  nyef
 * added remaining conditional branch instructions
 *
 * Revision 1.43  2000/03/18 21:10:45  nyef
 * unified all writes to D registers behind a set of macros
 *
 * Revision 1.42  2000/03/18 19:45:00  nyef
 * delayed "V" flag evaluation
 *
 * Revision 1.41  2000/03/18 19:23:19  nyef
 * cleaned up a bit of duplication in the shift/rotate instructions
 *
 * Revision 1.40  2000/03/18 18:49:12  nyef
 * delayed "C" and "X" flag evaluation
 *
 * Revision 1.39  2000/03/18 18:27:13  nyef
 * cleaned up a "big oops" with the new flag interface
 *
 * Revision 1.38  2000/03/18 18:20:43  nyef
 * delayed "N" flag evaluation
 *
 * Revision 1.37  2000/03/18 18:13:25  nyef
 * added a new flag get/set interface
 * changed flag handling to use the new flag interface
 *
 * Revision 1.36  2000/03/04 22:45:23  nyef
 * added BHI and BLE instructions
 *
 * Revision 1.35  2000/01/14 04:39:44  nyef
 * added an emu68k_run() function to handle cycle-counted operation
 *
 * Revision 1.34  2000/01/14 03:56:51  nyef
 * fixed and enabled CMPI, ADDQ, and SUBQ instructions
 *
 * Revision 1.33  2000/01/12 03:22:59  nyef
 * fixed and enabled the ROXL.[BWL] ??, Dn instructions
 *
 * Revision 1.32  2000/01/09 19:41:58  nyef
 * fixed and enabled some more shift instructions
 * changed the decoding on LSL instructions to cover ASL as well
 *
 * Revision 1.31  1999/12/27 00:58:52  nyef
 * made it easy to re-enable all disabled instructions at once
 * fixed and enabled a handful more instructions
 *
 * Revision 1.30  1999/12/25 05:13:04  nyef
 * debugged and enabled CLR.B, CLR.W, ADDQ.W, AND.L, AND.L, ADD.L, LSR.W,
 *     LSR.L, and LSL.W
 *
 * Revision 1.29  1999/12/19 00:02:38  nyef
 * enabled BCC, BCS, CMP.W, and CMP.L
 *
 * Revision 1.28  1999/12/18 23:31:21  nyef
 * enabled the remaining MOVEM instructions
 *
 * Revision 1.27  1999/12/18 23:01:07  nyef
 * enabled LINK and UNLK instructions
 * fixed 16-bit write cycle cost
 *
 * Revision 1.26  1999/12/18 22:15:12  nyef
 * cleaned up a lot of stuff in the opcode handlers
 *
 * Revision 1.25  1999/12/18 20:31:06  nyef
 * fixed SUBQ.B, SUBQ.W, and SUBQ.[WL] An
 *
 * Revision 1.24  1999/12/18 18:54:55  nyef
 * disabled any instruction there is no test case for
 * added more cycle counting code
 * cleaned up some debug output
 * cleaned up an opfetch
 *
 * Revision 1.23  1999/12/18 04:24:27  nyef
 * cleaned up some stuff with the MOVEA instructions
 *
 * Revision 1.22  1999/12/18 04:18:44  nyef
 * fixed sign extension in MOVEA.W emulation
 *
 * Revision 1.21  1999/12/18 04:08:52  nyef
 * added SUB.B and SUB.W instruction emulation
 *
 * Revision 1.20  1999/12/18 03:05:50  nyef
 * added ADDA.W instruction emulation
 *
 * Revision 1.19  1999/12/18 02:43:02  nyef
 * changed most variable declarations to use the new type interface
 *
 * Revision 1.18  1999/12/18 01:52:30  nyef
 * changed ADD instructions to evaluate the X flag
 *
 * Revision 1.17  1999/12/17 02:48:20  nyef
 * added BPL and BMI instructions
 *
 * Revision 1.16  1999/12/17 02:21:15  nyef
 * added EOR instruction emulation
 *
 * Revision 1.15  1999/12/17 01:45:22  nyef
 * fixed a typo in m68k_do_unimp()
 *
 * Revision 1.14  1999/12/16 05:03:43  nyef
 * fixed cycle timing and other suchwhat in the DBF instruction
 *
 * Revision 1.13  1999/12/16 04:47:57  nyef
 * refactored and added cycle counts to the Bcc family of instructions
 *
 * Revision 1.12  1999/12/15 03:49:04  nyef
 * added implementation of the NOT instructions
 *
 * Revision 1.11  1999/12/15 03:06:25  nyef
 * fixed a major bug in the DBF instruction
 *
 * Revision 1.10  1999/12/15 02:32:31  nyef
 * fixed a bug in the stack pop long routine
 * added more cycle timing data
 *
 * Revision 1.9  1999/12/15 01:56:55  nyef
 * fixed a bug in the new 32-bit write routine
 *
 * Revision 1.8  1999/12/15 00:56:28  nyef
 * changed the write sequence cycle count to reflect the
 *   official motorola docu
 *
 * Revision 1.7  1999/12/13 03:14:38  nyef
 * fixed address calculation for amode 10 (PC relative indexed)
 * added more cycle counting code
 *
 * Revision 1.6  1999/12/12 21:49:44  nyef
 * created procedures to handle opword fetches
 * changed several opfetch operations to use the new opfetch code
 * added more cycle counting code
 *
 * Revision 1.5  1999/12/12 17:28:13  nyef
 * created procedures to handle reading and writing memory
 * changed a lot of memory accesses to use the new memory access code
 * started adding code for cycle counting
 *
 * Revision 1.4  1999/09/08 01:05:06  nyef
 * added numerous instructions, cleaned up numerous bugs
 *
 * Revision 1.3  1999/08/07 15:37:45  nyef
 * added the final amode, some debugging stuff, and one less bug
 *
 * Revision 1.2  1999/06/19 18:44:27  nyef
 * added preliminary implementation of amode 6 (register indirect index)
 *
 * Revision 1.1  1999/04/17 22:24:55  nyef
 * Initial revision
 *
 */
