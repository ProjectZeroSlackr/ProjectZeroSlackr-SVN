/*
 * sms.c
 *
 * Sega Master System emulation.
 */

/* $Id: sms.c,v 1.45 2001/02/27 04:14:37 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "sms9918.h"
#include "sms_psg.h"
#include "event.h"

#define SMS_RAMSIZE 0xa000 /* 0x2000 ram + 0x8000 cart ram */

unsigned char *sms_memory;
cal_cpu sms_cpu;
sms9918 sms_vdp;

unsigned char *sms_rombase;
unsigned char *sms_bank0;
unsigned char *sms_bank1;
unsigned char *sms_bank2;
unsigned char *sms_cart_memory;
int sms_romsize;
int sms_uses_battery;
rom_file sms_battery_file;

const struct joypad_button_template sms_joypad_template_pause = {
    7, {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x80} /* has pause button */
};

const struct joypad_button_template sms_joypad_template = {
    6, {0x01, 0x02, 0x04, 0x08, 0x10, 0x20}
};

struct joypad sms_joypad_1 = {
    &sms_joypad_template_pause, 0
};

struct joypad sms_joypad_2 = {
    &sms_joypad_template, 0
};

unsigned char sms_pageregs[4];

int is_gg;

ranged_mmu sms_mmu[] = {
    {0x0000, 0x03ff, NULL, 0, 0},
    {0x0400, 0x3fff, NULL, 0, 0},
    {0x4000, 0x7fff, NULL, 0, 0},
    {0x8000, 0xbfff, NULL, 0, 0},
    {0xc000, 0xdfff, NULL, 0, 0},
    {0xe000, 0xffff, NULL, 0, 0},
};

void sms_pagesync(void)
{
    sms_bank0 = &sms_rombase[(sms_pageregs[1] << 14) & (sms_romsize - 1)];
    sms_bank1 = &sms_rombase[(sms_pageregs[2] << 14) & (sms_romsize - 1)];
    if (sms_pageregs[0] & 0x08) {
	sms_bank2 = &sms_cart_memory[((sms_pageregs[0] & 0x04) << 12)];
	sms_uses_battery = 1;
    } else {
	sms_bank2 = &sms_rombase[(sms_pageregs[3] << 14) & (sms_romsize - 1)];
    }

    sms_mmu[1].data = sms_bank0;
    sms_mmu[2].data = sms_bank1 - 0x4000;
    sms_mmu[3].data = sms_bank2 - 0x8000;
}

void sms_init_mmu(rom_file romfile)
{
    if (romfile->size & 0x200) {
	sms_rombase = romfile->data + 0x200;
	sms_romsize = romfile->size - 0x200;
    } else {
	sms_rombase = romfile->data;
	sms_romsize = romfile->size;
    }

    if (sms_romsize & 0x40) {
	/* Okay, who the heck thought up the 64-byte footer? */
	deb_printf("64-byte footer detected, ignoring.\n");
	sms_romsize -= 0x40;
    }
    
    sms_mmu[0].data = sms_rombase;
    sms_mmu[4].data = sms_memory - 0xc000;
    sms_mmu[5].data = sms_memory - 0xe000;
    
    sms_pageregs[0] = 0;
    sms_pageregs[1] = 0;
    sms_pageregs[2] = 1;
    sms_pageregs[3] = 2;
    sms_pagesync();
}

unsigned char sms_read(cal_cpu cpu, unsigned long addr)
{
    if (addr < 0x400) {
	return sms_rombase[addr];
    } else if (addr < 0x4000) {
	return sms_bank0[addr];
    } else if (addr < 0x8000) {
	return sms_bank1[addr & 0x3fff];
    } else if (addr < 0xc000) {
	return sms_bank2[addr & 0x3fff];
    } else {
	return sms_memory[addr & 0x1fff];
    }
}

void sms_write(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if (addr >= 0xc000) {
	if (addr >= 0xfffc) {
	    sms_pageregs[addr & 3] = data;
	    sms_pagesync();
	}
	sms_memory[addr & 0x1fff] = data;
    } else if (addr > 0x8000) {
	if (sms_pageregs[0] & 0x08) {
	    sms_bank2[addr & 0x3fff] = data;
	}
    }
}

memread8_t sms_readfunc = &sms_read;
memwrite8_t sms_writefunc = &sms_write;

unsigned char sms_io_read(cal_cpu cpu, unsigned long address)
{
    if ((address == 0x00) && is_gg) {
	return (sms_joypad_1.data & 0x80) ^ 0x80; /* simulated pause button */
    } else if ((address & 0xfe) == 0xbe) {
	if (address & 1) {
	    return sms9918_readport1(sms_vdp);
	} else {
	    return sms9918_readport0(sms_vdp);
	}
    } else if (((address & 0xfe) == 0xdc) || ((address & 0xfe) == 0xc0)) {
	/* Dummy controller emulation */
	if (address & 1) {
	    /* FIXME: Country select goes here and at writes to 0x3f */
	    return (sms_joypad_2.data >> 2) ^ 0xff;
	} else {
	    ui_update_joypad(&sms_joypad_1);
	    ui_update_joypad(&sms_joypad_2);
	    return (((sms_joypad_1.data) & 0x3f) | (sms_joypad_2.data << 6)) ^ 0xff;
	}
    } else if ((address & 0xfe) == 0x7e) {
	return sms9918_readscanline(sms_vdp);
    } else {
	deb_printf("sms: I/O read 0x%02x.\n", address & 0xff);
    }

    return 0;
}

