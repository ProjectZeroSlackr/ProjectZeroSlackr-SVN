/*
 * coleco.c
 *
 * ColecoVision emulation.
 */

/* $Id: coleco.c,v 1.15 2001/02/27 04:14:50 nyef Exp $ */

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

#define CV_RAMSIZE 0x400

unsigned char *cv_memory;
unsigned char *cv_rom;
unsigned char *cv_game_rom;
cal_cpu cv_cpu;
tms9918 cv_vdp;

int cv_last_nmi_state;

unsigned char *cv_rombase;

const struct joypad_button_template cv_joypad_template = {
    6, {0x01, 0x04, 0x08, 0x02, 0x10, 0x20}
};

struct joypad cv_joypad_1 = {
    &cv_joypad_template, 0
};

struct keypad cv_keypad_1;

int cv_joypad_state;

ranged_mmu cv_mmu[] = {
    {0x0000, 0x1fff, NULL, 0, 0},
    {0x2000, 0x5fff, NULL, 0, 0},
    {0x6000, 0x63ff, NULL, 0, 0},
    {0x6400, 0x67ff, NULL, 0, 0},
    {0x6800, 0x6bff, NULL, 0, 0},
    {0x6c00, 0x6fff, NULL, 0, 0},
    {0x7000, 0x73ff, NULL, 0, 0},
    {0x7400, 0x77ff, NULL, 0, 0},
    {0x7800, 0x7bff, NULL, 0, 0},
    {0x7c00, 0x7fff, NULL, 0, 0},
    {0x8000, 0xffff, NULL, 0, 0},
};

void cv_init_mmu(void)
{
    cv_mmu[0].data = cv_rom;
    cv_mmu[2].data = cv_memory - 0x6000;
    cv_mmu[3].data = cv_memory - 0x6400;
    cv_mmu[4].data = cv_memory - 0x6800;
    cv_mmu[5].data = cv_memory - 0x6c00;
    cv_mmu[6].data = cv_memory - 0x7000;
    cv_mmu[7].data = cv_memory - 0x7400;
    cv_mmu[8].data = cv_memory - 0x7800;
    cv_mmu[9].data = cv_memory - 0x7c00;
    cv_mmu[10].data = cv_game_rom - 0x8000;
}

unsigned char cv_read(cal_cpu cpu, unsigned long addr)
{
    if (addr < 0x2000) {
	return cv_rom[addr];
    } else if (addr < 0x6000) {
	deb_printf("cv_read: read unassigned address 0x%04x.\n", addr);
	return 0;
    } else if (addr < 0x8000) {
	return cv_memory[addr & 0x3ff];
    } else {
	return cv_game_rom[addr & 0x7fff];
    }
}

void cv_write(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if ((addr & 0xe000) == 0x6000) {
	cv_memory[addr & 0x3ff] = data;
    }
}

memread8_t cv_readfunc = &cv_read;
memwrite8_t cv_writefunc = &cv_write;

u8 cv_keypad_codes[12] = {
    0x5, 0x2, 0x8, 0x3, 0xd, 0xc, 0x1, 0xa, 0xe, 0x4, 0x6, 0x9,
};

u8 cv_keypad_read(void)
{
    int i;

    for (i = 0; i < 12; i++) {
	if ((cv_keypad_1.data) & (1 << i)) {
	    return cv_keypad_codes[i];
	}
    }
    
    return 0x00;
}

unsigned char cv_io_read(cal_cpu cpu, unsigned long address)
{
    if ((address & 0xe0) == 0xa0) {
	if (address & 1) {
	    return tms9918_readport1(cv_vdp);
	} else {
	    return tms9918_readport0(cv_vdp);
	}
    } else if ((address & 0xe0) == 0xe0) {
	if (address & 0x02) {
	    /* FIXME: controller 2? */
	    return 0xff;
	} else {
	    ui_update_joypad(&cv_joypad_1);
	    if (cv_joypad_state) {
		/* joystick mode */
		return ((cv_joypad_1.data & 0x0f) | ((cv_joypad_1.data & 0x10)? 0x40: 0)) ^ 0xff;
	    } else {
		/* keypad mode */
		return (cv_keypad_read() | ((cv_joypad_1.data & 0x20)? 0x40: 0)) ^ 0xff;
	    }
	}
    } else {
	deb_printf("cv_io_read: 0x%02x.\n", address & 0xff);
    }

    return 0;
}

void cv_io_write(cal_cpu cpu, unsigned long address, unsigned char data)
{
    if ((address & 0xe0) == 0x80) {
	/* set controls to keypad mode */
	cv_joypad_state = 0;
    } else if ((address & 0xe0) == 0xa0) {
	if (address & 1) {
	    tms9918_writeport1(cv_vdp, data);
	} else {
	    tms9918_writeport0(cv_vdp, data);
	}
    } else if ((address & 0xe0) == 0xc0) {
	/* set controls to joystick mode */
	cv_joypad_state = 1;
    } else if ((address & 0xe0) == 0xe0) {
	sms_psg_write(data);
    } else {
	deb_printf("cv_io_write: 0x%02x = 0x%02x.\n", address & 0xff, data);
    }
}

