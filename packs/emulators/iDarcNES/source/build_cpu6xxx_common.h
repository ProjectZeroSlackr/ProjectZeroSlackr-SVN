/*
 * build_cpu6xxx_common.h
 *
 * common code/data for the 6502 and 6280 core generators
 */

/* $Id: build_cpu6xxx_common.h,v 1.1 2000/10/02 01:51:05 nyef Exp $ */

#ifndef BUILD_CPU6XXX_COMMON_H
#define BUILD_CPU6XXX_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "types.h"

FILE *outfile;

int handlers[0x100];
int cur_handler;

const char file_header[] = "/*\n * %s\n *\n * opcode handlers for " CORE " core\n *\n * MACHINE GENERATED. DO NOT EDIT.\n */\n\n#include \"" CORE "int.h\"\n\n";

const char file_footer[] = "/* EOF */\n";

typedef void (*instr_emitter_t)(void);

void emit(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(outfile, format, args);
    va_end(args);
}

void emit_file_header(char *filename)
{
    emit(file_header, filename);
}

void emit_file_footer(void)
{
    emit(file_footer);
}

void clear_handlers(void)
{
    int i;

    for (i = 0; i < 0x100; i++) {
	handlers[i] = 0;
    }

    cur_handler = 1;
}

void emit_decode_table(void)
{
    int i;

    emit("ophandler_t " CORE "_decode[0x100] = {\n");
    
    for (i = 0; i < 0x100; i++) {
	if ((i & 3) == 0) {
	    emit("   ");
	}
	emit(" " CORE "_op_%d,", handlers[i]);
	if ((i & 3) == 3) {
	    emit(" /* 0x%02x - 0x%02x */\n", i & 0xfc, i);
	}
    }

    emit("};\n\n");
}

void emit_procheader(u8 opcode)
{
    emit("void " CORE "_op_%d(struct " CORE "_context *context)\n", cur_handler);
    emit("{\n");
    emit("    /* 0x%02x */\n", opcode);
    handlers[opcode] = cur_handler;
}

void emit_proctrailer(void)
{
    emit("}\n\n");

    cur_handler++;
}

struct {
    u8 opcode;
    char *instruction;
    char *flag;
    int set;
    int is_lazy;
} flag_instructions[] = {
    {0x18, "CLC", "FLAG_C", 0, 1},
    {0x38, "SEC", "FLAG_C", 1, 1},
    {0xd8, "CLD", "FLAG_D", 0, 0},
    {0xf8, "SED", "FLAG_D", 1, 0},
    {0x58, "CLI", "FLAG_I", 0, 0},
    {0x78, "SEI", "FLAG_I", 1, 0},
    {0xb8, "CLV", "FLAG_V", 0, 1},
};

void emit_flag_instructions(void)
{
    int i;

    for (i = 0; i < (sizeof(flag_instructions) / sizeof(flag_instructions[0])); i++) {
	emit_procheader(flag_instructions[i].opcode);
	emit("    /* %s */\n", flag_instructions[i].instruction);
	if (flag_instructions[i].is_lazy) {
	    emit("#ifdef LAZY_FLAG_EVALUATION\n");
	    emit("    context->flag_%c = %d;\n", tolower(flag_instructions[i].instruction[2]), flag_instructions[i].set);
	    emit("#else\n");
	}
	if (flag_instructions[i].set) {
	    emit("    context->flags |= %s;\n", flag_instructions[i].flag);
	} else {
	    emit("    context->flags &= ~%s;\n", flag_instructions[i].flag);
	}
	if (flag_instructions[i].is_lazy) {
	    emit("#endif\n");
	}
	emit("    context->cycles_left -= 2;\n");
	emit_proctrailer();
    }
}

