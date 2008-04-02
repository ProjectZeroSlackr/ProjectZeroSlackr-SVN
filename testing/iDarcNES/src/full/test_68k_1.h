/*
 * test_68k_1.h
 *
 * first new test suite for the emu68000 core
 */

/* $Id: test_68k_1.h,v 1.8 2000/12/11 18:13:57 nyef Exp $ */

/*
 * test branch instructions (0x6xxx)
 */

void test_branch_taken_short(int condition, u16 flags, u32 target, u16 offset)
{
    SET_MEMORY(0x002000, 0x6000 + (condition << 8) + offset);
    SET_MEMORY(0x002002, 0x0000); /* branch always reads next word */
    emu68k_set_flags(context, flags);
    context->pc = 0x002000;
    context->cycles_left = 10;

    do_run(NULL);

    check_pc(target);
    check_cycles(0);
}

void test_branch_taken_long(int condition, u16 flags, u32 target, u16 offset)
{
    SET_MEMORY(0x002000, 0x6000 + (condition << 8));
    SET_MEMORY(0x002002, offset);
    emu68k_set_flags(context, flags);
    context->pc = 0x002000;
    context->cycles_left = 10;

    do_run(NULL);

    check_pc(target);
    check_cycles(0);
}

void test_branch_not_taken_short(int condition, u16 flags, u32 target, u16 offset)
{
    SET_MEMORY(0x002000, 0x6042 + (condition << 8));
    SET_MEMORY(0x002002, 0x4242); /* branch always reads next word */
    emu68k_set_flags(context, flags);
    context->pc = 0x002000;
    context->cycles_left = 8;

    do_run(NULL);

    check_pc(0x002002);
    check_cycles(0);
}

void test_branch_not_taken_long(int condition, u16 flags, u32 target, u16 offset)
{
    SET_MEMORY(0x002000, 0x6000 + (condition << 8));
    SET_MEMORY(0x002002, 0x4242);
    emu68k_set_flags(context, flags);
    context->pc = 0x002000;
    context->cycles_left = 12;

    do_run(NULL);

    check_pc(0x002004);
    check_cycles(0);
}

typedef void (*branch_testfunc)(int, u16, u32, u16);

struct branch_test {
    branch_testfunc func;
    u32 target;
    u16 offset;
};

struct branch_test branch_taken_tests[] = {
    {test_branch_taken_short, 0x002004, 0x02},
    {test_branch_taken_short, 0x002000, 0xfe},
    {test_branch_taken_short, 0x002080, 0x7e},
    {test_branch_taken_short, 0x001f82, 0x80},
    
    {test_branch_taken_long, 0x002002, 0x0000},
    {test_branch_taken_long, 0x002000, 0xfffe},
    {test_branch_taken_long, 0x00a000, 0x7ffe},
    {test_branch_taken_long, 0xffa002, 0x8000},
    
    {NULL, 0, 0}, /* must be last entry */
};

struct branch_test branch_not_taken_tests[] = {
    {test_branch_not_taken_short, 0, 0},
    {test_branch_not_taken_long, 0, 0},
    {NULL, 0, 0}, /* must be last entry */
};

struct branch_data {
    char *name;
    int condition;
    u16 flags;
    int should_take;
};

struct branch_data branch_test_data[] = {
    {"BRA taken", 0x0, 0x0000, 1},
    {"BRA taken", 0x0, 0x001f, 1},

    {"BHI taken", 0x2, 0x0000, 1},
    {"BHI taken", 0x2, 0x001a, 1},
    {"BHI not taken", 0x2, 0x0001, 0},
    {"BHI not taken", 0x2, 0x0004, 0},
    {"BHI not taken", 0x2, 0x0005, 0},
    {"BHI not taken", 0x2, 0x001b, 0},
    {"BHI not taken", 0x2, 0x001d, 0},
    {"BHI not taken", 0x2, 0x001f, 0},

    {"BLS taken", 0x3, 0x0001, 1},
    {"BLS taken", 0x3, 0x0004, 1},
    {"BLS taken", 0x3, 0x0005, 1},
    {"BLS taken", 0x3, 0x001b, 1},
    {"BLS taken", 0x3, 0x001d, 1},
    {"BLS taken", 0x3, 0x001f, 1},
    {"BLS not taken", 0x3, 0x0000, 0},
    {"BLS not taken", 0x3, 0x001a, 0},

    {"BCC taken", 0x4, 0x0000, 1},
    {"BCC taken", 0x4, 0x001e, 1},
    {"BCC not taken", 0x4, 0x0001, 0},
    {"BCC not taken", 0x4, 0x001f, 0},

    {"BCS taken", 0x5, 0x0001, 1},
    {"BCS taken", 0x5, 0x001f, 1},
    {"BCS not taken", 0x5, 0x0000, 0},
    {"BCS not taken", 0x5, 0x001e, 0},

    {"BNE taken", 0x6, 0x0000, 1},
    {"BNE taken", 0x6, 0x001b, 1},
    {"BNE not taken", 0x6, 0x0004, 0},
    {"BNE not taken", 0x6, 0x001f, 0},

    {"BEQ taken", 0x7, 0x0004, 1},
    {"BEQ taken", 0x7, 0x001f, 1},
    {"BEQ not taken", 0x7, 0x0000, 0},
    {"BEQ not taken", 0x7, 0x001b, 0},

    {"BVC taken", 0x8, 0x0000, 1},
    {"BVC taken", 0x8, 0x001d, 1},
    {"BVC not taken", 0x8, 0x0002, 0},
    {"BVC not taken", 0x8, 0x001f, 0},

    {"BVS taken", 0x9, 0x0002, 1},
    {"BVS taken", 0x9, 0x001f, 1},
    {"BVS not taken", 0x9, 0x0000, 0},
    {"BVS not taken", 0x9, 0x001d, 0},

    {"BPL taken", 0xa, 0x0000, 1},
    {"BPL taken", 0xa, 0x0017, 1},
    {"BPL not taken", 0xa, 0x0008, 0},
    {"BPL not taken", 0xa, 0x001f, 0},

