/*
 * build_cpu6280.c
 *
 * build the cpu6280 core opcode handlers
 */

/* $Id: build_cpu6280.c,v 1.33 2000/10/02 01:51:02 nyef Exp $ */

#define CORE "cpu6280"

#include "build_cpu6xxx_common.h"

struct amode amode_immediate = {
    2, "Imm",
    "\n",
    "    %s = OPFETCH(context);\n",
    "#error Write to Immediate addressing mode\n",
};

struct amode amode_zpage = {
    4, "ZPage",
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
    5, "Abs",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += (OPFETCH(context) << 8);\n",
    "    %s = context->readfunc(context->cpu, addr);\n",
    "    context->writefunc(context->cpu, addr, %s);\n"
};

struct amode amode_absx = {
    5, "Abs, X",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += (OPFETCH(context) << 8);\n"
    "    addr += context->reg_x;\n",
    "    %s = context->readfunc(context->cpu, addr);\n",
    "    context->writefunc(context->cpu, addr, %s);\n"
};

struct amode amode_absy = {
    5, "Abs, Y",
    "    u16 addr;\n\n"
    "    addr = OPFETCH(context);\n"
    "    addr += (OPFETCH(context) << 8);\n"
    "    addr += context->reg_y;\n",
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
    7, "(Ind, X)",
    "    u8 addr1;\n"
    "    u16 addr2;\n\n"
    "    addr1 = OPFETCH(context) + context->reg_x;\n"
    "    addr2 = context->zpage[addr1++];\n"
    "    addr2 += context->zpage[addr1] << 8;\n",
    "    %s = context->readfunc(context->cpu, addr2);\n",
    "    context->writefunc(context->cpu, addr2, %s);\n"
};

struct amode amode_indy = {
    7, "(Ind), Y",
    "    u8 addr1;\n"
    "    u16 addr2;\n\n"
    "    addr1 = OPFETCH(context);\n"
    "    addr2 = context->zpage[addr1++];\n"
    "    addr2 += context->zpage[addr1] << 8;\n"
    "    addr2 += context->reg_y;\n",
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
    {0x12, 0, &amode_ind},
    {0x01, 0, &amode_indx},
    {0x11, 0, &amode_indy},
    {0x00, 0, NULL} /* must be last entry */
};