struct big8_instruction {
    u8 opcode_base;
    char *instruction;
    int is_sta;
    char *block;
} big8_instructions[] = {
    {0x00, "ORA", 0, "    result = context->reg_a |= tmp;\n"},
    {0x20, "AND", 0, "    result = context->reg_a &= tmp;\n"},
    {0x40, "EOR", 0, "    result = context->reg_a ^= tmp;\n"},
    {0x60, "ADC", 0,
     /* FIXME: probably gives bogus N and V flag values in decimal mode */
     "    result = context->reg_a + tmp + (STATUS_CARRY(context));\n"
     "    SET_FLAG_V(context, (((result ^ tmp) & (context->reg_a ^ result)) & 0x80));\n"
     "    if (context->flags & FLAG_D) {\n"
     "        u8 al;\n"
     "        u8 ah;\n"
     "\n"
     "        al = (context->reg_a & 0x0f) + (tmp & 0x0f) + (STATUS_CARRY(context));\n"
     "        if (al > 9) al += 6;\n"
     "        ah = (context->reg_a >> 4) + (tmp >> 4) + (al > 0x0f);\n"
     "        if (ah > 9) ah += 6;\n"
     "        SET_FLAG_C(context, (ah > 15));\n"
     "        context->reg_a = (ah << 4) | (al & 0x0f);\n"
     "    } else {\n"
     "        SET_FLAG_C(context, !!(result & 0xff00));\n"
     "        context->reg_a = result;\n"
     "    }\n"
     "    result &= 0xff;\n"},
    {0x80, "STA", 1, NULL},
    {0xa0, "LDA", 0, "    result = context->reg_a = tmp;\n"},
    {0xc0, "CMP", 0,
     "    result = context->reg_a - tmp;\n"
     "    SET_FLAG_C(context, !(result & 0xff00));\n"
     "    result &= 0xff;\n"},
    {0xe0, "SBC", 0,
     "    result = (context->reg_a - tmp) - (1 - (STATUS_CARRY(context)));\n"
     "    SET_FLAG_V(context, (((context->reg_a ^ tmp) & (context->reg_a ^ result)) & 0x80));\n"
     "    if (context->flags & FLAG_D) {\n"
     "        u8 al;\n"
     "        u8 ah;\n"
     "\n"
     "        al = ((context->reg_a & 0x0f) - (tmp & 0x0f)) - (1 - (STATUS_CARRY(context)));\n"
     "        if (al & 0x10) al -= 6;\n"
     "        ah = ((context->reg_a >> 4) - (tmp >> 4)) - ((al & 0x10) >> 4);\n"
     "        if (ah & 0x10) ah -= 6;\n"
     "        context->reg_a = (ah << 4) | (al & 0x0f);\n"
     "    } else {\n"
     "        context->reg_a = result;\n"
     "    }\n"
     "    SET_FLAG_C(context, !(result & 0xff00));\n"
     "    result &= 0xff;\n"},
};

struct amode {
    int cycles;
    char *admode;
    char *preblock;
    char *readblock;
    char *writeblock;
};

extern struct amode amode_immediate;
extern struct amode amode_zpage;
extern struct amode amode_zpagex;
extern struct amode amode_zpagey;
extern struct amode amode_abs;
extern struct amode amode_absx;
extern struct amode amode_absy;
extern struct amode amode_ind;
extern struct amode amode_indx;
extern struct amode amode_indy;
extern struct amode amode_rega;
extern struct amode amode_regx;
extern struct amode amode_regy;

struct big8_amodes {
    u8 opcode_delta;
    int is_imm;
    struct amode *amode;
};

extern struct big8_amodes big8_addmodes[];

void emit_big8(struct big8_instruction *instr)
{
    int i;
    
    for (i = 0; big8_addmodes[i].amode; i++) {
	if (instr->is_sta && big8_addmodes[i].is_imm) {
	    continue;
	}
	
	emit_procheader(instr->opcode_base + big8_addmodes[i].opcode_delta);

	emit("    /* %s %s */\n", instr->instruction, big8_addmodes[i].amode->admode);
	
	if (!instr->is_sta) {
	    emit("    u8 tmp;\n");
	    emit("    u16 result;\n");
	}
	
	emit(big8_addmodes[i].amode->preblock);

	if (!instr->is_sta) {
	    emit(big8_addmodes[i].amode->readblock, "tmp");
	    emit(instr->block);
	    emit("    EVAL_FLAGNZ(context, result);\n");
	} else {
	    emit(big8_addmodes[i].amode->writeblock, "context->reg_a");
	}

	emit("    context->cycles_left -= %d;\n", big8_addmodes[i].amode->cycles);
	
	emit_proctrailer();
    }
}

