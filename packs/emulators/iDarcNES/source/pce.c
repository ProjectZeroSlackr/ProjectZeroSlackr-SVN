/*
 * pce.c
 *
 * contains PC-Engine specific code and data.
 */

/* $Id: pce.c,v 1.55 2001/02/27 04:21:06 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "tool.h"
#include "system.h"
#include "cal.h"
#include "ui.h"
#include "event.h"
#include "pce_vdp.h"
#include "cpu6280int.h" /* FIXME: technically a no-no */

#ifdef PCE_CD_SUPPORT
unsigned char pce_cd_read(unsigned short address);
void pce_cd_write(unsigned short address, unsigned char data);
#endif

cal_cpu pce_cpu;
pce_vdp pce_video;
int pce_joyswitch;
int pce_country_select; /* 0x40 or 0x00 */
char *pce_mpr; /* FIXME: should only be compiled in when compiling both 6280 cores */
rom_file pce_rom_file;
unsigned char *pce_ram;

struct {
    u8 preload;
    u8 value;
    u8 status;
} pce_timer;

const struct joypad_button_template pce_joypad_template = {
    8, {0x10, 0x40, 0x80, 0x20, 0x01, 0x02, 0x04, 0x08}
};

struct joypad pce_joypad = {
    &pce_joypad_template, 0
};

#define PCE_RAMSIZE 0x8000
#define PCE_SAVERAM 0x2000

unsigned char *pce_mmap[256];

unsigned char pce_read_mem(cal_cpu cpu, unsigned long addr)
{
    unsigned char bank;

    bank = pce_mpr[(addr >> 13) & 7];
    
    if (bank == 0xff) {
	switch (addr & 0x1c00) {
	case 0x0000: /* HuC6270 VDC */
	    return pce_vdp_read(pce_video, addr);
	    break;
	case 0x0400: /* HuC6260 VCE */
	    return pce_vce_read(pce_video, addr);
	    break;
	case 0x0800: /* PSG */
	    break;
	case 0x0c00: /* Timers */
	    if (addr & 1) {
		return pce_timer.status;
	    } else {
		return pce_timer.value;
	    }
	    break;
	case 0x1000: /* I/O (joypad) */
	    ui_update_joypad(&pce_joypad);
	    if (pce_joyswitch) {
		return ((pce_joypad.data >> 4) | pce_country_select) ^ 0xff;
	    } else {
		return ((pce_joypad.data & 0x0f) | pce_country_select) ^ 0xff;
	    }
	    break;
	case 0x1400: /* IRQ control */
	    break;
#ifdef PCE_CD_SUPPORT
	case 0x1800: /* PCE CD system */
	    return pce_cd_read(addr & 0x1fff);
	    break;
#endif
	default:
	    deb_printf("pce: I/O read 0x%04hx, returning 0x00.\n", addr & 0x1fff);
	}
	return 0x00;
    } else {
	register unsigned char *memptr;

	memptr = pce_mmap[bank];

	if (!memptr) {
	    deb_printf("pce: read from NULL memory region 0x%02x (0x%04hx).\n", bank, addr);
	    system_flags |= F_QUIT;
	    return 0;
	} else {
	    return memptr[addr & 0x1fff];
	}
    }
}

