/*
 * cpu6280opc.c
 *
 * opcode handlers for cpu6280 core
 *
 * MACHINE GENERATED. DO NOT EDIT.
 */

#include "cpu6280int.h"

void cpu6280_op_1(struct cpu6280_context *context)
{
    /* 0x18 */
    /* CLC */
#ifdef LAZY_FLAG_EVALUATION
    context->flag_c = 0;
#else
    context->flags &= ~FLAG_C;
#endif
    context->cycles_left -= 2;
}

void cpu6280_op_2(struct cpu6280_context *context)
{
    /* 0x38 */
    /* SEC */
#ifdef LAZY_FLAG_EVALUATION
    context->flag_c = 1;
#else
    context->flags |= FLAG_C;
#endif
    context->cycles_left -= 2;
}

void cpu6280_op_3(struct cpu6280_context *context)
{
    /* 0xd8 */
    /* CLD */
    context->flags &= ~FLAG_D;
    context->cycles_left -= 2;
}

void cpu6280_op_4(struct cpu6280_context *context)
{
    /* 0xf8 */
    /* SED */
    context->flags |= FLAG_D;
    context->cycles_left -= 2;
}

void cpu6280_op_5(struct cpu6280_context *context)
{
    /* 0x58 */
    /* CLI */
    context->flags &= ~FLAG_I;
    context->cycles_left -= 2;
}

void cpu6280_op_6(struct cpu6280_context *context)
{
    /* 0x78 */
    /* SEI */
    context->flags |= FLAG_I;
    context->cycles_left -= 2;
}

void cpu6280_op_7(struct cpu6280_context *context)
{
    /* 0xb8 */
    /* CLV */
#ifdef LAZY_FLAG_EVALUATION
    context->flag_v = 0;
#else
    context->flags &= ~FLAG_V;
#endif
    context->cycles_left -= 2;
}

void cpu6280_op_8(struct cpu6280_context *context)
{
    /* 0x09 */
    /* ORA Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_9(struct cpu6280_context *context)
{
    /* 0x05 */
    /* ORA ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_10(struct cpu6280_context *context)
{
    /* 0x15 */
    /* ORA ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_11(struct cpu6280_context *context)
{
    /* 0x0d */
    /* ORA Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_12(struct cpu6280_context *context)
{
    /* 0x1d */
    /* ORA Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_13(struct cpu6280_context *context)
{
    /* 0x19 */
    /* ORA Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_14(struct cpu6280_context *context)
{
    /* 0x12 */
    /* ORA (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_15(struct cpu6280_context *context)
{
    /* 0x01 */
    /* ORA (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_16(struct cpu6280_context *context)
{
    /* 0x11 */
    /* ORA (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a |= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_17(struct cpu6280_context *context)
{
    /* 0x29 */
    /* AND Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_18(struct cpu6280_context *context)
{
    /* 0x25 */
    /* AND ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_19(struct cpu6280_context *context)
{
    /* 0x35 */
    /* AND ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_20(struct cpu6280_context *context)
{
    /* 0x2d */
    /* AND Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_21(struct cpu6280_context *context)
{
    /* 0x3d */
    /* AND Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_22(struct cpu6280_context *context)
{
    /* 0x39 */
    /* AND Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_23(struct cpu6280_context *context)
{
    /* 0x32 */
    /* AND (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_24(struct cpu6280_context *context)
{
    /* 0x21 */
    /* AND (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_25(struct cpu6280_context *context)
{
    /* 0x31 */
    /* AND (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a &= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_26(struct cpu6280_context *context)
{
    /* 0x49 */
    /* EOR Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_27(struct cpu6280_context *context)
{
    /* 0x45 */
    /* EOR ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_28(struct cpu6280_context *context)
{
    /* 0x55 */
    /* EOR ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_29(struct cpu6280_context *context)
{
    /* 0x4d */
    /* EOR Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_30(struct cpu6280_context *context)
{
    /* 0x5d */
    /* EOR Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_31(struct cpu6280_context *context)
{
    /* 0x59 */
    /* EOR Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_32(struct cpu6280_context *context)
{
    /* 0x52 */
    /* EOR (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_33(struct cpu6280_context *context)
{
    /* 0x41 */
    /* EOR (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_34(struct cpu6280_context *context)
{
    /* 0x51 */
    /* EOR (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a ^= tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_35(struct cpu6280_context *context)
{
    /* 0x69 */
    /* ADC Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_36(struct cpu6280_context *context)
{
    /* 0x65 */
    /* ADC ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_37(struct cpu6280_context *context)
{
    /* 0x75 */
    /* ADC ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_38(struct cpu6280_context *context)
{
    /* 0x6d */
    /* ADC Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_39(struct cpu6280_context *context)
{
    /* 0x7d */
    /* ADC Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_40(struct cpu6280_context *context)
{
    /* 0x79 */
    /* ADC Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_41(struct cpu6280_context *context)
{
    /* 0x72 */
    /* ADC (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_42(struct cpu6280_context *context)
{
    /* 0x61 */
    /* ADC (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_43(struct cpu6280_context *context)
{
    /* 0x71 */
    /* ADC (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a + tmp + (STATUS_CARRY(context));
    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));
        if (al > 9) al += 6;
        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);
        if (ah > 9) ah += 6;
        SET_FLAG_C(context, (ah > 15));
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        SET_FLAG_C(context, !!(result & 0xff00));
        context->reg_a = result;
    }
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_44(struct cpu6280_context *context)
{
    /* 0x85 */
    /* STA ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->zpage[addr] = context->reg_a;
    context->cycles_left -= 4;
}

void cpu6280_op_45(struct cpu6280_context *context)
{
    /* 0x95 */
    /* STA ZPage, X */
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    context->zpage[addr] = context->reg_a;
    context->cycles_left -= 4;
}

void cpu6280_op_46(struct cpu6280_context *context)
{
    /* 0x8d */
    /* STA Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->writefunc(context->cpu, addr, context->reg_a);
    context->cycles_left -= 5;
}

void cpu6280_op_47(struct cpu6280_context *context)
{
    /* 0x9d */
    /* STA Abs, X */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    context->writefunc(context->cpu, addr, context->reg_a);
    context->cycles_left -= 5;
}

void cpu6280_op_48(struct cpu6280_context *context)
{
    /* 0x99 */
    /* STA Abs, Y */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    context->writefunc(context->cpu, addr, context->reg_a);
    context->cycles_left -= 5;
}

void cpu6280_op_49(struct cpu6280_context *context)
{
    /* 0x92 */
    /* STA (Ind) */
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    context->writefunc(context->cpu, addr2, context->reg_a);
    context->cycles_left -= 7;
}

void cpu6280_op_50(struct cpu6280_context *context)
{
    /* 0x81 */
    /* STA (Ind, X) */
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    context->writefunc(context->cpu, addr2, context->reg_a);
    context->cycles_left -= 7;
}

void cpu6280_op_51(struct cpu6280_context *context)
{
    /* 0x91 */
    /* STA (Ind), Y */
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    context->writefunc(context->cpu, addr2, context->reg_a);
    context->cycles_left -= 7;
}