void emit_big8_instructions(void)
{
    int i;

    for (i = 0; i < (sizeof(big8_instructions) / sizeof(big8_instructions[0])); i++) {
	emit_big8(&big8_instructions[i]);
    }
}

struct branch_instruction {
    u8 opcode;
    char *instruction;
    char *flag;
    char test;
};

extern struct branch_instruction branch_instructions[];

void emit_branch_instructions(void)
{
    int i;

    for (i = 0; branch_instructions[i].instruction; i++) {
	emit_procheader(branch_instructions[i].opcode);
	emit("    /* %s */\n", branch_instructions[i].instruction);
	if (branch_instructions[i].flag) {
	    emit("    if (%c(STATUS_%s(context))) {\n", branch_instructions[i].test, branch_instructions[i].flag);
	}
	emit("        s8 delta;\n\n");
	emit("        delta = OPFETCH(context);\n");
	emit("        if (((context->pc & 0xff) + delta) & 0xff00) {\n");
	emit("            context->cycles_left -= 1;\n");
	emit("        }\n");
	emit("        context->pc += delta;\n");
	emit("        context->cycles_left -= 3;\n");
	if (branch_instructions[i].flag) {
	    emit("    } else {\n");
	    emit("        context->pc++;\n");
	    emit("        context->cycles_left -= 2;\n");
	    emit("    }\n");
	}
	emit_proctrailer();
    }
}

struct stack_instruction {
    u8 opcode;
    char *instruction;
    int is_push;
    int eval_nz;
    int is_phlags;
    char *reg;
};

extern struct stack_instruction stack_instructions[];

void emit_stack_instructions(void)
{
    int i;

    for (i = 0; stack_instructions[i].instruction; i++) {
	emit_procheader(stack_instructions[i].opcode);
	emit("    /* %s */\n", stack_instructions[i].instruction);
	if (stack_instructions[i].is_phlags) {
	    emit("    u8 tmp;\n\n");
	}
	if (stack_instructions[i].is_push) {
	    if (stack_instructions[i].is_phlags) {
		emit("    tmp = GET_FLAGS(context);\n");
	    }
	    emit("    PUSH_BYTE(context, %s);\n", stack_instructions[i].reg);
	} else {
	    emit("    %s = POP_BYTE(context);\n", stack_instructions[i].reg);
	    if (stack_instructions[i].is_phlags) {
		emit("    SET_FLAGS(context, tmp);\n");
	    }
	}
	if (stack_instructions[i].eval_nz) {
	    emit("    EVAL_FLAGNZ(context, %s);\n", stack_instructions[i].reg);
	}
	if (stack_instructions[i].is_push) {
	    emit("    context->cycles_left -= 3;\n");
	} else {
	    emit("    context->cycles_left -= 4;\n");
	}
	emit_proctrailer();
    }
}

struct reg_transfer_instruction {
    u8 opcode;
    char *instruction;
    char *reg1;
    char *reg2;
    int is_swap;
    int check_flags;
};

extern struct reg_transfer_instruction reg_transfer_instructions[];

void emit_reg_transfer_instructions(void)
{
    int i;

    for (i = 0; reg_transfer_instructions[i].instruction; i++) {
	emit_procheader(reg_transfer_instructions[i].opcode);
	emit("    /* %s */\n", reg_transfer_instructions[i].instruction);
	if (reg_transfer_instructions[i].is_swap) {
	    emit("    u8 tmp;\n");
	    emit("\n");
	    emit("    tmp = %s;\n", reg_transfer_instructions[i].reg1);
	    emit("    %s = %s;\n", reg_transfer_instructions[i].reg1, reg_transfer_instructions[i].reg2);
	    emit("    %s = tmp;\n", reg_transfer_instructions[i].reg2);
	    emit("    context->cycles_left -= 3;\n");
	} else {
	    emit("    %s = %s;\n", reg_transfer_instructions[i].reg2, reg_transfer_instructions[i].reg1);
	    emit("    context->cycles_left -= 2;\n");
	}
	if (reg_transfer_instructions[i].check_flags) {
	    emit("    EVAL_FLAGNZ(context, %s);\n", reg_transfer_instructions[i].reg2);
	}
	emit_proctrailer();
    }
}

struct bit_test_instruction {
    u8 opcode;
    char *instruction;
    int is_tst;
    char *rmw_op;
    struct amode *amode;
};

