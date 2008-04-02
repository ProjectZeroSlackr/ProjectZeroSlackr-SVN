/*
 * test_68k.c
 *
 * test the "junk68k" core.
 */

/* $Id: test_68k.c,v 1.11 2000/04/23 01:49:09 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "types.h"
#include "ui.h"
#include "cal.h"
#include "emu68k.h"
#include "tool.h"

#define MAX_MEMLOCS 32

struct emu68k_context context_true;
struct emu68k_context *context = &context_true;

u32 system_flags;

int did_test_fail;
int num_passed;
int num_failed;
int in_test;

char cur_test[256];

int cur_line;

typedef void (*testfunc)(void);

struct memloc {
    u32 address;
    u16 data;
};

struct memloc memlocs[MAX_MEMLOCS];
int memlocs_used;

#define SET_MEMORY(addr, data16) do { memlocs[memlocs_used].data = data16; memlocs[memlocs_used].address = addr; memlocs_used++; } while (0)

u8 test_read8(cal_cpu cpu, u32 address)
{
    int i;
    u32 addr;
    
    addr = address & 0xffffff;

    for (i = 0; i < memlocs_used; i++) {
	if (memlocs[i].address == (addr & ~1)) {
	    if (addr & 1) {
		return memlocs[i].data & 0xff;
	    } else {
		return memlocs[i].data >> 8;
	    }
	}
    }

    printf("read8 access to unknown location 0x%06lx.\n", addr);

    did_test_fail = 1;
    
    return 0;
}

void test_write8(cal_cpu cpu, u32 address, u8 data)
{
    int i;
    u32 addr;

    addr = address & 0xffffff;
    
    for (i = 0; i < memlocs_used; i++) {
	if (memlocs[i].address == (addr & ~1)) {
	    if (addr & 1) {
		memlocs[i].data &= 0xff00;
		memlocs[i].data |= data;
	    } else {
		memlocs[i].data &= 0xff;
		memlocs[i].data |= data << 8;
	    }
	    return;
	}
    }

    printf("write8 access 0x%02x to unknown location 0x%06lx.\n", data, addr);

    did_test_fail = 1;
}

u16 test_read16(cal_cpu cpu, u32 address)
{
    int i;
    u32 addr;
    
    addr = address & 0xffffff;

    if (addr & 1) {
	printf("BUS error.\n");
	did_test_fail = 1;
	return 0;
    }

    for (i = 0; i < memlocs_used; i++) {
	if (memlocs[i].address == (addr)) {
	    return memlocs[i].data;
	}
    }

    printf("read16 access to unknown location 0x%06lx.\n", addr);

    did_test_fail = 1;
    
    return 0;
}

void test_write16(cal_cpu cpu, u32 address, u16 data)
{
    int i;
    u32 addr;

    addr = address & 0xffffff;

    if (addr & 1) {
	printf("BUS error.\n");
	did_test_fail = 1;
	return;
    }

    for (i = 0; i < memlocs_used; i++) {
	if (memlocs[i].address == (addr)) {
	    memlocs[i].data = data;
	    return;
	}
    }

    printf("write16 access 0x%04hx to unknown location 0x%06lx.\n", data, addr);

    did_test_fail = 1;
}

memread8_t read8table[1] = {test_read8};
memwrite8_t write8table[1] = {test_write8};
memread16_t read16table[1] = {test_read16};
memwrite16_t write16table[1] = {test_write16};

void clear_cpu(void)
{
    int i;

    for (i = 0; i < 8; i++) {
	context->regs_a[i] = 0;
	context->regs_d[i] = 0;
    }

    context->pc = 0;
/*      context->flags = 0; */
    emu68k_set_flags(context, 0);
    context->other_sp = 0;
    
    context->cycles_left = 0;
    
    context->read8table = read8table;
    context->write8table = write8table;
    context->read16table = read16table;
    context->write16table = write16table;
    context->memshift = 0;
    context->memmask = 0;
    context->cpu = NULL;

    memlocs_used = 0;
}

void deb_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    did_test_fail = 1;
}

