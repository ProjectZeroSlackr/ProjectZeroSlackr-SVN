/*
 * nes_ppu.c
 *
 * NES PPU emulation
 */

/* $Id: nes_ppu.c,v 1.112 2001/03/06 06:02:38 nyef Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes_ppu.h"
#include "nes.h"
#include "tool.h"
#include "ui.h"
#include "video.h"
#include "mappers.h"
#include "nes_psg.h"
#include "cal.h"
#include "tiledraw.h"
#include "palette.h"
#include "cop.h"
#include "nespal.c"

extern int frameskip;
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

struct nes_ppu nes_ppu_temp = {
    0, 0, 0, 0,
};

nes_ppu nes_ppu_true = &nes_ppu_temp;

unsigned char PPU_sprite_ram[0x100];

/* ppu memory areas */
unsigned char *PPU_memory; /* PPU Ram */

unsigned char *PPU_banks[8];

unsigned char **PPU_patterns[2] = {
    &PPU_banks[0],
    &PPU_banks[4],
};

unsigned char *PPU_nametables[4];

ppulatch_t ppu_latchfunc; /* for mapper support */

long PPU_amode; /* $2006 */

void PPU_render_scan_line(nes_ppu ppu);

extern cal_cpu cpu;

int nesppu_periodic(nes_ppu ppu)
{
    if (((ppu->scanline < 240) || (ppu->scanline == 262)) && (ppu->control_2 & 0x18)) {
	ppu->mapper->hsync(ppu->mapper, cpu, 1);
    } else {
	ppu->mapper->hsync(ppu->mapper, cpu, 0);
    }
    if (ppu->scanline < 240) {
	ppu->refresh_data &= 0x7be0;
	ppu->refresh_data |= ppu->refresh_temp & 0x041f;
	PPU_render_scan_line(ppu);
    } else if (ppu->scanline == 240) {
	ppu->status = 0x80;
	if (frameskip==0) video_display_buffer();
    }

    ppu->scanline++;
    if (ppu->scanline == 262) {
	ppu->status = 0;
	ppu->scanline = 0;
	ppu->refresh_data = ppu->refresh_temp;
    }

    return (ppu->control_1 & 0x80) && (ppu->scanline == 243);
}

void nesppu_cache_tile_line(unsigned char pattern0, unsigned char pattern1, unsigned char *cache)
{
    unsigned char chunks_0;
    unsigned char chunks_1;
    unsigned char *cacheptr;

    cacheptr = cache;
    
    chunks_0 = ((pattern0 >> 1) & 0x55) | (pattern1 & 0xaa);
    chunks_1 = (pattern0 & 0x55) | ((pattern1 << 1) & 0xaa);
    
    *cacheptr++ = (chunks_0 >> 6) & 3;
    *cacheptr++ = (chunks_1 >> 6) & 3;
    *cacheptr++ = (chunks_0 >> 4) & 3;
    *cacheptr++ = (chunks_1 >> 4) & 3;
    *cacheptr++ = (chunks_0 >> 2) & 3;
    *cacheptr++ = (chunks_1 >> 2) & 3;
    *cacheptr++ = chunks_0 & 3;
    *cacheptr++ = chunks_1 & 3;
}

void nesppu_cache_chr_rom(unsigned char *chr_data, int chr_size, unsigned char *tilecache)
{
    int page;
    int tile;
    int line;
    unsigned char *cacheptr;
    unsigned char *dataptr;

    cacheptr = tilecache;
    
    for (page = 0; page < (chr_size << 3); page++) {
	for (line = 0; line < 8; line++) {
	    dataptr = chr_data + (page * 0x400) + line;
	    for (tile = 0; tile < 0x40; tile++) {
		nesppu_cache_tile_line(*dataptr, *(dataptr + 8), cacheptr);
		cacheptr += 8;
		dataptr += 0x10;
	    }
	}
    }
}

void PPU_init(nes_rom romfile)
{
    int i;

    nes_ppu_true->romfile = romfile;

    video_setsize(256, 240);
    video_setpal(64, nes_palbase_red, nes_palbase_green, nes_palbase_blue);
    PPU_memory = malloc(0x3000);

    nes_ppu_true->render_palette = new_palette_8(8, 4);
    
    if (romfile->chr_size) {
	nes_ppu_true->tilecache = malloc(romfile->chr_size * 0x2000 * 4);
	nesppu_cache_chr_rom(romfile->chr_data, romfile->chr_size, nes_ppu_true->tilecache);
    } else {
	nes_ppu_true->tilecache = calloc(1, 0x2000 * 4);
	nes_ppu_true->pattern_cache[0] = nes_ppu_true->tilecache + 0x0000;
	nes_ppu_true->pattern_cache[1] = nes_ppu_true->tilecache + 0x1000;
	nes_ppu_true->pattern_cache[2] = nes_ppu_true->tilecache + 0x2000;
	nes_ppu_true->pattern_cache[3] = nes_ppu_true->tilecache + 0x3000;
	nes_ppu_true->pattern_cache[4] = nes_ppu_true->tilecache + 0x4000;
	nes_ppu_true->pattern_cache[5] = nes_ppu_true->tilecache + 0x5000;
	nes_ppu_true->pattern_cache[6] = nes_ppu_true->tilecache + 0x6000;
	nes_ppu_true->pattern_cache[7] = nes_ppu_true->tilecache + 0x7000;
    }

    for (i = 0; i < 8; i++) {
	PPU_banks[i] = PPU_memory + (i * 0x400);
    }

    if (romfile->mirror_vertical) {
	PPU_mirror_vertical();
    } else {
	PPU_mirror_horizontal();
    }
    
    nes_ppu_true->chr_is_rom = romfile->chr_size;
}