void cpu6280_op_52(struct cpu6280_context *context)
{
    /* 0xa9 */
    /* LDA Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_53(struct cpu6280_context *context)
{
    /* 0xa5 */
    /* LDA ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_54(struct cpu6280_context *context)
{
    /* 0xb5 */
    /* LDA ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_55(struct cpu6280_context *context)
{
    /* 0xad */
    /* LDA Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_56(struct cpu6280_context *context)
{
    /* 0xbd */
    /* LDA Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_57(struct cpu6280_context *context)
{
    /* 0xb9 */
    /* LDA Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_58(struct cpu6280_context *context)
{
    /* 0xb2 */
    /* LDA (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_59(struct cpu6280_context *context)
{
    /* 0xa1 */
    /* LDA (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_60(struct cpu6280_context *context)
{
    /* 0xb1 */
    /* LDA (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a = tmp;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_61(struct cpu6280_context *context)
{
    /* 0xc9 */
    /* CMP Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_62(struct cpu6280_context *context)
{
    /* 0xc5 */
    /* CMP ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_63(struct cpu6280_context *context)
{
    /* 0xd5 */
    /* CMP ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_64(struct cpu6280_context *context)
{
    /* 0xcd */
    /* CMP Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_65(struct cpu6280_context *context)
{
    /* 0xdd */
    /* CMP Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_66(struct cpu6280_context *context)
{
    /* 0xd9 */
    /* CMP Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_67(struct cpu6280_context *context)
{
    /* 0xd2 */
    /* CMP (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_68(struct cpu6280_context *context)
{
    /* 0xc1 */
    /* CMP (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_69(struct cpu6280_context *context)
{
    /* 0xd1 */
    /* CMP (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = context->reg_a - tmp;
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_70(struct cpu6280_context *context)
{
    /* 0xe9 */
    /* SBC Imm */
    u8 tmp;
    u16 result;

    tmp = OPFETCH(context);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_71(struct cpu6280_context *context)
{
    /* 0xe5 */
    /* SBC ZPage */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_72(struct cpu6280_context *context)
{
    /* 0xf5 */
    /* SBC ZPage, X */
    u8 tmp;
    u16 result;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    tmp = context->zpage[addr];
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_73(struct cpu6280_context *context)
{
    /* 0xed */
    /* SBC Abs */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_74(struct cpu6280_context *context)
{
    /* 0xfd */
    /* SBC Abs, X */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    tmp = context->readfunc(context->cpu, addr);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_75(struct cpu6280_context *context)
{
    /* 0xf9 */
    /* SBC Abs, Y */
    u8 tmp;
    u16 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    tmp = context->readfunc(context->cpu, addr);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_76(struct cpu6280_context *context)
{
    /* 0xf2 */
    /* SBC (Ind) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_77(struct cpu6280_context *context)
{
    /* 0xe1 */
    /* SBC (Ind, X) */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context) + context->reg_x;
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    tmp = context->readfunc(context->cpu, addr2);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_78(struct cpu6280_context *context)
{
    /* 0xf1 */
    /* SBC (Ind), Y */
    u8 tmp;
    u16 result;
    u8 addr1;
    u16 addr2;

    addr1 = OPFETCH(context);
    addr2 = context->zpage[addr1++];
    addr2 += context->zpage[addr1] << 8;
    addr2 += context->reg_y;
    tmp = context->readfunc(context->cpu, addr2);
    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));
    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));
    if (context->flags & FLAG_D) {
        u8 al;
        u8 ah;

        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));
        if (al & 0x10) al -= 6;
        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);
        if (ah & 0x10) ah -= 6;
        context->reg_a = (ah << 4) | (al & 0x0f);
    } else {
        context->reg_a = result;
    }
    SET_FLAG_C(context, !(result & 0xff00));
    result &= 0xff;
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 7;
}

void cpu6280_op_79(struct cpu6280_context *context)
{
    /* 0x80 */
    /* BRA */
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
}