char *strip_blanks(char *buf)
{
    char *bufptr;
    
    bufptr = buf;

    while (isblank(*bufptr)) {
	bufptr++;
    }

    return bufptr;
}

typedef int (*line_handler)(char *);

struct parse_table {
    char *match;
    line_handler handler;
};

int do_start(char *buf)
{
    if (in_test) {
	printf("start found while already in test.\n");
	return 1;
    }

    if (!*buf) {
	printf("test has no name.\n");
	return 1;
    }

    strcpy(cur_test, buf);
    
    did_test_fail = 0;
    clear_cpu();
    in_test = 1;
    return 0;
}

int do_set_reg(char *buf)
{
    unsigned long *regset;
    int reg;
    char *bufptr;
    
    if (buf[0] == 'a') {
	regset = context->regs_a;
    } else if (buf[0] == 'd') {
	regset = context->regs_d;
    } else {
	printf("unknown reg.\n");
	return 1;
    }

    if ((buf[1] < '0') || (buf[1] > '7')) {
	printf("unknown reg.\n");
	return 1;
    }

    reg = buf[1] - '0';
    
    bufptr = buf + 2;

    bufptr = strip_blanks(bufptr);
    
    if (*bufptr != '=') {
	printf("expected '='.\n");
	return 1;
    }

    bufptr++;
    
    bufptr = strip_blanks(bufptr);

    regset[reg] = strtoul(bufptr, &bufptr, 0);

    return 0;
}

int do_set_mem(char *buf)
{
    char *bufptr;
    unsigned long addr;
    unsigned short data;

    addr = strtoul(buf, &bufptr, 0);

    bufptr = strip_blanks(bufptr);

    if (*bufptr != '=') {
	printf("expected '='.\n");
	return 1;
    }

    bufptr++;
    
    bufptr = strip_blanks(bufptr);
    
    data = strtoul(bufptr, &bufptr, 0);

    SET_MEMORY(addr, data);
    
    return 0;
}

int do_set_flags(char *buf)
{
/*      context->flags = strtoul(buf, NULL, 0); */
    emu68k_set_flags(context, strtoul(buf, NULL, 0));
    
    return 0;
}

int do_set_other_sp(char *buf)
{
    context->other_sp = strtoul(buf, NULL, 0);
    
    return 0;
}

int do_set_cycles(char *buf)
{
    if (!isdigit(*buf)) {
	printf("expected number.\n");
	return 1;
    }
    
    context->cycles_left = strtol(buf, NULL, 0);
    
    return 0;
}

int do_set_pc(char *buf)
{
    context->pc = strtoul(buf, NULL, 0);
    
    return 0;
}

struct parse_table set_line_types[] = {
    {"reg",      do_set_reg},
    {"register", do_set_reg},
    {"mem",      do_set_mem},
    {"memory",   do_set_mem},
    {"pc",       do_set_pc},
    {"cycles",   do_set_cycles},
    {"flags",    do_set_flags},
    {"other_sp", do_set_other_sp},
    {NULL, NULL}, /* must be last entry */
};

int do_set(char *buf)
{
    struct parse_table *cur_match;
    char *bufptr;
    int match_len;

    if (!in_test) {
	printf("set found while not in test.\n");
	return 1;
    }
    
    bufptr = buf;

    for (cur_match = set_line_types; cur_match->match; cur_match++) {
	match_len = strlen(cur_match->match);
	if (!strncmp(bufptr, cur_match->match, match_len)) {
	    if (bufptr[match_len] && (!isblank(bufptr[match_len]))) {
		continue;
	    }
	    
	    bufptr += match_len;
	    bufptr = strip_blanks(bufptr);
	    
	    return cur_match->handler(bufptr);
	}
    }

    return 1;
}

int do_run(char *buf)
{
    if (!in_test) {
	printf("run found while not in test.\n");
	return 1;
    }
    
    emu68k_step(context);
    return 0;
}

void check_reg(u32 reg, u32 data)
{
    if (reg != data) {
	printf("check reg: expected 0x%08lx, found 0x%08lx.\n", data, reg);
	did_test_fail = 1;
    }
}