    {"BMI taken", 0xb, 0x0008, 1},
    {"BMI taken", 0xb, 0x001f, 1},
    {"BMI not taken", 0xb, 0x0000, 0},
    {"BMI not taken", 0xb, 0x0017, 0},

    {"BGE taken", 0xc, 0x0000, 1},
    {"BGE taken", 0xc, 0x000a, 1},
    {"BGE taken", 0xc, 0x0015, 1},
    {"BGE taken", 0xc, 0x001f, 1},
    {"BGE not taken", 0xc, 0x0002, 0},
    {"BGE not taken", 0xc, 0x0008, 0},
    {"BGE not taken", 0xc, 0x001d, 0},
    {"BGE not taken", 0xc, 0x0017, 0},

    {"BLT taken", 0xd, 0x0002, 1},
    {"BLT taken", 0xd, 0x0008, 1},
    {"BLT taken", 0xd, 0x001d, 1},
    {"BLT taken", 0xd, 0x0017, 1},
    {"BLT not taken", 0xd, 0x0000, 0},
    {"BLT not taken", 0xd, 0x000a, 0},
    {"BLT not taken", 0xd, 0x0015, 0},
    {"BLT not taken", 0xd, 0x001f, 0},

    {"BGT taken", 0xe, 0x0000, 1},
    {"BGT taken", 0xe, 0x000a, 1},
    {"BGT taken", 0xe, 0x0011, 1},
    {"BGT taken", 0xe, 0x001b, 1},
    {"BGT not taken", 0xe, 0x0002, 0},
    {"BGT not taken", 0xe, 0x0008, 0},
    {"BGT not taken", 0xe, 0x001d, 0},
    {"BGT not taken", 0xe, 0x0017, 0},
    {"BGT not taken", 0xe, 0x0004, 0},
    {"BGT not taken", 0xe, 0x000e, 0},
    {"BGT not taken", 0xe, 0x0015, 0},
    {"BGT not taken", 0xe, 0x001f, 0},

    {"BLE taken", 0xf, 0x0002, 1},
    {"BLE taken", 0xf, 0x0008, 1},
    {"BLE taken", 0xf, 0x001d, 1},
    {"BLE taken", 0xf, 0x0017, 1},
    {"BLE taken", 0xf, 0x0009, 1},
    {"BLE taken", 0xf, 0x0003, 1},
    {"BLE taken", 0xf, 0x0015, 1},
    {"BLE taken", 0xf, 0x001f, 1},
    {"BLE not taken", 0xf, 0x0000, 0},
    {"BLE not taken", 0xf, 0x000a, 0},
    {"BLE not taken", 0xf, 0x0011, 0},
    {"BLE not taken", 0xf, 0x001b, 0},

    {NULL, 0, 0, 0}, /* must be last entry */
};

void test_branch_instructions(void)
{
    struct branch_data *cur_data;
    struct branch_test *cur_test;

    for (cur_data = branch_test_data; cur_data->name; cur_data++) {
	if (cur_data->should_take) {
	    cur_test = branch_taken_tests;
	} else {
	    cur_test = branch_not_taken_tests;
	}
	
	for (; cur_test->func; cur_test++) {
	    do_start(cur_data->name);
	    cur_test->func(cur_data->condition, cur_data->flags,
			   cur_test->target, cur_test->offset);
	    do_done(NULL);
	}
    }
}

/*
 * test "directed" ALU instructions (OR, AND, ADD, SUB) (0x[89cd]xxx)
 */

struct directed_alu_data {
    char *name;
    u16 opcode_base;
    u32 src_1;
    u32 src_2;
    u32 dest;
    u16 flags_in;
    u16 flags_out;
    int mem_tests;
    int reg_tests;
};

struct directed_alu_data directed_alu_test_data[] = {
    {"OR.B", 0x8000, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"OR.B", 0x8000, 0x00005555, 0x0000aaaa, 0x0000aaff, 0x001f, 0x0018, 1, 1},
    {"OR.B", 0x8000, 0xffaa0505, 0xaaff5050, 0xaaff5055, 0x0000, 0x0000, 1, 1},

    {"OR.W", 0x8040, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"OR.W", 0x8040, 0x55555555, 0xaaaaaaaa, 0xaaaaffff, 0x001f, 0x0018, 1, 1},
    {"OR.W", 0x8040, 0xffaa0505, 0xaaff5050, 0xaaff5555, 0x0000, 0x0000, 1, 1},

    {"OR.L", 0x8080, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"OR.L", 0x8080, 0x55555555, 0xaaaaaaaa, 0xffffffff, 0x001f, 0x0018, 1, 1},
    {"OR.L", 0x8080, 0x05050505, 0x50505050, 0x55555555, 0x0000, 0x0000, 1, 1},

    {"AND.B", 0xc000, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"AND.B", 0xc000, 0x0000aaaf, 0x000055fa, 0x000055aa, 0x001f, 0x0018, 1, 1},
    {"AND.B", 0xc000, 0xffaa0555, 0xaaff505a, 0xaaff5050, 0x0000, 0x0000, 1, 1},

    {"AND.W", 0xc040, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"AND.W", 0xc040, 0xaaaaffff, 0x5555af5a, 0x5555af5a, 0x001f, 0x0018, 1, 1},
    {"AND.W", 0xc040, 0xffaaf505, 0xaaff5055, 0xaaff5005, 0x0000, 0x0000, 1, 1},

    {"AND.L", 0xc080, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014, 1, 1},
    {"AND.L", 0xc080, 0xa5a5a5a5, 0xaaaaaaaa, 0xa0a0a0a0, 0x001f, 0x0018, 1, 1},
    {"AND.L", 0xc080, 0xf5f5f5f5, 0x5f5f5f5f, 0x55555555, 0x0000, 0x0000, 1, 1},

