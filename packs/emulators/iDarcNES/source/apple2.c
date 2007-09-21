/*
 * apple2.c
 *
 * driver for Apple ][ computers
 */

/* $Id: apple2.c,v 1.18 2001/03/05 00:04:52 nyef Exp $ */

#include <stdlib.h>

#include "ui.h"
#include "cal.h"
#include "types.h"
#include "tool.h"
#include "apple2_vdp.h"
#include "apple2_cards.h"
#include "event.h"
#include "menu.h"
#include "cpu6502.h"

/* manifest constants */

#define APPLE2_MEMORY_SIZE 0xc000

#define APPLE2_NUM_ROMS 6
#define ROM_D0 0
#define ROM_D8 1
#define ROM_E0 2
#define ROM_E8 3
#define ROM_F0 4
#define ROM_F8 5


/* emulated system data structures */

struct apple2_mainboard {
    struct cpu6502_context *cpu;
    apple2_vdp vdp;
    event_scheduler scheduler;
    u8 *memory;

    u8 *roms[APPLE2_NUM_ROMS];

    int langcard_writable;
    int langcard_readable;
    u8 *langcard_low_bank;
    u8 *langcard_high_bank;
    u8 *langcard_memory;

    struct apple2_card *cards[8];

    u8 keyboard_latch;

    int need_reset;
};

struct apple2_mainboard apple2_true;
struct apple2_mainboard *apple2 = &apple2_true;


/* keyboard I/O */

void apple2_press_keyboard(struct apple2_mainboard *apple2, u8 key)
{
    apple2->keyboard_latch = key | 0x80;
}

u8 apple2_read_keyboard(struct apple2_mainboard *apple2)
{
    return apple2->keyboard_latch;
}

void apple2_clear_keyboard_strobe(struct apple2_mainboard *apple2)
{
    apple2->keyboard_latch &= 0x7f;
}


/* main I/O dispatch */

u8 apple2_read_io(u32 addr)
{
/*      deb_printf("io read 0x%04x.\n", addr); */
    if (addr < 0xc080) { /* Built-In I/O */
	if (addr < 0xc010) { /* keyboard */
	    return apple2_read_keyboard(apple2);
	} else if (addr < 0xc020) { /* keyboard strobe */
	    apple2_clear_keyboard_strobe(apple2);
	    return 0x00; /* FIXME: real return value? */
	} else if (addr < 0xc050) { /* unimplemented */
	    return 0x00;
	} else if (addr < 0xc058) { /* Video */
	    a2v_io(apple2->vdp, addr);
	    return 0x00;
	}
    } else if (addr < 0xc100) { /* Card I/O */
	u8 card;

	card = (addr >> 4) & 0x07;

	if (apple2->cards[card] && apple2->cards[card]->read) {
	    return apple2->cards[card]->read(apple2->cards[card], addr);
	}
    } else { /* Card PROM */
	u8 card;

	card = (addr >> 8) & 0x07;

	if (apple2->cards[card] && apple2->cards[card]->rom) {
	    return apple2->cards[card]->rom[addr & 0xff];
	}
    }
    return 0x00;
}

void apple2_write_io(u32 addr, u8 data)
{
/*      deb_printf("io write 0x%04x, 0x%02x.\n", addr, data); */
    if (addr < 0xc080) { /* Built-In I/O */
	if (addr < 0xc010) { /* keyboard, read-only (ignore) */
	} else if (addr < 0xc020) { /* keyboard strobe */
	    apple2_clear_keyboard_strobe(apple2);
	} else if (addr < 0xc050) { /* unimplemented */
	    return;
	} else if (addr < 0xc058) { /* Video */
	    a2v_io(apple2->vdp, addr);
	    return;
	}
    } else if (addr < 0xc100) { /* Card I/O */
	u8 card;

	card = (addr >> 4) & 0x07;

	if (apple2->cards[card] && apple2->cards[card]->write) {
	    apple2->cards[card]->write(apple2->cards[card], addr, data);
	}
    }
}