int do_check_reg(char *buf)
{
    unsigned long *regset;
    unsigned long data;
    int reg;
    char *bufptr;
    
    if (buf[0] == 'a') {
	regset = context->regs_a;
    } else if (buf[0] == 'd') {
	regset = context->regs_d;
    } else {
	printf("unknown reg.\n");
	return 1;
    }

    if ((buf[1] < '0') || (buf[1] > '7')) {
	printf("unknown reg.\n");
	return 1;
    }

    reg = buf[1] - '0';
    
    bufptr = buf + 2;

    bufptr = strip_blanks(bufptr);
    
    if (*bufptr != '=') {
	printf("expected '='.\n");
	return 1;
    }

    bufptr++;
    
    bufptr = strip_blanks(bufptr);

    data = strtoul(bufptr, &bufptr, 0);

    check_reg(regset[reg], data);

    return 0;
}

void check_memory(u32 addr, u16 data)
{
    u16 real_data;

    real_data = test_read16(NULL, addr);
    
    if (data != real_data) {
	printf("check mem: expected 0x%04hx, found 0x%04hx.\n", data, real_data);
	did_test_fail = 1;
    }
}

int do_check_mem(char *buf)
{
    char *bufptr;
    unsigned long addr;
    unsigned short data;

    addr = strtoul(buf, &bufptr, 0);

    bufptr = strip_blanks(bufptr);

    if (*bufptr != '=') {
	printf("expected '='.\n");
	return 1;
    }

    bufptr++;
    
    bufptr = strip_blanks(bufptr);
    
    data = strtoul(bufptr, &bufptr, 0);

    check_memory(addr, data);
    
    return 0;
}

void check_flags(u16 flags)
{
    u16 real_flags;
    
    real_flags = emu68k_get_flags(context);

    if (flags != real_flags) {
	printf("check flags: expected 0x%04hx, found 0x%04hx.\n", flags, real_flags);
	did_test_fail = 1;
    }
}

int do_check_flags(char *buf)
{
    unsigned short data;
    
    if (!isdigit(*buf)) {
	printf("expected number.\n");
	return 1;
    }
    
    data = strtoul(buf, NULL, 0);

    check_flags(data);
    
    return 0;
}

int do_check_other_sp(char *buf)
{
    unsigned long data;
    
    if (!isdigit(*buf)) {
	printf("expected number.\n");
	return 1;
    }
    
    data = strtoul(buf, NULL, 0);

    if (data != context->other_sp) {
	printf("check other_sp: expected 0x%08lx, found 0x%08lx.\n", data, context->other_sp);
	did_test_fail = 1;
    }
    
    return 0;
}

void check_cycles(int cycles)
{
    if (cycles != context->cycles_left) {
	printf("check cycles: expected 0x%04hx, found 0x%04hx.\n", cycles, context->cycles_left);
	did_test_fail = 1;
    }
}

int do_check_cycles(char *buf)
{
    int data;
    
    data = strtol(buf, NULL, 0);

    check_cycles(data);
    
    return 0;
}

void check_pc(u32 pc)
{
    if (pc != (context->pc & 0xffffff)) {
	printf("check pc: expected 0x%06lx, found 0x%06lx.\n", pc, context->pc & 0x00ffffff);
	did_test_fail = 1;
    }
}

int do_check_pc(char *buf)
{
    unsigned long data;
    
    data = strtoul(buf, NULL, 0);
    
    check_pc(data);

    return 0;
}

struct parse_table check_line_types[] = {
    {"reg",      do_check_reg},
    {"register", do_check_reg},
    {"mem",      do_check_mem},
    {"memory",   do_check_mem},
    {"pc",       do_check_pc},
    {"cycles",   do_check_cycles},
    {"flags",    do_check_flags},
    {"other_sp", do_check_other_sp},
    {NULL, NULL}, /* must be last entry */
};

int do_check(char *buf)
{
    struct parse_table *cur_match;
    char *bufptr;
    int match_len;

    if (!in_test) {
	printf("check found while not in test.\n");
	return 1;
    }
    
    bufptr = buf;

    for (cur_match = check_line_types; cur_match->match; cur_match++) {
	match_len = strlen(cur_match->match);
	if (!strncmp(bufptr, cur_match->match, match_len)) {
	    if (bufptr[match_len] && (!isblank(bufptr[match_len]))) {
		continue;
	    }
	    
	    bufptr += match_len;
	    bufptr = strip_blanks(bufptr);
	    
	    return cur_match->handler(bufptr);
	}
    }

    return 1;
}