    {"SUB.B", 0x9000, 0xaaaaaa55, 0xa5a5a555, 0xa5a5a500, 0x001f, 0x0004, 1, 1},
    {"SUB.B", 0x9000, 0xaaaaaaaa, 0xa5a5a555, 0xa5a5a5ab, 0x0000, 0x0019, 1, 1},
    {"SUB.B", 0x9000, 0xaaaaaa55, 0xa5a5a5aa, 0xa5a5a555, 0x0000, 0x0000, 1, 1},
    {"SUB.B", 0x9000, 0xaaaaaaff, 0xa5a5a500, 0xa5a5a501, 0x0000, 0x0013, 1, 1},
    
    {"SUB.W", 0x9040, 0xaaaa5500, 0xa5a55500, 0xa5a50000, 0x001f, 0x0004, 1, 1},
    {"SUB.W", 0x9040, 0xaaaaaa00, 0xa5a55500, 0xa5a5ab00, 0x0000, 0x0019, 1, 1},
    {"SUB.W", 0x9040, 0xaaaa5500, 0xa5a5aa00, 0xa5a55500, 0x0000, 0x0000, 1, 1},
    {"SUB.W", 0x9040, 0xaaaaff00, 0xa5a50000, 0xa5a50100, 0x0000, 0x0013, 1, 1},
    
    {"SUB.L", 0x9080, 0x55000000, 0x55000000, 0x00000000, 0x001f, 0x0004, 1, 1},
    {"SUB.L", 0x9080, 0xaa000000, 0x55000000, 0xab000000, 0x0000, 0x0019, 1, 1},
    {"SUB.L", 0x9080, 0x55000000, 0xaa000000, 0x55000000, 0x0000, 0x0000, 1, 1},
    {"SUB.L", 0x9080, 0xff000000, 0x00000000, 0x01000000, 0x0000, 0x0013, 1, 1},

    {"ADD.B", 0xd000, 0x5a5a5a00, 0xa5a5a500, 0xa5a5a500, 0x001f, 0x0004, 1, 1},
    {"ADD.B", 0xd000, 0x5a5a5a01, 0xa5a5a502, 0xa5a5a503, 0x001f, 0x0000, 1, 1},
    {"ADD.B", 0xd000, 0x5a5a5a40, 0xa5a5a540, 0xa5a5a580, 0x001f, 0x000a, 1, 1},
    {"ADD.B", 0xd000, 0x5a5a5ac0, 0xa5a5a540, 0xa5a5a500, 0x001f, 0x0015, 1, 1},
    
    {"ADD.W", 0xd040, 0x5a5a0000, 0xa5a50000, 0xa5a50000, 0x001f, 0x0004, 1, 1},
    {"ADD.W", 0xd040, 0x5a5a0100, 0xa5a50200, 0xa5a50300, 0x001f, 0x0000, 1, 1},
    {"ADD.W", 0xd040, 0x5a5a4000, 0xa5a54000, 0xa5a58000, 0x001f, 0x000a, 1, 1},
    {"ADD.W", 0xd040, 0x5a5ac000, 0xa5a54000, 0xa5a50000, 0x001f, 0x0015, 1, 1},
    
    {"ADD.L", 0xd080, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0004, 1, 1},
    {"ADD.L", 0xd080, 0x01000000, 0x02000000, 0x03000000, 0x001f, 0x0000, 1, 1},
    {"ADD.L", 0xd080, 0x40000000, 0x40000000, 0x80000000, 0x001f, 0x000a, 1, 1},
    {"ADD.L", 0xd080, 0xc0000000, 0x40000000, 0x00000000, 0x001f, 0x0015, 1, 1},
    
    {"CMP.B", 0xb000, 0xaaaaaa55, 0xa5a5a555, 0xa5a5a555, 0x001f, 0x0014, 0, 1},
    {"CMP.B", 0xb000, 0xaaaaaaaa, 0xa5a5a555, 0xa5a5a555, 0x0000, 0x0009, 0, 1},
    {"CMP.B", 0xb000, 0xaaaaaa55, 0xa5a5a5aa, 0xa5a5a5aa, 0x0000, 0x0000, 0, 1},
    {"CMP.B", 0xb000, 0xaaaaaaff, 0xa5a5a500, 0xa5a5a500, 0x0000, 0x0003, 0, 1},
    
    {"CMP.W", 0xb040, 0xaaaa5500, 0xa5a55500, 0xa5a55500, 0x001f, 0x0014, 0, 1},
    {"CMP.W", 0xb040, 0xaaaaaa00, 0xa5a55500, 0xa5a55500, 0x0000, 0x0009, 0, 1},
    {"CMP.W", 0xb040, 0xaaaa5500, 0xa5a5aa00, 0xa5a5aa00, 0x0000, 0x0000, 0, 1},
    {"CMP.W", 0xb040, 0xaaaaff00, 0xa5a50000, 0xa5a50000, 0x0000, 0x0003, 0, 1},
    
    {"CMP.L", 0xb080, 0x55000000, 0x55000000, 0x55000000, 0x001f, 0x0014, 0, 1},
    {"CMP.L", 0xb080, 0xaa000000, 0x55000000, 0x55000000, 0x0000, 0x0009, 0, 1},
    {"CMP.L", 0xb080, 0x55000000, 0xaa000000, 0xaa000000, 0x0000, 0x0000, 0, 1},
    {"CMP.L", 0xb080, 0xff000000, 0x00000000, 0x00000000, 0x0000, 0x0003, 0, 1},

    {"EOR.B", 0xb000, 0xa5a5a5ff, 0x5a5a5aff, 0x5a5a5a00, 0x001f, 0x0014, 1, 0},
    {"EOR.B", 0xb000, 0xa5a5a555, 0x5a5a5aaa, 0x5a5a5aff, 0x001f, 0x0018, 1, 0},
    {"EOR.B", 0xb000, 0xa5a5a55f, 0x5a5a5a05, 0x5a5a5a5a, 0x000f, 0x0000, 1, 0},
    