extern struct bit_test_instruction bit_test_instructions[];

void emit_bit_test_instructions(void)
{
    int i;
    struct amode *cur_amode;

    for (i = 0; bit_test_instructions[i].instruction; i++) {
	cur_amode = bit_test_instructions[i].amode;
    
	emit_procheader(bit_test_instructions[i].opcode);
	emit("    /* %s %s */\n", bit_test_instructions[i].instruction, cur_amode->admode);
	emit("    u8 data;\n");
	if (bit_test_instructions[i].is_tst) {
	    emit("    u8 reg = OPFETCH(context);\n");
	} else {
	    emit("    u8 reg = context->reg_a;\n");
	}
	emit(cur_amode->preblock);
	emit(cur_amode->readblock, "data");
	emit("#ifndef LAZY_FLAG_EVALUATION\n");
	emit("    context->flags &= ~(0xc0 | FLAG_Z);\n");
	emit("    context->flags |= data & 0xc0;\n");
	emit("    if (!(reg & data)) {\n");
	emit("        context->flags |= FLAG_Z;\n");
	emit("    }\n");
	emit("#else\n");
	emit("    context->flag_n = data;\n");
	emit("    context->flag_v = (data & FLAG_V) << 1;\n");
	emit("    context->flag_z = reg & data;\n");
	emit("#endif\n");
	if (bit_test_instructions[i].rmw_op) {
	    emit("    data %sreg;\n", bit_test_instructions[i].rmw_op);
	    emit(cur_amode->writeblock, "data");
	    emit("    context->cycles_left -= %d;\n", cur_amode->cycles + 2);
	} else if (bit_test_instructions[i].is_tst) {
	    emit("    context->cycles_left -= %d;\n", cur_amode->cycles + 3);
	} else {
	    emit("    context->cycles_left -= %d;\n", cur_amode->cycles);
	}
	emit_proctrailer();
    }
}

struct inc_dec_instruction {
    u8 opcode;
    char *instruction;
    char operation;
    struct amode *amode;
};

extern struct inc_dec_instruction inc_dec_instructions[];

void emit_inc_dec_instructions(void)
{
    int i;
    struct amode *cur_amode;

    for (i = 0; inc_dec_instructions[i].instruction; i++) {
	cur_amode = inc_dec_instructions[i].amode;
    
	emit_procheader(inc_dec_instructions[i].opcode);
	emit("    /* %s %s */\n", inc_dec_instructions[i].instruction, cur_amode->admode);
	emit("    u8 data;\n");
	emit(cur_amode->preblock);
	emit(cur_amode->readblock, "data");
	emit("    data %c= 1;\n", inc_dec_instructions[i].operation);
	emit(cur_amode->writeblock, "data");
	emit("    EVAL_FLAGNZ(context, data);\n");
	emit("    context->cycles_left -= %d;\n", cur_amode->cycles + 2);
	emit_proctrailer();
    }
}

struct load_store_instruction {
    u8 opcode;
    char *instruction;
    int is_store;
    int cycle_delta;
    char *reg;
    struct amode *amode;
};

extern struct load_store_instruction load_store_instructions[];

void emit_load_store_instructions(void)
{
    int i;
    struct amode *cur_amode;

    for (i = 0; load_store_instructions[i].instruction; i++) {
	cur_amode = load_store_instructions[i].amode;
    
	emit_procheader(load_store_instructions[i].opcode);
	emit("    /* %s %s */\n", load_store_instructions[i].instruction, cur_amode->admode);
	emit(cur_amode->preblock);
	if (!(load_store_instructions[i].is_store)) {
	    emit(cur_amode->readblock, load_store_instructions[i].reg);
	    emit("    EVAL_FLAGNZ(context, %s);\n", load_store_instructions[i].reg);
	} else {
	    emit(cur_amode->writeblock, load_store_instructions[i].reg);
	}
	emit("    context->cycles_left -= %d;\n", cur_amode->cycles + load_store_instructions[i].cycle_delta);
	emit_proctrailer();
    }
}

struct shift_rotate_instruction {
    u8 opcode;
    char *instruction;
    int is_rotate;
    int is_left;
} shift_rotate_instructions[] = {
    {0x00, "ASL", 0, 1},
    {0x20, "ROL", 1, 1},
    {0x40, "LSR", 0, 0},
    {0x60, "ROR", 1, 0},
    {0x00, NULL, 0, 0}, /* must be last entry */
};

