/*
 * nes.c
 *
 * contains NES specific code and data.
 */

/* $Id: nes.c,v 1.65 2001/02/27 04:14:47 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tool.h"
#include "ui.h"
#include "nes_ppu.h"
#include "mappers.h"
#include "system.h"
#include "nes.h"
#include "nes_psg.h"
#include "cal.h"
#include "event.h"
extern char* filename;
const struct joypad_button_template nes_joypad_template = {
    8, {0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08}
};

struct joypad nes_joypad_1 = {
    &nes_joypad_template, 0
};

struct joypad nes_joypad_2 = {
    &nes_joypad_template, 0
};

cal_cpu cpu;

nes_mapper mapper;
nes_rom nes_romfile;

#define NES_RAMSIZE 0x2800    /* Size of System and Cart Ram */

rom_file battery_file;

unsigned char *vromimage = NULL;  /* Cart VRom area */

unsigned char *bank_0 = NULL;     /* System Ram */

unsigned char *bank_6 = NULL;     /* Cart Ram */
unsigned char *bank_8 = NULL;     /* Rom Page */
unsigned char *bank_A = NULL;     /* Rom Page */
unsigned char *bank_C = NULL;     /* Rom Page */
unsigned char *bank_E = NULL;     /* Rom Page */

unsigned char bank8_reg;
unsigned char bankA_reg;
unsigned char bankC_reg;
unsigned char bankE_reg;

int joy1_latch;
int joy2_latch;

readhook_t nes_bank4_read_hook;
readhook_t nes_bank6_read_hook;
writehook_t nes_bank4_write_hook;
writehook_t nes_bank6_write_hook;
// They don't work
// ~Keripo
/*
void saves()
{
  char save[256];
  snprintf(save, 256, "/opt/Emulators/iDarcNES/Saves/%s.sav", filename);
  FILE *fp=fopen(save,"w");
  fprintf(fp,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n",vromimage,bank_0,bank_6,bank_8,bank_A,bank_C,bank_E);
  fclose(fp);
}
void loads()
{
  char save[256];
  snprintf(save, 256, "/opt/Emulators/iDarcNES/Saves/%s.sav", filename);
  FILE *fp=fopen(save,"r");
  fscanf(fp,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n",&vromimage,&bank_0,&bank_6,&bank_8,&bank_A,&bank_C,&bank_E);
  fclose(fp);
}
*/
unsigned char mask_bank_addr(unsigned char bank)
{
    unsigned char i;

    i = 0xff;

    while ((bank & i) >= (nes_romfile->prg_size << 1)) {
	i >>= 1;
    }

    return (bank & i);
}

u8 *bank_regs[4] = {&bank8_reg, &bankA_reg, &bankC_reg, &bankE_reg};
u8 **banks[4] = {&bank_8, &bank_A, &bank_C, &bank_E};

void nesprg_map_4k(int bank, int page)
{
}

void nesprg_map_8k(int bank, int page)
{
    *bank_regs[bank] = mask_bank_addr(page);
    *banks[bank] = nes_romfile->prg_data + (*bank_regs[bank] * 0x2000);
    set_cpu_bank_full(bank_8,bank_A,bank_C,bank_E);
}

void nesprg_map_16k(int bank, int page)
{
    nesprg_map_8k((bank << 1) + 0, (page << 1) + 0);
    nesprg_map_8k((bank << 1) + 1, (page << 1) + 1);
}

void nesprg_map_32k(int bank, int page)
{
    nesprg_map_16k((bank << 1) + 0, (page << 1) + 0);
    nesprg_map_16k((bank << 1) + 1, (page << 1) + 1);
}
extern int cycles_left;
void do_sprite_dma(int page)
{
    //cpu->burn(cpu,512);
    if (page < 0x20) { 
	memcpy(PPU_sprite_ram, bank_0 + ((page<< 8) & 0x7ff), 0x100);
    } else if (page < 0x60) {
	deb_printf("PPU_do_dma(): sprite DMA from I/O space, ignoring.\n");
    } else if (page < 0x80) {
	memcpy(PPU_sprite_ram, bank_6 + ((page<< 8) & 0x1fff), 0x100);
    } else if (page < 0xa0) {
	memcpy(PPU_sprite_ram, bank_8 + ((page<< 8) & 0x1fff), 0x100);
    } else if (page < 0xc0) {
	memcpy(PPU_sprite_ram, bank_A + ((page << 8) & 0x1fff), 0x100);
    } else if (page < 0xe0) {
	memcpy(PPU_sprite_ram, bank_C + ((page<< 8) & 0x1fff), 0x100);
    } else {
	memcpy(PPU_sprite_ram, bank_E + ((page << 8) & 0x1fff), 0x100);
    }
}

