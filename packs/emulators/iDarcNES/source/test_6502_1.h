/*
 * test_6502_1.h
 *
 * first new test suite for the cpu6502 core
 */

/* $Id: test_6502_1.h,v 1.3 2000/10/01 01:25:05 nyef Exp $ */

/*
 * useful macros
 */

#define TEST_DISPATCHER(type)         \
void test_##type##_instructions(void) \
{                                     \
    struct type##_test *cur_test;     \
                                      \
    for (cur_test = type##_tests; cur_test->name; cur_test++) { \
	do_start(cur_test->name);     \
	run_##type##_test(cur_test);  \
	do_done(NULL);                \
    }                                 \
}

/*
 * test branch instructions
 */

struct branch_test {
    char *name;
    u8 opcode;
    u8 offset;
    u8 flags;
    char *dest_pc;
    int cycles;
} branch_tests[] = {
    {"BPL taken 1", 0x10, 0x7f, 0x7f, "0x2081", 3},
    {"BPL taken 2", 0x10, 0x80, 0x7f, "0x1f82", 4},
    {"BPL taken 3", 0x10, 0x44, 0x00, "0x2046", 3},
    {"BPL not taken 1", 0x10, 0x55, 0x80, "0x2002", 2},
    {"BPL not taken 2", 0x10, 0x55, 0xff, "0x2002", 2},
    
    {"BMI taken 1", 0x30, 0x80, 0xff, "0x1f82", 4},
    {"BMI taken 2", 0x30, 0x44, 0x80, "0x2046", 3},
    {"BMI not taken 1", 0x30, 0x55, 0x00, "0x2002", 2},
    {"BMI not taken 2", 0x30, 0x55, 0x7f, "0x2002", 2},
    
    {"BVC taken 1", 0x50, 0x80, 0xbf, "0x1f82", 4},
    {"BVC taken 2", 0x50, 0x44, 0x00, "0x2046", 3},
    {"BVC not taken 1", 0x50, 0x55, 0x40, "0x2002", 2},
    {"BVC not taken 2", 0x50, 0x55, 0xff, "0x2002", 2},
    
    {"BVS taken 1", 0x70, 0x80, 0xff, "0x1f82", 4},
    {"BVS taken 2", 0x70, 0x44, 0x40, "0x2046", 3},
    {"BVS not taken 1", 0x70, 0x55, 0x00, "0x2002", 2},
    {"BVS not taken 2", 0x70, 0x55, 0xbf, "0x2002", 2},
    
    {"BCC taken 1", 0x90, 0x80, 0xfe, "0x1f82", 4},
    {"BCC taken 2", 0x90, 0x44, 0x00, "0x2046", 3},
    {"BCC not taken 1", 0x90, 0x55, 0x01, "0x2002", 2},
    {"BCC not taken 2", 0x90, 0x55, 0xff, "0x2002", 2},
    
    {"BCS taken 1", 0xb0, 0x80, 0xff, "0x1f82", 4},
    {"BCS taken 2", 0xb0, 0x44, 0x01, "0x2046", 3},
    {"BCS not taken 1", 0xb0, 0x55, 0x00, "0x2002", 2},
    {"BCS not taken 2", 0xb0, 0x55, 0xfe, "0x2002", 2},
    
    {"BNE taken 1", 0xd0, 0x80, 0xfd, "0x1f82", 4},
    {"BNE taken 2", 0xd0, 0x44, 0x00, "0x2046", 3},
    {"BNE not taken 1", 0xd0, 0x55, 0x02, "0x2002", 2},
    {"BNE not taken 2", 0xd0, 0x55, 0xff, "0x2002", 2},
    
    {"BEQ taken 1", 0xf0, 0x80, 0xff, "0x1f82", 4},
    {"BEQ taken 2", 0xf0, 0x44, 0x02, "0x2046", 3},
    {"BEQ not taken 1", 0xf0, 0x55, 0x00, "0x2002", 2},
    {"BEQ not taken 2", 0xf0, 0x55, 0xfd, "0x2002", 2},
    
    {NULL} /* must be last entry */
};

void run_branch_test(struct branch_test *test)
{
    context->pc = 0x2000;
    SET_MEMORY(0x2000, test->opcode);
    SET_MEMORY(0x2001, test->offset);
    context->cycles_left = test->cycles;
    SET_FLAGS(context, test->flags);
    
    do_run(NULL);

    do_check_pc(test->dest_pc);
    do_check_cycles("0");
}

TEST_DISPATCHER(branch);

/*
 * test shift/rotate instructions
 */