    {"EOR.W", 0xb040, 0xa5a5fff5, 0x5a5afff5, 0x5a5a0000, 0x001f, 0x0014, 1, 0},
    {"EOR.W", 0xb040, 0xa5a555f5, 0x5a5aaaaf, 0x5a5aff5a, 0x001f, 0x0018, 1, 0},
    {"EOR.W", 0xb040, 0xa5a55ff5, 0x5a5a05af, 0x5a5a5a5a, 0x000f, 0x0000, 1, 0},
    
    {"EOR.L", 0xb080, 0xfff5ffff, 0xfff5ffff, 0x00000000, 0x001f, 0x0014, 1, 0},
    {"EOR.L", 0xb080, 0x55f59999, 0xaaaf3333, 0xff5aaaaa, 0x001f, 0x0018, 1, 0},
    {"EOR.L", 0xb080, 0x5ff56666, 0x05afcccc, 0x5a5aaaaa, 0x000f, 0x0000, 1, 0},
    
    {NULL, 0, 0, 0, 0, 0, 0, 0}, /* must be last entry */
};

void dump_directed_alu_data(struct directed_alu_data *data, char *type)
{
    printf("%s: %04hx, 0x%08lx, 0x%08lx, 0x%08lx, 0x%02hx, 0x%02hx\n",
	   type, data->opcode_base, data->src_1, data->src_2, data->dest,
	   data->flags_in, data->flags_out);
}

void test_directed_alu_mem(struct directed_alu_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base | 0x0110);
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	SET_MEMORY(0x008000, data->src_2 >> 16);
	SET_MEMORY(0x008002, data->src_2 & 0xffff);
	context->cycles_left = 20;
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	SET_MEMORY(0x008000, data->src_2 & 0xffff);
	context->cycles_left = 12;
    } else {
	SET_MEMORY(0x008000, htom16(data->src_2 & 0xffff));
	context->cycles_left = 12;
    }
    context->regs_a[0] = 0x008000;
    context->regs_d[0] = data->src_1;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    
    do_run(NULL);

    check_pc(0x002002);
    check_cycles(0);
    check_flags(data->flags_out);
    check_reg(context->regs_d[0], data->src_1);
    
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	check_memory(0x008000, data->dest >> 16);
	check_memory(0x008002, data->dest & 0xffff);
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	check_memory(0x008000, data->dest & 0xffff);
    } else {
	check_memory(0x008000, htom16(data->dest & 0xffff));
    }

    if (did_test_fail) {
	dump_directed_alu_data(data, "mem");
    }
    
    do_done(NULL);
}

void test_directed_alu_reg_1(struct directed_alu_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base | 0x0010);
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	SET_MEMORY(0x008000, data->src_1 >> 16);
	SET_MEMORY(0x008002, data->src_1 & 0xffff);
	context->cycles_left = 14;
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	SET_MEMORY(0x008000, data->src_1 & 0xffff);
	context->cycles_left = 8;
    } else {
	SET_MEMORY(0x008000, htom16(data->src_1 & 0xffff));
	context->cycles_left = 8;
    }
    context->regs_a[0] = 0x008000;
    context->regs_d[0] = data->src_2;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    
    do_run(NULL);

    check_pc(0x002002);
    check_cycles(0);
    check_flags(data->flags_out);
    check_reg(context->regs_d[0], data->dest);
    
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	check_memory(0x008000, data->src_1 >> 16);
	check_memory(0x008002, data->src_1 & 0xffff);
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	check_memory(0x008000, data->src_1 & 0xffff);
    } else {
	check_memory(0x008000, htom16(data->src_1 & 0xffff));
    }
    
    if (did_test_fail) {
	dump_directed_alu_data(data, "reg_1");
    }
    
    do_done(NULL);
}

void test_directed_alu_reg_2(struct directed_alu_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base | 0x0001);
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	context->cycles_left = 8;
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	context->cycles_left = 4;
    } else {
	context->cycles_left = 4;
    }
    context->regs_a[0] = 0x008000;
    context->regs_d[0] = data->src_2;
    context->regs_d[1] = data->src_1;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    
    do_run(NULL);

    check_pc(0x002002);
    check_cycles(0);
    check_flags(data->flags_out);
    check_reg(context->regs_d[0], data->dest);
    
    check_reg(context->regs_d[1], data->src_1);
    
    if (did_test_fail) {
	dump_directed_alu_data(data, "reg_2");
    }
    
    do_done(NULL);
}

void test_directed_alu_instructions(void)
{
    struct directed_alu_data *cur_data;

    for (cur_data = directed_alu_test_data; cur_data->name; cur_data++) {
	if (cur_data->mem_tests) {
	    test_directed_alu_mem(cur_data);
	}
	if (cur_data->reg_tests) {
	    test_directed_alu_reg_1(cur_data);
	    test_directed_alu_reg_2(cur_data);
	}
    }
}

/*
 * test "immediate" instructions (ADDI, SUBI, ORI, ANDI, EORI, CMPI)
 */

struct immediate_data {
    char *name;
    u16 opcode_base;
    u32 src_1;
    u32 src_2;
    u32 dest;
    u16 flags_in;
    u16 flags_out;
};

struct immediate_data immediate_test_data[] = {
    {"ORI.B", 0x0000, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ORI.B", 0x0000, 0x00005555, 0x0000aaaa, 0x0000aaff, 0x001f, 0x0018},
    {"ORI.B", 0x0000, 0xffaa0505, 0xaaff5050, 0xaaff5055, 0x0000, 0x0000},