unsigned char nes_read_bank_0(cal_cpu cpu, unsigned long addr)
{
    return bank_0[addr & 0x07ff];
}

unsigned char nes_read_bank_2(cal_cpu cpu, unsigned long addr)
{
    return ppu_io_read(nes_ppu_true, addr);
}

unsigned char nes_read_bank_4(cal_cpu cpu, unsigned long addr)
{
    unsigned char retval;

    if (addr < 0x4016) {
	return 0x00; /* FIXME: sound register reads?? */
    } else if (addr == 0x4016) {
	retval = 0x40 | (joy1_latch & 1);
	joy1_latch >>= 1;
	return retval;
    } else if (addr == 0x4017) {
	retval = 0x40 | (joy2_latch & 1);
	joy2_latch >>= 1;
	return retval;
    } else if (nes_bank4_read_hook) {
	return nes_bank4_read_hook(mapper, addr);
    } else {
	deb_printf("unknown I/O read (0x%04x), returning 0.\n", addr);
	return 0;
    }
}

unsigned char nes_read_bank_6(cal_cpu cpu, unsigned long addr)
{
    if (nes_bank6_read_hook) {
	return nes_bank6_read_hook(mapper, addr);
    } else {
	return bank_6[addr & 0x1fff];
    }
}

unsigned char nes_read_bank_8(cal_cpu cpu, unsigned long addr)
{
    return bank_8[addr & 0x1fff];
}

unsigned char nes_read_bank_a(cal_cpu cpu, unsigned long addr)
{
    return bank_A[addr & 0x1fff];
}

unsigned char nes_read_bank_c(cal_cpu cpu, unsigned long addr)
{
    return bank_C[addr & 0x1fff];
}

unsigned char nes_read_bank_e(cal_cpu cpu, unsigned long addr)
{
    return bank_E[addr & 0x1fff];
}

void nes_write_bank_0(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    bank_0[addr & 0x07ff] = data;
}

void nes_write_bank_2(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    ppu_io_write(nes_ppu_true, addr, data);
}

void nes_write_bank_4(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if (addr < 0x4010) {
	sound_regs[addr & 15](data);
    } else if (addr < 0x4014) {
	/* FIXME: more sound regs go here */
    } else if (addr == 0x4014) {
	    do_sprite_dma(data);
    } else if (addr == 0x4015) {
	nes_psg_write_control(data);
    } else if (addr == 0x4016) {
	ui_update_joypad(&nes_joypad_1);
	ui_update_joypad(&nes_joypad_2);
	joy1_latch = nes_joypad_1.data;
	joy2_latch = nes_joypad_2.data;
    } else if (addr == 0x4017) {
	/* FIXME: IRQ control? */
    } else if (nes_bank4_write_hook) {
	nes_bank4_write_hook(mapper, addr, data);
    } else {
	deb_printf("unknown I/O write (0x%04x, 0x%02x).\n", addr, data);
    }
}

void nes_write_bank_6(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    if (nes_bank6_write_hook) {
	nes_bank6_write_hook(mapper, addr, data);
    } else {
	bank_6[addr & 0x1fff] = data;
    }
}

void nes_write_mapper(cal_cpu cpu, unsigned long addr, unsigned char data)
{
    mapper->write(mapper, addr, data);
}

memread8_t nes_real_readtable[8] = {
    nes_read_bank_0,
    nes_read_bank_2,
    nes_read_bank_4,
    nes_read_bank_6,
    nes_read_bank_8,
    nes_read_bank_a,
    nes_read_bank_c,
    nes_read_bank_e,
};

memwrite8_t nes_real_writetable[8] = {
    nes_write_bank_0,
    nes_write_bank_2,
    nes_write_bank_4,
    nes_write_bank_6,
    nes_write_mapper,
    nes_write_mapper,
    nes_write_mapper,
    nes_write_mapper,
};

u8 nes_read_handler(cal_cpu cpu, u32 addr)
{
    return nes_real_readtable[(addr >> 13) & 7](cpu, addr);
}

void nes_write_handler(cal_cpu cpu, u32 addr, u8 data)
{
    nes_real_writetable[(addr >> 13) & 7](cpu, addr, data);
}