struct shift_test {
    char *name;
    u8 opcode;
    u8 data_before;
    u8 data_after;
    u8 flags_before;
    u8 flags_after;
    int cycles;
} shift_tests[] = {
    {"ASL acc 1", 0x0a, 0x55, 0xaa, 0xff, 0xfc, 2},
    {"ASL acc 2", 0x0a, 0x55, 0xaa, 0x01, 0x80, 2},
    {"ASL acc 3", 0x0a, 0xaa, 0x54, 0x00, 0x01, 2},

    {"ROL acc 1", 0x2a, 0x55, 0xab, 0xff, 0xfc, 2},
    {"ROL acc 2", 0x2a, 0x55, 0xab, 0x01, 0x80, 2},
    {"ROL acc 3", 0x2a, 0x55, 0xaa, 0xfe, 0xfc, 2},
    {"ROL acc 4", 0x2a, 0xaa, 0x54, 0x00, 0x01, 2},
    {"ROL acc 5", 0x2a, 0xaa, 0x55, 0x01, 0x01, 2},

    {"LSR acc 1", 0x4a, 0x55, 0x2a, 0xff, 0x7d, 2},
    {"LSR acc 2", 0x4a, 0x55, 0x2a, 0x01, 0x01, 2},
    {"LSR acc 3", 0x4a, 0xaa, 0x55, 0x00, 0x00, 2},
    
    {"ROR acc 1", 0x6a, 0x55, 0xaa, 0xff, 0xfd, 2},
    {"ROR acc 2", 0x6a, 0x55, 0xaa, 0x01, 0x81, 2},
    {"ROR acc 3", 0x6a, 0x55, 0x2a, 0xfe, 0x7d, 2},
    {"ROR acc 4", 0x6a, 0xaa, 0x55, 0x00, 0x00, 2},
    {"ROR acc 5", 0x6a, 0xaa, 0xd5, 0x01, 0x80, 2},
    
    {NULL} /* must be last entry */
};

void run_shift_test(struct shift_test *test)
{
    context->pc = 0x2000;
    SET_MEMORY(0x2000, test->opcode);
    context->cycles_left = test->cycles;
    context->reg_a = test->data_before;
    SET_FLAGS(context, test->flags_before);
    
    do_run(NULL);

    do_check_pc("0x2001");
    check_reg(context->reg_a, test->data_after);
    check_flags(test->flags_after);
    do_check_cycles("0");
}

TEST_DISPATCHER(shift);

/*
 * test flag manipulation instructions
 */

struct flag_test {
    char *name;
    u8 opcode;
    u8 flags_before;
    u8 flags_after;
} flag_tests[] = {
    {"CLC 1", 0x18, 0x00, 0x00},
    {"CLC 2", 0x18, 0xff, 0xfe},

    {"SEC 1", 0x38, 0x00, 0x01},
    {"SEC 2", 0x38, 0xff, 0xff},

    {"CLI 1", 0x58, 0x00, 0x00},
    {"CLI 2", 0x58, 0xff, 0xfb},

    {"SEI 1", 0x78, 0x00, 0x04},
    {"SEI 2", 0x78, 0xff, 0xff},

    {"CLV 1", 0xb8, 0x00, 0x00},
    {"CLV 2", 0xb8, 0xff, 0xbf},

    {"CLD 1", 0xd8, 0x00, 0x00},
    {"CLD 2", 0xd8, 0xff, 0xf7},

    {"SED 1", 0xf8, 0x00, 0x08},
    {"SED 2", 0xf8, 0xff, 0xff},

    {NULL} /* must be last entry */
};

void run_flag_test(struct flag_test *test)
{
    context->pc = 0x2000;
    SET_MEMORY(0x2000, test->opcode);
    context->cycles_left = 2;
    SET_FLAGS(context, test->flags_before);
    
    do_run(NULL);

    do_check_pc("0x2001");
    check_flags(test->flags_after);
    do_check_cycles("0");
}

TEST_DISPATCHER(flag);

/*
 * test reg transfer instructions
 */

struct transfer_test {
    char *name;
    u8 opcode;
    u8 reg_a_before;
    u8 reg_x_before;
    u8 reg_y_before;
    u8 reg_s_before;
    u8 reg_p_before;
    u8 reg_a_after;
    u8 reg_x_after;
    u8 reg_y_after;
    u8 reg_s_after;
    u8 reg_p_after;
} transfer_tests[] = {
    {"TAX 1", 0xaa, 0x85,0x00,0x00,0x00,0x00, 0x85,0x85,0x00,0x00,0x80},
    {"TXA 1", 0x8a, 0x00,0x85,0x00,0x00,0x00, 0x85,0x85,0x00,0x00,0x80},
    
    {"TAY 1", 0xa8, 0x85,0x00,0x00,0x00,0x00, 0x85,0x00,0x85,0x00,0x80},
    {"TYA 1", 0x98, 0x00,0x00,0x85,0x00,0x00, 0x85,0x00,0x85,0x00,0x80},
    
    {"TXS 1", 0x9a, 0x00,0x85,0x00,0x00,0x00, 0x00,0x85,0x00,0x85,0x00},
    {"TSX 1", 0xba, 0x00,0x00,0x00,0x85,0x00, 0x00,0x85,0x00,0x85,0x80},
    
    {NULL} /* must be last entry */
};