    {"ORI.W", 0x0040, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ORI.W", 0x0040, 0x55555555, 0xaaaaaaaa, 0xaaaaffff, 0x001f, 0x0018},
    {"ORI.W", 0x0040, 0xffaa0505, 0xaaff5050, 0xaaff5555, 0x0000, 0x0000},
    
    {"ORI.L", 0x0080, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ORI.L", 0x0080, 0x55555555, 0xaaaaaaaa, 0xffffffff, 0x001f, 0x0018},
    {"ORI.L", 0x0080, 0x05050505, 0x50505050, 0x55555555, 0x0000, 0x0000},

    {"ANDI.B", 0x0200, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ANDI.B", 0x0200, 0x0000afaf, 0x000055fa, 0x000055aa, 0x001f, 0x0018},
    {"ANDI.B", 0x0200, 0xffaa0555, 0xaaff505a, 0xaaff5050, 0x0000, 0x0000},

    {"ANDI.W", 0x0240, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ANDI.W", 0x0240, 0xaaaaffff, 0x5555af5a, 0x5555af5a, 0x001f, 0x0018},
    {"ANDI.W", 0x0240, 0xffaaf505, 0xaaff5055, 0xaaff5005, 0x0000, 0x0000},

    {"ANDI.L", 0x0280, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0014},
    {"ANDI.L", 0x0280, 0xa5a5a5a5, 0xaaaaaaaa, 0xa0a0a0a0, 0x001f, 0x0018},
    {"ANDI.L", 0x0280, 0xf5f5f5f5, 0x5f5f5f5f, 0x55555555, 0x0000, 0x0000},

    {"SUBI.B", 0x0400, 0xaaaaaa55, 0xa5a5a555, 0xa5a5a500, 0x001f, 0x0004},
    {"SUBI.B", 0x0400, 0xaaaaaaaa, 0xa5a5a555, 0xa5a5a5ab, 0x0000, 0x0019},
    {"SUBI.B", 0x0400, 0xaaaaaa55, 0xa5a5a5aa, 0xa5a5a555, 0x0000, 0x0000},
    {"SUBI.B", 0x0400, 0xaaaaaaff, 0xa5a5a500, 0xa5a5a501, 0x0000, 0x0013},

    {"SUBI.W", 0x0440, 0xaaaa5500, 0xa5a55500, 0xa5a50000, 0x001f, 0x0004},
    {"SUBI.W", 0x0440, 0xaaaaaa00, 0xa5a55500, 0xa5a5ab00, 0x0000, 0x0019},
    {"SUBI.W", 0x0440, 0xaaaa5500, 0xa5a5aa00, 0xa5a55500, 0x0000, 0x0000},
    {"SUBI.W", 0x0440, 0xaaaaff00, 0xa5a50000, 0xa5a50100, 0x0000, 0x0013},

    {"SUBI.L", 0x0480, 0x55000000, 0x55000000, 0x00000000, 0x001f, 0x0004},
    {"SUBI.L", 0x0480, 0xaa000000, 0x55000000, 0xab000000, 0x0000, 0x0019},
    {"SUBI.L", 0x0480, 0x55000000, 0xaa000000, 0x55000000, 0x0000, 0x0000},
    {"SUBI.L", 0x0480, 0xff000000, 0x00000000, 0x01000000, 0x0000, 0x0013},

    {"ADDI.B", 0x0600, 0x5a5a5a00, 0xa5a5a500, 0xa5a5a500, 0x001f, 0x0004},
    {"ADDI.B", 0x0600, 0x5a5a5a01, 0xa5a5a502, 0xa5a5a503, 0x001f, 0x0000},
    {"ADDI.B", 0x0600, 0x5a5a5a40, 0xa5a5a540, 0xa5a5a580, 0x001f, 0x000a},
    {"ADDI.B", 0x0600, 0x5a5a5ac0, 0xa5a5a540, 0xa5a5a500, 0x001f, 0x0015},
    
    {"ADDI.W", 0x0640, 0x5a5a0000, 0xa5a50000, 0xa5a50000, 0x001f, 0x0004},
    {"ADDI.W", 0x0640, 0x5a5a0100, 0xa5a50200, 0xa5a50300, 0x001f, 0x0000},
    {"ADDI.W", 0x0640, 0x5a5a4000, 0xa5a54000, 0xa5a58000, 0x001f, 0x000a},
    {"ADDI.W", 0x0640, 0x5a5ac000, 0xa5a54000, 0xa5a50000, 0x001f, 0x0015},
    
    {"ADDI.L", 0x0680, 0x00000000, 0x00000000, 0x00000000, 0x001f, 0x0004},
    {"ADDI.L", 0x0680, 0x01000000, 0x02000000, 0x03000000, 0x001f, 0x0000},
    {"ADDI.L", 0x0680, 0x40000000, 0x40000000, 0x80000000, 0x001f, 0x000a},
    {"ADDI.L", 0x0680, 0xc0000000, 0x40000000, 0x00000000, 0x001f, 0x0015},

    {"EORI.B", 0x0a00, 0xa5a5a5ff, 0x5a5a5aff, 0x5a5a5a00, 0x001f, 0x0014},
    {"EORI.B", 0x0a00, 0xa5a5a555, 0x5a5a5aaa, 0x5a5a5aff, 0x001f, 0x0018},
    {"EORI.B", 0x0a00, 0xa5a5a55f, 0x5a5a5a05, 0x5a5a5a5a, 0x000f, 0x0000},
    
    {"EORI.W", 0x0a40, 0xa5a5fff5, 0x5a5afff5, 0x5a5a0000, 0x001f, 0x0014},
    {"EORI.W", 0x0a40, 0xa5a555f5, 0x5a5aaaaf, 0x5a5aff5a, 0x001f, 0x0018},
    {"EORI.W", 0x0a40, 0xa5a55ff5, 0x5a5a05af, 0x5a5a5a5a, 0x000f, 0x0000},
    
    {"EORI.L", 0x0a80, 0xfff5ffff, 0xfff5ffff, 0x00000000, 0x001f, 0x0014},
    {"EORI.L", 0x0a80, 0x55f59999, 0xaaaf3333, 0xff5aaaaa, 0x001f, 0x0018},
    {"EORI.L", 0x0a80, 0x5ff56666, 0x05afcccc, 0x5a5aaaaa, 0x000f, 0x0000},
    
    {"CMPI.B", 0x0c00, 0xaaaaaa55, 0xa5a5a555, 0xa5a5a555, 0x001f, 0x0014},
    {"CMPI.B", 0x0c00, 0xaaaaaaaa, 0xa5a5a555, 0xa5a5a555, 0x0000, 0x0009},
    {"CMPI.B", 0x0c00, 0xaaaaaa55, 0xa5a5a5aa, 0xa5a5a5aa, 0x0000, 0x0000},
    {"CMPI.B", 0x0c00, 0xaaaaaaff, 0xa5a5a500, 0xa5a5a500, 0x0000, 0x0003},
    
    {"CMPI.W", 0x0c40, 0xaaaa5500, 0xa5a55500, 0xa5a55500, 0x001f, 0x0014},
    {"CMPI.W", 0x0c40, 0xaaaaaa00, 0xa5a55500, 0xa5a55500, 0x0000, 0x0009},
    {"CMPI.W", 0x0c40, 0xaaaa5500, 0xa5a5aa00, 0xa5a5aa00, 0x0000, 0x0000},
    {"CMPI.W", 0x0c40, 0xaaaaff00, 0xa5a50000, 0xa5a50000, 0x0000, 0x0003},
    
    {"CMPI.L", 0x0c80, 0x55000000, 0x55000000, 0x55000000, 0x001f, 0x0014},
    {"CMPI.L", 0x0c80, 0xaa000000, 0x55000000, 0x55000000, 0x0000, 0x0009},
    {"CMPI.L", 0x0c80, 0x55000000, 0xaa000000, 0xaa000000, 0x0000, 0x0000},
    {"CMPI.L", 0x0c80, 0xff000000, 0x00000000, 0x00000000, 0x0000, 0x0003},

    {NULL, 0, 0, 0, 0, 0, 0}, /* must be last entry */
};

void dump_immediate_data(struct immediate_data *data, char *type)
{
    printf("%s: %04hx, 0x%08lx, 0x%08lx, 0x%08lx, 0x%02hx, 0x%02hx\n",
	   type, data->opcode_base, data->src_1, data->src_2, data->dest,
	   data->flags_in, data->flags_out);
}

void test_immediate_mem(struct immediate_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base | 0x0010);
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	SET_MEMORY(0x002002, data->src_1 >> 16);
	SET_MEMORY(0x002004, data->src_1 & 0xffff);
	
	SET_MEMORY(0x008000, data->src_2 >> 16);
	SET_MEMORY(0x008002, data->src_2 & 0xffff);

	context->cycles_left = 20;
	if ((data->opcode_base & 0x0e00) != 0x0c00) {
	    context->cycles_left += 4;
	}
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	SET_MEMORY(0x002002, data->src_1 & 0xffff);
	SET_MEMORY(0x008000, data->src_2 & 0xffff);
	context->cycles_left = 12;
    } else {
	SET_MEMORY(0x002002, data->src_1 & 0xffff);
	SET_MEMORY(0x008000, htom16(data->src_2 & 0xffff));
	context->cycles_left = 12;
    }

    if ((data->opcode_base & 0x0e00) != 0x0c00) {
	context->cycles_left += 4;
    }
    context->regs_a[0] = 0x008000;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    
    do_run(NULL);

    check_cycles(0);
    check_flags(data->flags_out);
    
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	check_memory(0x008000, data->dest >> 16);
	check_memory(0x008002, data->dest & 0xffff);
	check_pc(0x002006);
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	check_memory(0x008000, data->dest & 0xffff);
	check_pc(0x002004);
    } else {
	check_memory(0x008000, htom16(data->dest & 0xffff));
	check_pc(0x002004);
    }

    if (did_test_fail) {
	dump_immediate_data(data, "mem");
    }
    
    do_done(NULL);
}

void test_immediate_reg(struct immediate_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base | 0x0000);
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	SET_MEMORY(0x002002, data->src_1 >> 16);
	SET_MEMORY(0x002004, data->src_1 & 0xffff);
	