void sms_io_write(cal_cpu cpu, unsigned long address, unsigned char data)
{
    if ((address & 0xfe) == 0xbe) {
	if (address & 1) {
	    sms9918_writeport1(sms_vdp, data);
	} else {
	    sms9918_writeport0(sms_vdp, data);
	}
    } else if ((address & 0xfe) == 0x7e) {
	sms_psg_write(data);
    } else if ((address & 0xfe) == 0xf0) {
	/* I don't like seeing the sound I/O */
    } else {
	deb_printf("sms: I/O write 0x%02x = 0x%02x.\n", address & 0xff, data);
    }
}

void sms_init_cpu(void)
{
    sms_cpu = cal_create(CPUT_MARATZ80); /* FIXME: return value? */
    sms_cpu->setmmu0(sms_cpu, sms_mmu);
    sms_cpu->setmmu8(sms_cpu, 0, 0, &sms_readfunc, &sms_writefunc);
    sms_cpu->setiou(sms_cpu, sms_io_read, sms_io_write);
    sms_cpu->reset(sms_cpu);
}

void sms_init_joypads(void)
{
    if (ui_register_joypad(&sms_joypad_1)) {
	deb_printf("sms_run: joypad 1... registered.\n");
    } else {
	deb_printf("sms_run: joypad 1... not registered.\n");
    }
    
    if (ui_register_joypad(&sms_joypad_2)) {
	deb_printf("sms_run: joypad 2... registered.\n");
    } else {
	deb_printf("sms_run: joypad 2... not registered.\n");
    }
}

void sms_scanline(void)
{
    if (sms9918_periodic(sms_vdp)) {
	sms_cpu->irq(sms_cpu, 0xff);
    }
}

void sms_check_pause(void)
{
    /* NOTE: the use of the otherwise unused 0x40 bit is a hack */
    ui_update_joypad(&sms_joypad_1);
    if (sms_joypad_1.data & 0x80) {
	if (!(sms_joypad_1.data & 0x40)) {
	    sms_cpu->nmi(sms_cpu);
	}
	sms_joypad_1.data |= 0x40;
    } else {
	sms_joypad_1.data &= ~0x40;
    }
}