struct shift_rotate_amode {
    u8 opcode_delta;
    struct amode *amode;
} shift_rotate_amodes[] = {
    {0x0a, &amode_rega},
    {0x06, &amode_zpage},
    {0x16, &amode_zpagex},
    {0x0e, &amode_abs},
    {0x1e, &amode_absx},
    {0x00, NULL}, /* must be last entry */
};

void emit_shift_rotate_instructions(void)
{
    struct shift_rotate_instruction *cur_instr;
    struct shift_rotate_amode *cur_amode;
    
    for (cur_instr = shift_rotate_instructions; cur_instr->instruction; cur_instr++) {
	for (cur_amode = shift_rotate_amodes; cur_amode->amode; cur_amode++) {
	    emit_procheader(cur_instr->opcode + cur_amode->opcode_delta);
	    emit("    /* %s %s */\n", cur_instr->instruction, cur_amode->amode->admode);
	    emit("    u8 src_data;\n");
	    emit("    u8 dst_data;\n");
	    emit(cur_amode->amode->preblock);
	    emit(cur_amode->amode->readblock, "src_data");
	    emit("    dst_data = src_data %s 1;\n", cur_instr->is_left? "<<": ">>");
	    if (cur_instr->is_rotate) {
		if (cur_instr->is_left) {
		    emit("    dst_data |= STATUS_CARRY(context);\n");
		} else {
		    emit("    dst_data |= STATUS_CARRY(context) << 7;\n");
		}
	    }
	    emit(cur_amode->amode->writeblock, "dst_data");
	    if (cur_instr->is_left) {
		emit("    SET_FLAG_C(context, src_data >> 7);\n");
	    } else {
		emit("    SET_FLAG_C(context, src_data & FLAG_C);\n");
	    }
	    emit("    EVAL_FLAGNZ(context, dst_data);\n");
	    emit("    context->cycles_left -= %d;\n", cur_amode->amode->cycles + 2);
	    emit_proctrailer();
	}
    }
}

struct compare_amode {
    u8 opcode_delta;
    struct amode *amode;
} compare_amodes[] = {
    {0xc0, &amode_immediate},
    {0xc4, &amode_zpage},
    {0xcc, &amode_abs},
    {0x00, NULL},
};

void emit_compare_instructions(void)
{
    int i;
    struct compare_amode *cur_amode;

    for (i = 0; i < 2; i++) {
	for (cur_amode = compare_amodes; cur_amode->amode; cur_amode++) {
	    emit_procheader((i << 5) + cur_amode->opcode_delta);
	    emit("    /* CP%c %s */\n", 'Y' - i, cur_amode->amode->admode);
	    emit("    u8 tmp;\n");
	    emit("    u8 result;\n");
	    
	    emit(cur_amode->amode->preblock);
	    emit(cur_amode->amode->readblock, "tmp");
	    
	    emit("    result = context->reg_%c - tmp;\n", 'y' - i);
	    emit("    SET_FLAG_C(context, context->reg_%c >= tmp);\n", 'y'-i);
	    emit("    EVAL_FLAGNZ(context, result);\n");
	    emit("    context->cycles_left -= %d;\n", cur_amode->amode->cycles);
	    emit_proctrailer();
	}
    }
}

extern instr_emitter_t emitter_list[];

void build_core(void)
{
    int i;

    for (i = 0; emitter_list[i]; i++) {
	emitter_list[i]();
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
	printf("usage: %s <outfile>\n", argv[0]);
	return 1;
    }

    outfile = fopen(argv[1], "w");
    if (!outfile) {
	perror("Unable to open file for writing");
	return 1;
    }

    emit_file_header(argv[1]);

    clear_handlers();

    build_core();
    
    emit_decode_table();
    
    emit_file_footer();

    if (fclose(outfile) == EOF) {
	perror("Closing file");
	return 1;
    }
    
    return 0;
}

#endif /* BUILD_CPU6XXX_COMMON_H */

/*
 * $Log: build_cpu6xxx_common.h,v $
 * Revision 1.1  2000/10/02 01:51:05  nyef
 * Initial revision
 *
 */