	context->cycles_left = 16;

	if (((data->opcode_base & 0x0e00) == 0x0c00)
	    || ((data->opcode_base & 0x0e00) == 0x0200)) {
	    context->cycles_left -= 2;
	}
    } else if ((data->opcode_base & 0x00c0) == 0x0040) {
	SET_MEMORY(0x002002, data->src_1 & 0xffff);
	context->cycles_left = 8;
    } else {
	SET_MEMORY(0x002002, data->src_1 & 0xffff);
	context->cycles_left = 8;
    }

    context->regs_d[0] = data->src_2;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    
    do_run(NULL);

    check_cycles(0);
    check_flags(data->flags_out);
    check_reg(context->regs_d[0], data->dest);
    
    if ((data->opcode_base & 0x00c0) == 0x0080) {
	check_pc(0x002006);
    } else {
	check_pc(0x002004);
    }

    if (did_test_fail) {
	dump_immediate_data(data, "reg");
    }
    
    do_done(NULL);
}

void test_immediate_instructions(void)
{
    struct immediate_data *cur_data;

    for (cur_data = immediate_test_data; cur_data->name; cur_data++) {
	test_immediate_mem(cur_data);
  	test_immediate_reg(cur_data);
    }
}

/*
 * test shift/rotate instructions (0xe000)
 */

struct shift_immed_reg_data {
    char *name;
    u16 opcode_base;
    u32 src;
    u32 dest;
    u16 flags_in;
    u16 flags_out;
    int cycles;
};

struct shift_immed_reg_data shift_immed_test_data[] = {
    {"ASR.B", 0xe200, 0x55aa5500, 0x55aa5500, 0x001f, 0x0004, 8},
    {"ASR.B", 0xe200, 0x55aa55a5, 0x55aa55d2, 0x0000, 0x0019, 8},
    {"ASR.B", 0xe400, 0x55aa55a5, 0x55aa55e9, 0x001f, 0x0008, 10},
    {"ASR.B", 0xe000, 0x55aa55ff, 0x55aa55ff, 0x0000, 0x0019, 22},
    {"ASR.B", 0xe000, 0x55aa557f, 0x55aa5500, 0x0000, 0x0004, 22},

