/*
 * msx.c
 *
 * MSX emulation.
 */

/* $Id: msx.c,v 1.2 2001/02/27 04:14:32 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "tms9918.h"
#include "event.h"
#include "sms_psg.h" /* FIXME: Wrong! */

#define MSX_RAM_SIZE 0x10000

/*
 * common variable and structure definitions
 */

unsigned char *msx_memory;
cal_cpu msx_cpu;
tms9918 msx_vdp;

unsigned char *msx_rombase;
rom_file msx_romfile;

struct io_read_map {
    u8 mask; u8 addr;
    u8 (*handler)(u32);
};

struct io_write_map {
    u8 mask; u8 addr;
    void (*handler)(u32, u8);
};

void msx_sync_mmu(void);

/*
 * common I/O handlers
 */

u8 msx_read_tms9918(u32 address)
{
    if (address & 1) {
	return tms9918_readport1(msx_vdp);
    } else {
	return tms9918_readport0(msx_vdp);
    }
}

void msx_write_tms9918(u32 address, u8 data)
{
    if (address & 1) {
	tms9918_writeport1(msx_vdp, data);
    } else {
	tms9918_writeport0(msx_vdp, data);
    }
}

/*
 * MSX keyboard
 */

u8 msx_keyboard_data[11];

/*
 * The MSX keyboard is a scan-polled key array, configured as 11 rows of 8
 * bits on an 8255 PPI chip. The key encoding is approximately:
 *
 *    +---+---+---+---+---+---+---+---+
 *    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * +--+---+---+---+---+---+---+---+---+
 * | 0| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * +--+---+---+---+---+---+---+---+---+
 * | 1| 8 | 9 | - | = | \ | [ | ] | ; |
 * +--+---+---+---+---+---+---+---+---+
 * | 2| ' | ` | , | . | / |???| a | b |
 * +--+---+---+---+---+---+---+---+---+
 * | 3| c | d | e | f | g | h | i | j |
 * +--+---+---+---+---+---+---+---+---+
 * | 4| k | l | m | n | o | p | q | r |
 * +--+---+---+---+---+---+---+---+---+
 * | 5| s | t | u | v | w | x | y | z |
 * +--+---+---+---+---+---+---+---+---+
 * | 6|???|   |   |   |   |???|???|???|
 * +--+---+---+---+---+---+---+---+---+
 * | 7|???|???|   |???|   | BS|   | CR|
 * +--+---+---+---+---+---+---+---+---+
 * | 8| SP|???|???|DEL|???|???|???|???|
 * +--+---+---+---+---+---+---+---+---+
 * | 9|   |   |   |   |   |   |   |   |
 * +--+---+---+---+---+---+---+---+---+
 * |10|   |   |   |   |   |   |   |   |
 * +--+---+---+---+---+---+---+---+---+
 *
 * Where ??? indicates an unknown location with an obvious effect, and blank
 * spaces indicate no obvious effect (at least some of which are probably
 * bucky bits). 25 appears to be a composition key of some sort, and 60 appears
 * to be a shift. 73 could be tab. 65, 66, 67, and 70 could be function keys.
 * 81 appears to be HOME, and 82 insert. 84, 85, 86, and 87 are arrow keys.
 *
 * tredbook.txt says that rows 9 and 10 are unused.
 *
 * FIXME: Having the scan conversion data in a comment, and again in the array
 * below (in a completely unintelligable format, no less) is just asking for
 * trouble. Perhaps building msx_keyboard_map[] at runtime from a 11x8 array
 * of keycodes (pulled in via #defines in a "keyboard.h", most likely)?
 *
 * FIXME: Need to map the remaining ??? keys, and any non-alphanumeric
 * symbolic keys that we missed. And find the Control key if it exists.
 */