memread8_t nes_readtable[1] = {
    nes_read_handler,
};

memwrite8_t nes_writetable[1] = {
    nes_write_handler,
};

void nes_load_battery_file(rom_file romfile)
{
    battery_file = init_battery_file(romfile, 0x2000);
    if (battery_file) {
	bank_6 = battery_file->data;
    }
}

void nes_save_battery_file(void)
{
    if (battery_file) {
	save_battery_file(battery_file);
    }
}

void nes_scanline(void)
{
    if (nesppu_periodic(nes_ppu_true)) {
	cpu->nmi(cpu);
    }
}

struct event nes_events[] = {
    {NULL, 0, 113, nes_scanline}, // 113
    {NULL, 0, 7457, nes_psg_frame}, // 7457
    {NULL, 0, 113 * 262, NULL}, // 113*262
};

event_scheduler nes_init_events(void)
{
    event_scheduler scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, cpu);

    event_register(scheduler, &nes_events[0]);
    event_register(scheduler, &nes_events[1]);
    event_register(scheduler, &nes_events[2]);

    return scheduler;
}

void nes_shutdown(void)
{
    if (nes_romfile->uses_battery) {
	nes_save_battery_file();
    }
    PPU_shutdown();
    nes_psg_done();
    free(cpu);
    //free(nes_romfile);
    //free_romfile(nes_romfile);
    //free(bank_0);
}

nes_rom nes_create_romfile(rom_file romfile)
{
    nes_rom retval;

    retval = malloc(sizeof(struct nes_rom));

    if (!retval) {
	return NULL;
    }
    
    retval->file = romfile;
    retval->prg_size = romfile->data[4];
    retval->chr_size = romfile->data[5];
    retval->prg_data = romfile->data + 0x10;
    retval->uses_battery = romfile->data[6] & 2;
    retval->mirror_vertical = romfile->data[6] & 1;
    
    if (retval->chr_size > 0) { /* this works as is for FDS */
	retval->chr_data = retval->prg_data + (retval->prg_size * 0x4000);
    } else {
	retval->chr_data = NULL;
    }

    if (romfile->data[0] == 'F') { /* NOTE: Cheap hack */
	retval->mapper = 20;
    } else {
	retval->mapper = romfile->data[6] >> 4;
	if (*((u32 *)&romfile->data[12])) {
	    deb_printf("nes: dirty iNES header.\n");
	    deb_printf("nes: using 4-bit mapper id.\n");
	} else {
	    deb_printf("nes: using 8-bit mapper id.\n");
	    retval->mapper |= romfile->data[7] & 0xf0;
	}
    }

    return retval;
}

void nes_init_joypads(void)
{
    if (ui_register_joypad(&nes_joypad_1)) {
	deb_printf("nes_run: joypad 1... registered.\n");
    } else {
	deb_printf("nes_run: joypad 1... not registered.\n");
    }
    if (ui_register_joypad(&nes_joypad_2)) {
	deb_printf("nes_run: joypad 2... registered.\n");
    } else {
	deb_printf("nes_run: joypad 2... not registered.\n");
    }
}
extern int vframe;
extern int vtime;
extern int lastframe;
void nes_init_cpu(void)
{
    cpu = cal_create(CPUT_CPU6502);
    init_sram(bank_6);    
    set_cpu_bank_full(bank_8,bank_A,bank_C,bank_E);
    cpu->setmmu8(cpu, 13, 7, nes_readtable, nes_writetable);
    cpu->setzpage(cpu, &bank_0);
    cpu->reset(cpu);
    vframe=0;
    vtime=0;
    lastframe=0;
}
extern int nomenu;
void nes_run(rom_file romfile)
{
    event_scheduler scheduler;
    
    bank_0 = malloc(NES_RAMSIZE);
    memset(bank_0,0,NES_RAMSIZE);
  if(!nomenu){  nes_romfile = nes_create_romfile(romfile);

    if (!(nes_romfile && bank_0)) {
	if (nes_romfile) {
	    free(nes_romfile);
	}
	if (bank_0) {
	    free(bank_0);
	}
	deb_printf("out of memory\n");
	return;
    }


    if (!mapper_supported(nes_romfile->mapper)) {
	deb_printf("darcnes: mapper #%d unsupported.\n", nes_romfile->mapper);

	free(bank_0);
	free(nes_romfile);
	
	return;
    }
   } 
    dn_shutdown = &nes_shutdown;
    
    bank_6 = bank_0 + 0x800;
  if(!nomenu)   if (nes_romfile->uses_battery) {
	nes_load_battery_file(nes_romfile->file);
    }
    
 PPU_init(nes_romfile);
    
 if(!nomenu)    mapper = create_mapper(nes_ppu_true, nes_romfile);
  if(!nomenu)   nesppu_set_mapper(nes_ppu_true, mapper);

  if(!nomenu)   nes_init_joypads();
    
   if(!nomenu)  nes_psg_init();

   if(!nomenu)  nes_init_cpu();
    
    if(!nomenu) scheduler = nes_init_events();
    
/*     set_timeslice((void (*)(void *)) &nes_timeslice, cpu); */
    set_timeslice((void (*)(void *)) &event_timeslice, scheduler);
}