/* memory access emulation */

u8 apple2_read_ram(cal_cpu cpu, u32 addr)
{
    return apple2->memory[addr];
}

void apple2_write_ram(cal_cpu cpu, u32 addr, u8 data)
{
    apple2->memory[addr] = data;
}

u8 apple2_read_rom(cal_cpu cpu, u32 addr)
{
    int rom_bank;
    
    if (addr < 0xd000) {
	return apple2_read_io(addr);
    } else if (apple2->langcard_readable) {
	if (addr < 0xe000) {
	    return apple2->langcard_low_bank[addr & 0x0fff];
	} else {
	    return apple2->langcard_high_bank[addr & 0x1fff];
	}
    } else {
	rom_bank = (addr - 0xd000) >> 11;

	if (!apple2->roms[rom_bank]) {
	    return 0x00;
	}
	
	return apple2->roms[rom_bank][addr & 0x7ff];
    }
}

void apple2_write_rom(cal_cpu cpu, u32 addr, u8 data)
{
    if (addr < 0xd000) {
	apple2_write_io(addr, data);
    } else if (apple2->langcard_writable) {
	if (addr < 0xe000) {
	    apple2->langcard_low_bank[addr & 0x0fff] = data;
	} else {
	    apple2->langcard_high_bank[addr & 0x1fff] = data;
	}
    }
}

memread8_t apple2_readtable[4] = {
    apple2_read_ram,
    apple2_read_ram,
    apple2_read_ram,
    apple2_read_rom,
};

memwrite8_t apple2_writetable[4] = {
    apple2_write_ram,
    apple2_write_ram,
    apple2_write_ram,
    apple2_write_rom,
};

u8 apple2_read_hack(cal_cpu cpu, u32 addr)
{
    return apple2_readtable[(addr >> 14) & 3](cpu, addr);
}

void apple2_write_hack(cal_cpu cpu, u32 addr, u8 data)
{
    apple2_writetable[(addr >> 14) & 3](cpu, addr, data);
}

memread8_t apple2_readtable_hack[1] = { apple2_read_hack };
memwrite8_t apple2_writetable_hack[1] = { apple2_write_hack };

/* Language card emulation */

/*
 * From a document found on what looks to be the "ground" archive:
 *
 * For WR-enable LC RAM, the softswitch REQUIRES two successive reads
 *
 *                $C080       ;RD LC RAM bank2, WR-protect LC RAM
 * ROMIN      =   $C081       ;RD ROM, WR-enable LC RAM
 *                $C082       ;RD ROM, WR-protect LC RAM
 * LCBANK2    =   $C083       ;RD LC RAM bank2, WR-enable LC RAM
 *                $C088       ;RD LC RAM bank1, WR-protect LC RAM
 *                $C089       ;RD ROM, WR-enable LC RAM
 *                $C08A       ;RD ROM, WR-protect LC RAM
 * LCBANK1    =   $C08B       ;RD LC RAM bank1, WR-enable LC RAM
 *
 *  $C084-$C087 are echoes of $C080-$C083
 *  $C08C-$C08F are echoes of $C088-$C08B
 *  Bank 1 and Bank 2 here are the 4K banks at $D000-$DFFF. The
 *  remaining area from $E000-$FFFF is the same for both
 *  sets of switches.
 */

void a2_lang_line_toggle(int addr)
{
    apple2->langcard_writable = addr & 1;

    if (((addr & 3) == 0) || ((addr & 3) == 3)) {
	apple2->langcard_readable = 1;
    } else {
	apple2->langcard_readable = 0;
    }
    
    if (addr & 8) {
	apple2->langcard_low_bank = apple2->langcard_memory;
    } else {
	apple2->langcard_low_bank = apple2->langcard_memory + 0x1000;
    }
}

u8 a2_lang_read(struct apple2_card *card, u16 addr)
{
    a2_lang_line_toggle(addr);

    return 0x00;
}

void a2_lang_write(struct apple2_card *card, u16 addr, u8 data)
{
    a2_lang_line_toggle(addr);
}