u8 msx_keyboard_map[0x80] = {
    0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* 0x00 - 0x07 */
    0x20, 0x21, 0x10, 0x00, 0x00, 0x00, 0x00, 0x56, /* 0x08 - 0x0f */

    0x64, 0x42, 0x57, 0x61, 0x66, 0x62, 0x46, 0x54, /* 0x10 - 0x17 */
    0x55, 0x00, 0x87, 0x00, 0x00, 0x36, 0x60, 0x41, /* 0x18 - 0x1f */

    0x43, 0x44, 0x45, 0x47, 0x50, 0x51, 0x27, 0x00, /* 0x20 - 0x27 */
    0x00, 0x00, 0x00, 0x67, 0x65, 0x40, 0x63, 0x37, /* 0x28 - 0x2f */

    0x53, 0x52, 0x32, 0x33, 0x34, 0x00, 0x00, 0x90, /* 0x30 - 0x37 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x38 - 0x3f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x40 - 0x47 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x48 - 0x4f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x50 - 0x57 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x58 - 0x5f */
    
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x60 - 0x67 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x68 - 0x6f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x78 - 0x7f */
};

void msx_keyboard_event(u8 code)
{
    u8 keyposition;
    u8 mask;
    int row;

    keyposition = msx_keyboard_map[code & 0x7f];

    if (!keyposition) return;

    row = (keyposition >> 4) - 1;
    mask = 1 << (keyposition & 0x0f);
    
    if (code & 0x80) { /* keyup */
	msx_keyboard_data[row] &= ~mask;
    } else { /* keydown */
	msx_keyboard_data[row] |= mask;
    }
}

/*
 * FIXME: This 8255 is a hackjob. We will need another one soon, so fix it then
 */

u8 msx_8255_port_a; /* Primary Slot Register */
u8 msx_8255_port_c;

u8 msx_read_8255(u32 address)
{
    switch (address & 3) {
    case 0: /* Primary Slot Register */
	return msx_8255_port_a;
	
    case 1: /* Keyboard Column Input */
	if ((msx_8255_port_c & 0x0f) <= 10) {
	    return msx_keyboard_data[msx_8255_port_c & 0x0f] ^ 0xff;
	}
	return 0xff;

    case 2:
	return msx_8255_port_c;

    case 3:
	deb_printf("msx: 8255 read control (INVALID), returning 0xff.\n");
	return 0xff;
    }

    /* NOTREACHED */
    deb_printf("msx: 8255 read function switch fault.\n");
    return 0xff;
}

void msx_write_8255(u32 address, u8 data)
{
    switch (address & 3) {
    case 0:
	msx_8255_port_a = data;
	msx_sync_mmu();
	break;
	
    case 1:
	deb_printf("msx: 8255 write port '%c' = 0x%02x.\n",
		   'A' + (address & 3), data);
	break;

    case 2:
	msx_8255_port_c = data;
	break;
	
    case 3:
	if (data & 0x80) {
	    deb_printf("msx: 8255 mode set 0x%02x.\n", data);
	} else {
#if 0
	    deb_printf("msx: %s bit %d of port C.\n",
		       &((data & 1) << 1)["reset"], (data >> 1) & 7);
#endif
	    if (data & 1) {
		msx_8255_port_c |= (1 << ((data >> 1) & 7));
	    } else {
		msx_8255_port_c &= ~(1 << ((data >> 1) & 7));
	    }
	}
	break;
    }
}

/*
 * PSG interface
 */

u8 msx_read_psg(u32 address)
{
    /* FIXME: Implement? */
    
    return 0xff;
}

void msx_write_psg(u32 address, u8 data)
{
    /* FIXME: Implement? */
}

/*
 * MSX system definition
 */

ranged_mmu msx_mmu[] = {
    {0x0000, 0x3fff, NULL, 0, 0},
    {0x4000, 0x7fff, NULL, 0, 0},
    {0x8000, 0xbfff, NULL, 0, 0},
    {0xc000, 0xffff, NULL, 0, 0},
};

void msx_sync_mmu(void)
{
    int i;
    u8 slot;
    u8 *data;

    for (i = 0; i < 4; i++) {
	slot = (msx_8255_port_a >> (i << 1)) & 3;

	switch (slot) {
	case 0:
	    data = msx_rombase - ((i & 2) * 0x4000);
	    break;
	    
	case 1:
	    data = msx_memory;
	    break;
	    
	case 2:
	    data = NULL;
	    break;
	    
	case 3:
	    data = NULL;
	    break;

	default:
	    deb_printf("msx: switch failure in msx_sync_mmu();\n");
	    data = NULL;
	    break;
	}

	msx_mmu[i].data = data;
    }
}