void cv_scanline(void)
{
    if (tms9918_periodic(cv_vdp)) {
	if (!cv_last_nmi_state) {
	    cv_cpu->nmi(cv_cpu);
	    cv_last_nmi_state = 1;
	}
    } else {
	cv_last_nmi_state = 0;
    }
}

struct event cv_events[] = {
    {NULL, 0, 228, cv_scanline},
    {NULL, 0, 228 * 262, sms_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler cv_init_events(void)
{
    event_scheduler scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, cv_cpu);

    event_register(scheduler, &cv_events[0]);
    event_register(scheduler, &cv_events[1]);
    event_register(scheduler, &cv_events[2]);

    return scheduler;
}

void cv_load_bios(void)
{
    rom_file romfile;
    
    deb_printf("attempting to load bios file.\n");
    
    romfile = read_romimage("coleco.rom");

    if (!romfile) {
	deb_printf("could not open bios file.\n");
	deb_printf("things are about to get ugly.\n");
	return;
    }
    
    cv_rom = romfile->data;

    /* FIXME: memory leak */
}

void cv_init_controllers(void)
{
    if (ui_register_joypad(&cv_joypad_1)) {
	deb_printf("cv_run: joypad 1... registered.\n");
    } else {
	deb_printf("cv_run: joypad 1... not registered.\n");
    }

    if (keypad_register(&cv_keypad_1)) {
	deb_printf("cv_run: keypad 1... registered.\n");
    } else {
	deb_printf("cv_run: keypad 1... not registered.\n");
    }
}

void cv_init_cpu(void)
{
    cv_cpu = cal_create(CPUT_MARATZ80);
    cv_cpu->setmmu0(cv_cpu, cv_mmu);
    cv_cpu->setmmu8(cv_cpu, 0, 0, &cv_readfunc, &cv_writefunc);
    cv_cpu->setiou(cv_cpu, cv_io_read, cv_io_write);
    cv_cpu->reset(cv_cpu);

    cv_last_nmi_state = 0;
}

void cv_shutdown(void)
{
    sms_psg_done();
}

void cv_run(rom_file romfile)
{
    event_scheduler scheduler;

    cv_memory = calloc(1, CV_RAMSIZE);

    if (!cv_memory) {
	deb_printf("out of memory.\n");
    }

    cv_game_rom = romfile->data;
    
    cv_load_bios();

    cv_init_mmu();
    
    cv_init_cpu();
    
    cv_vdp = tms9918_create();
    
    dn_shutdown = cv_shutdown;
    
    sms_psg_init();

    cv_init_controllers();
    
    scheduler = cv_init_events();
    
    set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
}

/*
 * $Log: coleco.c,v $
 * Revision 1.15  2001/02/27 04:14:50  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.14  2000/08/26 00:58:00  nyef
 * converted to use the periodic event scheduler
 * added a call to sms_psg_vsync() once per frame
 *
 * Revision 1.13  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.12  2000/01/12 01:15:58  nyef
 * added some code to make NMI edge triggered (should be in the CPU core,
 * but it would require changing the interface, and I'm not ready to do
 * that yet)
 *
 * Revision 1.11  2000/01/02 03:28:34  nyef
 * fixed joypad directional controls
 * fixed problem with memory allocation
 *
 * Revision 1.10  2000/01/01 03:23:04  nyef
 * added preliminary keypad support
 *
 * Revision 1.9  1999/12/06 02:07:10  nyef
 * changed to use the read_romimage() interface to load the BIOS file
 *
 * Revision 1.8  1999/12/05 20:51:45  nyef
 * added ranged mmu support
 *
 * Revision 1.7  1999/12/05 20:44:23  nyef
 * cleaned up the startup process a bit
 * fixed a bug with the BIOS loader reading twice as much as nessecary
 *
 * Revision 1.6  1999/11/27 19:13:41  nyef
 * changed to not use procpointers to access the VDP
 *
 * Revision 1.5  1999/11/26 20:07:41  nyef
 * moved sound quality control out to the UI layer
 *
 * Revision 1.4  1999/11/20 05:27:56  nyef
 * hacked to work with new rom loading interface
 *
 * Revision 1.3  1999/07/01 01:26:34  nyef
 * added code to shutdown the sound system
 *
 * Revision 1.2  1999/06/12 00:18:50  nyef
 * added preliminary joypad support
 *
 * Revision 1.1  1999/06/08 01:47:26  nyef
 * Initial revision
 *
 */