void pce_write_mem(cal_cpu cpu, unsigned long addr, unsigned char data)
{
#ifdef PCE_CD_SUPPORT
    extern int pce_cd_init;
#endif
    
    unsigned char bank;

    bank = pce_mpr[(addr >> 13) & 7];
    
    if (bank == 0xff) {
	switch (addr & 0x1c00) {
	case 0x0000: /* HuC6270 VDC */
	    pce_vdp_write(pce_video, addr, data);
	    break;
	case 0x0400: /* HuC6260 VCE */
	    pce_vce_write(pce_video, addr, data);
	    break;
	case 0x0800: /* PSG */
	    break;
	case 0x0c00: /* Timers */
	    if (addr & 1) {
		if ((!(pce_timer.status & 1)) && (data & 1)) {
		    pce_timer.value = pce_timer.preload & 0x7f;
		}
		pce_timer.status = data;
	    } else {
		pce_timer.preload = data;
	    }
	    break;
	case 0x1000: /* I/O (joypad) */
	    pce_joyswitch = data & 1;
	    break;
	case 0x1400: /* IRQ control */
	    break;
#ifdef PCE_CD_SUPPORT
	case 0x1800:
	    pce_cd_write(addr & 0x1fff, data);
	    break;
#endif
	default:
	    deb_printf("pce: I/O write 0x%04hx = 0x%02x.\n", addr & 0x1fff, data);
	}
    } else if (bank == 0xf7) {
	/* Savegame RAM */
	pce_mmap[bank][addr & 0x1fff] = data;
    } else if (bank > 0xf7) {
	/* Normal RAM */
	pce_mmap[bank][addr & 0x1fff] = data;
#ifdef PCE_CD_SUPPORT
    } else if (pce_cd_init && (((bank >= 0x68) && (bank <= 0x87))) && (pce_mmap[bank])) {
	/* More RAM */
	pce_mmap[bank][addr & 0x1fff] = data;
#endif
    } else {
	/* It's in the ROM area */
	
	/* support for SF2CE silliness */
	if ((pce_rom_file->size == 0x280200)
	    || (pce_rom_file->size == 0x280000)) {
	    if ((addr & 0x1ffc) == 0x1ff0) {
		int i;
		
		pce_mmap[0x40] = pce_mmap[0] + 0x80000;
		pce_mmap[0x40] += (addr & 3) * 0x80000;
		
		for (i = 0x41; i <= 0x7f; i++) {
		    pce_mmap[i] = pce_mmap[i-1] + 0x2000;
		}
	    }
	} else {
	    deb_printf("pce: rom write %02x:%04hx = 0x%02x.\n", bank, addr & 0x1fff, data);
	}
    }
}

unsigned char pce_mem_read_byte(unsigned short addr)
{
    return pce_read_mem(pce_cpu, addr);
}

memread8_t pce_memread[1] = {pce_read_mem,};
memwrite8_t pce_memwrite[1] = {pce_write_mem,};

void pce_st0_hack(unsigned char data)
{
    pce_vdp_write(pce_video, 0, data);
}

void pce_st1_hack(unsigned char data)
{
    pce_vdp_write(pce_video, 2, data);
}

void pce_st2_hack(unsigned char data)
{
    pce_vdp_write(pce_video, 3, data);
}

void pce_scanline(void)
{
    if (pce_vdp_periodic(pce_video)) {
	pce_cpu->irq(pce_cpu, 2);
    }
}

void pce_timerhit(void)
{
    if (!(pce_timer.status & 1)) {
	return;
    }

    if (!pce_timer.value--) {
	pce_timer.value = pce_timer.preload & 0x7f;
	/* FIXME: TIQD? */
	pce_cpu->irq(pce_cpu, 4);
    }
}

struct event pce_events[] = {
    {NULL, 0, 496, pce_scanline},
    {NULL, 0, 1115, pce_timerhit},
    {NULL, 0, 496 * 262, NULL},
};

void pce_init_events(event_scheduler scheduler)
{
    int i;

    for (i = 0; i < (sizeof(pce_events) / sizeof(pce_events[0])); i++) {
	event_register(scheduler, &pce_events[i]);
    }
}

unsigned char bitswizzle[0x100];

void pce_swizzle_bits(void)
{
    int i;

    for (i = 0; i < 0x100; i++) {
	bitswizzle[i] = (i >> 7) & 0x01;
	bitswizzle[i] |= (i >> 5) & 0x02;
	bitswizzle[i] |= (i >> 3) & 0x04;
	bitswizzle[i] |= (i >> 1) & 0x08;
	bitswizzle[i] |= (i << 1) & 0x10;
	bitswizzle[i] |= (i << 3) & 0x20;
	bitswizzle[i] |= (i << 5) & 0x40;
	bitswizzle[i] |= (i << 7) & 0x80;
    }
    
    for (i = 0; i < pce_rom_file->size; i++) {
	pce_rom_file->data[i] = bitswizzle[pce_rom_file->data[i]];
    }
}

