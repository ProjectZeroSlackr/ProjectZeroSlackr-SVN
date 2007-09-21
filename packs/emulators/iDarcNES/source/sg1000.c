/*
 * sg1000.c
 *
 * Sega Game 1000 emulation.
 */

/* $Id: sg1000.c,v 1.11 2001/02/27 04:14:41 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "tms9918.h"
#include "sms_psg.h"
#include "event.h"

#define SG1K_RAMSIZE 0x2000
#define SC3K_RAMSIZE 0x8000

/*
 * common variable and structure definitions
 */

unsigned char *sg1k_memory;
cal_cpu sg1k_cpu;
tms9918 sg1k_vdp;

unsigned char *sg1k_rombase;
rom_file sg1k_romfile;
int sg1k_romsize;

struct io_read_map {
    u8 mask; u8 addr;
    u8 (*handler)(u32);
};

struct io_write_map {
    u8 mask; u8 addr;
    void (*handler)(u32, u8);
};

struct io_read_map *sega_io_read_map;
struct io_write_map *sega_io_write_map;

struct sega_system {
    int ram_size;
    ranged_mmu *mmu;
    void (*mmu_init)(void);
    void (*io_init)(void);
    memread8_t memread;
    memwrite8_t memwrite;
    struct io_read_map *io_read_map;
    struct io_write_map *io_write_map;
};

const struct joypad_button_template sg1k_joypad_template_pause = {
    7, {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x80} /* has pause button */
};

const struct joypad_button_template sg1k_joypad_template = {
    6, {0x01, 0x02, 0x04, 0x08, 0x10, 0x20}
};

struct joypad sg1k_joypad_1 = {
    &sg1k_joypad_template_pause, 0
};

struct joypad sg1k_joypad_2 = {
    &sg1k_joypad_template, 0
};

void sega_run_common(rom_file romfile, struct sega_system *system);

/*
 * common I/O handlers
 */

u8 sega_read_tms9918(u32 address)
{
    if (address & 1) {
	return tms9918_readport1(sg1k_vdp);
    } else {
	return tms9918_readport0(sg1k_vdp);
    }
}

u8 sega_read_joypads(u32 address)
{
    if (address & 1) {
	return (sg1k_joypad_2.data >> 2) ^ 0xff;
    } else {
	ui_update_joypad(&sg1k_joypad_1);
	ui_update_joypad(&sg1k_joypad_2);
	return (((sg1k_joypad_1.data) & 0x3f) | (sg1k_joypad_2.data << 6)) ^ 0xff;
    }
}

void sega_write_tms9918(u32 address, u8 data)
{
    if (address & 1) {
	tms9918_writeport1(sg1k_vdp, data);
    } else {
	tms9918_writeport0(sg1k_vdp, data);
    }
}

void sega_write_psg(u32 address, u8 data)
{
    sms_psg_write(data);
}

/*
 * sg1k system definition
 */

ranged_mmu sg1k_mmu[] = {
    {0x0000, 0x0000, NULL, 0, 0},
    {0x0000, 0x9fff, NULL, 0, 0},
    {0xa000, 0xbfff, NULL, 0, 0},
    {0xc000, 0xdfff, NULL, 0, 0},
    {0xe000, 0xffff, NULL, 0, 0},
};

void sg1k_init_mmu(void)
{
    sg1k_mmu[0].data = sg1k_rombase;
    sg1k_mmu[0].end = sg1k_romsize - 1;
    sg1k_mmu[1].start = sg1k_romsize;
    sg1k_mmu[2].data = sg1k_memory - 0xa000;
    sg1k_mmu[3].data = sg1k_memory - 0xc000;
    sg1k_mmu[4].data = sg1k_memory - 0xe000;
}

unsigned char sg1k_read(cal_cpu cpu, unsigned long addr)
{
    if (addr < sg1k_romsize) {
	return sg1k_rombase[addr];
    } else if (addr < 0xa000) {
	return 0x00;
    } else {
	return sg1k_memory[addr & 0x1fff];
    }
}

void sg1k_write(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if (addr >= 0xa000) {
	sg1k_memory[addr & 0x1fff] = data;
    }
}

struct io_read_map sg1k_io_read_map[] = {
    {0xfe, 0xbe, sega_read_tms9918},
    {0xfe, 0xdc, sega_read_joypads},
    {0xfe, 0xc0, sega_read_joypads},
    {0xfe, 0xde, NULL}, /* NOTE: May really want to be part of an 8255. */
    {0x00, 0x00, NULL} /* must be last entry */
};