    {"ASR.W", 0xe240, 0x55aa0000, 0x55aa0000, 0x001f, 0x0004, 8},
    {"ASR.W", 0xe240, 0x55aaa5a5, 0x55aad2d2, 0x0000, 0x0019, 8},
    {"ASR.W", 0xe440, 0x55aaa5a5, 0x55aae969, 0x001f, 0x0008, 10},
    {"ASR.W", 0xe040, 0x55aaffff, 0x55aaffff, 0x0000, 0x0019, 22},
    {"ASR.W", 0xe040, 0x55aa7fff, 0x55aa007f, 0x0000, 0x0011, 22},

    {"ASR.L", 0xe280, 0x00000000, 0x00000000, 0x001f, 0x0004, 10},
    {"ASR.L", 0xe280, 0x55aaaa55, 0x2ad5552a, 0x0000, 0x0011, 10},
    {"ASR.L", 0xe480, 0xa5aaaa54, 0xe96aaa95, 0x001f, 0x0008, 12},
    
    {"LSR.B", 0xe208, 0x55aa5500, 0x55aa5500, 0x001f, 0x0004, 8},
    {"LSR.B", 0xe208, 0x55aa55a5, 0x55aa5552, 0x0000, 0x0011, 8},
    {"LSR.B", 0xe408, 0x55aa55a5, 0x55aa5529, 0x001f, 0x0000, 10},
    {"LSR.B", 0xe008, 0x55aa55ff, 0x55aa5500, 0x0000, 0x0015, 22},
    {"LSR.B", 0xe008, 0x55aa557f, 0x55aa5500, 0x0000, 0x0004, 22},

    {"LSR.W", 0xe248, 0x55aa0000, 0x55aa0000, 0x001f, 0x0004, 8},
    {"LSR.W", 0xe248, 0x55aaa5a5, 0x55aa52d2, 0x0000, 0x0011, 8},
    {"LSR.W", 0xe448, 0x55aaa5a5, 0x55aa2969, 0x001f, 0x0000, 10},
    {"LSR.W", 0xe048, 0x55aaffff, 0x55aa00ff, 0x0000, 0x0011, 22},
    {"LSR.W", 0xe048, 0x55aa7fff, 0x55aa007f, 0x0000, 0x0011, 22},

    {"LSR.L", 0xe288, 0x00000000, 0x00000000, 0x001f, 0x0004, 10},
    {"LSR.L", 0xe288, 0x55aaaa55, 0x2ad5552a, 0x0000, 0x0011, 10},
    {"LSR.L", 0xe488, 0xa5aaaa54, 0x296aaa95, 0x001f, 0x0000, 12},

    /* LSL instructions are also decoded as ASL instructions */
    {"LSL.B", 0xe308, 0x55aa5500, 0x55aa5500, 0x001f, 0x0004, 8},
    {"LSL.B", 0xe308, 0x55aa55a5, 0x55aa554a, 0x0000, 0x0011, 8},
    {"LSL.B", 0xe508, 0x55aa55a5, 0x55aa5594, 0x001f, 0x0008, 10},
    {"LSL.B", 0xe108, 0x55aa55ff, 0x55aa5500, 0x0000, 0x0015, 22},
    {"LSL.B", 0xe108, 0x55aa55fe, 0x55aa5500, 0x0000, 0x0004, 22},

    {"LSL.W", 0xe348, 0x55aa0000, 0x55aa0000, 0x001f, 0x0004, 8},
    {"LSL.W", 0xe348, 0x55aaa5a5, 0x55aa4b4a, 0x0000, 0x0011, 8},
    {"LSL.W", 0xe548, 0x55aaa5a5, 0x55aa9694, 0x001f, 0x0008, 10},
    {"LSL.W", 0xe148, 0x55aaffff, 0x55aaff00, 0x0000, 0x0019, 22},
    {"LSL.W", 0xe148, 0x55aa7eff, 0x55aaff00, 0x0000, 0x0008, 22},

    {"LSL.L", 0xe388, 0x00000000, 0x00000000, 0x001f, 0x0004, 10},
    {"LSL.L", 0xe388, 0x55aaaa55, 0xab5554aa, 0x0000, 0x0008, 10},
    {"LSL.L", 0xe588, 0x95aaaa54, 0x56aaa950, 0x001f, 0x0000, 12},

    {"ROXR.B", 0xe210, 0x55aa5500, 0x55aa5500, 0x000f, 0x0004, 8},
    {"ROXR.B", 0xe210, 0x55aa55aa, 0x55aa5555, 0x0000, 0x0000, 8},
    {"ROXR.B", 0xe410, 0x55aa55aa, 0x55aa552a, 0x0000, 0x0011, 10},
    {"ROXR.B", 0xe410, 0x55aa55aa, 0x55aa556a, 0x0010, 0x0011, 10},

    {"ROXR.W", 0xe250, 0x55aa0000, 0x55aa0000, 0x000f, 0x0004, 8},
    {"ROXR.W", 0xe250, 0x55aaaa55, 0x55aa552a, 0x0000, 0x0011, 8},
    {"ROXR.W", 0xe450, 0x55aaaa55, 0x55aaea95, 0x0010, 0x0008, 10},
    {"ROXR.W", 0xe450, 0x55aa5555, 0x55aad555, 0x0010, 0x0008, 10},
    
    {"ROXR.L", 0xe290, 0x00000000, 0x00000000, 0x000f, 0x0004, 10},
    {"ROXR.L", 0xe290, 0xd5aaaa55, 0x6ad5552a, 0x0000, 0x0011, 10},
    {"ROXR.L", 0xe490, 0x15aaaa55, 0xc56aaa95, 0x001f, 0x0008, 12},
    
    {"ROXL.B", 0xe310, 0x55aa5500, 0x55aa5500, 0x000f, 0x0004, 8},
    {"ROXL.B", 0xe310, 0x55aa55aa, 0x55aa5554, 0x0000, 0x0011, 8},
    {"ROXL.B", 0xe510, 0x55aa55aa, 0x55aa55ab, 0x0010, 0x0008, 10},