void cpu6280_op_80(struct cpu6280_context *context)
{
    /* 0x10 */
    /* BPL */
    if (!(STATUS_NEGATIVE(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_81(struct cpu6280_context *context)
{
    /* 0x30 */
    /* BMI */
    if ( (STATUS_NEGATIVE(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_82(struct cpu6280_context *context)
{
    /* 0x50 */
    /* BVC */
    if (!(STATUS_OVERFLOW(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_83(struct cpu6280_context *context)
{
    /* 0x70 */
    /* BVS */
    if ( (STATUS_OVERFLOW(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_84(struct cpu6280_context *context)
{
    /* 0x90 */
    /* BCC */
    if (!(STATUS_CARRY(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_85(struct cpu6280_context *context)
{
    /* 0xb0 */
    /* BCS */
    if ( (STATUS_CARRY(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_86(struct cpu6280_context *context)
{
    /* 0xd0 */
    /* BNE */
    if (!(STATUS_ZERO(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_87(struct cpu6280_context *context)
{
    /* 0xf0 */
    /* BEQ */
    if ( (STATUS_ZERO(context))) {
        s8 delta;

        delta = OPFETCH(context);
        if (((context->pc & 0xff) + delta) & 0xff00) {
            context->cycles_left -= 1;
        }
        context->pc += delta;
        context->cycles_left -= 3;
    } else {
        context->pc++;
        context->cycles_left -= 2;
    }
}

void cpu6280_op_88(struct cpu6280_context *context)
{
    /* 0x0f */
    /* BBR0 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x01)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_89(struct cpu6280_context *context)
{
    /* 0x1f */
    /* BBR1 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x02)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_90(struct cpu6280_context *context)
{
    /* 0x2f */
    /* BBR2 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x04)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_91(struct cpu6280_context *context)
{
    /* 0x3f */
    /* BBR3 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x08)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_92(struct cpu6280_context *context)
{
    /* 0x4f */
    /* BBR4 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x10)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_93(struct cpu6280_context *context)
{
    /* 0x5f */
    /* BBR5 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x20)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_94(struct cpu6280_context *context)
{
    /* 0x6f */
    /* BBR6 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x40)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_95(struct cpu6280_context *context)
{
    /* 0x7f */
    /* BBR7 */
    u8 addr;

    addr = OPFETCH(context);

    if (!(context->zpage[addr] & 0x80)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_96(struct cpu6280_context *context)
{
    /* 0x8f */
    /* BBS0 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x01)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_97(struct cpu6280_context *context)
{
    /* 0x9f */
    /* BBS1 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x02)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_98(struct cpu6280_context *context)
{
    /* 0xaf */
    /* BBS2 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x04)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_99(struct cpu6280_context *context)
{
    /* 0xbf */
    /* BBS3 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x08)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_100(struct cpu6280_context *context)
{
    /* 0xcf */
    /* BBS4 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x10)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_101(struct cpu6280_context *context)
{
    /* 0xdf */
    /* BBS5 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x20)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_102(struct cpu6280_context *context)
{
    /* 0xef */
    /* BBS6 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x40)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_103(struct cpu6280_context *context)
{
    /* 0xff */
    /* BBS7 */
    u8 addr;

    addr = OPFETCH(context);

    if ( (context->zpage[addr] & 0x80)) {
        s8 delta;

        delta = OPFETCH(context);
        context->pc += delta;
        context->cycles_left -= 8;
    } else {
        context->pc++;
        context->cycles_left -= 6;
    }
}

void cpu6280_op_104(struct cpu6280_context *context)
{
    /* 0x48 */
    /* PHA */
    PUSH_BYTE(context, context->reg_a);
    context->cycles_left -= 3;
}

void cpu6280_op_105(struct cpu6280_context *context)
{
    /* 0x08 */
    /* PHP */
    u8 tmp;

    tmp = GET_FLAGS(context);
    PUSH_BYTE(context, tmp);
    context->cycles_left -= 3;
}

void cpu6280_op_106(struct cpu6280_context *context)
{
    /* 0xda */
    /* PHX */
    PUSH_BYTE(context, context->reg_x);
    context->cycles_left -= 3;
}

void cpu6280_op_107(struct cpu6280_context *context)
{
    /* 0x5a */
    /* PHY */
    PUSH_BYTE(context, context->reg_y);
    context->cycles_left -= 3;
}

void cpu6280_op_108(struct cpu6280_context *context)
{
    /* 0x68 */
    /* PLA */
    context->reg_a = POP_BYTE(context);
    EVAL_FLAGNZ(context, context->reg_a);
    context->cycles_left -= 4;
}

void cpu6280_op_109(struct cpu6280_context *context)
{
    /* 0x28 */
    /* PLP */
    u8 tmp;

    tmp = POP_BYTE(context);
    SET_FLAGS(context, tmp);
    context->cycles_left -= 4;
}

void cpu6280_op_110(struct cpu6280_context *context)
{
    /* 0xfa */
    /* PLX */
    context->reg_x = POP_BYTE(context);
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 4;
}

void cpu6280_op_111(struct cpu6280_context *context)
{
    /* 0x7a */
    /* PLY */
    context->reg_y = POP_BYTE(context);
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 4;
}

void cpu6280_op_112(struct cpu6280_context *context)
{
    /* 0x22 */
    /* SAX */
    u8 tmp;

    tmp = context->reg_a;
    context->reg_a = context->reg_x;
    context->reg_x = tmp;
    context->cycles_left -= 3;
}

void cpu6280_op_113(struct cpu6280_context *context)
{
    /* 0x42 */
    /* SAY */
    u8 tmp;

    tmp = context->reg_a;
    context->reg_a = context->reg_y;
    context->reg_y = tmp;
    context->cycles_left -= 3;
}

void cpu6280_op_114(struct cpu6280_context *context)
{
    /* 0x02 */
    /* SXY */
    u8 tmp;

    tmp = context->reg_x;
    context->reg_x = context->reg_y;
    context->reg_y = tmp;
    context->cycles_left -= 3;
}

void cpu6280_op_115(struct cpu6280_context *context)
{
    /* 0xaa */
    /* TAX */
    context->reg_x = context->reg_a;
    context->cycles_left -= 2;
    EVAL_FLAGNZ(context, context->reg_x);
}

void cpu6280_op_116(struct cpu6280_context *context)
{
    /* 0xa8 */
    /* TAY */
    context->reg_y = context->reg_a;
    context->cycles_left -= 2;
    EVAL_FLAGNZ(context, context->reg_y);
}

void cpu6280_op_117(struct cpu6280_context *context)
{
    /* 0xba */
    /* TSX */
    context->reg_x = context->reg_s;
    context->cycles_left -= 2;
    EVAL_FLAGNZ(context, context->reg_x);
}

void cpu6280_op_118(struct cpu6280_context *context)
{
    /* 0x8a */
    /* TXA */
    context->reg_a = context->reg_x;
    context->cycles_left -= 2;
    EVAL_FLAGNZ(context, context->reg_a);
}

void cpu6280_op_119(struct cpu6280_context *context)
{
    /* 0x9a */
    /* TXS */
    context->reg_s = context->reg_x;
    context->cycles_left -= 2;
}

void cpu6280_op_120(struct cpu6280_context *context)
{
    /* 0x98 */
    /* TYA */
    context->reg_a = context->reg_y;
    context->cycles_left -= 2;
    EVAL_FLAGNZ(context, context->reg_a);
}

void cpu6280_op_121(struct cpu6280_context *context)
{
    /* 0x89 */
    /* BIT Imm */
    u8 data;
    u8 reg = context->reg_a;

    data = OPFETCH(context);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 2;
}

void cpu6280_op_122(struct cpu6280_context *context)
{
    /* 0x24 */
    /* BIT ZPage */
    u8 data;
    u8 reg = context->reg_a;
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 4;
}

void cpu6280_op_123(struct cpu6280_context *context)
{
    /* 0x34 */
    /* BIT ZPage, X */
    u8 data;
    u8 reg = context->reg_a;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 4;
}

void cpu6280_op_124(struct cpu6280_context *context)
{
    /* 0x2c */
    /* BIT Abs */
    u8 data;
    u8 reg = context->reg_a;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 5;
}

void cpu6280_op_125(struct cpu6280_context *context)
{
    /* 0x3c */
    /* BIT Abs, X */
    u8 data;
    u8 reg = context->reg_a;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 5;
}

void cpu6280_op_126(struct cpu6280_context *context)
{
    /* 0x83 */
    /* TST ZPage */
    u8 data;
    u8 reg = OPFETCH(context);
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 7;
}

void cpu6280_op_127(struct cpu6280_context *context)
{
    /* 0xa3 */
    /* TST ZPage, X */
    u8 data;
    u8 reg = OPFETCH(context);
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 7;
}

void cpu6280_op_128(struct cpu6280_context *context)
{
    /* 0x93 */
    /* TST Abs */
    u8 data;
    u8 reg = OPFETCH(context);
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 8;
}

void cpu6280_op_129(struct cpu6280_context *context)
{
    /* 0xb3 */
    /* TST Abs, X */
    u8 data;
    u8 reg = OPFETCH(context);
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    context->cycles_left -= 8;
}

void cpu6280_op_130(struct cpu6280_context *context)
{
    /* 0x14 */
    /* TRB ZPage */
    u8 data;
    u8 reg = context->reg_a;
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    data &=~reg;
    context->zpage[addr] = data;
    context->cycles_left -= 6;
}

void cpu6280_op_131(struct cpu6280_context *context)
{
    /* 0x1c */
    /* TRB Abs */
    u8 data;
    u8 reg = context->reg_a;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    data &=~reg;
    context->writefunc(context->cpu, addr, data);
    context->cycles_left -= 7;
}

void cpu6280_op_132(struct cpu6280_context *context)
{
    /* 0x04 */
    /* TSB ZPage */
    u8 data;
    u8 reg = context->reg_a;
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    data |= reg;
    context->zpage[addr] = data;
    context->cycles_left -= 6;
}

void cpu6280_op_133(struct cpu6280_context *context)
{
    /* 0x0c */
    /* TSB Abs */
    u8 data;
    u8 reg = context->reg_a;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
#ifndef LAZY_FLAG_EVALUATION
    context->flags &= ~(0xc0 | FLAG_Z);
    context->flags |= data & 0xc0;
    if (!(reg & data)) {
        context->flags |= FLAG_Z;
    }
#else
    context->flag_n = data;
    context->flag_v = (data & FLAG_V) << 1;
    context->flag_z = reg & data;
#endif
    data |= reg;
    context->writefunc(context->cpu, addr, data);
    context->cycles_left -= 7;
}

void cpu6280_op_134(struct cpu6280_context *context)
{
    /* 0xe6 */
    /* INC ZPage */
    u8 data;
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
    data += 1;
    context->zpage[addr] = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 6;
}

void cpu6280_op_135(struct cpu6280_context *context)
{
    /* 0xf6 */
    /* INC ZPage, X */
    u8 data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    data = context->zpage[addr];
    data += 1;
    context->zpage[addr] = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 6;
}

void cpu6280_op_136(struct cpu6280_context *context)
{
    /* 0xee */
    /* INC Abs */
    u8 data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
    data += 1;
    context->writefunc(context->cpu, addr, data);
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 7;
}

void cpu6280_op_137(struct cpu6280_context *context)
{
    /* 0xfe */
    /* INC Abs, X */
    u8 data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    data = context->readfunc(context->cpu, addr);
    data += 1;
    context->writefunc(context->cpu, addr, data);
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 7;
}

void cpu6280_op_138(struct cpu6280_context *context)
{
    /* 0x1a */
    /* INC Acc */
    u8 data;

    data = context->reg_a;
    data += 1;
    context->reg_a = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_139(struct cpu6280_context *context)
{
    /* 0xe8 */
    /* INX  */
    u8 data;

    data = context->reg_x;
    data += 1;
    context->reg_x = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_140(struct cpu6280_context *context)
{
    /* 0xc8 */
    /* INY  */
    u8 data;

    data = context->reg_y;
    data += 1;
    context->reg_y = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_141(struct cpu6280_context *context)
{
    /* 0xc6 */
    /* DEC ZPage */
    u8 data;
    u8 addr;

    addr = OPFETCH(context);
    data = context->zpage[addr];
    data -= 1;
    context->zpage[addr] = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 6;
}

void cpu6280_op_142(struct cpu6280_context *context)
{
    /* 0xd6 */
    /* DEC ZPage, X */
    u8 data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    data = context->zpage[addr];
    data -= 1;
    context->zpage[addr] = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 6;
}

void cpu6280_op_143(struct cpu6280_context *context)
{
    /* 0xce */
    /* DEC Abs */
    u8 data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    data = context->readfunc(context->cpu, addr);
    data -= 1;
    context->writefunc(context->cpu, addr, data);
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 7;
}

void cpu6280_op_144(struct cpu6280_context *context)
{
    /* 0xde */
    /* DEC Abs, X */
    u8 data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    data = context->readfunc(context->cpu, addr);
    data -= 1;
    context->writefunc(context->cpu, addr, data);
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 7;
}

void cpu6280_op_145(struct cpu6280_context *context)
{
    /* 0x3a */
    /* DEC Acc */
    u8 data;

    data = context->reg_a;
    data -= 1;
    context->reg_a = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_146(struct cpu6280_context *context)
{
    /* 0xca */
    /* DEX  */
    u8 data;

    data = context->reg_x;
    data -= 1;
    context->reg_x = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_147(struct cpu6280_context *context)
{
    /* 0x88 */
    /* DEY  */
    u8 data;

    data = context->reg_y;
    data -= 1;
    context->reg_y = data;
    EVAL_FLAGNZ(context, data);
    context->cycles_left -= 2;
}

void cpu6280_op_148(struct cpu6280_context *context)
{
    /* 0xa2 */
    /* LDX Imm */

    context->reg_x = OPFETCH(context);
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 2;
}

void cpu6280_op_149(struct cpu6280_context *context)
{
    /* 0xa6 */
    /* LDX ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->reg_x = context->zpage[addr];
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 4;
}

void cpu6280_op_150(struct cpu6280_context *context)
{
    /* 0xb6 */
    /* LDX ZPage, Y */
    u8 addr;

    addr = OPFETCH(context) + context->reg_y;
    context->reg_x = context->zpage[addr];
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 4;
}

void cpu6280_op_151(struct cpu6280_context *context)
{
    /* 0xae */
    /* LDX Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->reg_x = context->readfunc(context->cpu, addr);
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 5;
}

void cpu6280_op_152(struct cpu6280_context *context)
{
    /* 0xbe */
    /* LDX Abs, Y */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_y;
    context->reg_x = context->readfunc(context->cpu, addr);
    EVAL_FLAGNZ(context, context->reg_x);
    context->cycles_left -= 5;
}

void cpu6280_op_153(struct cpu6280_context *context)
{
    /* 0xa0 */
    /* LDY Imm */

    context->reg_y = OPFETCH(context);
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 2;
}

void cpu6280_op_154(struct cpu6280_context *context)
{
    /* 0xa4 */
    /* LDY ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->reg_y = context->zpage[addr];
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 4;
}

void cpu6280_op_155(struct cpu6280_context *context)
{
    /* 0xb4 */
    /* LDY ZPage, X */
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    context->reg_y = context->zpage[addr];
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 4;
}

void cpu6280_op_156(struct cpu6280_context *context)
{
    /* 0xac */
    /* LDY Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->reg_y = context->readfunc(context->cpu, addr);
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 5;
}

void cpu6280_op_157(struct cpu6280_context *context)
{
    /* 0xbc */
    /* LDY Abs, X */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    context->reg_y = context->readfunc(context->cpu, addr);
    EVAL_FLAGNZ(context, context->reg_y);
    context->cycles_left -= 5;
}

void cpu6280_op_158(struct cpu6280_context *context)
{
    /* 0x62 */
    /* CLA Acc */

    context->reg_a = 0;
    context->cycles_left -= 2;
}

void cpu6280_op_159(struct cpu6280_context *context)
{
    /* 0x82 */
    /* CLX  */

    context->reg_x = 0;
    context->cycles_left -= 2;
}

void cpu6280_op_160(struct cpu6280_context *context)
{
    /* 0xc2 */
    /* CLY  */

    context->reg_y = 0;
    context->cycles_left -= 2;
}

void cpu6280_op_161(struct cpu6280_context *context)
{
    /* 0x86 */
    /* STX ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->zpage[addr] = context->reg_x;
    context->cycles_left -= 4;
}

void cpu6280_op_162(struct cpu6280_context *context)
{
    /* 0x96 */
    /* STX ZPage, Y */
    u8 addr;

    addr = OPFETCH(context) + context->reg_y;
    context->zpage[addr] = context->reg_x;
    context->cycles_left -= 4;
}

void cpu6280_op_163(struct cpu6280_context *context)
{
    /* 0x8e */
    /* STX Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->writefunc(context->cpu, addr, context->reg_x);
    context->cycles_left -= 5;
}

void cpu6280_op_164(struct cpu6280_context *context)
{
    /* 0x84 */
    /* STY ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->zpage[addr] = context->reg_y;
    context->cycles_left -= 4;
}

void cpu6280_op_165(struct cpu6280_context *context)
{
    /* 0x94 */
    /* STY ZPage, X */
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    context->zpage[addr] = context->reg_y;
    context->cycles_left -= 4;
}

void cpu6280_op_166(struct cpu6280_context *context)
{
    /* 0x8c */
    /* STY Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->writefunc(context->cpu, addr, context->reg_y);
    context->cycles_left -= 5;
}

void cpu6280_op_167(struct cpu6280_context *context)
{
    /* 0x64 */
    /* STZ ZPage */
    u8 addr;

    addr = OPFETCH(context);
    context->zpage[addr] = 0;
    context->cycles_left -= 4;
}

void cpu6280_op_168(struct cpu6280_context *context)
{
    /* 0x74 */
    /* STZ ZPage, X */
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    context->zpage[addr] = 0;
    context->cycles_left -= 4;
}

void cpu6280_op_169(struct cpu6280_context *context)
{
    /* 0x9c */
    /* STZ Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    context->writefunc(context->cpu, addr, 0);
    context->cycles_left -= 5;
}

void cpu6280_op_170(struct cpu6280_context *context)
{
    /* 0x9e */
    /* STZ Abs, X */
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    context->writefunc(context->cpu, addr, 0);
    context->cycles_left -= 5;
}

void cpu6280_op_171(struct cpu6280_context *context)
{
    /* 0x0a */
    /* ASL Acc */
    u8 src_data;
    u8 dst_data;

    src_data = context->reg_a;
    dst_data = src_data << 1;
    context->reg_a = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 2;
}

void cpu6280_op_172(struct cpu6280_context *context)
{
    /* 0x06 */
    /* ASL ZPage */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context);
    src_data = context->zpage[addr];
    dst_data = src_data << 1;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_173(struct cpu6280_context *context)
{
    /* 0x16 */
    /* ASL ZPage, X */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    src_data = context->zpage[addr];
    dst_data = src_data << 1;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_174(struct cpu6280_context *context)
{
    /* 0x0e */
    /* ASL Abs */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data << 1;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_175(struct cpu6280_context *context)
{
    /* 0x1e */
    /* ASL Abs, X */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data << 1;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_176(struct cpu6280_context *context)
{
    /* 0x2a */
    /* ROL Acc */
    u8 src_data;
    u8 dst_data;

    src_data = context->reg_a;
    dst_data = src_data << 1;
    dst_data |= STATUS_CARRY(context);
    context->reg_a = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 2;
}

void cpu6280_op_177(struct cpu6280_context *context)
{
    /* 0x26 */
    /* ROL ZPage */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context);
    src_data = context->zpage[addr];
    dst_data = src_data << 1;
    dst_data |= STATUS_CARRY(context);
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_178(struct cpu6280_context *context)
{
    /* 0x36 */
    /* ROL ZPage, X */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    src_data = context->zpage[addr];
    dst_data = src_data << 1;
    dst_data |= STATUS_CARRY(context);
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_179(struct cpu6280_context *context)
{
    /* 0x2e */
    /* ROL Abs */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data << 1;
    dst_data |= STATUS_CARRY(context);
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_180(struct cpu6280_context *context)
{
    /* 0x3e */
    /* ROL Abs, X */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data << 1;
    dst_data |= STATUS_CARRY(context);
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data >> 7);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_181(struct cpu6280_context *context)
{
    /* 0x4a */
    /* LSR Acc */
    u8 src_data;
    u8 dst_data;

    src_data = context->reg_a;
    dst_data = src_data >> 1;
    context->reg_a = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 2;
}

void cpu6280_op_182(struct cpu6280_context *context)
{
    /* 0x46 */
    /* LSR ZPage */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context);
    src_data = context->zpage[addr];
    dst_data = src_data >> 1;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_183(struct cpu6280_context *context)
{
    /* 0x56 */
    /* LSR ZPage, X */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    src_data = context->zpage[addr];
    dst_data = src_data >> 1;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_184(struct cpu6280_context *context)
{
    /* 0x4e */
    /* LSR Abs */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data >> 1;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_185(struct cpu6280_context *context)
{
    /* 0x5e */
    /* LSR Abs, X */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data >> 1;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_186(struct cpu6280_context *context)
{
    /* 0x6a */
    /* ROR Acc */
    u8 src_data;
    u8 dst_data;

    src_data = context->reg_a;
    dst_data = src_data >> 1;
    dst_data |= STATUS_CARRY(context) << 7;
    context->reg_a = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 2;
}

void cpu6280_op_187(struct cpu6280_context *context)
{
    /* 0x66 */
    /* ROR ZPage */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context);
    src_data = context->zpage[addr];
    dst_data = src_data >> 1;
    dst_data |= STATUS_CARRY(context) << 7;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_188(struct cpu6280_context *context)
{
    /* 0x76 */
    /* ROR ZPage, X */
    u8 src_data;
    u8 dst_data;
    u8 addr;

    addr = OPFETCH(context) + context->reg_x;
    src_data = context->zpage[addr];
    dst_data = src_data >> 1;
    dst_data |= STATUS_CARRY(context) << 7;
    context->zpage[addr] = dst_data;
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 6;
}

void cpu6280_op_189(struct cpu6280_context *context)
{
    /* 0x6e */
    /* ROR Abs */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data >> 1;
    dst_data |= STATUS_CARRY(context) << 7;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_190(struct cpu6280_context *context)
{
    /* 0x7e */
    /* ROR Abs, X */
    u8 src_data;
    u8 dst_data;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    addr += context->reg_x;
    src_data = context->readfunc(context->cpu, addr);
    dst_data = src_data >> 1;
    dst_data |= STATUS_CARRY(context) << 7;
    context->writefunc(context->cpu, addr, dst_data);
    SET_FLAG_C(context, src_data & FLAG_C);
    EVAL_FLAGNZ(context, dst_data);
    context->cycles_left -= 7;
}

void cpu6280_op_191(struct cpu6280_context *context)
{
    /* 0x07 */
    /* RMB0 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~1;

    context->cycles_left -= 7;
}

void cpu6280_op_192(struct cpu6280_context *context)
{
    /* 0x17 */
    /* RMB1 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~2;

    context->cycles_left -= 7;
}

void cpu6280_op_193(struct cpu6280_context *context)
{
    /* 0x27 */
    /* RMB2 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~4;

    context->cycles_left -= 7;
}

void cpu6280_op_194(struct cpu6280_context *context)
{
    /* 0x37 */
    /* RMB3 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~8;

    context->cycles_left -= 7;
}

void cpu6280_op_195(struct cpu6280_context *context)
{
    /* 0x47 */
    /* RMB4 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~16;

    context->cycles_left -= 7;
}

void cpu6280_op_196(struct cpu6280_context *context)
{
    /* 0x57 */
    /* RMB5 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~32;

    context->cycles_left -= 7;
}

void cpu6280_op_197(struct cpu6280_context *context)
{
    /* 0x67 */
    /* RMB6 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~64;

    context->cycles_left -= 7;
}

void cpu6280_op_198(struct cpu6280_context *context)
{
    /* 0x77 */
    /* RMB7 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] &= ~128;

    context->cycles_left -= 7;
}

void cpu6280_op_199(struct cpu6280_context *context)
{
    /* 0x87 */
    /* SMB0 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 1;

    context->cycles_left -= 7;
}

void cpu6280_op_200(struct cpu6280_context *context)
{
    /* 0x97 */
    /* SMB1 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 2;

    context->cycles_left -= 7;
}

void cpu6280_op_201(struct cpu6280_context *context)
{
    /* 0xa7 */
    /* SMB2 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 4;

    context->cycles_left -= 7;
}

void cpu6280_op_202(struct cpu6280_context *context)
{
    /* 0xb7 */
    /* SMB3 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 8;

    context->cycles_left -= 7;
}

void cpu6280_op_203(struct cpu6280_context *context)
{
    /* 0xc7 */
    /* SMB4 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 16;

    context->cycles_left -= 7;
}

void cpu6280_op_204(struct cpu6280_context *context)
{
    /* 0xd7 */
    /* SMB5 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 32;

    context->cycles_left -= 7;
}

void cpu6280_op_205(struct cpu6280_context *context)
{
    /* 0xe7 */
    /* SMB6 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 64;

    context->cycles_left -= 7;
}

void cpu6280_op_206(struct cpu6280_context *context)
{
    /* 0xf7 */
    /* SMB7 */
    u8 addr;

    addr = OPFETCH(context);

    context->zpage[addr] |= 128;

    context->cycles_left -= 7;
}

void cpu6280_op_207(struct cpu6280_context *context)
{
    /* 0xc0 */
    /* CPY Imm */
    u8 tmp;
    u8 result;

    tmp = OPFETCH(context);
    result = context->reg_y - tmp;
    SET_FLAG_C(context, context->reg_y >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_208(struct cpu6280_context *context)
{
    /* 0xc4 */
    /* CPY ZPage */
    u8 tmp;
    u8 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_y - tmp;
    SET_FLAG_C(context, context->reg_y >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_209(struct cpu6280_context *context)
{
    /* 0xcc */
    /* CPY Abs */
    u8 tmp;
    u8 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_y - tmp;
    SET_FLAG_C(context, context->reg_y >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_210(struct cpu6280_context *context)
{
    /* 0xe0 */
    /* CPX Imm */
    u8 tmp;
    u8 result;

    tmp = OPFETCH(context);
    result = context->reg_x - tmp;
    SET_FLAG_C(context, context->reg_x >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 2;
}

void cpu6280_op_211(struct cpu6280_context *context)
{
    /* 0xe4 */
    /* CPX ZPage */
    u8 tmp;
    u8 result;
    u8 addr;

    addr = OPFETCH(context);
    tmp = context->zpage[addr];
    result = context->reg_x - tmp;
    SET_FLAG_C(context, context->reg_x >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 4;
}

void cpu6280_op_212(struct cpu6280_context *context)
{
    /* 0xec */
    /* CPX Abs */
    u8 tmp;
    u8 result;
    u16 addr;

    addr = OPFETCH(context);
    addr += (OPFETCH(context) << 8);
    tmp = context->readfunc(context->cpu, addr);
    result = context->reg_x - tmp;
    SET_FLAG_C(context, context->reg_x >= tmp);
    EVAL_FLAGNZ(context, result);
    context->cycles_left -= 5;
}

void cpu6280_op_213(struct cpu6280_context *context)
{
    /* 0xf3 */
    /* TAI */
    u16 source;
    u16 dest;
    u32 length;

    source = OPFETCH(context);
    source |= OPFETCH(context) << 8;
    dest = OPFETCH(context);
    dest |= OPFETCH(context) << 8;
    length = OPFETCH(context);
    length |= OPFETCH(context) << 8;

    if (!length) length = 0x10000;
    context->cycles_left -= (17 + (length * 6));

    for (;;) {
        context->writefunc(context->cpu, dest++, context->readfunc(context->cpu, source++));
        if (!--length) break;
        context->writefunc(context->cpu, dest++, context->readfunc(context->cpu, source--));
        if (!--length) break;
    }
}

void cpu6280_op_214(struct cpu6280_context *context)
{
    /* 0xc3 */
    /* TDD */
    u16 source;
    u16 dest;
    u32 length;

    source = OPFETCH(context);
    source |= OPFETCH(context) << 8;
    dest = OPFETCH(context);
    dest |= OPFETCH(context) << 8;
    length = OPFETCH(context);
    length |= OPFETCH(context) << 8;

    if (!length) length = 0x10000;
    context->cycles_left -= (17 + (length * 6));

    for (;;) {
        context->writefunc(context->cpu, dest--, context->readfunc(context->cpu, source--));
        if (!--length) break;
        context->writefunc(context->cpu, dest--, context->readfunc(context->cpu, source--));
        if (!--length) break;
    }
}

void cpu6280_op_215(struct cpu6280_context *context)
{
    /* 0xe3 */
    /* TIA */
    u16 source;
    u16 dest;
    u32 length;

    source = OPFETCH(context);
    source |= OPFETCH(context) << 8;
    dest = OPFETCH(context);
    dest |= OPFETCH(context) << 8;
    length = OPFETCH(context);
    length |= OPFETCH(context) << 8;

    if (!length) length = 0x10000;
    context->cycles_left -= (17 + (length * 6));

    for (;;) {
        context->writefunc(context->cpu, dest++, context->readfunc(context->cpu, source++));
        if (!--length) break;
        context->writefunc(context->cpu, dest--, context->readfunc(context->cpu, source++));
        if (!--length) break;
    }
}

void cpu6280_op_216(struct cpu6280_context *context)
{
    /* 0x73 */
    /* TII */
    u16 source;
    u16 dest;
    u32 length;

    source = OPFETCH(context);
    source |= OPFETCH(context) << 8;
    dest = OPFETCH(context);
    dest |= OPFETCH(context) << 8;
    length = OPFETCH(context);
    length |= OPFETCH(context) << 8;

    if (!length) length = 0x10000;
    context->cycles_left -= (17 + (length * 6));

    for (;;) {
        context->writefunc(context->cpu, dest++, context->readfunc(context->cpu, source++));
        if (!--length) break;
        context->writefunc(context->cpu, dest++, context->readfunc(context->cpu, source++));
        if (!--length) break;
    }
}

void cpu6280_op_217(struct cpu6280_context *context)
{
    /* 0xd3 */
    /* TIN */
    u16 source;
    u16 dest;
    u32 length;

    source = OPFETCH(context);
    source |= OPFETCH(context) << 8;
    dest = OPFETCH(context);
    dest |= OPFETCH(context) << 8;
    length = OPFETCH(context);
    length |= OPFETCH(context) << 8;

    if (!length) length = 0x10000;
    context->cycles_left -= (17 + (length * 6));

    for (;;) {
        context->writefunc(context->cpu, dest, context->readfunc(context->cpu, source++));
        if (!--length) break;
        context->writefunc(context->cpu, dest, context->readfunc(context->cpu, source++));
        if (!--length) break;
    }
}

void cpu6280_op_218(struct cpu6280_context *context)
{
    /* 0x4c */
    /* JMP Abs */
    u16 addr;

    addr = OPFETCH(context);
    addr |= OPFETCH(context) << 8;
    context->pc = addr;
    context->cycles_left -= 4;
}

void cpu6280_op_219(struct cpu6280_context *context)
{
    /* 0x6c */
    /* JMP Ind */
    u16 addr;

    addr = OPFETCH(context);
    addr |= OPFETCH(context) << 8;
    context->pc = context->readfunc(context->cpu, addr++);
    context->pc |= context->readfunc(context->cpu, addr) << 8;
    context->cycles_left -= 7;
}

void cpu6280_op_220(struct cpu6280_context *context)
{
    /* 0x7c */
    /* JMP Ind, X */
    u16 addr;

    addr = OPFETCH(context);
    addr |= OPFETCH(context) << 8;
    addr += context->reg_x;
    context->pc = context->readfunc(context->cpu, addr++);
    context->pc |= context->readfunc(context->cpu, addr) << 8;
    context->cycles_left -= 7;
}

void cpu6280_op_221(struct cpu6280_context *context)
{
    /* 0x20 */
    /* JSR */
    u16 addr;

    addr = OPFETCH(context);
    PUSH_BYTE(context, context->pc >> 8);
    PUSH_BYTE(context, context->pc & 0xff);
    addr |= OPFETCH(context) << 8;
    context->pc = addr;
    context->cycles_left -= 7;
}

void cpu6280_op_222(struct cpu6280_context *context)
{
    /* 0x44 */
    /* BSR */
    s8 delta;

    PUSH_BYTE(context, context->pc >> 8);
    PUSH_BYTE(context, context->pc & 0xff);
    delta = OPFETCH(context);
    context->pc += delta;
    context->cycles_left -= 8;
}

void cpu6280_op_223(struct cpu6280_context *context)
{
    /* 0x60 */
    /* RTS */

    context->pc = POP_BYTE(context);
    context->pc |= POP_BYTE(context) << 8;
    context->pc++;
    context->cycles_left -= 7;
}

void cpu6280_op_224(struct cpu6280_context *context)
{
    /* 0x40 */
    /* RTI */

    SET_FLAGS(context, POP_BYTE(context));
    context->pc = POP_BYTE(context);
    context->pc |= POP_BYTE(context) << 8;
    context->cycles_left -= 7;
}

void cpu6280_op_225(struct cpu6280_context *context)
{
    /* 0x00 */
    /* BRK */

    context->pc++;
    PUSH_BYTE(context, context->pc >> 8);
    PUSH_BYTE(context, context->pc & 0xff);
    PUSH_BYTE(context, (GET_FLAGS(context)) | FLAG_B);
    context->flags |= FLAG_I;
    context->flags &= ~FLAG_D;
    context->pc = context->readfunc(context->cpu, VECTOR_BRK_LO);
    context->pc |= context->readfunc(context->cpu, VECTOR_BRK_HI) << 8;
    context->cycles_left -= 8;
}

void cpu6280_op_226(struct cpu6280_context *context)
{
    /* 0xea */
    /* NOP */
}

void cpu6280_op_227(struct cpu6280_context *context)
{
    /* 0xd4 */
    /* CSH */
}

void cpu6280_op_228(struct cpu6280_context *context)
{
    /* 0x54 */
    /* CSL */
}

void cpu6280_op_229(struct cpu6280_context *context)
{
    /* 0x03 */
    /* ST0 */
    void pce_st0_hack(u8);
    pce_st0_hack(OPFETCH(context));
    context->cycles_left -= 4;
}

void cpu6280_op_230(struct cpu6280_context *context)
{
    /* 0x13 */
    /* ST1 */
    void pce_st1_hack(u8);
    pce_st1_hack(OPFETCH(context));
    context->cycles_left -= 4;
}

void cpu6280_op_231(struct cpu6280_context *context)
{
    /* 0x23 */
    /* ST2 */
    void pce_st2_hack(u8);
    pce_st2_hack(OPFETCH(context));
    context->cycles_left -= 4;
}

void cpu6280_op_232(struct cpu6280_context *context)
{
    /* 0x43 */
    /* TMAi */
    u8 i;

    i = OPFETCH(context);
    if (i & 0x80) { context->reg_a = context->mpr[7]; }
    if (i & 0x40) { context->reg_a = context->mpr[6]; }
    if (i & 0x20) { context->reg_a = context->mpr[5]; }
    if (i & 0x10) { context->reg_a = context->mpr[4]; }
    if (i & 0x08) { context->reg_a = context->mpr[3]; }
    if (i & 0x04) { context->reg_a = context->mpr[2]; }
    if (i & 0x02) { context->reg_a = context->mpr[1]; }
    if (i & 0x01) { context->reg_a = context->mpr[0]; }
    context->cycles_left -= 4;
}

void cpu6280_op_233(struct cpu6280_context *context)
{
    /* 0x53 */
    /* TAMi */
    u8 i;

    i = OPFETCH(context);
    if (i & 0x80) { context->mpr[7] = context->reg_a; }
    if (i & 0x40) { context->mpr[6] = context->reg_a; }
    if (i & 0x20) { context->mpr[5] = context->reg_a; }
    if (i & 0x10) { context->mpr[4] = context->reg_a; }
    if (i & 0x08) { context->mpr[3] = context->reg_a; }
    if (i & 0x04) { context->mpr[2] = context->reg_a; }
    if (i & 0x02) { context->mpr[1] = context->reg_a; }
    if (i & 0x01) { context->mpr[0] = context->reg_a; }
    context->cycles_left -= 5;
}

void cpu6280_op_234(struct cpu6280_context *context)
{
    /* 0xf4 */
    /* SET */
    u8 i;
    u8 tmp;

    i = OPFETCH(context);
    if (((i & 0x83) == 0x01) || ((i & 0x9f) == 0x12)) {
        tmp = context->reg_a;
        context->reg_a = context->zpage[context->reg_x];
        cpu6280_decode[i](context);
        context->zpage[context->reg_x] = context->reg_a;
        context->reg_a = tmp;
        context->cycles_left -= 5;
    } else {
        cpu6280_decode[i](context);
        context->cycles_left -= 2;
    }
}

ophandler_t cpu6280_decode[0x100] = {
    cpu6280_op_225, cpu6280_op_15, cpu6280_op_114, cpu6280_op_229, /* 0x00 - 0x03 */
    cpu6280_op_132, cpu6280_op_9, cpu6280_op_172, cpu6280_op_191, /* 0x04 - 0x07 */
    cpu6280_op_105, cpu6280_op_8, cpu6280_op_171, cpu6280_op_0, /* 0x08 - 0x0b */
    cpu6280_op_133, cpu6280_op_11, cpu6280_op_174, cpu6280_op_88, /* 0x0c - 0x0f */
    cpu6280_op_80, cpu6280_op_16, cpu6280_op_14, cpu6280_op_230, /* 0x10 - 0x13 */
    cpu6280_op_130, cpu6280_op_10, cpu6280_op_173, cpu6280_op_192, /* 0x14 - 0x17 */
    cpu6280_op_1, cpu6280_op_13, cpu6280_op_138, cpu6280_op_0, /* 0x18 - 0x1b */
    cpu6280_op_131, cpu6280_op_12, cpu6280_op_175, cpu6280_op_89, /* 0x1c - 0x1f */
    cpu6280_op_221, cpu6280_op_24, cpu6280_op_112, cpu6280_op_231, /* 0x20 - 0x23 */
    cpu6280_op_122, cpu6280_op_18, cpu6280_op_177, cpu6280_op_193, /* 0x24 - 0x27 */
    cpu6280_op_109, cpu6280_op_17, cpu6280_op_176, cpu6280_op_0, /* 0x28 - 0x2b */
    cpu6280_op_124, cpu6280_op_20, cpu6280_op_179, cpu6280_op_90, /* 0x2c - 0x2f */
    cpu6280_op_81, cpu6280_op_25, cpu6280_op_23, cpu6280_op_0, /* 0x30 - 0x33 */
    cpu6280_op_123, cpu6280_op_19, cpu6280_op_178, cpu6280_op_194, /* 0x34 - 0x37 */
    cpu6280_op_2, cpu6280_op_22, cpu6280_op_145, cpu6280_op_0, /* 0x38 - 0x3b */
    cpu6280_op_125, cpu6280_op_21, cpu6280_op_180, cpu6280_op_91, /* 0x3c - 0x3f */
    cpu6280_op_224, cpu6280_op_33, cpu6280_op_113, cpu6280_op_232, /* 0x40 - 0x43 */
    cpu6280_op_222, cpu6280_op_27, cpu6280_op_182, cpu6280_op_195, /* 0x44 - 0x47 */
    cpu6280_op_104, cpu6280_op_26, cpu6280_op_181, cpu6280_op_0, /* 0x48 - 0x4b */
    cpu6280_op_218, cpu6280_op_29, cpu6280_op_184, cpu6280_op_92, /* 0x4c - 0x4f */
    cpu6280_op_82, cpu6280_op_34, cpu6280_op_32, cpu6280_op_233, /* 0x50 - 0x53 */
    cpu6280_op_228, cpu6280_op_28, cpu6280_op_183, cpu6280_op_196, /* 0x54 - 0x57 */
    cpu6280_op_5, cpu6280_op_31, cpu6280_op_107, cpu6280_op_0, /* 0x58 - 0x5b */
    cpu6280_op_0, cpu6280_op_30, cpu6280_op_185, cpu6280_op_93, /* 0x5c - 0x5f */
    cpu6280_op_223, cpu6280_op_42, cpu6280_op_158, cpu6280_op_0, /* 0x60 - 0x63 */
    cpu6280_op_167, cpu6280_op_36, cpu6280_op_187, cpu6280_op_197, /* 0x64 - 0x67 */
    cpu6280_op_108, cpu6280_op_35, cpu6280_op_186, cpu6280_op_0, /* 0x68 - 0x6b */
    cpu6280_op_219, cpu6280_op_38, cpu6280_op_189, cpu6280_op_94, /* 0x6c - 0x6f */
    cpu6280_op_83, cpu6280_op_43, cpu6280_op_41, cpu6280_op_216, /* 0x70 - 0x73 */
    cpu6280_op_168, cpu6280_op_37, cpu6280_op_188, cpu6280_op_198, /* 0x74 - 0x77 */
    cpu6280_op_6, cpu6280_op_40, cpu6280_op_111, cpu6280_op_0, /* 0x78 - 0x7b */
    cpu6280_op_220, cpu6280_op_39, cpu6280_op_190, cpu6280_op_95, /* 0x7c - 0x7f */
    cpu6280_op_79, cpu6280_op_50, cpu6280_op_159, cpu6280_op_126, /* 0x80 - 0x83 */
    cpu6280_op_164, cpu6280_op_44, cpu6280_op_161, cpu6280_op_199, /* 0x84 - 0x87 */
    cpu6280_op_147, cpu6280_op_121, cpu6280_op_118, cpu6280_op_0, /* 0x88 - 0x8b */
    cpu6280_op_166, cpu6280_op_46, cpu6280_op_163, cpu6280_op_96, /* 0x8c - 0x8f */
    cpu6280_op_84, cpu6280_op_51, cpu6280_op_49, cpu6280_op_128, /* 0x90 - 0x93 */
    cpu6280_op_165, cpu6280_op_45, cpu6280_op_162, cpu6280_op_200, /* 0x94 - 0x97 */
    cpu6280_op_120, cpu6280_op_48, cpu6280_op_119, cpu6280_op_0, /* 0x98 - 0x9b */
    cpu6280_op_169, cpu6280_op_47, cpu6280_op_170, cpu6280_op_97, /* 0x9c - 0x9f */
    cpu6280_op_153, cpu6280_op_59, cpu6280_op_148, cpu6280_op_127, /* 0xa0 - 0xa3 */
    cpu6280_op_154, cpu6280_op_53, cpu6280_op_149, cpu6280_op_201, /* 0xa4 - 0xa7 */
    cpu6280_op_116, cpu6280_op_52, cpu6280_op_115, cpu6280_op_0, /* 0xa8 - 0xab */
    cpu6280_op_156, cpu6280_op_55, cpu6280_op_151, cpu6280_op_98, /* 0xac - 0xaf */
    cpu6280_op_85, cpu6280_op_60, cpu6280_op_58, cpu6280_op_129, /* 0xb0 - 0xb3 */
    cpu6280_op_155, cpu6280_op_54, cpu6280_op_150, cpu6280_op_202, /* 0xb4 - 0xb7 */
    cpu6280_op_7, cpu6280_op_57, cpu6280_op_117, cpu6280_op_0, /* 0xb8 - 0xbb */
    cpu6280_op_157, cpu6280_op_56, cpu6280_op_152, cpu6280_op_99, /* 0xbc - 0xbf */
    cpu6280_op_207, cpu6280_op_68, cpu6280_op_160, cpu6280_op_214, /* 0xc0 - 0xc3 */
    cpu6280_op_208, cpu6280_op_62, cpu6280_op_141, cpu6280_op_203, /* 0xc4 - 0xc7 */
    cpu6280_op_140, cpu6280_op_61, cpu6280_op_146, cpu6280_op_0, /* 0xc8 - 0xcb */
    cpu6280_op_209, cpu6280_op_64, cpu6280_op_143, cpu6280_op_100, /* 0xcc - 0xcf */
    cpu6280_op_86, cpu6280_op_69, cpu6280_op_67, cpu6280_op_217, /* 0xd0 - 0xd3 */
    cpu6280_op_227, cpu6280_op_63, cpu6280_op_142, cpu6280_op_204, /* 0xd4 - 0xd7 */
    cpu6280_op_3, cpu6280_op_66, cpu6280_op_106, cpu6280_op_0, /* 0xd8 - 0xdb */
    cpu6280_op_0, cpu6280_op_65, cpu6280_op_144, cpu6280_op_101, /* 0xdc - 0xdf */
    cpu6280_op_210, cpu6280_op_77, cpu6280_op_0, cpu6280_op_215, /* 0xe0 - 0xe3 */
    cpu6280_op_211, cpu6280_op_71, cpu6280_op_134, cpu6280_op_205, /* 0xe4 - 0xe7 */
    cpu6280_op_139, cpu6280_op_70, cpu6280_op_226, cpu6280_op_0, /* 0xe8 - 0xeb */
    cpu6280_op_212, cpu6280_op_73, cpu6280_op_136, cpu6280_op_102, /* 0xec - 0xef */
    cpu6280_op_87, cpu6280_op_78, cpu6280_op_76, cpu6280_op_213, /* 0xf0 - 0xf3 */
    cpu6280_op_234, cpu6280_op_72, cpu6280_op_135, cpu6280_op_206, /* 0xf4 - 0xf7 */
    cpu6280_op_4, cpu6280_op_75, cpu6280_op_110, cpu6280_op_0, /* 0xf8 - 0xfb */
    cpu6280_op_0, cpu6280_op_74, cpu6280_op_137, cpu6280_op_103, /* 0xfc - 0xff */
};

/* EOF */