struct io_write_map sg1k_io_write_map[] = {
    {0xfe, 0xbe, sega_write_tms9918},
    {0xfe, 0x7e, sega_write_psg},
    {0xfe, 0xde, NULL}, /* NOTE: May really want to be part of an 8255. */
    {0x00, 0x00, NULL} /* must be last entry */
};

struct sega_system sg1k_system = {
    SG1K_RAMSIZE,
    sg1k_mmu, sg1k_init_mmu,
    NULL,
    sg1k_read, sg1k_write,
    sg1k_io_read_map, sg1k_io_write_map,
};

void sg1k_run(rom_file romfile)
{
    sega_run_common(romfile, &sg1k_system);
}

/*
 * sc3k I/O handlers
 */

u16 sc3k_keyboard_data[7];

/*
 * The sc3k keyboard is a scan-polled key array, configured as 7 rows (the
 * 8th row is used to read the controllers, which would explain the constant
 * writes to 0xde and 0xdf in sg1000 games) of 12 bits (at least, it _looks_
 * like 12 bits) on an 8255 PPI chip. The key encoding is approximately:
 *
 *    +---+---+---+---+---+---+---+
 *    | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * +--+---+---+---+---+---+---+---+
 * | 0| 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * +--+---+---+---+---+---+---+---+
 * | 1| Q | W | E | R | T | Y | U |
 * +--+---+---+---+---+---+---+---+
 * | 2| A | S | D | F | G | H | J |
 * +--+---+---+---+---+---+---+---+
 * | 3| Z | X | C | V | B | N | M |
 * +--+---+---+---+---+---+---+---+
 * | 4|???|???|???|???|   |   |   |
 * +--+---+---+---+---+---+---+---+
 * | 5| , | . | / |   |???|???|???|
 * +--+---+---+---+---+---+---+---+
 * | 6| K | L | ; | : | ] | CR|???|
 * +--+---+---+---+---+---+---+---+
 * | 7| I | O | P | @ | [ |   |   |
 * +--+---+---+---+---+---+---+---+
 * | 8| 8 | 9 | 0 | - | ^ |Yen|???|
 * +--+---+---+---+---+---+---+---+
 * | 9|   |   |   |   |   |   |???|
 * +--+---+---+---+---+---+---+---+
 * |10|   |   |   |   |   |   |   |
 * +--+---+---+---+---+---+---+---+
 * |11|   |   |   |   |   |   |   |
 * +--+---+---+---+---+---+---+---+
 *
 * Where ??? indicates an unknown location with an obvious effect, and blank
 * spaces indicate no obvious effect (at least some of which are probably
 * bucky bits). 14 is some sort of kana lock, and there's a graphics lock as
 * well as cursor controls and a clear screen key.
 *
 * FIXME: Having the scan conversion data in a comment, and again in the array
 * below (in a completely unintelligable format, no less) is just asking for
 * trouble. Perhaps building sc3k_keyboard_map[] at runtime from a 12x7 array
 * of keycodes (pulled in via #defines in a "keyboard.h", most likely)?
 *
 * FIXME: Still need to find where the bucky bits are mapped, and determine
 * exactly which ??? key does what. Also need to map them in the array below.
 */