/*
 * $Log: nes.c,v $
 * Revision 1.65  2001/02/27 04:14:47  nyef
 * changed to use the new event and cal interfaces
 *
 * Revision 1.64  2000/10/07 07:20:48  nyef
 * removed sync_banks() (it wasn't being used anymore)
 *
 * Revision 1.63  2000/10/07 07:18:46  nyef
 * fixed small bug in nes_create_romfile()
 *
 * Revision 1.62  2000/10/05 08:45:09  nyef
 * moved PPU_memory from nes.c to nes_ppu.c
 *
 * Revision 1.61  2000/10/05 08:37:31  nyef
 * moved sprite DMA handling from nes_ppu.c to nes.c
 *
 * Revision 1.60  2000/10/05 06:53:24  nyef
 * added new PRG-ROM mapping interface
 *
 * Revision 1.59  2000/10/02 14:19:20  nyef
 * converted to new psg interface
 * converted to use event scheduler
 *
 * Revision 1.58  2000/09/23 16:31:37  nyef
 * fixed bug in mapper identification introduced last revision
 *
 * Revision 1.57  2000/09/16 23:04:26  nyef
 * cleaned up nes_create_romfile()
 *
 * Revision 1.56  2000/09/16 22:49:16  nyef
 * added mirror_vertical field to nes_rom structure
 *
 * Revision 1.55  2000/09/16 22:45:40  nyef
 * added a uses_battery field to the nes_rom structure
 *
 * Revision 1.54  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.53  2000/05/01 00:35:48  nyef
 * changed to use the new cpu6502 core
 *
 * Revision 1.52  2000/02/19 20:05:52  nyef
 * fixed to work with the new read/write hook interface
 *
 * Revision 1.51  1999/12/06 00:33:59  nyef
 * changed to use new battery file support
 *
 * Revision 1.50  1999/12/04 04:47:58  nyef
 * removed some unused includes
 *
 * Revision 1.49  1999/12/03 04:27:48  nyef
 * cleaned out some more dead code
 *
 * Revision 1.48  1999/12/03 04:26:53  nyef
 * cleaned up nes_run() (fixed a couple memory leaks while I was at it)
 *
 * Revision 1.47  1999/12/03 04:02:56  nyef
 * moved the battery file code out into their own functions
 *
 * Revision 1.46  1999/12/03 03:55:52  nyef
 * cleaned out some dead code
 *
 * Revision 1.45  1999/11/26 20:07:46  nyef
 * moved sound quality control out to the UI layer
 *
 * Revision 1.44  1999/11/21 17:23:03  nyef
 * moved palette stuff out to nes_ppu.c
 *
 * Revision 1.43  1999/11/20 05:33:13  nyef
 * fixed to work with new rom loading interface
 * rebuild rom file handling
 *
 * Revision 1.42  1999/11/14 07:00:05  nyef
 * changed to use new mapper interface
 *
 * Revision 1.41  1999/10/28 01:03:29  nyef
 * made sync_banks() mask the bank regs down to the ROM size
 *
 * Revision 1.40  1999/08/20 02:24:29  nyef
 * reduced cycles per scanline to 113
 *
 * Revision 1.39  1999/08/01 18:31:11  nyef
 * fixed second joypad support
 *
 * Revision 1.38  1999/07/18 19:59:06  nyef
 * changed to quit on unimplimented instruction
 *
 * Revision 1.37  1999/07/01 01:26:41  nyef
 * added code to shutdown the sound system
 *
 * Revision 1.36  1999/06/19 15:08:44  nyef
 * continued reorganizing the NES PPU
 *
 * Revision 1.35  1999/06/05 02:41:53  nyef
 * converted to use new joypad interface
 * added second joypad support
 *
 * Revision 1.34  1999/05/29 22:31:43  nyef
 * started reorganizing the NES PPU
 *
 * Revision 1.33  1999/05/04 01:41:51  nyef
 * fixed unsupported mapper message to say "darcnes" instead of "tool"
 *
 * Revision 1.32  1999/04/17 20:08:49  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.31  1999/02/19 02:07:06  nyef
 * added hooks for reading and writing banks 4 and 6
 *
 * Revision 1.30  1999/02/16 01:51:42  nyef
 * removed useless reference to io_nes.h
 *
 * Revision 1.29  1999/02/15 03:25:26  nyef
 * added patch from Michael Vance to squash warnings
 *
 * Revision 1.28  1999/02/14 18:33:09  nyef
 * moved I/O and memory access in from io_nes.S
 *
 * Revision 1.27  1999/02/14 06:39:09  nyef
 * added preliminary support for FDS
 *
 * Revision 1.26  1999/02/14 06:18:37  nyef
 * added lack of support for Famicom Disk System
 *
 * Revision 1.25  1999/02/09 01:50:44  nyef
 * added battery file patch from Michael Vance
 *
 * Revision 1.24  1999/01/05 04:28:57  nyef
 * added a call to set the palette.
 *
 * Revision 1.23  1998/12/31 03:53:27  nyef
 * added support for 8-bit mapper ids.
 *
 * Revision 1.22  1998/12/24 04:55:56  nyef
 * fixed nes_timeslice() to return approximately once per vsync.
 *
 * Revision 1.21  1998/12/21 02:57:47  nyef
 * added preliminary battery save support.
 *
 * Revision 1.20  1998/12/21 00:01:20  nyef
 * changed allocation of ramblock to use calloc(3).
 * This should fix SMB1 under DOS.
 *
 * Revision 1.19  1998/12/18 04:20:19  nyef
 * removed some old code. cenerally cleand up.
 *
 * Revision 1.18  1998/12/11 04:32:38  nyef
 * fixed to be compatable with all 6502 cpu cores available under cal.
 *
 * Revision 1.17  1998/12/06 04:21:23  nyef
 * added code to set psg playback quality.
 *
 * Revision 1.16  1998/11/29 18:28:08  nyef
 * started converting to use the new memory access spec and the CAL.
 *
 * Revision 1.15  1998/10/11 21:46:59  nyef
 * changed to use the emu6502a core. Lolo1 now works.
 *
 * Revision 1.14  1998/08/30 17:06:10  nyef
 * changed the vrom check to use the new ppu_flags system.
 * moved the vrom check to after the call to PPU_init().
 *
 * Revision 1.13  1998/08/26 00:27:32  nyef
 * removed code to set the initial ppu mirroring. (It was already in ppu.c)
 *
 * Revision 1.12  1998/08/25 03:35:34  nyef
 * added code to set the initial ppu mirroring from the header data.
 *
 * Revision 1.11  1998/08/22 16:28:29  nyef
 * added check for setting PPU_use_vrom based on vrom size.
 *
 * Revision 1.10  1998/08/22 00:59:47  nyef
 * added sound hooks to nes_timeslice() and nes_run().
 *
 * Revision 1.9  1998/08/15 01:13:33  nyef
 * added test for unimplimented instruction in nes_timeslice()
 *
 * Revision 1.8  1998/08/02 04:17:26  nyef
 * changed every printf to deb_printf for debug console.
 *
 * Revision 1.7  1998/08/02 04:10:42  nyef
 * added ui.h to the include section and changed nes_timeslice to take
 * a void * instead of a struct emu6502_context *.
 *
 * Revision 1.6  1998/08/01 22:33:22  nyef
 * added nes_timeslice() to facilitate a user interface.
 * changes nes_run() to use nes_timeslice.
 *
 * Revision 1.5  1998/07/26 23:11:19  nyef
 * changed read/write table allocation to allocate extra room for zpage
 *
 * Revision 1.4  1998/07/14 01:44:04  nyef
 * added initializer for disassemble()
 *
 * Revision 1.3  1998/07/14 01:28:22  nyef
 * moved trace code in from debug.c
 *
 * Revision 1.2  1998/07/12 21:25:41  nyef
 * made regs static
 *
 * Revision 1.1  1998/07/11 22:18:09  nyef
 * Initial revision
 *
 */