struct branch_instruction branch_instructions[] = {
    {0x80, "BRA", NULL, 0},
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

void emit_bit_test_branch_instructions(void)
{
    int i;

    for (i = 0; i < 0x10; i++) {
	emit_procheader((i << 4) + 0x0f);
	emit("    /* BB%c%d */\n", 'R' + (i >> 3), i & 7);
	emit("    u8 addr;\n");
	emit("\n");
	emit("    addr = OPFETCH(context);\n");
	emit("\n");
	emit("    if (%c(context->zpage[addr] & 0x%02x)) {\n", (i & 8)? ' ': '!', 1 << (i & 7));
	emit("        s8 delta;\n");
	emit("\n");
	emit("        delta = OPFETCH(context);\n");
	emit("        context->pc += delta;\n");
	emit("        context->cycles_left -= 8;\n");
	emit("    } else {\n");
	emit("        context->pc++;\n");
	emit("        context->cycles_left -= 6;\n");
	emit("    }\n");
	emit_proctrailer();
    }
}

struct stack_instruction stack_instructions[] = {
    {0x48, "PHA", 1, 0, 0, "context->reg_a"},
    {0x08, "PHP", 1, 0, 1, "tmp"},
    {0xda, "PHX", 1, 0, 0, "context->reg_x"},
    {0x5a, "PHY", 1, 0, 0, "context->reg_y"},
    {0x68, "PLA", 0, 1, 0, "context->reg_a"},
    {0x28, "PLP", 0, 0, 1, "tmp"},
    {0xfa, "PLX", 0, 1, 0, "context->reg_x"},
    {0x7a, "PLY", 0, 1, 0, "context->reg_y"},
    {0x00, NULL} /* must be last entry */
};

struct reg_transfer_instruction reg_transfer_instructions[] = {
    {0x22, "SAX", "context->reg_a", "context->reg_x", 1, 0},
    {0x42, "SAY", "context->reg_a", "context->reg_y", 1, 0},
    {0x02, "SXY", "context->reg_x", "context->reg_y", 1, 0},
    {0xaa, "TAX", "context->reg_a", "context->reg_x", 0, 1},
    {0xa8, "TAY", "context->reg_a", "context->reg_y", 0, 1},
    {0xba, "TSX", "context->reg_s", "context->reg_x", 0, 1},
    {0x8a, "TXA", "context->reg_x", "context->reg_a", 0, 1},
    {0x9a, "TXS", "context->reg_x", "context->reg_s", 0, 0},
    {0x98, "TYA", "context->reg_y", "context->reg_a", 0, 1},
    {0x00, NULL} /* must be last entry */
};

struct bit_test_instruction bit_test_instructions[] = {
    {0x89, "BIT", 0, NULL, &amode_immediate},
    {0x24, "BIT", 0, NULL, &amode_zpage},
    {0x34, "BIT", 0, NULL, &amode_zpagex},
    {0x2c, "BIT", 0, NULL, &amode_abs},
    {0x3c, "BIT", 0, NULL, &amode_absx},
    {0x83, "TST", 1, NULL, &amode_zpage},
    {0xa3, "TST", 1, NULL, &amode_zpagex},
    {0x93, "TST", 1, NULL, &amode_abs},
    {0xb3, "TST", 1, NULL, &amode_absx},
    {0x14, "TRB", 0, "&=~", &amode_zpage},
    {0x1c, "TRB", 0, "&=~", &amode_abs},
    {0x04, "TSB", 0, "|= ", &amode_zpage},
    {0x0c, "TSB", 0, "|= ", &amode_abs},
    {0x00, NULL} /* must be last entry */
};

struct inc_dec_instruction inc_dec_instructions[] = {
    {0xe6, "INC", '+', &amode_zpage},
    {0xf6, "INC", '+', &amode_zpagex},
    {0xee, "INC", '+', &amode_abs},
    {0xfe, "INC", '+', &amode_absx},
    {0x1a, "INC", '+', &amode_rega},
    {0xe8, "INX", '+', &amode_regx},
    {0xc8, "INY", '+', &amode_regy},
    {0xc6, "DEC", '-', &amode_zpage},
    {0xd6, "DEC", '-', &amode_zpagex},
    {0xce, "DEC", '-', &amode_abs},
    {0xde, "DEC", '-', &amode_absx},
    {0x3a, "DEC", '-', &amode_rega},
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
    {0x62, "CLA", 1, 2, "0", &amode_rega},
    {0x82, "CLX", 1, 2, "0", &amode_regx},
    {0xc2, "CLY", 1, 2, "0", &amode_regy},
    {0x86, "STX", 1, 0, "context->reg_x", &amode_zpage},
    {0x96, "STX", 1, 0, "context->reg_x", &amode_zpagey},
    {0x8e, "STX", 1, 0, "context->reg_x", &amode_abs},
    {0x84, "STY", 1, 0, "context->reg_y", &amode_zpage},
    {0x94, "STY", 1, 0, "context->reg_y", &amode_zpagex},
    {0x8c, "STY", 1, 0, "context->reg_y", &amode_abs},
    {0x64, "STZ", 1, 0, "0", &amode_zpage},
    {0x74, "STZ", 1, 0, "0", &amode_zpagex},
    {0x9c, "STZ", 1, 0, "0", &amode_abs},
    {0x9e, "STZ", 1, 0, "0", &amode_absx},
    {0x00, NULL} /* must be last entry */
};

void emit_bit_manipulation_instructions(void)
{
    int i;

    for (i = 0; i < 16; i++) {
	emit_procheader((i << 4) + 0x07);
	emit("    /* %cMB%d */\n", 'R' + (i >> 3), i & 7);
	emit("    u8 addr;\n");
	emit("\n");
	emit("    addr = OPFETCH(context);\n");
	emit("\n");
	emit("    context->zpage[addr] %s%d;\n", (i & 8)? "|= ": "&= ~", 1 << (i & 7));
	emit("\n");
	emit("    context->cycles_left -= 7;\n");
	emit_proctrailer();
    }
}

struct block_instruction {
    u8 opcode;
    char *instruction;
    char *src_op1;
    char *src_op2;
    char *dst_op1;
    char *dst_op2;
} block_instructions[] = {
    {0xf3, "TAI", "++", "--", "++", "++"},
    {0xc3, "TDD", "--", "--", "--", "--"},
    {0xe3, "TIA", "++", "++", "++", "--"},
    {0x73, "TII", "++", "++", "++", "++"},
    {0xd3, "TIN", "++", "++", "", ""},
    {0x00, NULL, NULL, NULL, NULL, NULL}, /* must be last entry */
};

void emit_block_instructions(void)
{
    struct block_instruction *cur_instr;

    for (cur_instr = block_instructions; cur_instr->instruction; cur_instr++) {
	emit_procheader(cur_instr->opcode);
	emit("    /* %s */\n", cur_instr->instruction);
	emit("    u16 source;\n");
	emit("    u16 dest;\n");
	emit("    u32 length;\n");
	emit("\n");
	emit("    source = OPFETCH(context);\n");
	emit("    source |= OPFETCH(context) << 8;\n");
	emit("    dest = OPFETCH(context);\n");
	emit("    dest |= OPFETCH(context) << 8;\n");
	emit("    length = OPFETCH(context);\n");
	emit("    length |= OPFETCH(context) << 8;\n");
	emit("\n");
	emit("    if (!length) length = 0x10000;\n");
	emit("    context->cycles_left -= (17 + (length * 6));\n");
	emit("\n");
	emit("    for (;;) {\n");
	emit("        context->writefunc(context->cpu, dest%s, context->readfunc(context->cpu, source%s));\n", cur_instr->dst_op1, cur_instr->src_op1);
	emit("        if (!--length) break;\n");
	emit("        context->writefunc(context->cpu, dest%s, context->readfunc(context->cpu, source%s));\n", cur_instr->dst_op2, cur_instr->src_op2);
	emit("        if (!--length) break;\n");
	emit("    }\n");
	emit_proctrailer();
    }
}

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
     "    context->cycles_left -= 4;\n"},
    {0x6c, "    /* JMP Ind */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    context->pc = context->readfunc(context->cpu, addr++);\n"
     "    context->pc |= context->readfunc(context->cpu, addr) << 8;\n"
     "    context->cycles_left -= 7;\n"},
    {0x7c, "    /* JMP Ind, X */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    addr += context->reg_x;\n"
     "    context->pc = context->readfunc(context->cpu, addr++);\n"
     "    context->pc |= context->readfunc(context->cpu, addr) << 8;\n"
     "    context->cycles_left -= 7;\n"},
    {0x20, "    /* JSR */\n"
     "    u16 addr;\n"
     "\n"
     "    addr = OPFETCH(context);\n"
     "    PUSH_BYTE(context, context->pc >> 8);\n"
     "    PUSH_BYTE(context, context->pc & 0xff);\n"
     "    addr |= OPFETCH(context) << 8;\n"
     "    context->pc = addr;\n"
     "    context->cycles_left -= 7;\n"},
    {0x44, "    /* BSR */\n"
     "    s8 delta;\n"
     "\n"
     "    PUSH_BYTE(context, context->pc >> 8);\n"
     "    PUSH_BYTE(context, context->pc & 0xff);\n"
     "    delta = OPFETCH(context);\n"
     "    context->pc += delta;\n"
     "    context->cycles_left -= 8;\n"},
    {0x60, "    /* RTS */\n"
     "\n"
     "    context->pc = POP_BYTE(context);\n"
     "    context->pc |= POP_BYTE(context) << 8;\n"
     "    context->pc++;\n"
     "    context->cycles_left -= 7;\n"},
    {0x40, "    /* RTI */\n"
     "\n"
     "    SET_FLAGS(context, POP_BYTE(context));\n"
     "    context->pc = POP_BYTE(context);\n"
     "    context->pc |= POP_BYTE(context) << 8;\n"
     "    context->cycles_left -= 7;\n"},
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
     "    context->cycles_left -= 8;\n"},
    {0xea, "    /* NOP */\n"},
    {0xd4, "    /* CSH */\n"},
    {0x54, "    /* CSL */\n"},
    {0x03, "    /* ST0 */\n"
     "    void pce_st0_hack(u8);\n"
     "    pce_st0_hack(OPFETCH(context));\n"
     "    context->cycles_left -= 4;\n"},
    {0x13, "    /* ST1 */\n"
     "    void pce_st1_hack(u8);\n"
     "    pce_st1_hack(OPFETCH(context));\n"
     "    context->cycles_left -= 4;\n"},
    {0x23, "    /* ST2 */\n"
     "    void pce_st2_hack(u8);\n"
     "    pce_st2_hack(OPFETCH(context));\n"
     "    context->cycles_left -= 4;\n"},
    {0x43, "    /* TMAi */\n"
     "    u8 i;\n"
     "\n"
     "    i = OPFETCH(context);\n"
     "    if (i & 0x80) { context->reg_a = context->mpr[7]; }\n"
     "    if (i & 0x40) { context->reg_a = context->mpr[6]; }\n"
     "    if (i & 0x20) { context->reg_a = context->mpr[5]; }\n"
     "    if (i & 0x10) { context->reg_a = context->mpr[4]; }\n"
     "    if (i & 0x08) { context->reg_a = context->mpr[3]; }\n"
     "    if (i & 0x04) { context->reg_a = context->mpr[2]; }\n"
     "    if (i & 0x02) { context->reg_a = context->mpr[1]; }\n"
     "    if (i & 0x01) { context->reg_a = context->mpr[0]; }\n"
     "    context->cycles_left -= 4;\n"},
    {0x53, "    /* TAMi */\n"
     "    u8 i;\n"
     "\n"
     "    i = OPFETCH(context);\n"
     "    if (i & 0x80) { context->mpr[7] = context->reg_a; }\n"
     "    if (i & 0x40) { context->mpr[6] = context->reg_a; }\n"
     "    if (i & 0x20) { context->mpr[5] = context->reg_a; }\n"
     "    if (i & 0x10) { context->mpr[4] = context->reg_a; }\n"
     "    if (i & 0x08) { context->mpr[3] = context->reg_a; }\n"
     "    if (i & 0x04) { context->mpr[2] = context->reg_a; }\n"
     "    if (i & 0x02) { context->mpr[1] = context->reg_a; }\n"
     "    if (i & 0x01) { context->mpr[0] = context->reg_a; }\n"
     "    context->cycles_left -= 5;\n"},
    {0xf4, "    /* SET */\n"
     /* NOTE: this uses some black magic to determine what to do */
     "    u8 i;\n"
     "    u8 tmp;\n"
     "\n"
     "    i = OPFETCH(context);\n"
     /* the if is checking for ADC, AND, EOR, and ORA instructions */
     "    if (((i & 0x83) == 0x01) || ((i & 0x9f) == 0x12)) {\n"
     "        tmp = context->reg_a;\n"
     "        context->reg_a = context->zpage[context->reg_x];\n"
     "        cpu6280_decode[i](context);\n"
     "        context->zpage[context->reg_x] = context->reg_a;\n"
     "        context->reg_a = tmp;\n"
     "        context->cycles_left -= 5;\n"
     "    } else {\n"
     "        cpu6280_decode[i](context);\n"
     "        context->cycles_left -= 2;\n"
     "    }\n"},
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
    emit_bit_test_branch_instructions,
    emit_stack_instructions,
    emit_reg_transfer_instructions,
    emit_bit_test_instructions,
    emit_inc_dec_instructions,
    emit_load_store_instructions,
    emit_shift_rotate_instructions,
    emit_bit_manipulation_instructions,
    emit_compare_instructions,
    emit_block_instructions,
    emit_misc_instructions,
    NULL, /* must be last entry */
};

