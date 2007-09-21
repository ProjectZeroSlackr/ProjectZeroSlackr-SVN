/*
 * nes_ppu.h
 *
 * emulation interface for NES PPU
 */

/* $Id: nes_ppu.h,v 1.22 2000/10/09 16:37:50 nyef Exp $ */

#ifndef NES_PPU_H
#define NES_PPU_H

#ifndef __CAL_H__
#include "cal.h"
#endif

#ifndef NES_H
#include "nes.h"
#endif
#include "palette.h"
struct nes_ppu {
    u16 scanline;
    u16 refresh_data;
    u16 refresh_temp;
    int finescroll;

    u8 palette[0x40];
    palette render_palette;

    u8 control_1; /* $2000 */
    u8 control_2; /* $2001 */
    u8 status; /* $2002 */
    u8 sprite_addy; /* $2003 */

    u8 address_latch;
    u8 read_latch;
    u16 address;
    
    u8 *tilecache;
    u8 *pattern_cache[8];

    int pageram_size;
    u8 *pageram_memory;
    u8 *pageram_tilecache;

    int chr_is_rom;
    
    nes_mapper mapper;
    nes_rom romfile;
};
typedef struct nes_ppu *nes_ppu;
extern nes_ppu nes_ppu_true;

/* this include must come after the typedef for nes_ppu */
#ifndef MAPPERS_H
#include "mappers.h"
#endif

/* PPU control */
void PPU_enter_cpu(cal_cpu cpu);
void PPU_leave_cpu(cal_cpu cpu);
void PPU_mirror_horizontal(void);
void PPU_mirror_vertical(void);
void PPU_mirror_one_low(void);
void PPU_mirror_one_high(void);
int nesppu_periodic(nes_ppu ppu);
void nesppu_set_mapper(nes_ppu ppu, nes_mapper mapper);

/* PPU memory initialization */
void PPU_init(nes_rom romfile);

void PPU_shutdown();
/* PPU memory mapping */
void nesppu_map_1k(nes_ppu ppu, int bank, int page);
void nesppu_map_2k(nes_ppu ppu, int bank, int page);
void nesppu_map_4k(nes_ppu ppu, int bank, int page);
void nesppu_map_8k(nes_ppu ppu, int bank, int page);

/* PPU functions */
void PPU_render_video_frame(void);

/* PPU I/O */
void ppu_io_write(nes_ppu ppu, unsigned short addr, unsigned char value);
unsigned char ppu_io_read(nes_ppu ppu, unsigned short addr);

/* PPU debug output */
void PPU_dump_memory(short address);
void PPU_dump_pattern(short address);

/* PPU nametables */
extern unsigned char *PPU_nametables[4];

/* PPU sprite RAM */
extern unsigned char PPU_sprite_ram[0x100];

/* support for mappers that use latches */
typedef void (* ppulatch_t)(nes_mapper mapper, unsigned short address);
extern ppulatch_t ppu_latchfunc;

/* support for paged CHR RAM */
void nesppu_paged_ram_init(int num_8k_pages);
void nesppu_paged_ram_mode(int enabled);

#endif /* NES_PPU_H */

/*
 * $Log: nes_ppu.h,v $
 * Revision 1.22  2000/10/09 16:37:50  nyef
 * added preliminary support for paged CHR RAM
 *
 * Revision 1.21  2000/10/05 08:55:28  nyef
 * fixed the redundant include guard for mappers.h to use the correct symbol
 *
 * Revision 1.20  2000/10/05 08:37:35  nyef
 * moved sprite DMA handling from nes_ppu.c to nes.c
 *
 * Revision 1.19  2000/10/05 08:29:15  nyef
 * fixed the idempotency code to not violate ANSI quite so flagrantly
 *
 * Revision 1.18  2000/05/07 02:12:31  nyef
 * added "extern" to some variable declarations
 *
 * Revision 1.17  1999/12/04 04:54:10  nyef
 * removed useless include of emu6502.h
 *
 * Revision 1.16  1999/11/20 05:38:28  nyef
 * rebuilt romfile handling
 *
 * Revision 1.15  1999/11/16 02:14:22  nyef
 * removed PPU_banks[] and PPU_bank_XX (obsolete interface)
 *
 * Revision 1.14  1999/11/15 04:05:12  nyef
 * implemented preliminary version of new CHR ROM mapping interface
 *
 * Revision 1.13  1999/11/14 16:20:25  nyef
 * fixed latch function interface to work with new mapper interface
 *
 * Revision 1.12  1999/11/14 07:00:13  nyef
 * changed to use new mapper interface
 *
 * Revision 1.11  1999/06/05 02:52:31  nyef
 * added prototype for nesppu_periodic()
 *
 * Revision 1.10  1999/05/29 22:31:47  nyef
 * started reorganizing the NES PPU
 *
 * Revision 1.9  1999/02/14 18:29:40  nyef
 * added function prototypes required for I/O access
 *
 * Revision 1.8  1999/01/02 00:41:53  nyef
 * added PPU_banks for mappers that find it easier to index through that.
 *
 * Revision 1.7  1999/01/01 21:47:40  nyef
 * added PPU_nametables for mappers that need to modify them by hand.
 *
 * Revision 1.6  1998/12/11 04:33:57  nyef
 * fixed for full compatability with cal.
 *
 * Revision 1.5  1998/10/01 02:34:00  nyef
 * added preliminary support for latches as per mappers 9 and 10
 *
 * Revision 1.4  1998/08/30 17:03:17  nyef
 * removed PPU_use_vrom in favor of a more general ppu_flags setup.
 *
 * Revision 1.3  1998/08/29 22:11:25  nyef
 * removed reference to PPU_video_buffer. (no longer used)
 *
 * Revision 1.2  1998/07/18 21:43:57  nyef
 * added support for one-screen mirroring
 *
 * Revision 1.1  1998/07/11 22:18:15  nyef
 * Initial revision
 *
 */
