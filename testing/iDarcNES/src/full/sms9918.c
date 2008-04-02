/*
 * sms9918.c
 *
 * sms custom VDP emulation.
 */

/* $Id: sms9918.c,v 1.57 2000/08/26 00:26:19 nyef Exp $ */

#include <stdlib.h>
#include <string.h>
#include "sms9918.h"
#include "ui.h"
#include "video.h"
#include "blitters.h"
#include "tiledraw.h"
#include "types.h"

/* manifest constants and flag defines */

#define SMS_VDP_RAMSIZE 0x4000

#define TF_ADDRWRITE 1
#define TF_GAMEGEAR 2


/* tilecache stuff */

u8 sms9918_tilecache[0x8000];
u8 sms9918_tilecache_rev[0x8000];
void sms9918_cache_tile(sms9918 vdp, u16 addr);


/* namecache stuff */

struct sms9918_namecache {
    u8 *front_tile;
    u8 *back_tile;
    u8 *palette;
    int is_vflip;
} sms9918_namecache[0x2000];
void sms9918_cache_name(sms9918 vdp, u16 addr);


/* vdp data structure */

struct sms9918 {
    u8 flags;
    u8 readahead;
    u8 addrsave;
    u8 status;
    u8 *memory;
    u8 regs[16];
    u8 palette[64];
    u8 palette_xlat[32];
    u16 address;
    u16 cur_scanline;
    u8 linecounter;
    
    int coarsescroll;
    int finescroll;
    struct sms9918_namecache *nametable;
};


/* Palette definition */

