/*
 * build_cpu6502.c
 *
 * build the cpu6502 core opcode handlers
 */

/* $Id: build_cpu6502.c,v 1.9 2000/10/02 01:50:59 nyef Exp $ */

#define CORE "cpu6502"

#include "build_cpu6xxx_common.h"

struct amode amode_immediate = {
    2, "Imm",
    "\n",
    "    %s = OPFETCH(context);\n",
    "#error Write to Immediate addressing mode\n",
};

struct amode amode_zpage = {
    3, "ZPage",
    "    u8 addr;\n\n    addr = OPFETCH(context);\n",
    "    %s = context->zpage[addr];\n",
    "    context->zpage[addr] = %s;\n",
};

struct amode amode_zpagex = {
    4, "ZPage, X",
    "    u8 addr;\n\n    addr = OPFETCH(context) + context->reg_x;\n",
    "    %s = context->zpage[addr];\n",
    "    context->zpage[addr] = %s;\n"
};

struct amode amode_zpagey = {
    4, "ZPage, Y",
    "    u8 addr;\n\n    addr = OPFETCH(context) + context->reg_y;\n",
    "    %s = context->zpage[addr];\n",
    "    context->zpage[addr] = %s;\n"
};

struct amode amode_abs = {
    4, "Abs",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += (OPFETCH(context) << 8);\n",
    "    %s = context->readfunc(context->cpu, addr);\n",
    "    context->writefunc(context->cpu, addr, %s);\n"
};

struct amode amode_absx = {
    4, "Abs, X",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += context->reg_x;\n"
    "    if (addr >= 0x100) context->cycles_left--;\n"
    "    addr += (OPFETCH(context) << 8);\n",
    "    %s = context->readfunc(context->cpu, addr);\n",
    "    context->writefunc(context->cpu, addr, %s);\n"
};

struct amode amode_absy = {
    4, "Abs, Y",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += context->reg_y;\n"
    "    if (addr >= 0x100) context->cycles_left--;\n"
    "    addr += (OPFETCH(context) << 8);\n",
    "    %s = context->readfunc(context->cpu, addr);\n",
    "    context->writefunc(context->cpu, addr, %s);\n"
};

struct amode amode_ind = {
    7, "(Ind)",
    "    u8 addr1;\n"
    "    u16 addr2;\n\n"
    "    addr1 = OPFETCH(context);\n"
    "    addr2 = context->zpage[addr1++];\n"
    "    addr2 += context->zpage[addr1] << 8;\n",
    "    %s = context->readfunc(context->cpu, addr2);\n",
    "    context->writefunc(context->cpu, addr2, %s);\n"
};

struct amode amode_indx = {
    6, "(Ind, X)",
    "    u8 addr1;\n"
    "    u16 addr2;\n\n"
    "    addr1 = OPFETCH(context) + context->reg_x;\n"
    "    addr2 = context->zpage[addr1++];\n"
    "    addr2 += context->zpage[addr1] << 8;\n",
    "    %s = context->readfunc(context->cpu, addr2);\n",
    "    context->writefunc(context->cpu, addr2, %s);\n"
};

struct amode amode_indy = {
    5, "(Ind), Y",
    "    u8 addr1;\n"
    "    u16 addr2;\n\n"
    "    addr1 = OPFETCH(context);\n"
    "    addr2 = context->zpage[addr1++];\n"
    "    addr2 += context->reg_y;\n"
    "    if (addr2 >= 0x100) context->cycles_left--;\n"
    "    addr2 += context->zpage[addr1] << 8;\n",
    "    %s = context->readfunc(context->cpu, addr2);\n",
    "    context->writefunc(context->cpu, addr2, %s);\n"
};

struct amode amode_rega = {
    0, "Acc",
    "\n",
    "    %s = context->reg_a;\n",
    "    context->reg_a = %s;\n",
};

struct amode amode_regx = {
    0, "",
    "\n",
    "    %s = context->reg_x;\n",
    "    context->reg_x = %s;\n",
};

struct amode amode_regy = {
    0, "",
    "\n",
    "    %s = context->reg_y;\n",
    "    context->reg_y = %s;\n",
};

