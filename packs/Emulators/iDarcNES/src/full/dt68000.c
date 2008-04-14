/*
 * dt68000.c
 *
 * Instruction decode for m68000 CPU
 */

/* $Id: dt68000.c,v 1.2 1999/12/02 01:53:51 nyef Exp $ */

#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include "cal.h"
#include "ui.h"
#include "tool.h"

#define S_NONE 0
#define S_BYTE 1
#define S_WORD 2
#define S_LONG 3

#define F_AMSRC   0x0001
#define F_AMDST   0x0002
#define F_RELJMP  0x0004
#define F_SRCREG  0x0008
#define F_DSTREG  0x0010
#define F_DSTREGA 0x0020
#define F_IMDATA  0x00c0
#define F_IMBYTE  0x0040
#define F_IMWORD  0x0080
#define F_IMLONG  0x00c0
#define F_LOBYTE  0x0100
#define F_SRCDST  0x0200
#define F_QUICK   0x0400
#define F_SRCREG2 0x0800

typedef struct {
    unsigned short mask;
    unsigned short pattern;
    unsigned char *name;
    int size;
    int flags;
} m68k_instr_data;

typedef struct {
} m68k_amode_data;

unsigned char *romimage;

static m68k_instr_data instrs[] = {
    {0xffc0, 0x0000, "ori.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0040, "ori.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0080, "ori.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xf1c0, 0x0100, "btst", S_BYTE, F_AMSRC | F_DSTREG | F_SRCDST},
    {0xf1c0, 0x0100, "bchg", S_BYTE, F_AMSRC | F_DSTREG | F_SRCDST},
    {0xf1c0, 0x0100, "bclr", S_BYTE, F_AMSRC | F_DSTREG | F_SRCDST},
    {0xf1c0, 0x0100, "bset", S_BYTE, F_AMSRC | F_DSTREG | F_SRCDST},
    {0xffc0, 0x0200, "andi.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0240, "andi.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0280, "andi.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xffc0, 0x0400, "subi.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0440, "subi.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0480, "subi.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xffc0, 0x0600, "addi.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0640, "addi.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0680, "addi.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xffc0, 0x0800, "btst", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0840, "bchg", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0880, "bclr", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x08c0, "bset", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0a00, "eori.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0a40, "eori.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0a80, "eori.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xffc0, 0x0c00, "cmpi.b", S_BYTE, F_AMSRC | F_IMBYTE | F_SRCDST},
    {0xffc0, 0x0c40, "cmpi.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x0c80, "cmpi.l", S_LONG, F_AMSRC | F_IMLONG | F_SRCDST},
    {0xf000, 0x1000, "move.b", S_BYTE, F_AMSRC | F_AMDST},
    {0xf1c0, 0x2040, "movea.l", S_LONG, F_AMSRC | F_AMDST},
    {0xf000, 0x2000, "move.l", S_LONG, F_AMSRC | F_AMDST},
    {0xf1c0, 0x3040, "movea.w", S_WORD, F_AMSRC | F_AMDST},
    {0xf000, 0x3000, "move.w", S_WORD, F_AMSRC | F_AMDST},
    {0xf1c0, 0x41c0, "lea", S_NONE, F_AMSRC | F_DSTREG},
    {0xffc0, 0x4a00, "tst.b", S_BYTE, F_AMSRC},
    {0xffc0, 0x4a40, "tst.w", S_WORD, F_AMSRC},
    {0xffc0, 0x4a80, "tst.l", S_LONG, F_AMSRC},
    {0xffc0, 0x4c80, "movem.w", S_WORD, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xffc0, 0x4cc0, "movem.l", S_LONG, F_AMSRC | F_IMWORD | F_SRCDST},
    {0xfff8, 0x4e60, "move usp"},
    {0xfff8, 0x4e68, "move usp"},
    {0xfff8, 0x50c8, "dbt", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x51c8, "dbf", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x52c8, "dbhi", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x53c8, "dbls", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x54c8, "dbcc", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x55c8, "dbcs", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x56c8, "dbne", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x57c8, "dbeq", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x58c8, "dbvc", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x59c8, "dbvs", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5ac8, "dbpl", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5bc8, "dbmi", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5cc8, "dbge", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5dc8, "dblt", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5ec8, "dbgt", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xfff8, 0x5fc8, "dble", S_NONE, F_RELJMP | F_SRCREG | F_SRCDST},
    {0xf1c0, 0x5000, "addq.b", S_BYTE, F_QUICK | F_AMSRC | F_SRCDST},
    {0xf1c0, 0x5040, "addq.w", S_WORD, F_QUICK | F_AMSRC | F_SRCDST},
    {0xf1c0, 0x5080, "addq.l", S_LONG, F_QUICK | F_AMSRC | F_SRCDST},
    {0xf1c0, 0x5100, "subq.b", S_BYTE, F_QUICK | F_AMSRC | F_SRCDST},
    {0xf1c0, 0x5140, "subq.w", S_WORD, F_QUICK | F_AMSRC | F_SRCDST},
    {0xf1c0, 0x5180, "subq.l", S_LONG, F_QUICK | F_AMSRC | F_SRCDST},
    {0xff00, 0x6000, "bra", S_NONE, F_RELJMP},
    {0xff00, 0x6100, "bsr", S_NONE, F_RELJMP},
    {0xff00, 0x6200, "bhi", S_NONE, F_RELJMP},
    {0xff00, 0x6300, "bls", S_NONE, F_RELJMP},
    {0xff00, 0x6400, "bcc", S_NONE, F_RELJMP},
    {0xff00, 0x6500, "bcs", S_NONE, F_RELJMP},
    {0xff00, 0x6600, "bne", S_NONE, F_RELJMP},
    {0xff00, 0x6700, "beq", S_NONE, F_RELJMP},
    {0xff00, 0x6800, "bvc", S_NONE, F_RELJMP},
    {0xff00, 0x6900, "bvs", S_NONE, F_RELJMP},
    {0xff00, 0x6a00, "bpl", S_NONE, F_RELJMP},
    {0xff00, 0x6b00, "bmi", S_NONE, F_RELJMP},
    {0xff00, 0x6c00, "bge", S_NONE, F_RELJMP},
    {0xff00, 0x6d00, "blt", S_NONE, F_RELJMP},
    {0xff00, 0x6e00, "bgt", S_NONE, F_RELJMP},
    {0xff00, 0x6f00, "ble", S_NONE, F_RELJMP},
    {0xf100, 0x7000, "moveq", S_NONE, F_LOBYTE | F_DSTREG},
    {0xf1c0, 0xd000, "add.b", S_BYTE, F_DSTREG | F_AMSRC},
    {0xf1c0, 0xd040, "add.w", S_WORD, F_DSTREG | F_AMSRC},
    {0xf1c0, 0xd080, "add.l", S_LONG, F_DSTREG | F_AMSRC},
    {0xf1c0, 0xd0c0, "adda.w", S_WORD, F_DSTREG | F_AMSRC},
    {0xf1c0, 0xd100, "add.b", S_BYTE, F_SRCREG | F_AMSRC | F_SRCDST},
    {0xf1c0, 0xd140, "add.w", S_WORD, F_SRCREG | F_AMSRC | F_SRCDST},
    {0xf1c0, 0xd180, "add.l", S_LONG, F_SRCREG | F_AMSRC | F_SRCDST},
    {0xf1c0, 0xd1c0, "adda.l", S_LONG, F_SRCREG | F_AMSRC | F_SRCDST},
    {0xf1f8, 0xe008, "lsr.b", S_BYTE, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe048, "lsr.w", S_WORD, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe088, "lsr.l", S_LONG, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe028, "lsr.b", S_BYTE, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0xf1f8, 0xe068, "lsr.w", S_WORD, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0xf1f8, 0xe0a8, "lsr.l", S_LONG, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0xf1f8, 0xe108, "lsl.b", S_BYTE, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe148, "lsl.w", S_WORD, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe188, "lsl.l", S_LONG, F_QUICK | F_SRCREG2},
    {0xf1f8, 0xe128, "lsl.b", S_BYTE, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0xf1f8, 0xe168, "lsl.w", S_WORD, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0xf1f8, 0xe1a8, "lsl.l", S_LONG, F_DSTREG | F_SRCREG2 | F_SRCDST},
    {0x0000, 0x0000, "<unknown>", S_NONE, 0},
};

static unsigned long handle_amode(char *buf, unsigned long addr, int size, int reg, int mode)
{
    unsigned long count = 0;
    unsigned short data1;
    unsigned short data2;
    
    switch (mode) {
    case 0:
	*buf++ = 'd';
	*buf++ = '0' + reg;
	break;
    case 1:
	*buf++ = 'a';
	*buf++ = '0' + reg;
	break;
    case 2:
	*buf++ = '(';
	*buf++ = 'a';
	*buf++ = '0' + reg;
	*buf++ = ')';
	break;
    case 3:
	*buf++ = '(';
	*buf++ = 'a';
	*buf++ = '0' + reg;
	*buf++ = ')';
	*buf++ = '+';
	break;
    case 4:
	*buf++ = '-';
	*buf++ = '(';
	*buf++ = 'a';
	*buf++ = '0' + reg;
	*buf++ = ')';
	break;
    case 5:
	data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	count += 2;
	buf += sprintf(buf, "(0x%04hx, a%d)", data1, reg);
	break;
    case 6:
	data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	count += 2;
	buf += sprintf(buf, "(0x%02x, a%d, %c%d%s)", data1 & 0xff, reg, (data1 & 0x8000)? 'a': 'd', (data1 >> 12) & 7, (data1 & 0x0800)? ".l": ".w");
	break;
    case 7:
	switch (reg) {
	case 0:
	    data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	    count += 2;
	    buf += sprintf(buf, "(0x%04hx)", data1);
	    break;
	case 1:
	    data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	    data2 = htons(((unsigned short *)romimage)[(addr + 2) >> 1]);
	    count += 4;
	    buf += sprintf(buf, "(0x%04hx%04hx)", data1, data2);
	    break;
	case 2:
	    data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	    count += 2;
	    buf += sprintf(buf, "(0x%04hx, pc)", data1);
	    break;
	case 3:
	    data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	    count += 2;
	    buf += sprintf(buf, "(0x%02x, pc, %c%d%s)", data1 & 0xff, (data1 & 0x8000)? 'a': 'd', (data1 >> 12) & 7, (data1 & 0x0800)? ".l": ".w");
	    break;
	case 4:
	    /* FIXME: data size? */
	    data1 = htons(((unsigned short *)romimage)[(addr) >> 1]);
	    if (size == S_LONG) {
		data2 = htons(((unsigned short *)romimage)[(addr + 2) >> 1]);
		count += 4;
		buf += sprintf(buf, "#0x%04hx%04hx", data1, data2);
	    } else {
		count += 2;
		if (size == S_WORD) {
		    buf += sprintf(buf, "#0x%04hx", data1);
		} else {
		    buf += sprintf(buf, "#0x%02hx", data1 & 0xff);
		}
	    }
	    break;
	default:
	    buf += sprintf(buf, "<unknown>");
	    break;
	}
	break;
    }
    *buf++ = '\0';
    return count;
}

static unsigned long disassemble_instr(char *buf, unsigned long addr)
{
    unsigned long count;
    m68k_instr_data *cur_instr;
    unsigned short opword;
    unsigned short data1;
    unsigned short data2;
    char buf1[20];
    char buf2[20];
    int done;
    
    count = 2;
    done = 0;
    opword = htons(((unsigned short *)romimage)[addr >> 1]);
    buf1[0] = '\0';
    buf2[0] = '\0';

    for (cur_instr = instrs; !done; cur_instr++) {
	if ((opword & cur_instr->mask) == cur_instr->pattern) {
	    if (cur_instr->flags & F_RELJMP) {
		if (((opword & 0xff) && (!(cur_instr->flags & F_SRCREG)))) {
		    sprintf(buf1, "0x%06lx", addr + count + ((signed char)opword));
		} else {
		    data1 = htons(((unsigned short *)romimage)[(addr + count) >> 1]);
		    sprintf(buf1, "0x%06lx", addr + count + ((signed short)data1));
		    count += 2;
		}
		if (cur_instr->flags & F_SRCDST) {
		    strcpy(buf2, buf1);
		}
	    }
	    if (cur_instr->flags & F_LOBYTE) {
		sprintf(buf1, "#0x%02x", opword & 0xff);
	    }
	    if ((cur_instr->flags & F_IMDATA) == F_IMBYTE) {
		data1 = htons(((unsigned short *)romimage)[(addr + count) >> 1]);
		sprintf(buf1, "#0x%02x", data1 & 0xff);
		count += 2;
	    }
	    if ((cur_instr->flags & F_IMDATA) == F_IMWORD) {
		data1 = htons(((unsigned short *)romimage)[(addr + count) >> 1]);
		sprintf(buf1, "#0x%04hx", data1);
		count += 2;
	    }
	    if ((cur_instr->flags & F_IMDATA) == F_IMLONG) {
		data1 = htons(((unsigned short *)romimage)[(addr + count) >> 1]);
		data2 = htons(((unsigned short *)romimage)[(addr + count + 2) >> 1]);
		sprintf(buf1, "#0x%04hx%04hx", data1, data2);
		count += 4;
	    }
	    if (cur_instr->flags & F_QUICK) {
		sprintf(buf1, "#%d", ((opword >> 9) & 7)? ((opword >> 9) & 7): 8);
	    }
	    if (cur_instr->flags & F_SRCREG) {
		sprintf(buf1, "d%d", opword & 7);
	    }
	    if (cur_instr->flags & F_SRCREG2) {
		sprintf(buf2, "d%d", opword & 7);
	    }
	    if (cur_instr->flags & F_DSTREG) {
		if (cur_instr->flags & F_SRCDST) {
		    sprintf(buf1, "d%d", (opword >> 9) & 7);
		} else {
		    sprintf(buf2, "d%d", (opword >> 9) & 7);
		}
	    }
	    if (cur_instr->flags & F_DSTREGA) {
		if (cur_instr->flags & F_SRCDST) {
		    sprintf(buf1, "a%d", (opword >> 9) & 7);
		} else {
		    sprintf(buf2, "a%d", (opword >> 9) & 7);
		}
	    }
	    if (cur_instr->flags & F_AMSRC) {
		count += handle_amode((cur_instr->flags & F_SRCDST)? buf2: buf1, addr + count, cur_instr->size, opword & 7, (opword >> 3) & 7);
	    }
	    if (cur_instr->flags & F_AMDST) {
		count += handle_amode(buf2, addr + count, cur_instr->size, (opword >> 9) & 7, (opword >> 6) & 7);
	    }
	    buf += sprintf(buf, "0x%06lx: 0x%04hx", addr, opword);
	    buf += sprintf(buf, "\t%s", cur_instr->name);
	    if (buf1[0]) {
		buf += sprintf(buf, " %s", buf1);
		if (buf2[0]) {
		    buf += sprintf(buf, ", %s", buf2);
		}
	    }
	    *buf++ = '\n';
	    *buf++ = '\0';
	    done = 1;
	}
    }
    
    return count;
}

int dt68000_disassemble(unsigned char *image, unsigned long start_addr, unsigned long end_addr)
{
    unsigned long cur_addr;
    unsigned long count;
    unsigned long len;
    char buf[128];
    
    romimage = image;
    
    cur_addr = start_addr;
    count = 0;

    while (cur_addr <= end_addr) { /* FIXME: breaks on wrap-around */
	len = disassemble_instr(buf, cur_addr);
	deb_printf(buf);
	cur_addr += len;
	count += len;
    }
    
    return count;
}

/*
 * $Log: dt68000.c,v $
 * Revision 1.2  1999/12/02 01:53:51  nyef
 * added some more instructions to the instruction table
 * hacked up the interface to accept a ROM image as a parameter
 *
 * Revision 1.1  1999/09/08 01:02:24  nyef
 * Initial revision
 *
 */