void pce_init_mmap()
{
    int i;
    int offset;
    int romsize;
    unsigned char *romimage;

    offset = 0;
    romsize = pce_rom_file->size;
    romimage = pce_rom_file->data;
    
    if (romsize & 0x200) {
	printf("pce_init_mmap(): compensating for 512-byte header.\n");
	offset = 0x200;
    }
    
    for (i = 0; i < 0xf7; i++) {
	pce_mmap[i] = ((((i * 0x2000) + offset) < romsize)? (romimage + (i * 0x2000) + offset): 0);
    }

    if (romsize == (offset + 0x60000)) {
	deb_printf("pce: compensating for possible split rom.\n");
	for (i = 0; i < 0x10; i++) {
	    pce_mmap[0x40 + i] = romimage + offset + (i * 0x2000) + 0x40000;
	}
    }
    
    if (romsize == (offset + 0x80000)) {
	deb_printf("pce: compensating for possible split rom.\n");
	for (i = 0; i < 0x20; i++) {
	    pce_mmap[0x40 + i] = romimage + offset + (i * 0x2000) + 0x40000;
	}
    }
    
    pce_mmap[0xf7] = pce_ram + PCE_RAMSIZE;

    for (i = 0xf8; i < 0xfc; i++) {
	pce_mmap[i] = pce_ram + ((i - 0xf8) * 0x2000);
    }

    for (i = 0xfc; i < 0x100; i++) {
	pce_mmap[i] = NULL;
    }
}

void pce_run(rom_file romfile)
{
    event_scheduler scheduler;
    
    pce_ram = malloc(PCE_RAMSIZE + PCE_SAVERAM);

    pce_rom_file = romfile;
    pce_init_mmap();

/*      pce_cpu = cal_create(CPUT_MARAT6280); */
/*      pce_mpr = pce_cpu->data.d_marat6280->mpr; */

    pce_cpu = cal_create(CPUT_CPU6280);
    pce_mpr = pce_cpu->data.d_cpu6280->mpr;

    scheduler = new_event_scheduler(cal_event_delay_callback, pce_cpu);
    
    pce_video = pce_vdp_create();

/*     pce_swizzle_bits(); */
    pce_country_select = 0x40;
    
    if ((pce_ram) && (pce_cpu) && (pce_video) && (scheduler)) {
	pce_init_events(scheduler);
	
	pce_cpu->setzpage(pce_cpu, pce_ram);
	pce_cpu->setmmu8(pce_cpu, 0, 0, pce_memread, pce_memwrite);
	pce_cpu->reset(pce_cpu);

	if (ui_register_joypad(&pce_joypad)) {
	    deb_printf("pce_run: joypad... registered.\n");
	} else {
	    deb_printf("pce_run: joypad... not registered.\n");
	}
	
	set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
    } else {
	printf("out of memory\n");
    }
}