void run_transfer_test(struct transfer_test *test)
{
    context->pc = 0x2000;
    SET_MEMORY(0x2000, test->opcode);
    context->cycles_left = 2;
    context->reg_a = test->reg_a_before;
    context->reg_x = test->reg_x_before;
    context->reg_y = test->reg_y_before;
    context->reg_s = test->reg_s_before;
    SET_FLAGS(context, test->reg_p_before);
    
    do_run(NULL);

    do_check_pc("0x2001");
    check_reg(context->reg_a, test->reg_a_after);
    check_reg(context->reg_x, test->reg_x_after);
    check_reg(context->reg_y, test->reg_y_after);
    check_reg(context->reg_s, test->reg_s_after);
    check_flags(test->reg_p_after);
    do_check_cycles("0");
}

TEST_DISPATCHER(transfer);

/*
 * test ADC and SBC instructions
 */

struct adc_sbc_test {
    char *name;
    u8 opcode;
    u8 value;
    u8 start_a;
    u8 end_a;
    u8 start_p;
    u8 end_p;
} adc_sbc_tests[] = {
    /* This test data checked on an Apple IIgs (65816). So there. */
    /* We are testing primarily for the behavior of the V flag */

    /* ADC tests checked via "800:18 a9 start_a 69 value 00 N 800G" */
    {"ADC 1", 0x69, 0x00, 0x00, 0x00, 0x30, 0x32},
    {"ADC 2", 0x69, 0xff, 0x00, 0xff, 0x30, 0xb0},
    {"ADC 3", 0x69, 0x01, 0xff, 0x00, 0x30, 0x33},
    {"ADC 4", 0x69, 0x00, 0x7f, 0x7f, 0x30, 0x30},
    {"ADC 5", 0x69, 0x00, 0x80, 0x80, 0x30, 0xb0},
    {"ADC 6", 0x69, 0x01, 0x7f, 0x80, 0x30, 0xf0},
    {"ADC 7", 0x69, 0xff, 0x80, 0x7f, 0x30, 0x71},

    /* SBC tests checked via "800:38 a9 start_a e9 value 00 N 800G" */
    {"SBC 1", 0xe9, 0x00, 0x00, 0x00, 0x31, 0x33},
    {"SBC 2", 0xe9, 0xff, 0xff, 0x00, 0x31, 0x33},
    {"SBC 3", 0xe9, 0x01, 0x00, 0xff, 0x31, 0xb0},
    {"SBC 4", 0xe9, 0x00, 0x7f, 0x7f, 0x31, 0x31},
    {"SBC 5", 0xe9, 0x00, 0x80, 0x80, 0x31, 0xb1},
    {"SBC 6", 0xe9, 0x01, 0x80, 0x7f, 0x31, 0x71},
    {"SBC 7", 0xe9, 0xff, 0x7f, 0x80, 0x31, 0xf0},

    {NULL} /* must be last entry */
};

void run_adc_sbc_test(struct adc_sbc_test *test)
{
    context->pc = 0x2000;
    SET_MEMORY(0x2000, test->opcode);
    SET_MEMORY(0x2001, test->value);
    context->cycles_left = 2;
    context->reg_a = test->start_a;
    SET_FLAGS(context, test->start_p);
    
    do_run(NULL);

    do_check_pc("0x2002");
    check_reg(context->reg_a, test->end_a);
    check_flags(test->end_p);
    do_check_cycles("0");
}

TEST_DISPATCHER(adc_sbc);

/*
 * test dispatch code
 */

typedef void (*internal_testfunc)(void);

internal_testfunc testfuncs[] = {
    test_branch_instructions,
    test_shift_instructions,
    test_flag_instructions,
    test_transfer_instructions,
    test_adc_sbc_instructions,
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
 * $Log: test_6502_1.h,v $
 * Revision 1.3  2000/10/01 01:25:05  nyef
 * added tests for ADC and SBC viz the V flag
 *
 * Revision 1.2  2000/09/23 02:30:15  nyef
 * added tests for shift/rotate instructions
 * added tests for flag instructions
 * added tests for register transfer instructions
 *
 * Revision 1.1  2000/09/19 02:00:37  nyef
 * Initial revision
 *
 */