int sms_palbase_red[64] = {
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

int sms_palbase_green[64] = {
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
};

int sms_palbase_blue[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};


/* port 0 (data) emulation */

u8 sms9918_readport0(sms9918 vdp)
{
    u8 retval;

    if (vdp->address & 0x4000) {
	if (!(vdp->flags & TF_GAMEGEAR)) {
	    retval = vdp->palette[(vdp->address++) & 0x1f];
	    vdp->address &= 0x401f;
	} else {
	    retval = vdp->palette[(vdp->address++) & 0x3f];
	    vdp->address &= 0x403f;
	}
    } else {
	retval = vdp->readahead;
	vdp->readahead = vdp->memory[vdp->address++];
	vdp->address &= 0x3fff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void sms9918_writeport0(sms9918 vdp, u8 data)
{
    if (vdp->address & 0x4000) {
	if (!(vdp->flags & TF_GAMEGEAR)) {
	    vdp->palette_xlat[(vdp->address) & 0x1f] = vid_pre_xlat[data&0x3f];
	    vdp->palette[(vdp->address++) & 0x1f] = data;
	    vdp->address &= 0x401f;
	} else {
	    u8 tmp;
	    vdp->palette[(vdp->address) & 0x3f] = data;

	    tmp = (vdp->palette[((vdp->address) & 0x3f) | 1] << 2) & 0x30;
	    tmp |= (vdp->palette[(vdp->address) & 0x3e] >> 4) & 0x0c;
	    tmp |= (vdp->palette[(vdp->address) & 0x3e] >> 2) & 0x03;

	    vdp->palette_xlat[((vdp->address) >> 1) & 0x1f] = vid_pre_xlat[tmp];
	    vdp->address++;
	    vdp->address &= 0x403f;
	}
    } else {
	vdp->readahead = data;
	vdp->memory[vdp->address] = data;
	sms9918_cache_tile(vdp, vdp->address);
	sms9918_cache_name(vdp, vdp->address);
	vdp->address++;
	vdp->address &= 0x3fff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
}


/* port 1 (status/address/palette/registers) emulation */

u8 sms9918_readport1(sms9918 vdp)
{
    u8 retval;

    retval = vdp->status;
    vdp->status &= 0x3f;
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void sms9918_write_register(sms9918 vdp, int reg, u8 data)
{
    vdp->regs[reg] = data;
    if (reg == 2) {
	vdp->nametable = &sms9918_namecache[(data & 14) << 9];
    } else if (reg == 8) {
	vdp->coarsescroll = (32 - ((data + 7) >> 3)) & 0x1f;
	vdp->finescroll = (8 - (data & 7)) & 7;
    }
}

void sms9918_writeport1(sms9918 vdp, u8 data)
{
    if (vdp->flags & TF_ADDRWRITE) {
	if (data & 0x80) {
	    if (data & 0x40) {
		if (!(vdp->flags & TF_GAMEGEAR)) {
		    vdp->address = (vdp->addrsave | (data << 8)) & 0x401f;
		} else {
		    vdp->address = (vdp->addrsave | (data << 8)) & 0x403f;
		}
	    } else {
		sms9918_write_register(vdp, data & 15, vdp->addrsave);
	    }
	} else {
	    vdp->address = (vdp->addrsave | (data << 8)) & 0x3fff;
	    if (!(data & 0x40)) {
		vdp->readahead = vdp->memory[vdp->address++];
		vdp->address &= 0x3fff;
	    }
	}
	vdp->flags &= ~TF_ADDRWRITE;
    } else {
	vdp->addrsave = data;
	vdp->flags |= TF_ADDRWRITE;
    }
}


/* scanline read emulation */

u8 sms9918_readscanline(sms9918 vdp)
{
    return (vdp->cur_scanline < 192)? vdp->cur_scanline: 255;
}

const u8 sms9918_cache_palette[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
				      9, 10, 11, 12, 13, 14, 15};
void sms9918_cache_tile(sms9918 vdp, u16 addr)
{
    u8 *cur_vbp;
    u8 *cur_vbp2;
    u8 *tiledata;
    u16 tile;
    int line;

    line = (addr << 1) & 0x38;
    tile = addr >> 5;
    
    cur_vbp = &sms9918_tilecache[(tile << 6) + line];
    cur_vbp2 = &sms9918_tilecache_rev[(tile << 6) + line];
    tiledata = &vdp->memory[addr & ~3];

    blit_4_8(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, sms9918_cache_palette, cur_vbp);
    blit_4_8_rev(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, sms9918_cache_palette, cur_vbp2);
}

void sms9918_cache_name(sms9918 vdp, unsigned short addr)
{
    u8 *tiledata;
    u16 cur_tile;
    struct sms9918_namecache *namecache;
    
    cur_tile = ltoh16(*((unsigned short *)(&vdp->memory[addr & ~1])));
    namecache = &sms9918_namecache[addr >> 1];
    
    if (cur_tile & 0x200) {
	tiledata = sms9918_tilecache_rev;
    } else {
	tiledata = sms9918_tilecache;
    }
    
    tiledata += ((cur_tile & 0x1ff) << 6);

    if (cur_tile & 0x400) {
	namecache->is_vflip = 1;
    } else {
	namecache->is_vflip = 0;
    }
    
    if (cur_tile & 0x1000) {
	namecache->back_tile = NULL;
	namecache->front_tile = tiledata;
    } else {
	namecache->back_tile = tiledata;
	namecache->front_tile = NULL;
    }
    
    if (cur_tile & 0x800) {
	namecache->palette = &vdp->palette_xlat[16];
    } else {
	namecache->palette = &vdp->palette_xlat[0];
    }
}

void sms9918_preload_namecache(sms9918 vdp)
{
    int i;
    for (i = 0; i < 0x2000; i++) {
	sms9918_cache_name(vdp, i << 1);
    }
}

/* video rendering */

u8 *sms_back_tiles[33];
u8 *sms_front_tiles[33];
u8 *sms_back_palettes[33];
u8 sms_back_finescroll;

void sms9918_init_backcache(sms9918 vdp)
{
    struct sms9918_namecache *cur_pos;
    struct sms9918_namecache *cur_tile;
    int cur_scanline;
    int cur_pattern_line;
    int pattern_offset;
    int pattern_offset_rev;
    int scroll;
    int i;

    cur_scanline = vdp->cur_scanline + vdp->regs[9];
    while (cur_scanline > 223) {
	cur_scanline -= 224;
    }
    
    cur_pattern_line = cur_scanline & 7;
    pattern_offset = cur_pattern_line << 3;
    pattern_offset_rev = (7 - cur_pattern_line) << 3;
    
    cur_pos = vdp->nametable;
    cur_pos += (cur_scanline >> 3) << 5;
    
    if ((vdp->regs[0] & 0x40) && (cur_scanline < 16)) {
	scroll = 0;
	sms_back_finescroll = 0;
    } else {
	scroll = vdp->coarsescroll;
	sms_back_finescroll = vdp->finescroll;
    }
    
    for (i = 0; i < 33; i++) {
	cur_tile = &cur_pos[scroll];
	scroll++;
	scroll &= 0x1f;

	sms_back_tiles[i] = cur_tile->back_tile;
	sms_front_tiles[i] = cur_tile->front_tile;
	sms_back_palettes[i] = cur_tile->palette;

	if (cur_tile->is_vflip) {
	    if (sms_back_tiles[i]) {
		sms_back_tiles[i] += pattern_offset_rev;
	    } else {
		sms_front_tiles[i] += pattern_offset_rev;
	    }
	} else {
	    if (sms_back_tiles[i]) {
		sms_back_tiles[i] += pattern_offset;
	    } else {
		sms_front_tiles[i] += pattern_offset;
	    }
	}
    }
}

int sms9918_cache_sprites(sms9918 vdp, u8 **tiledata, u8 *sprite_x)
{
    u8 *sprite_table;
    u16 cur_tile;
    int cur_pattern_line;
    int sprite_8x16;
    int i;
    int num_sprites;

    num_sprites = 0;
    sprite_table = &vdp->memory[(vdp->regs[5] & 0x7e) << 7];
    sprite_8x16 = (vdp->regs[1] & 2)? 16: 8;
    
    for (i = 0; i < 64; i++) {
	if (sprite_table[i] == 208) {
	    break;
	}

	cur_pattern_line = vdp->cur_scanline - sprite_table[i];
	cur_pattern_line &= 0xff;

	if (!(cur_pattern_line < sprite_8x16)) {
	    continue;
	}
	
	if (num_sprites == 8) {
	    /* FIXME: Sprite overflow flag? */
	    break;
	}

	cur_tile = sprite_table[128 + (i << 1) + 1] | ((vdp->regs[6] & 4) << 6);
	if (vdp->regs[1] & 2) {
	    cur_tile &= 0x1fe;
	}
	tiledata[num_sprites] = &vdp->memory[(cur_tile << 5) + (cur_pattern_line << 2)];
	sprite_x[num_sprites] = sprite_table[128 + (i << 1) + 0];
	num_sprites++;
    }

    return num_sprites;
}

void sms9918_render_sprites(sms9918 vdp, u8 *cur_vbp)
{
    u8 *tiledata[8];
    u8 sprite_x[8];
    int i;
    int num_sprites;
    
    num_sprites = sms9918_cache_sprites(vdp, tiledata, sprite_x);
    
    for (i = num_sprites - 1; i >= 0; i--) {
	if (sprite_x[i] > 248) {
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 256 - sprite_x[i], 0, &vdp->palette_xlat[16],
			 &cur_vbp[sprite_x[i]]);
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 8 - (256 - sprite_x[i]), 256 - sprite_x[i], &vdp->palette_xlat[16],
			 &cur_vbp[0]);
	} else {
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 8, 0, &vdp->palette_xlat[16],
			 &cur_vbp[sprite_x[i]]);
	}
    }
}

void sms9918_render_sprites_gg(sms9918 vdp, unsigned char *cur_vbp)
{
    u8 *tiledata[8];
    u8 sprite_x[8];
    int i;
    int num_sprites;

    num_sprites = sms9918_cache_sprites(vdp, tiledata, sprite_x);
    
    for (i = num_sprites - 1; i >= 0; i--) {
	if (sprite_x[i] < 40) {
	    continue;
	} else if (sprite_x[i] < 48) {
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 8, -(sprite_x[i] - 48), &vdp->palette_xlat[16],
			 &cur_vbp[0]);
	} else if (sprite_x[i] < (160 + 40)) {
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 8, 0, &vdp->palette_xlat[16],
			 &cur_vbp[sprite_x[i] - 48]);
	} else if (sprite_x[i] < (160 + 48)) {
	    blit_4_8_czt(tiledata[i][3], tiledata[i][2],
			 tiledata[i][1], tiledata[i][0],
			 (160 + 48) - sprite_x[i], 0, &vdp->palette_xlat[16],
			 &cur_vbp[sprite_x[i] - 48]);
	}
    }
}
extern int frameskip;
void sms9918_render_line_sms(sms9918 vdp)
{
    if (vdp->cur_scanline==0) { frameskip=(frameskip==0)?1:0; } 
    if (frameskip==0)
    {
    u8 *video_buffer;
    int i;

    video_buffer = video_get_vbp(vdp->cur_scanline);
    
    if (vdp->regs[1] & 0x40) {
	sms9918_init_backcache(vdp);
	memset(video_buffer, sms_back_palettes[0][0], 256);
	tiledraw_8(video_buffer, sms_back_tiles, sms_back_palettes, 32, sms_back_finescroll);
	sms9918_render_sprites(vdp, video_buffer);
	tiledraw_8(video_buffer, sms_front_tiles, sms_back_palettes, 32, sms_back_finescroll);
	if (vdp->regs[0] & 0x20) {
	    for (i = 0; i < 8; i++) {
		video_buffer[i] = vid_pre_xlat[0];
	    }
	}
    } else {
	/* FIXME: floodfill with black */
    }
    }
}