struct big8_amodes big8_addmodes[] = {
    {0x09, 1, &amode_immediate},
    {0x05, 0, &amode_zpage},
    {0x15, 0, &amode_zpagex},
    {0x0d, 0, &amode_abs},
    {0x1d, 0, &amode_absx},
    {0x19, 0, &amode_absy},
    {0x01, 0, &amode_indx},
    {0x11, 0, &amode_indy},
    {0x00, 0, NULL} /* must be last entry */
};

struct branch_instruction branch_instructions[] = {
    {0x10, "BPL", "NEGATIVE", '!'},
    {0x30, "BMI", "NEGATIVE", ' '},
    {0x50, "BVC", "OVERFLOW", '!'},
    {0x70, "BVS", "OVERFLOW", ' '},
    {0x90, "BCC", "CARRY", '!'},
    {0xb0, "BCS", "CARRY", ' '},
    {0xd0, "BNE", "ZERO", '!'},
    {0xf0, "BEQ", "ZERO", ' '},
    {0x00, NULL} /* must be last entry */
};

struct stack_instruction stack_instructions[] = {
    {0x48, "PHA", 1, 0, 0, "context->reg_a"},
    {0x08, "PHP", 1, 0, 1, "tmp"},
    {0x68, "PLA", 0, 1, 0, "context->reg_a"},
    {0x28, "PLP", 0, 0, 1, "tmp"},
    {0x00, NULL} /* must be last entry */
};

struct reg_transfer_instruction reg_transfer_instructions[] = {
    {0xaa, "TAX", "context->reg_a", "context->reg_x", 0, 1},
    {0xa8, "TAY", "context->reg_a", "context->reg_y", 0, 1},
    {0xba, "TSX", "context->reg_s", "context->reg_x", 0, 1},
    {0x8a, "TXA", "context->reg_x", "context->reg_a", 0, 1},
    {0x9a, "TXS", "context->reg_x", "context->reg_s", 0, 0},
    {0x98, "TYA", "context->reg_y", "context->reg_a", 0, 1},
    {0x00, NULL} /* must be last entry */
};

struct bit_test_instruction bit_test_instructions[] = {
    {0x24, "BIT", 0, NULL, &amode_zpage},
    {0x2c, "BIT", 0, NULL, &amode_abs},
    {0x00, NULL} /* must be last entry */
};

struct inc_dec_instruction inc_dec_instructions[] = {
    {0xe6, "INC", '+', &amode_zpage},
    {0xf6, "INC", '+', &amode_zpagex},
    {0xee, "INC", '+', &amode_abs},
    {0xfe, "INC", '+', &amode_absx},
    {0xe8, "INX", '+', &amode_regx},
    {0xc8, "INY", '+', &amode_regy},
    {0xc6, "DEC", '-', &amode_zpage},
    {0xd6, "DEC", '-', &amode_zpagex},
    {0xce, "DEC", '-', &amode_abs},
    {0xde, "DEC", '-', &amode_absx},
    {0xca, "DEX", '-', &amode_regx},
    {0x88, "DEY", '-', &amode_regy},
    {0x00, NULL} /* must be last entry */
};

struct load_store_instruction load_store_instructions[] = {
    {0xa2, "LDX", 0, 0, "context->reg_x", &amode_immediate},
    {0xa6, "LDX", 0, 0, "context->reg_x", &amode_zpage},
    {0xb6, "LDX", 0, 0, "context->reg_x", &amode_zpagey},
    {0xae, "LDX", 0, 0, "context->reg_x", &amode_abs},
    {0xbe, "LDX", 0, 0, "context->reg_x", &amode_absy},
    {0xa0, "LDY", 0, 0, "context->reg_y", &amode_immediate},
    {0xa4, "LDY", 0, 0, "context->reg_y", &amode_zpage},
    {0xb4, "LDY", 0, 0, "context->reg_y", &amode_zpagex},
    {0xac, "LDY", 0, 0, "context->reg_y", &amode_abs},
    {0xbc, "LDY", 0, 0, "context->reg_y", &amode_absx},
    {0x86, "STX", 1, 0, "context->reg_x", &amode_zpage},
    {0x96, "STX", 1, 0, "context->reg_x", &amode_zpagey},
    {0x8e, "STX", 1, 0, "context->reg_x", &amode_abs},
    {0x84, "STY", 1, 0, "context->reg_y", &amode_zpage},
    {0x94, "STY", 1, 0, "context->reg_y", &amode_zpagex},
    {0x8c, "STY", 1, 0, "context->reg_y", &amode_abs},
    {0x00, NULL} /* must be last entry */
};