void nesppu_set_mapper(nes_ppu ppu, nes_mapper mapper)
{
    ppu->mapper = mapper;
}

int nesppu_mask_page_to(int page, int mask)
{
    /*
     * NOTE: This is two cheap hacks piled one atop another. The first is
     * simply anding page with mask - 1, which relies on mask being a power
     * of two (I believe it is in all good dumps of non-pirate games). The
     * second is to not actually mask the page if the page number is less
     * than the mask. This fixes for the unfortunately rather popular bad
     * dump of Zelda II, which is missing some otherwise unused CHR-ROM
     * pages, "but it works with every other emulator". The _reason_ it
     * works with every other emulator, of course, is because every other
     * emulator author got fed up with hearing "but it works with NESticle",
     * and worked around the problem in some way, but people don't want to
     * hear that, they just want to play their thrice-damned "p1r473 R0MZ".
     */
    if (page < mask) return page;
    return (page & (mask - 1));
}

void nesppu_map_1k(nes_ppu ppu, int bank, int page)
{
    int masked_page;
    int num_banks;
    u8 *mem_base;
    u8 *cache_base;

    if (ppu->chr_is_rom) {
	num_banks = ppu->romfile->chr_size << 3;
	mem_base = ppu->romfile->chr_data;
	cache_base = ppu->tilecache;
    } else {
	num_banks = ppu->pageram_size << 3;
	mem_base = ppu->pageram_memory;
	cache_base = ppu->pageram_tilecache;
    }
    
    if (!num_banks) {
	return;
    }
    
    masked_page = nesppu_mask_page_to(page, num_banks);
    PPU_banks[bank] = mem_base + (masked_page * 0x400);
    ppu->pattern_cache[bank] = cache_base + (masked_page * 0x400 * 4);
}