/*
 * $Log: pce.c,v $
 * Revision 1.55  2001/02/27 04:21:06  nyef
 * removed useless include of cpudefs.h
 *
 * Revision 1.54  2001/02/27 04:14:45  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.53  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.52  2000/03/10 01:49:01  nyef
 * moved event scheduler out to it's own module
 *
 * Revision 1.51  2000/02/19 23:28:12  nyef
 * changed to work with the new vdp interface
 *
 * Revision 1.50  2000/02/13 05:37:39  nyef
 * fixed some bugs with the timer implementation
 *
 * Revision 1.49  2000/02/06 22:29:41  nyef
 * added preliminary timer implementation
 * changed interrupt calls to use correct IRQ numbers
 *
 * Revision 1.48  2000/02/06 18:55:54  nyef
 * added a periodic event scheduler to replace the old timeslice code
 *
 * Revision 1.47  2000/01/24 05:06:36  nyef
 * changed to use cpu6280 instead of marat6280
 *
 * Revision 1.46  2000/01/09 04:00:58  nyef
 * increased cycles per scanline
 *
 * Revision 1.45  1999/12/04 04:47:02  nyef
 * removed some useless includes and dead code
 *
 * Revision 1.44  1999/11/20 05:28:28  nyef
 * fixed to work with new rom loading interface
 *
 * Revision 1.43  1999/08/29 13:05:47  nyef
 * updated to new IRQ interface
 *
 * Revision 1.42  1999/08/07 16:25:46  nyef
 * fixed to automatically select a usable CPU core based on cpudefs.h
 *
 * Revision 1.41  1999/08/01 23:42:56  nyef
 * added preliminary support for Marat6280 core
 *
 * Revision 1.40  1999/06/20 18:29:17  nyef
 * put some ram where the battery memory is supposed to go.
 *
 * Revision 1.39  1999/06/20 14:29:07  nyef
 * added another split-rom compensator to pce_init_mmap()
 *
 * Revision 1.38  1999/06/05 02:41:27  nyef
 * converted to use new joypad interface
 *
 * Revision 1.37  1999/06/03 02:08:45  nyef
 * added support for more extra ram found in the CD system
 * cleaned up a minor issue with the mmap initializer
 *
 * Revision 1.36  1999/06/02 01:07:40  nyef
 * fixed the debug message for writes to ROM to be even more informative
 * cleaned up the memory write handler
 * added support for the extra RAM present in the CD system
 *
 * Revision 1.35  1999/06/01 00:34:34  nyef
 * made the debug message for writes to the ROM area more informative
 *
 * Revision 1.34  1999/05/31 01:01:42  nyef
 * added some hooks for CD support
 *
 * Revision 1.33  1999/05/25 02:13:45  nyef
 * added a check for a null pointer dereference when reading memory
 *
 * Revision 1.32  1999/05/19 00:05:53  nyef
 * fixed small problem with the sf2ce bank switcher
 *
 * Revision 1.31  1999/05/08 23:37:43  nyef
 * added support for the SF2CE extended rom stuff
 *
 * Revision 1.30  1999/05/08 16:31:52  nyef
 * added country selection hooks
 * added bits to get the debugger working as a disassembler
 * added 3 megabit split rom support
 * added code to convert unconverted US roms
 *
 * Revision 1.29  1999/04/17 20:09:12  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.28  1999/03/05 03:32:44  nyef
 * fixed a bug in pce_init_mmap()
 *
 * Revision 1.27  1999/02/20 21:28:03  nyef
 * doubled number of cycles per scanline
 *
 * Revision 1.26  1999/02/15 03:25:52  nyef
 * added patch from Michael Vance to squash warnings
 *
 * Revision 1.25  1999/02/03 02:43:12  nyef
 * added translation table to fix joypad directions
 *
 * Revision 1.24  1999/02/02 16:20:34  nyef
 * added preliminary support for joypad 1
 *
 * Revision 1.23  1999/01/31 21:16:09  nyef
 * added debug output of joypad access
 *
 * Revision 1.22  1999/01/31 21:12:09  nyef
 * re-enabled interrupt generation
 *
 * Revision 1.21  1999/01/31 20:26:39  nyef
 * disabled interrupt generation.
 *
 * Revision 1.20  1999/01/30 23:27:30  nyef
 * added support for the pce vce
 *
 * Revision 1.19  1999/01/30 23:09:50  nyef
 * redesigned VDP I/O interface.
 *
 * Revision 1.18  1999/01/30 19:58:29  nyef
 * added i/o cases for all major devices.
 *
 * Revision 1.17  1999/01/30 19:30:13  nyef
 * reenabled output of I/O access to unknown ports.
 * added support for writing to RAM areas.
 *
 * Revision 1.16  1999/01/28 02:17:24  nyef
 * added routines to support the ST0, ST1, and ST2 instructions.
 *
 * Revision 1.15  1999/01/26 03:05:42  nyef
 * fixed to use 32bit "address bus"
 *
 * Revision 1.14  1999/01/17 02:12:43  nyef
 * added support for the new pce vdp
 *
 * Revision 1.13  1999/01/13 03:55:36  nyef
 * fixed to use the cal, the timeslicer, and the new memory access spec.
 *
 * Revision 1.12  1998/07/27 00:57:40  nyef
 * added compensation for 512-byte header in pce_init_mmap()
 *
 * Revision 1.11  1998/07/26 23:11:56  nyef
 * changed read/write table allocation to allocate extra room for zpage
 *
 * Revision 1.10  1998/07/14 10:25:21  nyef
 * added temporary assignments to cycles_left in pce_trace and pce_freerun
 *
 * Revision 1.9  1998/07/14 01:52:38  nyef
 * added initializer for disassemble()
 *
 * Revision 1.8  1998/07/14 01:37:21  nyef
 * added trace function for debugger
 *
 * Revision 1.7  1998/07/12 23:22:00  nyef
 * fixed two stupid mistakes in the mmap initializer
 *
 * Revision 1.6  1998/07/12 23:21:03  nyef
 * added initializer for memory mapping
 *
 * Revision 1.5  1998/07/12 23:03:07  nyef
 * fixed mem_read_byte()
 *
 * Revision 1.4  1998/07/12 22:36:05  nyef
 * set ram size to 32k (this figure is enough to cover the SuperGrafx)
 *
 * Revision 1.3  1998/07/12 21:50:00  nyef
 * finished fixing display_vectors()
 *
 * Revision 1.2  1998/07/12 21:24:57  nyef
 * started fixing display_vectors()
 * added freerun()
 * made regs static
 *
 * Revision 1.1  1998/07/11 22:17:59  nyef
 * Initial revision
 *
 */