struct event sms_events[] = {
    {NULL, 0, 228, sms_scanline},
    {NULL, 0, 228 * 200, sms_check_pause},
    {NULL, 0, 228 * 262, sms_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler sms_init_events(void)
{
    event_scheduler scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, sms_cpu);

    event_register(scheduler, &sms_events[0]);
    if (!is_gg) {
	event_register(scheduler, &sms_events[1]);
    }
    event_register(scheduler, &sms_events[2]);
    event_register(scheduler, &sms_events[3]);

    return scheduler;
}

void sms_load_battery_file(rom_file romfile)
{
    sms_battery_file = init_battery_file(romfile, 0x4000);
    if (sms_battery_file) {
	sms_cart_memory = sms_battery_file->data;
    } else {
	sms_cart_memory = sms_memory + 0x2000;
    }
}

void sms_save_battery_file(void)
{
    if (sms_battery_file) {
	save_battery_file(sms_battery_file);
    }
}

void sms_shutdown(void)
{
    if (sms_uses_battery) {
	sms_save_battery_file();
    }

    sms_psg_done();
}

void sms_run_common(rom_file romfile)
{
    event_scheduler scheduler;
    
    sms_memory = calloc(1, SMS_RAMSIZE);
    
    if (!sms_memory) {
	deb_printf("out of memory.\n");
	return;
    }
    
    sms_load_battery_file(romfile);
    sms_uses_battery = 0;
    
    sms_init_mmu(romfile);

    sms_init_cpu();
    
    sms_vdp = sms9918_create(is_gg); /* FIXME: return value? */
    
    sms_psg_init();
    
    dn_shutdown = sms_shutdown;

    sms_init_joypads();

    scheduler = sms_init_events();
    
/*     set_timeslice((void (*)(void *)) &sms_timeslice, sms_cpu); */
    set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
}

void sms_run(rom_file romfile)
{
    is_gg = 0;
    sms_run_common(romfile);
}

void gg_run(rom_file romfile)
{
    is_gg = 1;
    sms_run_common(romfile);
}


/*
 * $Log: sms.c,v $
 * Revision 1.45  2001/02/27 04:14:37  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.44  2000/08/26 00:58:55  nyef
 * cleaned up some dead code
 *
 * Revision 1.43  2000/08/26 00:26:38  nyef
 * moved the psg "vsync" call in from sms9918.c
 *
 * Revision 1.42  2000/08/26 00:14:48  nyef
 * converted to use the periodic event scheduler
 *
 * Revision 1.41  2000/08/22 01:52:50  nyef
 * added code to detect ROMs with a 64-byte footer and handle them correctly
 *
 * Revision 1.40  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.39  1999/12/06 00:33:43  nyef
 * changed to use new battery file support
 *
 * Revision 1.38  1999/12/03 03:53:08  nyef
 * moved a bunch of stuff out of sms_run_common(), some to new functions
 * moved battery file stuff out to it's own set of functions
 *
 * Revision 1.37  1999/11/28 17:12:33  nyef
 * cleared out some vestigial code for old CPU interfaces
 *
 * Revision 1.36  1999/11/28 17:00:33  nyef
 * cleared out some (disabled) debug code
 *
 * Revision 1.35  1999/11/27 18:32:50  nyef
 * split sms_run() out into sms_run_common(), sms_run(), and gg_run()
 * removed GG autodetect (now to be handled by guess_system())
 *
 * Revision 1.34  1999/11/26 20:07:36  nyef
 * moved sound quality control out to the UI layer
 *
 * Revision 1.33  1999/11/26 16:41:12  nyef
 * changed to not use procpointers to access the VDP
 *
 * Revision 1.32  1999/11/20 05:27:47  nyef
 * hacked to work with new rom loading interface
 *
 * Revision 1.31  1999/11/08 01:32:33  nyef
 * added support for ranged mmu
 *
 * Revision 1.30  1999/10/15 01:45:41  nyef
 * fixed polarity on gg pause button. (again)
 *
 * Revision 1.29  1999/08/29 13:05:42  nyef
 * updated to new IRQ interface
 *
 * Revision 1.28  1999/07/31 03:22:27  nyef
 * added (untested) second controller support
 * added SMS pause button support
 *
 * Revision 1.27  1999/07/01 01:26:37  nyef
 * added code to shutdown the sound system
 *
 * Revision 1.26  1999/06/06 17:50:47  nyef
 * changed to use the new mz80 memory interface
 *
 * Revision 1.25  1999/06/05 02:41:32  nyef
 * converted to use new joypad interface
 *
 * Revision 1.24  1999/05/02 18:27:19  nyef
 * added preliminary battery file support
 *
 * Revision 1.23  1999/05/01 23:57:49  nyef
 * added stuff to allow mz80 to directly access SMS memory
 *
 * Revision 1.22  1999/05/01 21:55:35  nyef
 * changed to quit on unimplimented instruction
 *
 * Revision 1.21  1999/05/01 21:55:02  nyef
 * fixed memory read/write routines
 *
 * Revision 1.20  1999/04/28 13:44:21  nyef
 * changed to add sound emulation
 *
 * Revision 1.19  1999/04/17 20:09:25  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.18  1999/03/12 02:13:18  nyef
 * fixed to tell vdp what system to emulate
 *
 * Revision 1.17  1999/02/23 02:57:28  nyef
 * removed aliasing of bankswitch regs to 0xdffc-0xdfff
 *
 * Revision 1.16  1999/02/15 03:26:08  nyef
 * added patch from Michael Vance to squash warnings
 *
 * Revision 1.15  1999/01/26 03:05:49  nyef
 * fixed to use 32bit "address bus"
 *
 * Revision 1.14  1999/01/14 02:18:43  nyef
 * fixed polarity on gg pause button.
 *
 * Revision 1.13  1999/01/13 02:48:31  nyef
 * added emulation of gg start button.
 *
 * Revision 1.12  1999/01/10 19:55:38  nyef
 * added aliasing of bankswitch regs to 0xdffc-0xdfff.
 * added detection of sms/gg images.
 *
 * Revision 1.11  1999/01/10 02:20:19  nyef
 * disabled debug logging of writes to I/O ports 0xf0 and 0xf1.
 *
 * Revision 1.10  1999/01/09 23:10:06  nyef
 * added support for reading the current scanline.
 *
 * Revision 1.9  1999/01/09 22:57:01  nyef
 * added aliased I/O ports for controller reads.
 *
 * Revision 1.8  1999/01/09 21:52:13  nyef
 * fixed stupid mistake in sms_pagesync().
 *
 * Revision 1.7  1999/01/08 19:35:58  nyef
 * fixed timing in sms_timeslice().
 *
 * Revision 1.6  1999/01/06 02:41:12  nyef
 * added code to prevent non-extant banks on rom from being swapped in.
 *
 * Revision 1.5  1999/01/05 05:40:19  nyef
 * hacked in primitive controller support.
 *
 * Revision 1.4  1999/01/03 22:07:39  nyef
 * added vdp emulation stuff.
 * added some other stuff too.
 *
 * Revision 1.3  1999/01/03 02:20:19  nyef
 * switched from CPUT_EMUZ80 to CPUT_MARATZ80.
 *
 * Revision 1.2  1999/01/02 06:40:50  nyef
 * added dummy I/O interface.
 *
 * Revision 1.1  1999/01/01 20:43:47  nyef
 * Initial revision
 *
 */