void nesppu_map_2k(nes_ppu ppu, int bank, int page)
{
    nesppu_map_1k(ppu, (bank << 1) + 0, (page << 1) + 0);
    nesppu_map_1k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_map_4k(nes_ppu ppu, int bank, int page)
{
    nesppu_map_2k(ppu, (bank << 1) + 0, (page << 1) + 0);
    nesppu_map_2k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_map_8k(nes_ppu ppu, int bank, int page)
{
    nesppu_map_4k(ppu, (bank << 1) + 0, (page << 1) + 0);
    nesppu_map_4k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_paged_ram_init(int num_8k_pages)
{
    nes_ppu_true->pageram_memory = malloc(num_8k_pages * 0x2000);
    nes_ppu_true->pageram_tilecache = malloc(num_8k_pages * 0x8000);
    nes_ppu_true->pageram_size = num_8k_pages;
}

void nesppu_paged_ram_mode(int enabled)
{
    if (enabled) {
	nes_ppu_true->chr_is_rom = 0;
    } else {
	nes_ppu_true->chr_is_rom = nes_ppu_true->romfile->chr_size;
	if (!nes_ppu_true->chr_is_rom) {
	    nes_ppu_true->pattern_cache[0] = nes_ppu_true->tilecache + 0x0000;
	    nes_ppu_true->pattern_cache[1] = nes_ppu_true->tilecache + 0x1000;
	    nes_ppu_true->pattern_cache[2] = nes_ppu_true->tilecache + 0x2000;
	    nes_ppu_true->pattern_cache[3] = nes_ppu_true->tilecache + 0x3000;
	    nes_ppu_true->pattern_cache[4] = nes_ppu_true->tilecache + 0x4000;
	    nes_ppu_true->pattern_cache[5] = nes_ppu_true->tilecache + 0x5000;
	    nes_ppu_true->pattern_cache[6] = nes_ppu_true->tilecache + 0x6000;
	    nes_ppu_true->pattern_cache[7] = nes_ppu_true->tilecache + 0x7000;
	}
    }
}

void PPU_mirror_horizontal()
{
    PPU_nametables[0] = &(PPU_memory[0x2000]);
    PPU_nametables[1] = &(PPU_memory[0x2000]);
    PPU_nametables[2] = &(PPU_memory[0x2400]);
    PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_mirror_vertical()
{
    PPU_nametables[0] = &(PPU_memory[0x2000]);
    PPU_nametables[1] = &(PPU_memory[0x2400]);
    PPU_nametables[2] = &(PPU_memory[0x2000]);
    PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_mirror_one_low()
{
    PPU_nametables[0] = &(PPU_memory[0x2000]);
    PPU_nametables[1] = &(PPU_memory[0x2000]);
    PPU_nametables[2] = &(PPU_memory[0x2000]);
    PPU_nametables[3] = &(PPU_memory[0x2000]);
}

void PPU_mirror_one_high()
{
    PPU_nametables[0] = &(PPU_memory[0x2400]);
    PPU_nametables[1] = &(PPU_memory[0x2400]);
    PPU_nametables[2] = &(PPU_memory[0x2400]);
    PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_write(nes_ppu ppu, unsigned short address, unsigned char value)
{
    address = address & 0x3fff;
    
    if (ppu_latchfunc) {
	ppu_latchfunc(ppu->mapper, address);
    }
    
    if (address >= 0x3f00) {
	u8 screen_color;

	screen_color = vid_pre_xlat[value & 0x3f];
	
	if ((address & 0x3f03) == 0x3f00) {
	    ppu->palette[0x00 + (address & 0x0f)] = value;
	    ppu->palette[0x10 + (address & 0x0f)] = value;
	    ppu->render_palette->set(ppu->render_palette,
				     0x00 + (address & 0x0f), value);
	    ppu->render_palette->set(ppu->render_palette,
				     0x10 + (address & 0x0f), value);
	} else {
	    ppu->palette[address & 0x1f] = value;
	    ppu->render_palette->set(ppu->render_palette,
				     address & 0x1f, value);
	}
    } else if (address > 0x1fff) {
	PPU_nametables[(address >> 10) & 3][address & 0x3ff] = value;
    } else if (!nes_ppu_true->chr_is_rom) {
	unsigned char *cur_bank;
	unsigned char *cache_bank;

	cur_bank = PPU_banks[(address >> 10)];
	cache_bank = ppu->pattern_cache[address >> 10];
	
	cur_bank[(address & 0x3ff)] = value;
	nesppu_cache_tile_line(cur_bank[(address & 0x3f7)], cur_bank[(address & 0x3ff) | 8], cache_bank + ((address & 0x3f0) >> 1) + ((address & 7) << 9));
    }
}

unsigned char PPU_read(nes_ppu ppu, unsigned short address)
{
    unsigned char value;
    
    address &= 0x3fff;

    if (ppu_latchfunc) {
	ppu_latchfunc(ppu->mapper, address);
    }
    
    if (address >= 0x3f00) {
	value = ppu->palette[address & 0x1f];
    } else if (address >= 0x2000) {
	value = PPU_nametables[(address >> 10) & 3][address & 0x3ff];
    } else {
        value = PPU_banks[(address >> 10)][(address & 0x3ff)];
    }
    
    return value;
}

struct spritedata {
    unsigned char vpos;
    unsigned char tile;
    unsigned char flags;
    unsigned char xpos;
};

struct {
    unsigned char xpos;
    unsigned char pattern0;
    unsigned char pattern8;
    unsigned char flags;
    u8 *pattern;
} spritecache[8];

#define SFL_VFLIP 0x80
#define SFL_HFLIP 0x40
#define SFL_PRIO  0x20
#define SFL_COLOR 0x03

int num_sprites_line;

void PPU_init_spritecache(nes_ppu ppu, unsigned short line, int is_8x16)
{
    int i;
    struct spritedata *sprites;
    unsigned char cur_tile;
    unsigned char cur_pattern_line;
    unsigned char *cur_pattern;
    int cur_ptbl;

    sprites = (struct spritedata *)PPU_sprite_ram;
    
    num_sprites_line = 0;

    cur_ptbl = (ppu->control_1 & 0x08)? 1: 0;
    
    /* FIXME: clear sprite count */
    
    for (i = 0; i < 64; i++) {
	if ((sprites[i].vpos + 1 <= line) &&
	    (sprites[i].vpos + (is_8x16? 17: 9) > line)) {
	    if (num_sprites_line > 7) {
		/* FIXME: set sprite count */
		break;
	    }
	    
	    spritecache[num_sprites_line].xpos = sprites[i].xpos;
	    spritecache[num_sprites_line].flags = sprites[i].flags;
	    
	    cur_pattern_line = (line - (sprites[i].vpos+1)) & (is_8x16? 15: 7);
	    if (sprites[i].flags & SFL_VFLIP) {
		cur_pattern_line = (is_8x16? 15: 7) - cur_pattern_line;
	    }
	    
	    cur_tile = sprites[i].tile;
	    if (is_8x16) {
		cur_ptbl = cur_tile & 1;
		cur_tile = (cur_pattern_line & 8)?
		    (cur_tile | 1): (cur_tile & 0xfe);
		cur_pattern_line &= 7;
	    }
	    
	    if (ppu_latchfunc) {
		ppu_latchfunc(ppu->mapper, (cur_ptbl << 12) | (cur_tile << 4) | (cur_pattern_line));
		ppu_latchfunc(ppu->mapper, (cur_ptbl << 12) | (cur_tile << 4) | (cur_pattern_line) | 8);
	    }
	    
	    cur_pattern = &(PPU_patterns[cur_ptbl][(cur_tile >> 6)]
			    [((cur_tile & 0x3f) << 4) + cur_pattern_line]);
	    
	    spritecache[num_sprites_line].pattern0 = cur_pattern[0];
	    spritecache[num_sprites_line].pattern8 = cur_pattern[8];

	    spritecache[num_sprites_line].pattern =
		&(ppu->pattern_cache[(cur_ptbl << 2) + (cur_tile >> 6)]
		  [((cur_tile & 0x3f) << 3) + (cur_pattern_line << 9)]);
	    
	    if ((i == 0) && ((cur_pattern[0]) || cur_pattern[8])) {
		ppu->status |= 0x40;
	    }
	    
	    num_sprites_line++;
	}
    }
}

void spritedraw_8(u8 *vbp, int offset, u8 *pattern, u8 *palette)
{
    int i;
    u8 *cur_vbp;
    u8 *tiledata;

    cur_vbp = vbp + offset;
    tiledata = pattern;
    
    for (i = 0; i < 8; i++) {
	if (*tiledata) {
	    *cur_vbp = palette[*tiledata];
	}

	tiledata++;
	cur_vbp++;
    }
}

void spritedraw_rev_8(u8 *vbp, int offset, u8 *pattern, u8 *palette)
{
    int i;
    u8 *cur_vbp;
    u8 *tiledata;

    cur_vbp = vbp + offset;
    tiledata = pattern + 7;
    
    for (i = 0; i < 8; i++) {
	if (*tiledata) {
	    *cur_vbp = palette[*tiledata];
	}

	tiledata--;
	cur_vbp++;
    }
}

void PPU_render_sprites(nes_ppu ppu, unsigned char *cur_vbp, int behind)
{
    int i;
    unsigned char *colors;

    if (!num_sprites_line) {
	return;
    }
    
    for (i = (num_sprites_line - 1); i >= 0; i--) {
	if (behind? !(spritecache[i].flags & SFL_PRIO):
	    (spritecache[i].flags & SFL_PRIO)) {
	    continue;
	}
	
	colors = ppu->render_palette->palettes[4 + (spritecache[i].flags & SFL_COLOR)];
	
	if (!(spritecache[i].flags & SFL_HFLIP)) {
	    spritedraw_8(cur_vbp, spritecache[i].xpos, spritecache[i].pattern, colors);
	} else {
	    spritedraw_rev_8(cur_vbp, spritecache[i].xpos, spritecache[i].pattern, colors);
	}
    }
}

void nesppu_cache_background(nes_ppu ppu, unsigned char **patterns, unsigned char **attrs)
{
    unsigned char *cur_tile;
    unsigned char tile_offset;
    unsigned char *cur_bank;
    unsigned char *cur_attrline;
    unsigned short cur_patternline;
    unsigned char cur_attrbase;
    unsigned char **cur_ptbl;
    int i;
    
    cur_patternline = (ppu->refresh_data >> 3) & 0x0e00;
    cur_attrbase = (ppu->refresh_data >> 4) & 4;
    cur_ptbl = &ppu->pattern_cache[(ppu->control_1 & 0x10) >> 2];
    
    cur_bank = PPU_nametables[(ppu->refresh_data >> 10) & 3];
    tile_offset = ppu->refresh_data & 0x1f;
    cur_tile = &cur_bank[(ppu->refresh_data & 0x3e0) + tile_offset];
    cur_attrline = cur_bank + ((ppu->refresh_data >> 4) & 0x38) + 0x3c0 + (tile_offset >> 2);
    
    cur_attrbase |= tile_offset & 2;
    
    for (i = 0; i < 33; i++) {
	patterns[i] = &(cur_ptbl[(*cur_tile >> 6)]
			[((*cur_tile & 0x3f) << 3) + cur_patternline]);
	
	if (ppu_latchfunc) {
	    ppu_latchfunc(ppu->mapper, ((ppu->control_1 & 0x10) << 8) | ((*cur_tile) << 4) | cur_patternline);
	    ppu_latchfunc(ppu->mapper, ((ppu->control_1 & 0x10) << 8) | ((*cur_tile) << 4) | cur_patternline | 8);
	}
	
	attrs[i] = ppu->render_palette->palettes[(*cur_attrline >> cur_attrbase) & 3];
	
	if (tile_offset & 1) {
	    cur_attrbase ^= 2;
	}
	
	if ((tile_offset & 3) == 3) {
	    cur_attrline++;
	}
	
	cur_tile++;
	tile_offset++;
	if (tile_offset > 31) {
	    tile_offset = 0;
	    cur_bank = PPU_nametables[((ppu->refresh_data >> 10) & 3) ^ 1];
	    cur_tile = &cur_bank[ppu->refresh_data & 0x3e0];
	    cur_attrline = cur_bank + ((ppu->refresh_data >> 4) & 0x38) + 0x3c0;
	}
    }
}

void PPU_render_background(nes_ppu ppu, char *cur_vbp)
{
    unsigned char *patterns[33];
    unsigned char *attrs[33];
    int i;

    nesppu_cache_background(ppu, patterns, attrs);
    
    if (ppu->control_2 & 0x02) {
	i = 0;
    } else {
	for (i = 0; i < 8; i++) {
	    *cur_vbp++ = vid_pre_xlat[0x0f];
	}
	i = 1;
    }

    tiledraw_8(cur_vbp, patterns + i, attrs + i, 32 - i, ppu->finescroll);
}

void floodfill_8(u8 *vbp, int width, u8 *palette, int index)
{
    memset(vbp, palette[index], width);
}
int tframeskip=1;
int tflip=0;
extern int roffset_x;
extern int roffset_y;
extern int H;
u32 lastframe=0;
u32 now=0;
u32 avgframe=0x3000;
void PPU_render_scan_line(nes_ppu ppu)
{

    if (ppu->scanline==0)
    {
    	    frameskip++;
	    if (frameskip>tframeskip) { frameskip=0; }
//	    if (tflip==0) { tflip=1; tframeskip=1; } else { tflip=0; tframeskip=2; } } 
	    lastframe=inl(0x60005010);
    }
    if (ppu->scanline==239) 
    {
	    now=inl(0x60005010);
	    avgframe=(avgframe+now-lastframe)>>1;
	    if (avgframe>0x343f) tframeskip++; else tframeskip--;
	    if (tframeskip<1) tframeskip=1;
	    if (tframeskip>2) tframeskip=2;
    }
//    if (ppu->scanline==0) { frameskip=(frameskip+1); if (frameskip>2) frameskip=0; }
//    if ((frameskip==0) && ((ppu->scanline & 1)==0))
    if ((frameskip==0)) // && (ppu->scanline>=roffset_y) && (ppu->scanline<roffset_y+H))
    {
    //while ((inl(COP_STATUS) & COP_LINE_REQ) != 0) {} //outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS); 

    unsigned char *cur_vbp;
    
    cur_vbp = video_get_vbp(ppu->scanline);
    
    floodfill_8(cur_vbp, 256, ppu->render_palette->palettes[0], 0);
    
    if (ppu->control_2 & 0x10) {
	PPU_init_spritecache(ppu, ppu->scanline, ppu->control_1 & 0x20);
	PPU_render_sprites(ppu, cur_vbp, 1);
    }
    if (ppu->control_2 & 0x08) {
	PPU_render_background(ppu, cur_vbp);
    }
    if (ppu->control_2 & 0x10) {
	PPU_render_sprites(ppu, cur_vbp, 0);
    }
    } 
    
    ppu->refresh_data += 0x1000;
    if (ppu->refresh_data & 0x8000) {
	unsigned short tmp;
	tmp = (ppu->refresh_data & 0x03e0) + 0x20;
	ppu->refresh_data &= 0x7c1f;
	if (tmp == 0x03c0) {
	    ppu->refresh_data ^= 0x0800;
	} else {
	    ppu->refresh_data |= (tmp & 0x03e0);
	}
    }
    
}
extern int updatelr;
extern int cycles_left;
void ppu_io_write(nes_ppu ppu, unsigned short addr, unsigned char value)
{
    switch (addr & 7) {
    case 0:
	ppu->control_1 = value;
	ppu->refresh_temp &= 0x73ff;
	ppu->refresh_temp |= (value & 3) << 10;
	break;
    case 1:
	ppu->control_2 = value;
	break;
    case 2:
	deb_printf("ppu_io_write(2, %02x): Unknown register.\n", value);
	break;
    case 3:
	ppu->sprite_addy = value;

	break;
    case 4:
	updatelr=0;
	PPU_sprite_ram[(ppu->sprite_addy++) & 0xff] = value;
	break;
    case 5:
	if (PPU_amode & 1) {
	    PPU_amode &= ~1;
	    ppu->refresh_temp &= 0x0c1f;
	    ppu->refresh_temp |= (value & 7) << 12;
	    ppu->refresh_temp |= (value << 2) & 0x03e0;
	} else {
	    PPU_amode |= 1;
	    ppu->refresh_temp &= 0x7fe0;
	    ppu->refresh_temp |= value >> 3;
	    ppu->finescroll = value & 7;
	}
	break;
    case 6:
	if (PPU_amode & 1) {
	    PPU_amode &= ~1;
	    ppu->address = (ppu->address_latch << 8) | value;

	    ppu->refresh_temp &= 0xff00;
	    ppu->refresh_temp |= value;
	    ppu->refresh_data = ppu->refresh_temp;
	} else {
	    PPU_amode |= 1;
	    ppu->address_latch = value;

	    ppu->refresh_temp &= 0x00ff;
	    ppu->refresh_temp |= (value & 0x3f) << 8;
	}
	break;
    case 7:
	if (PPU_amode & 1) {
	    deb_printf("ppu_w_2007(): ppu write during addr sequence.\n");
	}
	PPU_write(ppu, ppu->address, value);
	ppu->address += ((ppu->control_1 & 0x04)? 0x20: 1);
	break;
    }
}

unsigned char ppu_io_read(nes_ppu ppu, unsigned short addr)
{
    unsigned char retval;
    
    switch (addr & 7) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6:
	/* NOTE: to the best of my knowledge, these ports are synonymous */
	retval = ppu->status;
	
	ppu->status &= 0x7f;
	PPU_amode &= ~1;
	return retval;
	break;
    case 7:
	retval = ppu->read_latch;
	ppu->read_latch = PPU_read(ppu, ppu->address);
	ppu->address += ((ppu->control_1 & 0x04)? 0x20: 1);
	return retval;
	break;
    }
    return 0; /* gcc -O2 is a chode. */
}

u8 ppu_readreg(u32 addr)
{
	return ppu_io_read(nes_ppu_true,addr);
}
void ppu_writereg(u32 addr,u32 value)
{
	ppu_io_write(nes_ppu_true,addr,value);
}
/*
 * $Log: nes_ppu.c,v $
 * Revision 1.112  2001/03/06 06:02:38  nyef
 * "fixed" nesppu_mask_page_to() to not mask pages if page is < mask
 *
 * Revision 1.111  2000/10/09 16:37:55  nyef
 * added preliminary support for paged CHR RAM
 *
 * Revision 1.110  2000/10/05 08:45:13  nyef
 * moved PPU_memory from nes.c to nes_ppu.c
 *
 * Revision 1.109  2000/10/05 08:37:45  nyef
 * moved sprite DMA handling from nes_ppu.c to nes.c
 *
 * Revision 1.108  2000/10/02 14:18:29  nyef
 * removed call to nes_psg_vsync() (part of the psg rewrite)
 *
 * Revision 1.107  2000/09/16 22:49:21  nyef
 * added mirror_vertical field to nes_rom structure
 *
 * Revision 1.106  2000/08/04 23:33:38  nyef
 * moved the generic palette code out to palette.[ch]
 *
 * Revision 1.105  2000/07/26 01:26:13  nyef
 * changed the CHR ROM switching code to use guard clauses
 *
 * Revision 1.104  2000/07/26 01:23:30  nyef
 * hacked out the beginnings of a generic palette system
 *
 * Revision 1.103  2000/06/07 00:34:59  nyef
 * removed the call to ppu->mapper->vsync() in nesppu_periodic()
 *
 * Revision 1.102  2000/06/03 04:26:07  nyef
 * fixed palette mirroring
 *
 * Revision 1.101  2000/04/29 15:58:29  nyef
 * cleaned up the sprite renderer slightly
 *
 * Revision 1.100  2000/02/27 15:03:52  nyef
 * cleaned up the definition of struct nes_ppu
 * moved the address, address latch, and read latch to struct nes_ppu
 *
 * Revision 1.99  2000/02/27 06:16:40  nyef
 * removed (disused) ppu_flags variable
 *
 * Revision 1.98  2000/02/27 06:10:54  nyef
 * reduced number of sprites per line to the correct value of 8
 * cleaned up the sprite renderer slightly
 *
 * Revision 1.97  2000/02/27 06:01:00  nyef
 * cleaned up mirroring implementation
 * cleaned up memory access to nametable areas
 *
 * Revision 1.96  2000/02/05 19:46:58  nyef
 * cleared ppu status at end of vblank (fixes bigfoot)
 *
 * Revision 1.95  1999/12/04 04:38:07  nyef
 * removed inclusion of debug.h
 *
 * Revision 1.94  1999/11/26 01:28:36  nyef
 * fixed typos in nesppu_map_4k() and nesppu_map_8k()
 *
 * Revision 1.93  1999/11/23 03:21:40  nyef
 * changed to use new tiledraw interface
 *
 * Revision 1.92  1999/11/23 01:41:48  nyef
 * changed to use tile renderer from tiledraw.c
 *
 * Revision 1.91  1999/11/21 17:23:14  nyef
 * moved palette stuff in from nes.c
 *
 * Revision 1.90  1999/11/20 18:20:20  nyef
 * fixed typo in nesppu_map_8k()
 *
 * Revision 1.89  1999/11/20 18:10:23  nyef
 * fixed stupid mistake with CHR-RAM tile cache
 *
 * Revision 1.88  1999/11/20 17:44:01  nyef
 * fixed tile cache to work with CHR-RAM games
 *
 * Revision 1.87  1999/11/20 16:53:33  nyef
 * minor strength reduction in nesppu_cache_background()
 *
 * Revision 1.86  1999/11/20 16:36:18  nyef
 * added preliminary tile cache (only works on CHR-ROM games)
 *
 * Revision 1.85  1999/11/20 05:38:10  nyef
 * rebuilt romfile handling
 * rebuilt chr-rom mapping to automatically handle page out of range conditions
 * rebuilt chr-rom mapping to ignore mapping when using chr-ram
 *
 * Revision 1.84  1999/11/16 03:11:30  nyef
 * extracted nesppu_cache_background() from PPU_render_background()
 *
 * Revision 1.83  1999/11/16 03:06:52  nyef
 * changed PPU_render_background() to defer fetching the pattern data
 *     until actually blitting the data
 *
 * Revision 1.82  1999/11/16 02:36:22  nyef
 * removed PPU_bank_XX and simplified the code that depended on it
 *
 * Revision 1.81  1999/11/15 04:05:04  nyef
 * implemented preliminary version of new CHR ROM mapping interface
 *
 * Revision 1.80  1999/11/14 16:20:14  nyef
 * fixed latch function interface to work with new mapper interface
 *
 * Revision 1.79  1999/11/14 07:00:18  nyef
 * changed to use new mapper interface
 *
 * Revision 1.78  1999/11/14 04:12:56  nyef
 * moved some global variables into struct nes_ppu
 *
 * Revision 1.77  1999/11/02 02:19:07  nyef
 * took out change of revision 1.75 because it broke the snow demo
 *
 * Revision 1.76  1999/10/21 23:35:59  nyef
 * changed to use new mapper hblank interface
 *
 * Revision 1.75  1999/08/23 00:57:56  nyef
 * fixed NMI generation to only occur if the vblank flag hasn't been cleared.
 *
 * Revision 1.74  1999/08/20 02:25:52  nyef
 * fixed mapper hsync routine to be called 240 times per frame
 *
 * Revision 1.73  1999/07/18 01:24:05  nyef
 * fixed to use the same flag for writes to both $2005 and $2006
 *
 * Revision 1.72  1999/06/19 15:58:42  nyef
 * rebuilt the palette code
 *
 * Revision 1.71  1999/06/19 14:54:15  nyef
 * cleaned out a lot of dead code
 *
 * Revision 1.70  1999/06/19 14:10:48  nyef
 * added new refresh system
 *
 * Revision 1.69  1999/05/29 22:31:36  nyef
 * started reorganizing the NES PPU
 *
 * Revision 1.68  1999/05/02 18:12:09  nyef
 * removed some unused debug code
 * cleaned up the ppu memory writes a little
 *
 * Revision 1.67  1999/04/11 22:53:48  nyef
 * removed most of the macros involved in sprite rendering in favor of
 * a structure
 *
 * Revision 1.66  1999/04/11 18:20:54  nyef
 * based on tip from fx3, had reads from $2002 and family clear
 * ppu_scrollmode and ppu_amode.
 *
 * Revision 1.65  1999/02/16 01:52:18  nyef
 * removed useless reference to io_nes.h
 * removed dead i/o code formerly used by io_nes.S
 *
 * Revision 1.64  1999/02/14 18:30:07  nyef
 * moved typedef for nes_ppu to nes_ppu.h
 *
 * Revision 1.63  1999/02/07 17:06:51  nyef
 * changed to use video_setsize()
 *
 * Revision 1.62  1999/02/06 03:37:52  nyef
 * removed call to video_events()
 *
 * Revision 1.61  1999/01/26 02:47:30  nyef
 * disabled display of ppu mirroring mode.
 *
 * Revision 1.60  1999/01/24 05:19:02  nyef
 * changed ppu_io_write so that a mid-hblank write to $2000 can only
 * affect the horizontal nametable selection.
 *
 * Revision 1.59  1999/01/18 04:30:54  nyef
 * reorganized i/o emulation in preparation for removal of io_nes.S
 *
 * Revision 1.58  1999/01/11 02:23:17  nyef
 * changed to use blitters from blitters.c instead of nes_blit.c.
 *
 * Revision 1.57  1998/12/15 01:50:51  nyef
 * changed mapper hsync and vsync routines to conform to new spec.
 *
 * Revision 1.56  1998/12/12 03:28:32  nyef
 * changed ppu_r_2002() to clear the VBlank bit after reading ppu_status.
 *
 * Revision 1.55  1998/12/11 04:33:40  nyef
 * began fixing for full compatability with cal.
 *
 * Revision 1.54  1998/12/11 02:50:57  nyef
 * removed mid-hblank write detection logic from ppu_w_2006().
 *
 * Revision 1.53  1998/12/09 04:36:46  nyef
 * put the vblank length back down to what I originally worked it out as.
 * also changed some of the vblank timing signals.
 *
 * Revision 1.52  1998/12/04 02:18:51  nyef
 * in ppu_render_background(), moved call of ppu_latchfunc() to after
 * obtaining the address of the tile pattern. this fixed the right side
 * of the windows in fire emblem (mapper 10).
 *
 * Revision 1.51  1998/11/28 06:32:41  nyef
 * removed some superfluous code from PPU_render_background().
 *
 * Revision 1.50  1998/11/28 05:14:08  nyef
 * changed ppu_render_scan_line() to use memset to clear the video buffer.
 * Last time this was done, one of the clears was missed.
 * Also cleaned out some commented out code in ppu_render_scan_line().
 *
 * Revision 1.49  1998/11/05 17:02:33  nyef
 * performed strength reduction on tile attribute calculation
 * in ppu_render_background().
 *
 * Revision 1.48  1998/11/05 16:55:57  nyef
 * changed ppu_render_background() to obtain nice speedup.
 *
 * Revision 1.47  1998/11/05 15:17:20  nyef
 * simplified (slightly) the invocation of ppu_latchfunc() in
 * ppu_render_background().
 *
 * Revision 1.46  1998/11/05 04:05:05  nyef
 * changed ppu_render_scan_line() to use memset to clear the video buffer.
 *
 * Revision 1.45  1998/11/01 21:28:10  nyef
 * doubled vsync time again. Bucky O' Hare now works properly.
 *
 * Revision 1.44  1998/11/01 21:17:34  nyef
 * re-added the assignment to ppu_saved_nametable in ppu_enter_cpu().
 * also added some debugging code to ppu_w_2007().
 *
 * Revision 1.43  1998/11/01 18:18:35  nyef
 * added support for left-clipped background.
 *
 * Revision 1.42  1998/11/01 14:17:43  nyef
 * moved assignment to ppu_saved_nametable from ppu_enter_cpu()
 * to ppu_w_2000().
 *
 * Revision 1.41  1998/11/01 03:09:45  nyef
 * changed ppu_saved_hscrol, ppu_saved_vscrol, and ppu_saved_nametable
 * from int to unsigned char.
 *
 * Revision 1.40  1998/10/31 04:36:39  nyef
 * removed cur_attr from ppu_render_background.
 *
 * Revision 1.39  1998/10/31 04:02:12  nyef
 * re-added (preliminary) support for ppu reads.
 *
 * Revision 1.38  1998/10/31 03:55:32  nyef
 * removed some useless code from ppu_render_background()
 *
 * Revision 1.37  1998/10/29 03:29:23  nyef
 * fixed minor mistake in ppu_render_background().
 *
 * Revision 1.36  1998/10/29 03:17:34  nyef
 * Added preliminary support for Mid-HBlank PPU address writes.
 * There are still some graphics issues, but Crystalis and Kirby
 * are both playable.
 *
 * Revision 1.35  1998/10/29 01:29:07  nyef
 * moved all PPU I/O access in from io_nes.S.
 * access to $2007 is unimplimented.
 *
 * Revision 1.34  1998/10/22 02:02:59  nyef
 * moved ppu_cntrl1, ppu_cntrl2, ppu_status, ppu_spraddy, ppu_vscrol,
 * ppu_hscrol, ppu_scrollmode, PPU_addr and PPU_amode in from io_nes.S
 *
 * Revision 1.33  1998/10/19 13:07:33  nyef
 * added variable ppu_saved_vscrol to prevent vscroll outside of vblank.
 *
 * Revision 1.32  1998/10/03 20:38:43  nyef
 * changed PPU_write() to be more selective about palette values.
 *
 * Revision 1.31  1998/10/01 02:33:52  nyef
 * added preliminary support for latches as per mappers 9 and 10
 *
 * Revision 1.30  1998/09/26 13:58:14  nyef
 * fixed bug in PPU_dump_sprite_memory. (fixes behind/before display)
 *
 * Revision 1.29  1998/09/20 21:26:23  nyef
 * added two new debug hooks. (PPU_dump_sptite_memory and PPU_check_sprites)
 *
 * Revision 1.28  1998/09/12 19:09:43  nyef
 * added a check in the mirror control code so as to not spam debug
 * messages about setting the mirror state to the current one.
 *
 * Revision 1.27  1998/09/08 01:51:43  nyef
 * cleaned up the right-clip blit case in PPU_render_background.
 * more cleanup of the left-clip blit case in PPU_render_background.
 *
 * Revision 1.26  1998/09/08 01:35:04  nyef
 * cleaned up the left-clip blit cases in PPU_render_background.
 *
 * Revision 1.25  1998/09/08 01:09:08  nyef
 * minor bugfixes in PPU_write. (it wasn't handling palette writes correctly)
 *
 * Revision 1.24  1998/08/31 01:35:14  nyef
 * found that some games work better when PPU_banks is changed with
 * the mirroring.
 *
 * Revision 1.23  1998/08/30 20:36:19  nyef
 * added a call to the psg emulation on every vsync.
 *
 * Revision 1.22  1998/08/30 17:04:50  nyef
 * removed PPU_use_vrom and PPU_ntsc in favor of a more general ppu_flags
 * system. also added flags for nametable and patterntable modifications.
 *
 * Revision 1.21  1998/08/30 16:41:37  nyef
 * added a check of scrollflag in ppu_render_background.
 * this fixes some vscroll problems.
 *
 * Revision 1.20  1998/08/30 16:02:09  nyef
 * changed the mirroring to use a separate set of pointers so as to not
 * mess up the normal PPU memory map.
 *
 * Revision 1.19  1998/08/30 03:45:29  nyef
 * cleaned up some stuff left from the removal of the video buffer.
 * made a change to the way palette writes are handled in PPU_write().
 *
 * Revision 1.18  1998/08/29 22:10:33  nyef
 * moved control of all video buffers to the video interface.
 *
 * Revision 1.17  1998/08/28 03:06:16  nyef
 * changed PPU_write and the renderers to do some of the color translation.
 *
 * Revision 1.16  1998/08/26 02:38:20  nyef
 * added an additional indirection layer around the pattern tables.
 * used this to slightly clean up the renderers.
 *
 * Revision 1.15  1998/08/26 01:33:12  nyef
 * changed sprite dma to use memcpy(3)s instead of a for loop.
 * (loop invariant code motion)
 *
 * Revision 1.14  1998/08/23 22:42:22  nyef
 * added hooks for calling mappers on vsync and hsync.
 *
 * Revision 1.13  1998/08/16 20:42:58  nyef
 * clarified (and sped up) the behavior of the first for loop in
 * PPU_render_background(). (It still needs more work).
 *
 * Revision 1.12  1998/08/16 12:05:47  nyef
 * fixed bug with VFlipped 8x16 sprites.
 *
 * Revision 1.11  1998/08/15 17:30:43  nyef
 * added 8x16 sprite support.
 *
 * Revision 1.10  1998/08/15 15:36:29  nyef
 * changed PPU_render_sprites() to take advantage of the new reversed
 *   blitters. also clarified the behavior of the renderer.
 *
 * Revision 1.9  1998/08/15 05:00:04  nyef
 * changed PPU_render_background to take advantage of the new
 *   left-side blitters
 *
 * Revision 1.8  1998/08/15 04:16:20  nyef
 * changed PPU_render_background to take advantage of the new
 *   right-side blitters
 *
 * Revision 1.7  1998/08/02 13:37:31  nyef
 * changed every printf to deb_printf for debug console.
 *
 * Revision 1.6  1998/07/25 16:56:29  nyef
 * changed PPU_render_background to use the new blit compiler.
 *
 * Revision 1.5  1998/07/22 02:42:51  nyef
 * added default case to switch statement in PPU_render_background(). oops.
 * clarified (and sped up) the behavior of the first for loop in PPU_render_background(). (It still doesn't make as much sense as it should)
 *
 * Revision 1.4  1998/07/19 04:02:21  nyef
 * fixed the hscroll bug in blaster master (hopefully for the last time)
 *
 * Revision 1.3  1998/07/19 03:32:37  nyef
 * fixed sprite priority by inverting the draw order
 *
 * Revision 1.2  1998/07/18 21:43:17  nyef
 * added support for one-screen mirroring
 *
 * Revision 1.1  1998/07/11 22:18:17  nyef
 * Initial revision
 *
 */