void sms9918_render_line_gg(sms9918 vdp)
{
    u8 *video_buffer;

    video_buffer = video_get_vbp(vdp->cur_scanline - 24);
    
    if (vdp->regs[1] & 0x40) {
	sms9918_init_backcache(vdp);
	memset(video_buffer, sms_back_palettes[0][0], 160);
	tiledraw_8(video_buffer, sms_back_tiles + 6, sms_back_palettes + 6, 20, sms_back_finescroll);
	sms9918_render_sprites_gg(vdp, video_buffer);
	tiledraw_8(video_buffer, sms_front_tiles + 6, sms_back_palettes + 6, 20, sms_back_finescroll);
    } else {
	/* FIXME: floodfill with black */
    }
}


/* periodic task */

int sms9918_periodic(sms9918 vdp)
{
    /* FIXME: does not account for overscan. */
    if ((!(vdp->flags & TF_GAMEGEAR)) && (vdp->cur_scanline < 192)) {
	sms9918_render_line_sms(vdp);
    } else if ((vdp->flags & TF_GAMEGEAR) && (vdp->cur_scanline >= 24) && (vdp->cur_scanline < 168)) {
	sms9918_render_line_gg(vdp);
    } else if (vdp->cur_scanline == 192) {
/* 	video_events(); */
	video_display_buffer();
	vdp->status |= 0x80;
    }
    if (vdp->cur_scanline == 261) {
	vdp->cur_scanline = 0;
	vdp->linecounter = vdp->regs[10];
    } else {
	vdp->cur_scanline++;
	if ((vdp->cur_scanline < 192) && (!(vdp->linecounter--))) {
	    vdp->status |= 0x40;
	    vdp->linecounter = vdp->regs[10];
	}
    }
    return (((vdp->status & 0x80) && (vdp->regs[1] & 0x20)) ||
	    ((vdp->status & 0x40) && (vdp->regs[0] & 0x10)));
}