struct misc_instruction {
    u8 opcode;
    char *instruction;
} misc_instructions[] = {
    /* call/return group */
    {0x4c, "    /* JMP Abs */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    context->pc = addr;\n"
     "    context->cycles_left -= 3;\n"},
    {0x6c, "    /* JMP Ind */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    context->pc = context->readfunc(context->cpu, addr);\n"
     "    addr = (addr & 0xff00) | ((addr + 1) & 0x00ff);\n"
     "    context->pc |= context->readfunc(context->cpu, addr) << 8;\n"
     "    context->cycles_left -= 5;\n"},
    {0x20, "    /* JSR */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    PUSH_BYTE(context, context->pc >> 8);\n"
     "    PUSH_BYTE(context, context->pc & 0xff);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    context->pc = addr;\n"
     "    context->cycles_left -= 6;\n"},
    {0x60, "    /* RTS */\n"
     "\n"
     "    context->pc = POP_BYTE(context);\n"
     "    context->pc |= POP_BYTE(context) << 8;\n"
     "    context->pc++;\n"
     "    context->cycles_left -= 6;\n"},
    {0x40, "    /* RTI */\n"
     "\n"
     "    SET_FLAGS(context, POP_BYTE(context));\n"
     "    context->pc = POP_BYTE(context);\n"
     "    context->pc |= POP_BYTE(context) << 8;\n"
     "    context->cycles_left -= 6;\n"},
    {0x00, "    /* BRK */\n"
     "\n"
     "    context->pc++;\n"
     "    PUSH_BYTE(context, context->pc >> 8);\n"
     "    PUSH_BYTE(context, context->pc & 0xff);\n"
     "    PUSH_BYTE(context, (GET_FLAGS(context)) | FLAG_B);\n"
     "    context->flags |= FLAG_I;\n"
     "    context->flags &= ~FLAG_D;\n"
     "    context->pc = context->readfunc(context->cpu, VECTOR_BRK_LO);\n"
     "    context->pc |= context->readfunc(context->cpu, VECTOR_BRK_HI) << 8;\n"
     "    context->cycles_left -= 7;\n"},
    {0xea, "    /* NOP */\n"},
    {0x00, NULL}, /* must be last entry */
};

void emit_misc_instructions(void)
{
    struct misc_instruction *cur_instr;

    for (cur_instr = misc_instructions; cur_instr->instruction; cur_instr++) {
	emit_procheader(cur_instr->opcode);
	emit(cur_instr->instruction);
	emit_proctrailer();
    }
}

instr_emitter_t emitter_list[] = {
    emit_flag_instructions,
    emit_big8_instructions,
    emit_branch_instructions,
    emit_stack_instructions,
    emit_reg_transfer_instructions,
    emit_bit_test_instructions,
    emit_inc_dec_instructions,
    emit_load_store_instructions,
    emit_shift_rotate_instructions,
    emit_compare_instructions,
    emit_misc_instructions,
    NULL, /* must be last entry */
};

/*
 * $Log: build_cpu6502.c,v $
 * Revision 1.9  2000/10/02 01:50:59  nyef
 * moved a lot of stuff out to build_cpu6xxx_common.h
 *
 * Revision 1.8  2000/09/10 15:13:13  nyef
 * fixed a missed lazy flag evaluation patch (causing SBC problems)
 *
 * Revision 1.7  2000/09/09 15:32:03  nyef
 * fixed bug with overflow flag calculation in the SBC instruction
 *
 * Revision 1.6  2000/09/09 03:23:11  nyef
 * fixed bug where the C flag was being clobbered in SBC when in D mode
 *
 * Revision 1.5  2000/07/15 22:18:56  nyef
 * converted to use lazy flag evaluation
 *
 * Revision 1.4  2000/07/08 15:15:21  nyef
 * cleaned up and optimized the conditional branch instructions
 *
 * Revision 1.3  2000/07/08 14:55:58  nyef
 * cleaned up and optimized the absolute and indexed addressing modes
 *
 * Revision 1.2  2000/07/03 02:20:25  nyef
 * fixed instruction cycle times
 *
 * Revision 1.1  2000/05/01 00:34:17  nyef
 * Initial revision
 *
 */