void msx_init_mmu(void)
{
    msx_sync_mmu();
}

u8 msx_read(cal_cpu cpu, u32 addr)
{
    int page;

    page = (addr >> 14) & 3;

    if (msx_mmu[page].data) {
	return msx_mmu[page].data[addr];
    } else {
	return 0x00;
    }
}

void msx_write(cal_cpu cpu, u32 addr, u8 data)
{
    int page;

    page = (addr >> 14) & 3;

    if (msx_mmu[page].data == msx_memory) {
	msx_mmu[page].data[addr] = data;
    }
}

memread8_t msx_read_function = msx_read;
memwrite8_t msx_write_function = msx_write;

/*
 * Ports 0x90 and 0x91 appear to be used for the Parallel port.
 */

struct io_read_map msx_io_read_map[] = {
    {0xfe, 0x98, msx_read_tms9918},
    {0xfc, 0xa8, msx_read_8255},
    {0xfc, 0xa0, msx_read_psg},
    {0x00, 0x00, NULL} /* must be last entry */
};

struct io_write_map msx_io_write_map[] = {
    {0xfe, 0x98, msx_write_tms9918},
    {0xfc, 0xa0, msx_write_psg},
    {0xfc, 0xa8, msx_write_8255},
    {0x00, 0x00, NULL} /* must be last entry */
};

void msx_init_io(void)
{
    kb_init();
}

/*
 * common initialization and dispatch code
 */

unsigned char msx_io_read(cal_cpu cpu, u32 address)
{
    struct io_read_map *map;

    for (map = msx_io_read_map; map->mask; map++) {
	if ((address & map->mask) == map->addr) {
	    if (map->handler) {
		return map->handler(address);
	    } else {
		return 0;
	    }
	}
    }

    deb_printf("msx: I/O read 0x%02x.\n", address & 0xff);

    return 0;
}

void msx_io_write(cal_cpu cpu, u32 address, u8 data)
{
    struct io_write_map *map;

    for (map = msx_io_write_map; map->mask; map++) {
	if ((address & map->mask) == map->addr) {
	    if (map->handler) {
		map->handler(address, data);
	    }
	    
	    return;
	}
    }

    deb_printf("msx: I/O write 0x%02x = 0x%02x.\n", address & 0xff, data);
}

void msx_scanline(void)
{
    if (tms9918_periodic(msx_vdp)) {
	msx_cpu->irq(msx_cpu, 0xff);
    }
}

void msx_psg_vsync(void)
{
    sms_psg_vsync(); /* FIXME: Wrong! */
}

struct event msx_events[] = {
    {NULL, 0, 228, msx_scanline},
    {NULL, 0, 228 * 262, msx_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler msx_init_events(void)
{
    event_scheduler scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, msx_cpu);

    event_register(scheduler, &msx_events[0]);
    event_register(scheduler, &msx_events[1]);
    event_register(scheduler, &msx_events[2]);

    return scheduler;
}

void msx_init_cpu(void)
{
    msx_cpu = cal_create(CPUT_MARATZ80);
    msx_cpu->setmmu0(msx_cpu, msx_mmu);
    msx_cpu->setmmu8(msx_cpu, 0, 0, &msx_read_function, &msx_write_function);
    msx_cpu->setiou(msx_cpu, msx_io_read, msx_io_write);
    msx_cpu->reset(msx_cpu);
}

void msx_load_bios(void)
{
    msx_romfile = read_romimage("msx.rom");
    msx_rombase = msx_romfile->data;
}

void msx_run(rom_file romfile)
{
    event_scheduler scheduler;
    
    msx_memory = calloc(1, MSX_RAM_SIZE);
    
    if (!msx_memory) {
	deb_printf("out of memory.\n");
    }

    msx_load_bios();

    msx_init_mmu();

    msx_init_io();
    
    msx_init_cpu();

    sms_psg_init(); /* FIXME: Wrong! */
    
    msx_vdp = tms9918_create();
    
    scheduler = msx_init_events();
    
    set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
}

/*
 * $Log: msx.c,v $
 * Revision 1.2  2001/02/27 04:14:32  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.1  2000/11/25 15:00:26  nyef
 * Initial revision
 *
 */