/* initialization */

u8 sms9918_initial_register_values[11] = {
    0x36, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x00, 0x00, 0x00, 0xff,
};

void sms9918_init_registers(sms9918 vdp)
{
    int i;

    for (i = 0; i < 11; i++) {
	sms9918_write_register(vdp, i, sms9918_initial_register_values[i]);
    }
}

void sms9918_init(sms9918 vdp, int is_gamegear)
{
    if (!is_gamegear) {
	video_setsize(256, 192);
    } else {
	vdp->flags = TF_GAMEGEAR;
	video_setsize(160, 144);
    }
    sms9918_preload_namecache(vdp);
    sms9918_init_registers(vdp);
    video_setpal(64, sms_palbase_red, sms_palbase_green, sms_palbase_blue);
}

sms9918 sms9918_create(int is_gamegear)
{
    sms9918 retval;

    retval = calloc(1, sizeof(struct sms9918));

    if (retval) {
	retval->memory = calloc(1, SMS_VDP_RAMSIZE);
	
	if (!retval->memory) {
	    free(retval);
	    retval = NULL;
	}
    }
    
    if (!retval) {
	deb_printf("sms9918_create(): out of memory.\n");
	return NULL;
    }

    sms9918_init(retval, is_gamegear);
    
    return retval;
}