    {"ROXL.W", 0xe350, 0x55aa0000, 0x55aa0000, 0x000f, 0x0004, 8},
    {"ROXL.W", 0xe350, 0x55aaaa55, 0x55aa54aa, 0x0000, 0x0011, 8},
    {"ROXL.W", 0xe550, 0x55aaaa55, 0x55aaa957, 0x0010, 0x0008, 10},
    {"ROXL.W", 0xe550, 0x55aaaa00, 0x55aaa801, 0x0000, 0x0008, 10},

    {"ROXL.L", 0xe390, 0x00000000, 0x00000000, 0x000f, 0x0004, 10},
    {"ROXL.L", 0xe390, 0xd5aaaa55, 0xab5554aa, 0x0000, 0x0019, 10},
    {"ROXL.L", 0xe590, 0x15aaaa55, 0x56aaa956, 0x001f, 0x0000, 12},

    {"ROR.B", 0xe218, 0xaa55aa00, 0xaa55aa00, 0x001f, 0x0014, 8},
    {"ROR.B", 0xe218, 0xaa55aa05, 0xaa55aa82, 0x0000, 0x0009, 8},
    {"ROR.B", 0xe418, 0xaa55aa05, 0xaa55aa41, 0x001f, 0x0010, 10},
    {"ROR.B", 0xe018, 0xaa55aa05, 0xaa55aa05, 0x001f, 0x0010, 22},

    {"ROR.W", 0xe258, 0xaa550000, 0xaa550000, 0x001f, 0x0014, 8},
    {"ROR.W", 0xe258, 0xaa550055, 0xaa55802a, 0x0000, 0x0009, 8},
    {"ROR.W", 0xe458, 0xaa55005b, 0xaa55c016, 0x0010, 0x0019, 10},

    {"ROR.L", 0xe298, 0x00000000, 0x00000000, 0x001f, 0x0014, 10},
    {"ROR.L", 0xe298, 0x00550055, 0x802a802a, 0x0000, 0x0009, 10},
    {"ROR.L", 0xe498, 0x00550055, 0x40154015, 0x001f, 0x0010, 12},

    {"ROL.B", 0xe318, 0xaa55aa00, 0xaa55aa00, 0x001f, 0x0014, 8},
    {"ROL.B", 0xe318, 0xaa55aaa0, 0xaa55aa41, 0x0000, 0x0001, 8},
    {"ROL.B", 0xe318, 0xaa55aa50, 0xaa55aaa0, 0x0015, 0x0018, 8},
    {"ROL.B", 0xe518, 0xaa55aaa0, 0xaa55aa82, 0x0000, 0x0008, 10},
    {"ROL.B", 0xe118, 0xaa55aa05, 0xaa55aa05, 0x0000, 0x0001, 22},

    {"ROL.W", 0xe358, 0xaa550000, 0xaa550000, 0x001f, 0x0014, 8},
    {"ROL.W", 0xe358, 0xaa55aa00, 0xaa555401, 0x0000, 0x0001, 8},
    {"ROL.W", 0xe558, 0xaa55aa00, 0xaa55a802, 0x0000, 0x0008, 10},

    {"ROL.L", 0xe398, 0x00000000, 0x00000000, 0x001f, 0x0014, 10},
    {"ROL.L", 0xe398, 0xaa000000, 0x54000001, 0x0000, 0x0001, 10},
    {"ROL.L", 0xe598, 0xaa000000, 0xa8000002, 0x001f, 0x0018, 12},
    
    {NULL, 0, 0, 0, 0, 0, 0}, /* must be last entry */
};

void dump_shift_immed_reg_data(struct shift_immed_reg_data *data)
{
    printf("%s: 0x%04hx, 0x%08lx, 0x%08lx, 0x%04hx, 0x%04hx, %d.\n",
	   data->name, data->opcode_base, data->src, data->dest,
	   data->flags_in, data->flags_out, data->cycles);
}

void test_shift_immed_reg(struct shift_immed_reg_data *data)
{
    do_start(data->name);
    SET_MEMORY(0x002000, data->opcode_base);
    
    context->regs_d[0] = data->src;
    emu68k_set_flags(context, data->flags_in);
    context->pc = 0x002000;
    context->cycles_left = data->cycles;
    
    do_run(NULL);
    
    check_reg(context->regs_d[0], data->dest);
    check_flags(data->flags_out);
    check_cycles(0);
    
    if (did_test_fail) {
	dump_shift_immed_reg_data(data);
    }
    
    do_done(NULL);
}

void test_shift_instructions(void)
{
    struct shift_immed_reg_data *cur_data;

    for (cur_data = shift_immed_test_data; cur_data->name; cur_data++) {
  	test_shift_immed_reg(cur_data);
    }
}

/*
 * test dispatch code
 */

typedef void (*internal_testfunc)(void);

internal_testfunc testfuncs[] = {
    test_branch_instructions,
    test_directed_alu_instructions,
    test_immediate_instructions,
    test_shift_instructions,
    NULL, /* must be last entry */
};

void run_internal_tests(void)
{
    internal_testfunc *cur_func;

    for (cur_func = testfuncs; *cur_func; cur_func++) {
	(*cur_func)();
    }
}

/*
 * $Log: test_68k_1.h,v $
 * Revision 1.8  2000/12/11 18:13:57  nyef
 * added some more directed alu tests
 *
 * Revision 1.7  2000/06/25 15:21:29  nyef
 * changed to use do_run() instead of emu68k_step()
 *
 * Revision 1.6  2000/06/04 16:53:14  nyef
 * added tests for all shift/rotate immediate, reg instructions
 *
 * Revision 1.5  2000/04/23 14:51:23  nyef
 * added "immediate" instruction tests
 *
 * Revision 1.4  2000/04/23 03:07:40  nyef
 * added "directed alu instruction" tests for ADD instructions
 *
 * Revision 1.3  2000/04/23 02:43:49  nyef
 * added "directed alu instruction" tests for SUB instructions
 *
 * Revision 1.2  2000/04/23 01:49:43  nyef
 * added "directed alu instruction" tests for AND and OR instructions
 *
 * Revision 1.1  2000/04/22 22:52:37  nyef
 * Initial revision
 *
 */