struct apple2_card *apple2_lang_init(struct apple2_mainboard *apple2)
{
    struct apple2_card *retval;

    retval = malloc(sizeof(*retval));

    if (!retval) {
	return NULL;
    }

    retval->read = a2_lang_read;
    retval->write = a2_lang_write;
    retval->rom = NULL;

    apple2->langcard_memory = malloc(0x4000);
    if (!apple2->langcard_memory) {
	free(retval);
	return NULL;
    }

    apple2->langcard_high_bank = apple2->langcard_memory + 0x2000;

    return retval;
}


/* periodic events */

void apple2_scanline(void)
{
    if (apple2->need_reset) {
	apple2->need_reset = 0;
	cpu6502_reset(apple2->cpu);
    }
    
    a2v_periodic(apple2->vdp);
}

struct event apple2_events[] = {
    {NULL, 0, 65, apple2_scanline},
    {NULL, 0, 65 * 262, NULL},
};

void apple2_init_events(void)
{
    int i;

    for (i = 0; i < (sizeof(apple2_events) / sizeof(struct event)); i++) {
	event_register(apple2->scheduler, &apple2_events[i]);
    }
}

void apple2_register_event(struct apple2_mainboard *apple2, struct event *the_event)
{
    event_register(apple2->scheduler, the_event);
}


/* user interface hooks */

void apple2_menu_reset(void)
{
    apple2->need_reset = 1;
}

struct ui_menu apple2_menu[] = {
    {"Apple ][", MF_NONE,  NULL}, /* menu title */
    {"Reset",    MF_NONE,  (ui_menu_callback)apple2_menu_reset},
    {"S6D1",     MF_CHILD}, /* filled in later */
    {"S6D2",     MF_CHILD}, /* filled in later */
    {NULL, 0, NULL}, /* must be last entry */
};

void apple2_init_menus(void)
{
    menu_init(apple2_menu);
}

void apple2_set_child_menu(struct ui_menu *menu, int id)
{
    apple2_menu[id + 2].child_menu = menu;
}


/* system setup */

void apple2_load_rom(char *filename, int rom)
{
    rom_file romfile;
    
    deb_printf("attempting to load %02X ROM.\n", 0xd0 + (rom << 3));
    
    romfile = read_romimage(filename);

    if (!romfile) {
	deb_printf("could not open %02X ROM.\n", 0xd0 + (rom << 3));
	deb_printf("things could get ugly.\n");
	apple2->roms[rom] = NULL;
	return;
    }

    apple2->roms[rom] = romfile->data;

    /* FIXME: memory leak */
}

void apple2_load_bios(void)
{
    apple2->roms[ROM_D0] = NULL;
    apple2->roms[ROM_D8] = NULL;
    apple2->roms[ROM_E0] = NULL;
    apple2->roms[ROM_E8] = NULL;
    apple2->roms[ROM_F0] = NULL;
    apple2->roms[ROM_F8] = NULL;

#if 0 /* integer basic rom set */
    apple2_load_rom("programmers_aide_d0.rom", ROM_D0);

    apple2_load_rom("intbasic_e0.rom", ROM_E0);
    apple2_load_rom("intbasic_e8.rom", ROM_E8);
    apple2_load_rom("intbasic_f0.rom", ROM_F0);
#endif
    
#if 1 /* applesoft basic rom set */
    apple2_load_rom("fpbasic_d0.rom", ROM_D0);
    apple2_load_rom("fpbasic_d8.rom", ROM_D8);
    apple2_load_rom("fpbasic_e0.rom", ROM_E0);
    apple2_load_rom("fpbasic_e8.rom", ROM_E8);
    apple2_load_rom("fpbasic_f0.rom", ROM_F0);
#endif
    
/*      apple2_load_rom("monitor_f8.rom", ROM_F8); */
    apple2_load_rom("autostart_f8.rom", ROM_F8);
}