int do_done(char *buf)
{
    if (!in_test) {
	printf("done found while not in test.\n");
	return 1;
    }
    
    if (did_test_fail) {
	printf("test \"%s\" failed.\n", cur_test);
	num_failed++;
    } else {
	num_passed++;
    }

    in_test = 0;
    
    return 0;
}

struct parse_table line_types[] = {
    {"start", do_start},
    {"set",   do_set},
    {"run",   do_run},
    {"check", do_check},
    {"done",  do_done}, /* Do Done is a song by Kudo Shizuka */
    {NULL, NULL}, /* must be last entry */
};

int parse_test_file_line(char *buf)
{
    struct parse_table *cur_match;
    char *bufptr;
    int match_len;

    bufptr = buf;

    bufptr = strip_blanks(bufptr);

    if (!*bufptr) {
	/* blank line, no action */
	return 0;
    }
    
    if (*bufptr == '#') {
	/* comment, no action */
	return 0;
    }

    for (cur_match = line_types; cur_match->match; cur_match++) {
	match_len = strlen(cur_match->match);
	if (!strncmp(bufptr, cur_match->match, match_len)) {
	    if (bufptr[match_len] && (!isblank(bufptr[match_len]))) {
		continue;
	    }
	    
	    bufptr += match_len;
	    bufptr = strip_blanks(bufptr);
	    
	    return cur_match->handler(bufptr);
	}
    }

    return 1;
}

void parse_test_file(FILE *testfile)
{
    char buf[256];
    
    in_test = 0;
    
    while (!feof(testfile)) {
	cur_line++;
	if (!fgets(buf, 256, testfile)) {
	    return;
	}
	if (buf[strlen(buf) - 1] == '\n') {
	    buf[strlen(buf) - 1] = '\0';
	}
	if (parse_test_file_line(buf)) {
	    printf("Syntax error, line %d.\n", cur_line);
	    did_test_fail = 1;
	}
    }
}

#include "test_68k_1.h"

int main(int argc, char *argv[])
{
    num_passed = 0;
    num_failed = 0;

    printf("Running tests...\n");

    run_internal_tests();

    parse_test_file(stdin);

    if (in_test) {
	printf("Still in test at EOF.\n");
	printf("Syntax error, line %d.\n", cur_line + 1);
	num_failed++;
    }
    
    printf("Summary: %d passed, %d failed, %d total.\n", num_passed, num_failed, num_passed + num_failed);
    
    return 0;
}

/*
 * $Log: test_68k.c,v $
 * Revision 1.11  2000/04/23 01:49:09  nyef
 * added more support for compiled tests
 *
 * Revision 1.10  2000/04/22 22:53:21  nyef
 * added support for compiled tests in test_68k_1.h
 *
 * Revision 1.9  2000/03/18 18:12:19  nyef
 * changed to use new emu68k flags interface
 *
 * Revision 1.8  1999/12/18 17:07:18  nyef
 * added support for setting/checking other_sp
 *
 * Revision 1.7  1999/12/17 01:44:47  nyef
 * changed deb_printf() to print the debug string out and fail the test
 *
 * Revision 1.6  1999/12/17 01:27:28  nyef
 * fixed bug in output from do_check_mem()
 *
 * Revision 1.5  1999/12/13 01:53:56  nyef
 * fixed address truncation
 * fixed diagnostic message in test_write16()
 *
 * Revision 1.4  1999/12/13 01:44:49  nyef
 * fixed memory read functions to truncate addresses to 24 bits
 *
 * Revision 1.3  1999/12/13 01:26:29  nyef
 * added some diagnostic output to the cycle set and check functions
 *
 * Revision 1.2  1999/12/12 20:32:02  nyef
 * fixed bug with diagnostic message for cycle check failure
 *
 * Revision 1.1  1999/12/12 18:19:37  nyef
 * Initial revision
 *
 */