u8 sc3k_keyboard_map[0x80] = {
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, /* 0x00 - 0x07 */
    0x18, 0x28, 0x38, 0x00, 0x00, 0x00, 0x00, 0x11, /* 0x08 - 0x0f */

    0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x17, 0x27, /* 0x10 - 0x17 */
    0x37, 0x00, 0x66, 0x00, 0x00, 0x12, 0x22, 0x32, /* 0x18 - 0x1f */

    0x42, 0x52, 0x62, 0x72, 0x16, 0x26, 0x36, 0x00, /* 0x20 - 0x27 */
    0x00, 0x00, 0x00, 0x13, 0x23, 0x33, 0x43, 0x53, /* 0x28 - 0x2f */

    0x63, 0x73, 0x15, 0x25, 0x35, 0x00, 0x00, 0x00, /* 0x30 - 0x37 */
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

void sc3k_keyboard_event(u8 code)
{
    u8 keyposition;
    u16 mask;
    int row;

    keyposition = sc3k_keyboard_map[code & 0x7f];

    if (!keyposition) return;

    row = (keyposition >> 4) - 1;
    mask = 1 << (keyposition & 0x0f);
    
    if (code & 0x80) { /* keyup */
	sc3k_keyboard_data[row] &= ~mask;
    } else { /* keydown */
	sc3k_keyboard_data[row] |= mask;
    }
}

/*
 * FIXME: This 8255 is a hackjob. We will need another one soon, so fix it then
 */

u8 sc3k_8255_port_c;

u8 sc3k_read_8255(u32 address)
{
    switch (address & 3) {
    case 0: case 1:
	if ((sc3k_8255_port_c & 7) == 7) {
	    return sega_read_joypads(address);
	}

	if (address & 1) { /* port B */
	    return ((~sc3k_keyboard_data[(sc3k_8255_port_c & 7)]) >> 8) & 0x0f;
	} else { /* port A */
	    return ~sc3k_keyboard_data[(sc3k_8255_port_c & 7)];
	}
	return 0xff;

    case 2:
	return sc3k_8255_port_c;

    case 3:
	deb_printf("sc3k: 8255 read control (INVALID), returning 0xff.\n");
	return 0xff;
    }

    /* NOTREACHED */
    deb_printf("sc3k: 8255 read function switch fault.\n");
    return 0xff;
}

void sc3k_write_8255(u32 address, u8 data)
{
    switch (address & 3) {
    case 0: case 1:
	deb_printf("sc3k: 8255 write port '%c' = 0x%02x.\n",
		   'A' + (address & 3), data);
	break;

    case 2:
	sc3k_8255_port_c = data;
	break;
	
    case 3:
	if (data & 0x80) {
	    deb_printf("sc3k: 8255 mode set 0x%02x.\n", data);
	} else {
#if 0
	    deb_printf("sc3k: %s bit %d of port C.\n",
		       &((data & 1) << 1)["reset"], (data >> 1) & 7);
#endif
	    if (data & 1) {
		sc3k_8255_port_c |= (1 << ((data >> 1) & 7));
	    } else {
		sc3k_8255_port_c &= ~(1 << ((data >> 1) & 7));
	    }
	}
	break;
    }
}

/*
 * sc3k system definition
 */

ranged_mmu sc3k_mmu[] = {
    {0x0000, 0x0000, NULL, 0, 0},
    {0x0000, 0x7fff, NULL, 0, 0},
    {0x8000, 0xffff, NULL, 0, 0},
};

void sc3k_init_mmu(void)
{
    sc3k_mmu[0].data = sg1k_rombase;
    sc3k_mmu[0].end = sg1k_romsize - 1;
    sc3k_mmu[1].start = sg1k_romsize;
    sc3k_mmu[2].data = sg1k_memory - 0x8000;
}

unsigned char sc3k_read(cal_cpu cpu, unsigned long addr)
{
    if (addr < sg1k_romsize) {
	return sg1k_rombase[addr];
    } else if (addr < 0x8000) {
	return 0x00;
    } else {
	return sg1k_memory[addr & 0x7fff];
    }
}

void sc3k_write(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if (addr >= 0x8000) {
	sg1k_memory[addr & 0x7fff] = data;
    }
}

struct io_read_map sc3k_io_read_map[] = {
    {0xfe, 0xbe, sega_read_tms9918},
    {0xfc, 0xdc, sc3k_read_8255},
    {0x00, 0x00, NULL} /* must be last entry */
};

struct io_write_map sc3k_io_write_map[] = {
    {0xfe, 0xbe, sega_write_tms9918},
    {0xfe, 0x7e, sega_write_psg},
    {0xfc, 0xdc, sc3k_write_8255},
    {0x00, 0x00, NULL} /* must be last entry */
};

void sc3k_init_io(void)
{
    kb_init();
}

struct sega_system sc3k_system = {
    SC3K_RAMSIZE,
    sc3k_mmu, sc3k_init_mmu,
    sc3k_init_io,
    sc3k_read, sc3k_write,
    sc3k_io_read_map, sc3k_io_write_map,
};

void sc3k_run(rom_file romfile)
{
    sega_run_common(romfile, &sc3k_system);
}

/*
 * common initialization and dispatch code
 */

unsigned char sega_io_read(cal_cpu cpu, u32 address)
{
    struct io_read_map *map;

    for (map = sega_io_read_map; map->mask; map++) {
	if ((address & map->mask) == map->addr) {
	    if (map->handler) {
		return map->handler(address);
	    } else {
		return 0;
	    }
	}
    }

    deb_printf("sega: I/O read 0x%02x.\n", address & 0xff);

    return 0;
}

void sega_io_write(cal_cpu cpu, u32 address, u8 data)
{
    struct io_write_map *map;

    for (map = sega_io_write_map; map->mask; map++) {
	if ((address & map->mask) == map->addr) {
	    if (map->handler) {
		map->handler(address, data);
	    }
	    
	    return;
	}
    }

    deb_printf("sega: I/O write 0x%02x = 0x%02x.\n", address & 0xff, data);
}

void sg1k_scanline(void)
{
    if (tms9918_periodic(sg1k_vdp)) {
	sg1k_cpu->irq(sg1k_cpu, 0xff);
    }
}

void sg1k_check_pause(void)
{
    /* NOTE: the use of the otherwise unused 0x40 bit is a hack */
    ui_update_joypad(&sg1k_joypad_1);
    if (sg1k_joypad_1.data & 0x80) {
	if (!(sg1k_joypad_1.data & 0x40)) {
	    sg1k_cpu->nmi(sg1k_cpu);
	}
	sg1k_joypad_1.data |= 0x40;
    } else {
	sg1k_joypad_1.data &= ~0x40;
    }
}

struct event sg1k_events[] = {
    {NULL, 0, 228, sg1k_scanline},
    {NULL, 0, 228 * 200, sg1k_check_pause},
    {NULL, 0, 228 * 262, sms_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler sg1k_init_events(void)
{
    event_scheduler scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, sg1k_cpu);

    event_register(scheduler, &sg1k_events[0]);
    event_register(scheduler, &sg1k_events[1]);
    event_register(scheduler, &sg1k_events[2]);
    event_register(scheduler, &sg1k_events[3]);

    return scheduler;
}

void sg1k_init_cpu(struct sega_system *system)
{
    sg1k_cpu = cal_create(CPUT_MARATZ80);
    sg1k_cpu->setmmu0(sg1k_cpu, system->mmu);
    sg1k_cpu->setmmu8(sg1k_cpu, 0, 0, &system->memread, &system->memwrite);
    sg1k_cpu->setiou(sg1k_cpu, sega_io_read, sega_io_write);
    sg1k_cpu->reset(sg1k_cpu);
}

void sega_init_joypads(void)
{
    if (ui_register_joypad(&sg1k_joypad_1)) {
	deb_printf("sg1k_run: joypad 1... registered.\n");
    } else {
	deb_printf("sg1k_run: joypad 1... not registered.\n");
    }
    
    if (ui_register_joypad(&sg1k_joypad_2)) {
	deb_printf("sg1k_run: joypad 2... registered.\n");
    } else {
	deb_printf("sg1k_run: joypad 2... not registered.\n");
    }
}

void sega_run_common(rom_file romfile, struct sega_system *system)
{
    event_scheduler scheduler;
    
    sg1k_memory = calloc(1, system->ram_size);
    
    if (!sg1k_memory) {
	deb_printf("out of memory.\n");
    }

    sg1k_rombase = romfile->data;
    sg1k_romsize = romfile->size;

    sega_io_read_map = system->io_read_map;
    sega_io_write_map = system->io_write_map;
    
    system->mmu_init();
    
    sg1k_init_cpu(system);
    
    sg1k_vdp = tms9918_create();
    
    sms_psg_init();

    sega_init_joypads();

    if (system->io_init) {
	system->io_init();
    }

    dn_shutdown = sms_psg_done; /* NOTE: cheating */
    
    scheduler = sg1k_init_events();
    
    set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
}

/*
 * $Log: sg1000.c,v $
 * Revision 1.11  2001/02/27 04:14:41  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.10  2000/11/24 18:59:35  nyef
 * fixed stupid mistake with the keyboard map
 *
 * Revision 1.9  2000/11/24 18:48:03  nyef
 * added mostly working sc3k keyboard support
 *
 * Revision 1.8  2000/11/23 16:30:16  nyef
 * added preliminary support for the sc3000
 * changed the design for easier support of different 8-bit sega systems
 *
 * Revision 1.7  2000/08/26 00:58:31  nyef
 * converted to use the periodic event scheduler
 * added a call to sms_psg_vsync() once per frame
 *
 * Revision 1.6  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.5  1999/12/05 19:47:41  nyef
 * fixed to shutdown the PSG when exiting
 *
 * Revision 1.4  1999/12/04 01:03:52  nyef
 * cleaned up sg1k_run() and friends
 *
 * Revision 1.3  1999/11/28 16:59:52  nyef
 * cleared out some vestigial code (old CPU interface)
 *
 * Revision 1.2  1999/11/27 20:45:01  nyef
 * disabled debug output on reads from I/O ports 0xde and 0xdf
 *
 * Revision 1.1  1999/11/27 20:07:56  nyef
 * Initial revision
 *
 */