void apple2_init_cards(void)
{
    apple2->cards[0] = NULL;
    apple2->cards[1] = NULL;
    apple2->cards[2] = NULL;
    apple2->cards[3] = NULL;
    apple2->cards[4] = NULL;
    apple2->cards[5] = NULL;
    apple2->cards[6] = NULL;
    apple2->cards[7] = NULL;

    apple2->cards[0] = apple2_lang_init(apple2);
    apple2->cards[6] = apple2_disk_init(apple2);
}

void apple2_init_cpu(void)
{
    struct cpu6502_context *cpu;

    cpu = cpu6502_create();
    apple2->cpu = cpu;
    cpu6502_setzpage(cpu, apple2->memory);
    cpu6502_setmemhandlers(cpu, apple2_read_hack, apple2_write_hack);
    cpu6502_reset(cpu);
}

void apple2_init_keyboard(void)
{
    extern void kb_init(void);

    kb_init();
}

void apple2_run(rom_file romfile)
{
    if (romfile) {
	deb_printf("apple2_run: ignoring romfile.\n");
    }

    apple2->memory = malloc(APPLE2_MEMORY_SIZE);

    if (!apple2->memory) {
	deb_printf("unable to allocate system memory.\n");
	return;
    }

    apple2->langcard_readable = 0;
    apple2->langcard_writable = 0;

    apple2->need_reset = 0;
    
    apple2_load_bios();

    apple2_init_cards();
    
    apple2_init_cpu();

    apple2_init_keyboard();
    
    apple2->vdp = a2v_init(apple2->memory); /* FIXME: return value? */

    apple2->scheduler = new_event_scheduler(cpu6502_event_delay_callback, apple2->cpu);

    apple2_init_events();

    apple2_init_menus();
    
    set_timeslice((void (*)(void *)) &event_timeslice, apple2->scheduler);
}

/*
 * $Log: apple2.c,v $
 * Revision 1.18  2001/03/05 00:04:52  nyef
 * converted to use the new cpu6502 interface instead of the cal interface
 *
 * Revision 1.17  2001/02/27 04:14:53  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.16  2000/11/29 05:34:19  nyef
 * changed to use the cpu6502 core
 *
 * Revision 1.15  2000/07/01 15:44:09  nyef
 * added preliminary per-card menu hook
 *
 * Revision 1.14  2000/06/29 01:24:50  nyef
 * renamed the menu functions more appropriately
 *
 * Revision 1.13  2000/06/29 01:05:56  nyef
 * moved menu interface out from ui.h to menu.h
 *
 * Revision 1.12  2000/06/25 17:20:43  nyef
 * added a menu with a reset item
 *
 * Revision 1.11  2000/03/13 02:18:50  nyef
 * removed some annoying debug output for the language card
 *
 * Revision 1.10  2000/03/12 21:46:30  nyef
 * added preliminary language card support
 *
 * Revision 1.9  2000/03/11 01:13:34  nyef
 * moved card interface structure out to apple2_cards.h
 * added a function to register a periodic event for a card
 * fixed card I/O decoding
 *
 * Revision 1.8  2000/03/10 03:34:13  nyef
 * started adding support for peripheral cards
 *
 * Revision 1.7  2000/03/10 03:08:21  nyef
 * converted to use the periodic event scheduler
 *
 * Revision 1.6  2000/03/08 03:17:32  nyef
 * added code to load the BASIC romsets
 * switched to using the autostart ROM
 *
 * Revision 1.5  2000/03/06 00:39:11  nyef
 * added code to initialize keyboard
 *
 * Revision 1.4  2000/03/02 03:10:40  nyef
 * added support for loading roms other than the F8 rom
 *
 * Revision 1.3  2000/03/02 02:53:28  nyef
 * added keyboard I/O emulation
 * cleaned up some random stuff
 *
 * Revision 1.2  2000/01/01 20:54:23  nyef
 * fixed to use the correct number of cycles per line
 *
 * Revision 1.1  2000/01/01 04:12:14  nyef
 * Initial revision
 *
 */