/*
 * $Log: sms9918.c,v $
 * Revision 1.57  2000/08/26 00:26:19  nyef
 * moved the psg "vsync" call out to sms.c
 *
 * Revision 1.56  2000/02/27 15:59:28  nyef
 * cleaned up the sprite renderers
 *
 * Revision 1.55  2000/02/24 01:15:08  nyef
 * fixed problems with GG sprite clipping and background rendering
 *
 * Revision 1.54  1999/12/24 01:03:48  nyef
 * fixed a bug in the namecache added when switching type systems
 *
 * Revision 1.53  1999/12/07 02:11:29  nyef
 * changed to use new types interface
 *
 * Revision 1.52  1999/12/03 01:40:42  nyef
 * added initializers for all important registers
 * cleaned up the creation and initialization logic somewhat
 *
 * Revision 1.51  1999/12/01 01:37:46  nyef
 * hacked in a temporary patch to get terminator working (needs proper fix)
 *
 * Revision 1.50  1999/11/27 23:49:08  nyef
 * changed to use NULL instead of the blank tile used previously
 *
 * Revision 1.49  1999/11/26 23:50:40  nyef
 * fixed sms9918_render_sprites() to handle sprites with an x > 248 properly
 *
 * Revision 1.48  1999/11/26 17:28:02  nyef
 * re-restructured the tilecache (reversed part of revision 1.35)
 * added a namecache to cache the nametable data
 *
 * Revision 1.47  1999/11/26 16:47:27  nyef
 * fixed scrolling bug introduced while speeding up the renderer
 *
 * Revision 1.46  1999/11/26 16:42:33  nyef
 * moved the vdp data structure in from sms9918.h
 * stripped out the procpointers from the data structure
 *
 * Revision 1.45  1999/11/26 16:10:07  nyef
 * more tweaks to the tile renderer
 *
 * Revision 1.44  1999/11/26 16:04:32  nyef
 * more strength reductions in the renderer
 *
 * Revision 1.43  1999/11/26 15:51:06  nyef
 * applied some strength reductions (loop-invariant code motion) to the renderer
 *
 * Revision 1.42  1999/11/26 15:38:08  nyef
 * move writing registers out from sms9918_writeport1() to
 *     sms9918_write_register()
 *
 * Revision 1.41  1999/11/25 17:11:08  nyef
 * re-added the left column display disable
 *
 * Revision 1.40  1999/11/25 16:37:47  nyef
 * split sms9918_render_line() into SMS and GG versions
 *
 * Revision 1.39  1999/11/23 03:30:12  nyef
 * fixed a bug with finescroll on the right side of the screen
 *
 * Revision 1.38  1999/11/23 03:21:47  nyef
 * changed to use new tiledraw interface
 *
 * Revision 1.37  1999/11/23 01:40:58  nyef
 * moved the tile renderer out to tiledraw.c
 *
 * Revision 1.36  1999/11/22 03:48:25  nyef
 * restructured the tile renderer
 * (lost the left column display disable in the shuffle)
 *
 * Revision 1.35  1999/11/14 01:39:53  nyef
 * Reorganized the tilecache so as not to hammer the CPU cache so much
 * Changed the tilecache to cache tiles as the data is written
 *
 * Revision 1.34  1999/10/17 22:46:47  nyef
 * fixed potential problem with GG palette address setting
 *
 * Revision 1.33  1999/10/16 17:03:36  nyef
 * added second renderer for GG emulation
 *
 * Revision 1.32  1999/08/07 16:20:38  nyef
 * added fix for MSB_FIRST systems
 *
 * Revision 1.31  1999/07/08 02:36:33  nyef
 * fixed to not display background on left 8 pixels of the screen when
 * display of left column disabled
 *
 * fixed broken scrolling of leftmost 0-7 pixels of the background
 *
 * Revision 1.30  1999/07/05 23:28:25  nyef
 * fixed sprite renderer to display sprites at the top of the screen
 *
 * Revision 1.29  1999/06/13 20:44:12  nyef
 * cleaned up a silliness in the code generated for sms9918_cache_tile()
 *
 * Revision 1.28  1999/06/13 20:15:02  nyef
 * changed to use memset(3) to handle background color on transparent tiles
 *
 * Revision 1.27  1999/05/08 15:41:44  nyef
 * fixed problem with blue color in gg palette code
 *
 * Revision 1.26  1999/05/05 00:24:45  nyef
 * fixed tile transparencies in renderer
 *
 * Revision 1.25  1999/05/01 05:36:57  nyef
 * split up the renderer code into "background" and "forground" versions
 *
 * Revision 1.24  1999/05/01 05:16:09  nyef
 * split the background renderer up into two separate functions
 *
 * Revision 1.23  1999/05/01 04:41:31  nyef
 * removed non-tilecache renderer code
 *
 * Revision 1.22  1999/05/01 03:32:05  nyef
 * cleaned up the background renderer a little more
 * added a reversed tilecache
 *
 * Revision 1.21  1999/04/28 16:48:51  nyef
 * cleaned up the background renderer a little
 * added a tilecache system
 *
 * Revision 1.20  1999/04/28 13:44:47  nyef
 * changed to add sound emulation
 *
 * Revision 1.19  1999/03/12 02:12:45  nyef
 * added preliminary GameGear palette support
 *
 * Revision 1.18  1999/02/07 17:06:47  nyef
 * changed to use video_setsize()
 *
 * Revision 1.17  1999/02/06 03:37:42  nyef
 * removed call to video_events()
 *
 * Revision 1.16  1999/01/24 17:26:48  nyef
 * fixed background renderer to display the final few pixels on the
 * right side while h-scrolling.
 *
 * Revision 1.15  1999/01/24 17:17:39  nyef
 * switched to using blitters from blitters.c for sprite rendering.
 * got rid of the old blitter system.
 *
 * Revision 1.14  1999/01/11 02:25:05  nyef
 * switched to using blitters from blitters.c for background rendering.
 *
 * Revision 1.13  1999/01/10 02:13:32  nyef
 * reversed sprite display order.
 *
 * Revision 1.12  1999/01/10 00:02:42  nyef
 * changed sms9918_readscanline() to return 255 for values > 191.
 *
 * Revision 1.11  1999/01/09 23:09:55  nyef
 * added support for reading the current scanline.
 *
 * Revision 1.10  1999/01/09 22:01:47  nyef
 * fixed problem with tile priorities and horizontal scroll.
 *
 * Revision 1.9  1999/01/09 01:31:58  nyef
 * added support for 8x16 sprites.
 * limited number of sprites per line to 8 (supposedly the real system
 * does this too.).
 *
 * Revision 1.8  1999/01/09 00:48:35  nyef
 * fixed stupid mistake in tile priority code.
 *
 * Revision 1.7  1999/01/09 00:45:47  nyef
 * added preliminary support for vscroll.
 *
 * Revision 1.6  1999/01/09 00:39:12  nyef
 * added preliminary implimentation of tile priority flag.
 *
 * Revision 1.5  1999/01/08 19:36:21  nyef
 * improved timing in sms9918_periodic().
 *
 * Revision 1.4  1999/01/08 02:24:25  nyef
 * added line interrupts.
 *
 * Revision 1.3  1999/01/08 02:07:25  nyef
 * added horizontal scroll.
 *
 * Revision 1.2  1999/01/07 05:20:35  nyef
 * fixed sprite data table lookup.
 * added sprite tileset selection.
 * fixed sprite palette lookup.
 *
 * Revision 1.1  1999/01/07 03:17:07  nyef
 * Initial revision
 *
 */