/*
 * $Log: build_cpu6280.c,v $
 * Revision 1.33  2000/10/02 01:51:02  nyef
 * moved a lot of stuff out to build_cpu6xxx_common.h
 *
 * Revision 1.32  2000/09/09 16:01:20  nyef
 * converted to use lazy flag evaluation
 * fixed a cycle-counting bug with the PL[APXY] instructions
 *
 * Revision 1.31  2000/09/09 02:11:14  nyef
 * fixed bug where the C flag was being clobbered in SBC when in D mode
 *
 * Revision 1.30  2000/09/07 18:43:53  nyef
 * fixed overflow calculation for ADC
 *
 * Revision 1.29  2000/02/11 03:26:09  nyef
 * added SET instruction
 *
 * Revision 1.28  2000/01/27 01:27:04  nyef
 * added some flag manipulation whatnot to the BRK instruction
 *
 * Revision 1.27  2000/01/27 00:15:50  nyef
 * changed emit_procheader() to assign the current handler to an opcode
 * changed emit_procheader() to output the opcode as a comment
 *
 * Revision 1.26  2000/01/24 05:01:46  nyef
 * added (not strictly tested) implementation of ADC in decimal mode
 *
 * Revision 1.25  2000/01/24 04:27:55  nyef
 * added (untested) implementation of SBC in decimal mode
 *
 * Revision 1.24  2000/01/24 03:43:43  nyef
 * fixed a bug with the bit test instructions
 *
 * Revision 1.23  2000/01/24 03:43:27  nyef
 * added (untested) preliminary support for ADC and SBC
 *
 * Revision 1.22  2000/01/24 00:45:33  nyef
 * added (untested) NOP, CSH, CSL, ST0, ST1, ST2, TAMi, and TMAi
 *
 * Revision 1.21  2000/01/24 00:13:14  nyef
 * added (untested) block transfer instructions
 *
 * Revision 1.20  2000/01/23 23:03:51  nyef
 * added JMP, JSR, BSR, RTS, RTI, and BRK instructions
 *
 * Revision 1.19  2000/01/23 20:00:04  nyef
 * added CPX and CPY instructions
 *
 * Revision 1.18  2000/01/23 18:33:15  nyef
 * fixed problem with debug comments on RMBi, SMBi, BBRi, and BBSi instructions
 *
 * Revision 1.17  2000/01/23 18:30:26  nyef
 * added RMBi and SMBi instructions
 *
 * Revision 1.16  2000/01/23 16:49:14  nyef
 * added shift/rotate instructions
 *
 * Revision 1.15  2000/01/23 05:50:51  nyef
 * added LDX, LDY, STX, STY, CLA, CLX, CLY, and STZ instructions
 *
 * Revision 1.14  2000/01/23 01:10:49  nyef
 * added increment and decrement instructions
 *
 * Revision 1.13  2000/01/22 22:58:41  nyef
 * added bit test instructions (BIT, TST, TSB, TRB)
 *
 * Revision 1.12  2000/01/22 19:19:13  nyef
 * separated amodes out into separate structures from the "big 8" list
 *
 * Revision 1.11  2000/01/22 16:23:47  nyef
 * added register transfer instructions
 *
 * Revision 1.10  2000/01/22 05:10:09  nyef
 * added PHA, PHP, PHX, PHY, PLA, PLP, PLX, and PLY instructions
 *
 * Revision 1.9  2000/01/22 04:09:54  nyef
 * added BBRi and BBSi instructions
 *
 * Revision 1.8  2000/01/22 03:33:36  nyef
 * added branch instructions (except BBRi and BBSi)
 *
 * Revision 1.7  2000/01/22 02:25:39  nyef
 * added (Indirect); (Indirect, X); and (Indirect), Y "big 8" addressing modes
 *
 * Revision 1.6  2000/01/21 02:27:38  nyef
 * added Absolute; Absolute, X; and Absolute, Y "big 8" addressing modes
 *
 * Revision 1.5  2000/01/20 04:07:56  nyef
 * added Immediate and ZPage, X "big 8" addressing modes
 *
 * Revision 1.4  2000/01/19 04:11:01  nyef
 * added CMP "big 8" instruction
 *
 * Revision 1.3  2000/01/19 01:34:57  nyef
 * added all "big 8" instructions other than ADC, SBC, and CMP (zpage only)
 *
 * Revision 1.2  2000/01/17 03:03:42  nyef
 * added code to create opcode handlers
 * added all flag instructions other than SET
 *
 * Revision 1.1  2000/01/17 01:04:33  nyef
 * Initial revision
 *
 */
