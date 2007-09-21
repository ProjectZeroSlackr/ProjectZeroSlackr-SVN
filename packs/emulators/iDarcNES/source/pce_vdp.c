/*
 * pce_vdp.c
 *
 * HuC6270/HuC6260 emulation.
 */

/* $Id: pce_vdp.c,v 1.50 2000/09/18 03:50:27 nyef Exp $ */

#include <stdlib.h>
#include <string.h>
#include "pce_vdp.h"
#include "ui.h"
#include "video.h"
#include "blitters.h"
#include "tiledraw.h"
#include "types.h"

#define PCE_VDP_RAMSIZE 0x10000

/* FIXME: these sizes may be wrong */
u8 pce_tilecache[PCE_VDP_RAMSIZE << 1];
u8 pce_tilecache_dirty[PCE_VDP_RAMSIZE >> 5];

struct pce_vdp {
    u8 flags;
    u8 status;
    u8 cur_reg;
    u8 *memory;
    u16 mem_read_addr;
    u16 mem_write_addr;
    u8 regs_l[32];
    u8 regs_h[32];
    u8 last_width;
    u16 last_height;
    u16 palette_addr;
    u8 palette_l[512];
    u8 palette_h[512];
    u8 palette_xlat[512];
    u16 cur_scanline;
    u16 y_scroll;
    int display_width;
};

int pce_palbase_red[64] = {
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
};

int pce_palbase_blue[64] = {
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

int pce_palbase_green[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

u8 pce_vce_read(pce_vdp vdp, u16 addr)
{
    u8 retval;

    if ((addr & 6) == 4) {
	if (addr & 1) {
	    retval = vdp->palette_h[vdp->palette_addr];
	    vdp->palette_addr += 1;
	    vdp->palette_addr &= 0x1ff;
	    return retval;
	} else {
	    return vdp->palette_l[vdp->palette_addr];
	}
    }
    return 0x00;
}

#define PCE_COLOR_XLAT(x) vid_pre_xlat[((vdp->palette_l[x] & 0x6) >> 1) | ((vdp->palette_l[x] & 0x30) >> 2) | ((vdp->palette_l[x] & 0x80) >> 3) | ((vdp->palette_h[x] & 1) << 5)]
void pce_vce_write(pce_vdp vdp, u16 addr, u8 data)
{
    switch (addr & 7) {
    case 0:
	/* FIXME: add control register */
	break;
    case 2:
	vdp->palette_addr &= 0x100;
	vdp->palette_addr |= data;
	break;
    case 3:
	if (data & 1) {
	    vdp->palette_addr |= 0x100;
	} else {
	    vdp->palette_addr &= 0xff;
	}
	break;
    case 4:
	vdp->palette_l[vdp->palette_addr] = data;
	vdp->palette_xlat[vdp->palette_addr] = PCE_COLOR_XLAT(vdp->palette_addr);
	break;
    case 5:
	vdp->palette_h[vdp->palette_addr] = data & 1;
	vdp->palette_xlat[vdp->palette_addr] = PCE_COLOR_XLAT(vdp->palette_addr);
	vdp->palette_addr += 1;
	vdp->palette_addr &= 0x1ff;
	break;
    }
}

unsigned char pce_vdp_read(pce_vdp vdp, u16 addr)
{
    u8 retval;
    
    switch (addr & 3) {
    case 0:
/* 	deb_printf("pce_vdp: statread.\n"); */
	retval = vdp->status;
	vdp->status &= ~0x24; /* clear vblank bit */ /* FIXME: may be wrong */
	return retval;
    case 1:
	return 0x00;
    case 2:
	if (vdp->cur_reg != 2) {
	    deb_printf("pce_vdp: r1l cur_reg %d.\n", vdp->cur_reg);
	} else {
	    return vdp->memory[(vdp->mem_read_addr << 1) + 0];
	}
	return 0x00;
    case 3:
	if (vdp->cur_reg != 2) {
	    deb_printf("pce_vdp: r1h cur_reg %d.\n", vdp->cur_reg);
	} else {
	    return vdp->memory[(vdp->mem_read_addr++ << 1) + 1];
	}
	return 0x00;
    }
    return 0x00;
}

void pce_vdp_write(pce_vdp vdp, u16 addr, u8 data)
{
    switch (addr & 3) {
    case 0:
	vdp->cur_reg = data & 0x1f;
	break;
    case 1:
	break;
    case 2:
	switch (vdp->cur_reg) {
	case 0:
	    vdp->mem_write_addr &= 0xff00;
	    vdp->mem_write_addr |= data;
	    break;
	case 1:
	    vdp->mem_read_addr &= 0xff00;
	    vdp->mem_read_addr |= data;
	    break;
	case 2:
	    vdp->memory[((vdp->mem_write_addr & 0x7fff) << 1) + 0] = data;
	    pce_tilecache_dirty[(vdp->mem_write_addr & 0x7fff) >> 4] = 1;
	    break;
	case 5: /* Control */
	    vdp->regs_l[5] = data;
	    break;
	case 6: /* Raster counter */
/* 	    deb_printf("pce_vdp: rcrl %d.\n", data); */
	    vdp->regs_l[6] = data;
	    break;
	case 7: /* BG X-Scroll */
	    vdp->regs_l[7] = data;
	    break;
	case 8: /* BG Y-Scroll */
/*  	    deb_printf("pce_vdp: BYR low set 0x%02x, %d.\n", data, vdp->cur_scanline); */
	    vdp->regs_l[8] = data;
	    vdp->y_scroll &= 0xff00;
	    vdp->y_scroll |= data;
	    break;
	case 9: /* Memory width? */
	    vdp->regs_l[9] = data;
	    break;
	case 10: /* Horizontal Sync? */
	    break;
	case 11: /* Horizontal Display? */
	    vdp->regs_l[11] = data;
/* 	    deb_printf("pce_vdp: hdw = %d.\n", data & 0x7f); */
	    break;
	case 12: /* Vertical Sync? */
	    break;
	case 13: /* Vertical Display? */
	    break;
	case 14: /* Vertical Display End? */
	    break;
	case 15: /* DMA Control */
	    break;
	case 19: /* Sprite attributes? */
	    vdp->regs_l[19] = data;
	    break;
	default:
	    deb_printf("pce_vdp: regwrite %d l = 0x%02x.\n", vdp->cur_reg, data);
	}
	break;
    case 3:
	switch (vdp->cur_reg) {
	case 0:
	    vdp->mem_write_addr &= 0x00ff;
	    vdp->mem_write_addr |= (data << 8);
	    break;
	case 1:
	    vdp->mem_read_addr &= 0x00ff;
	    vdp->mem_read_addr |= (data << 8);
	    break;
	case 2:
	    vdp->memory[((vdp->mem_write_addr & 0x7fff) << 1) + 1] = data;
	    pce_tilecache_dirty[(vdp->mem_write_addr & 0x7fff) >> 4] = 1;
#if 0
	    vdp->mem_write_addr += (1 << ((vdp->regs_h[5] & 0x18)? ((vdp->regs_h[5] & 0x18) >> 3) + 4: 0)); /* FIXME: is this right? */
#else
	    switch (vdp->regs_h[5] & 0x18) {
	    case 0x00:
		vdp->mem_write_addr += 1;
		break;
	    case 0x08:
		vdp->mem_write_addr += 32;
		break;
	    case 0x10:
		vdp->mem_write_addr += 64;
		break;
	    case 0x18:
		vdp->mem_write_addr += 128;
		break;
	    }
#endif
	    break;
	case 5: /* Control */
	    vdp->regs_h[5] = data;
	    break;
	case 6: /* Raster counter */
	    vdp->regs_h[6] = data;
	    break;
	case 7: /* BG X-Scroll */
	    vdp->regs_h[7] = data;
	    break;
	case 8: /* BG Y-Scroll */
/*  	    deb_printf("pce_vdp: BYR high set 0x%02x, %d.\n", data, vdp->cur_scanline); */
	    vdp->regs_h[8] = data;
	    vdp->y_scroll &= 0x00ff;
	    vdp->y_scroll |= (data << 8);
	    break;
	case 9: /* Memory width? */
	    break;
	case 10: /* Horizontal Sync? */
	    break;
	case 11: /* Horizontal Display? */
	    break;
	case 12: /* Vertical Sync? */
	    break;
	case 13: /* Vertical Display? */
	    break;
	case 14: /* Vertical Display End? */
	    break;
	case 15: /* DMA Control */
	    break;
	case 19: /* Sprite attributes? */
	    vdp->regs_h[19] = data;
	    break;
	default:
	    deb_printf("pce_vdp: regwrite %d h = 0x%02x.\n", vdp->cur_reg, data);
	}
	break;
    }
}

const u8 pce_cache_palette[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
				  9, 10, 11, 12, 13, 14, 15};
void pce_cache_tile(pce_vdp vdp, u32 tile)
{
    u8 *cur_vbp;
    u8 *tiledata;
    int i;

    pce_tilecache_dirty[tile] = 0;
    cur_vbp = &pce_tilecache[tile << 6];
    tiledata = &vdp->memory[tile << 5];

    for (i = 8; i; i--) {
	blit_4_8(tiledata[17], tiledata[16], tiledata[1], tiledata[0], 8, 0, pce_cache_palette, cur_vbp);
	cur_vbp += 8;
	tiledata += 2;
    }
}

u8 *pce_cached_patterns[129];
u8 *pce_cached_palettes[129];

void pce_vdp_render_background(pce_vdp vdp, u8 *cur_vbp)
{
    u16 *linebase;
    u16 cur_pos;
    u32 cur_tile;
    u8 *cur_pattern;
    u16 cur_scanline;
    u8 *palette;
    int finescroll;
    int cur_pattern_line;
    int linemask;
    int i;

    cur_scanline = (vdp->y_scroll) & ((vdp->regs_l[9] & 0x40)? 511: 255);
    cur_pattern_line = cur_scanline & 7;
    
    if ((vdp->regs_l[9] & 0x30) == 0x00) {
	linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 2];
	linemask = 0x1f;
    } else if ((vdp->regs_l[9] & 0x30) == 0x10) {
	linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 3];
	linemask = 0x3f;
    } else {
	linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 4];
	linemask = 0x7f;
    }

    cur_pos = (vdp->regs_l[7] >> 3) + ((vdp->regs_h[7] & 3) << 5);
    finescroll = vdp->regs_l[7] & 7;

    for (i = 0; i <= vdp->last_width + 1; i++) {
	cur_pos &= linemask;
	cur_tile = ltoh16(linebase[cur_pos++]);
	palette = &vdp->palette_xlat[(cur_tile >> 8) & 0xf0];
	if (pce_tilecache_dirty[cur_tile & 0x7ff]) {
	    pce_cache_tile(vdp, cur_tile & 0x7ff);
	}
	cur_pattern = pce_tilecache + (cur_pattern_line << 3) + ((cur_tile & 0x7ff) << 6);
	pce_cached_patterns[i] = cur_pattern;
	pce_cached_palettes[i] = palette;
    }

    tiledraw_8(cur_vbp, pce_cached_patterns, pce_cached_palettes, vdp->last_width + 1, finescroll);
}

struct satb_entry {
    u16 ypos;
    u16 xpos;
    u16 pattern;
    u16 flags;
};

#define SF_VFLIP 0x8000
#define SF_VSIZEMASK 0x3000
#define SF_VSIZE1 0x0000
#define SF_VSIZE2 0x1000
#define SF_VSIZE4 0x3000
#define SF_HFLIP 0x0800
#define SF_HSIZE 0x0100
#define SF_SPBG 0x0080
#define SF_PALETTE 0x000f

void pce_vdp_blit_tile_clipped(u8 *vbp, u8 pat0, u8 pat1, u8 pat2, u8 pat3, u8 *palette, int position, int width)
{
    if (position < -8) {
	return;
    }
    
    if (position >= width) {
	return;
    }

    if (position < 0) {
	blit_4_8_czt(pat0, pat1, pat2, pat3, 8 + position, -position, palette, vbp);
	return;
    }

    if ((position + 8) >= width) {
	blit_4_8_czt(pat0, pat1, pat2, pat3, width - position, 0, palette, vbp + position);
	return;
    }

    blit_4_8_czt(pat0, pat1, pat2, pat3, 8, 0, palette, vbp + position);
}

void pce_vdp_blit_tile_clipped_rev(u8 *vbp, u8 pat0, u8 pat1, u8 pat2, u8 pat3, u8 *palette, int position, int width)
{
    if (position < -8) {
	return;
    }
    
    if (position >= width) {
	return;
    }

    if (position < 0) {
	blit_4_8_czt_rev(pat0, pat1, pat2, pat3, 8 + position, -position, palette, vbp);
	return;
    }

    if ((position + 8) >= width) {
	blit_4_8_czt_rev(pat0, pat1, pat2, pat3, width - position, 0, palette, vbp + position);
	return;
    }

    blit_4_8_czt_rev(pat0, pat1, pat2, pat3, 8, 0, palette, vbp + position);
}

void pce_vdp_draw_sprite(u8 *cur_vbp, u8 *pattern, u8 *palette, int position, int display_width)
{
    pce_vdp_blit_tile_clipped(cur_vbp, pattern[97], pattern[65], pattern[33], pattern[1], palette, position, display_width);
    pce_vdp_blit_tile_clipped(cur_vbp, pattern[96], pattern[64], pattern[32], pattern[0], palette, position + 8, display_width);
}

void pce_vdp_draw_sprite_rev(u8 *cur_vbp, u8 *pattern, u8 *palette, int position, int display_width)
{
    pce_vdp_blit_tile_clipped_rev(cur_vbp, pattern[97], pattern[65], pattern[33], pattern[1], palette, position + 8, display_width);
    pce_vdp_blit_tile_clipped_rev(cur_vbp, pattern[96], pattern[64], pattern[32], pattern[0], palette, position, display_width);
}

void pce_vdp_render_sprites(pce_vdp vdp, unsigned char *cur_vbp, int behind)
{
    struct satb_entry *satb;
    struct satb_entry cur_sprite;
    int cur_pattern_line;
    u8 *cur_pattern;
    u8 *palette;
    int i;
    int sprite_vsize;
    
    satb = (struct satb_entry *)(((unsigned short *)vdp->memory) + (((vdp->regs_h[19] & 0x7f) << 8) + vdp->regs_l[19]));

    for (i = 63; i >= 0; i--) {
	cur_sprite.ypos = ltoh16(satb[i].ypos);
	cur_sprite.xpos = ltoh16(satb[i].xpos);
	cur_sprite.pattern = ltoh16(satb[i].pattern);
	cur_sprite.flags = ltoh16(satb[i].flags);
	
	/* sprite priority check */
	if ((!(cur_sprite.flags & SF_SPBG)) == (!behind)) {
	    continue;
	}
	
	/* upper bound visibility check */
	if ((cur_sprite.ypos & 0x3ff) > (vdp->cur_scanline + 64)) {
	    continue;
	}

	cur_pattern_line = (vdp->cur_scanline + 64) - (cur_sprite.ypos & 0x3ff);

	/* v-size check */
	switch (cur_sprite.flags & SF_VSIZEMASK) {
	case SF_VSIZE1:
	    sprite_vsize = 16;
	    break;
	    
	case SF_VSIZE2:
	    sprite_vsize = 32;
	    break;
	    
	case SF_VSIZE4:
	    sprite_vsize = 64;
	    break;
	    
	default:
	    /* Invalid size code */
	    deb_printf("pce_vdp: bogus sprite size code.\n");
	    continue;
	}

	/* lower bound visibility check */
	if (cur_pattern_line >= sprite_vsize) {
	    continue;
	}
	
	/* y-flip check */
	if (cur_sprite.flags & SF_VFLIP) {
	    cur_pattern_line = (sprite_vsize - 1) - cur_pattern_line;
	}
	
	if (cur_sprite.pattern & 0x400) {
	    deb_printf("pce_vdp: sprite pattern 0x400 set.\n");
	}
	cur_pattern = &vdp->memory[(cur_sprite.pattern & 0x3ff) << 6];
	cur_pattern += (cur_pattern_line & 0x0f) << 1;
	cur_pattern += (cur_pattern_line & 0x30) << 4;

	palette = &vdp->palette_xlat[0x100 + ((cur_sprite.flags & SF_PALETTE) << 4)];
	
	if (!(cur_sprite.flags & SF_HFLIP)) {
	    pce_vdp_draw_sprite(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
	    
	    cur_pattern += 0x80;
	    
	    if (cur_sprite.flags & SF_HSIZE) {
		pce_vdp_draw_sprite(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 16, vdp->display_width);
	    }
	} else {
	    if (!(cur_sprite.flags & SF_HSIZE)) {
		pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
	    } else {
		pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 16, vdp->display_width);
	    
		cur_pattern += 0x80;
	    
		pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
	    }
	}
    }
}

void pce_vdp_render_line(pce_vdp vdp)
{
    u8 *cur_vbp;

    cur_vbp = video_get_vbp(vdp->cur_scanline);

    memset(cur_vbp, vdp->palette_xlat[0], vdp->display_width);
    if (vdp->regs_l[5] & 0x40) {
	pce_vdp_render_sprites(vdp, cur_vbp, 1);
    }
    if (vdp->regs_l[5] & 0x80) {
	pce_vdp_render_background(vdp, cur_vbp);
    }
    if (vdp->regs_l[5] & 0x40) {
	pce_vdp_render_sprites(vdp, cur_vbp, 0);
    }
}

int pce_vdp_periodic(pce_vdp vdp)
{
    if ((vdp->cur_scanline + 0x40) == (vdp->regs_l[6] + (vdp->regs_h[6] << 8))) {
/*  	deb_printf("pce_vdp: RCR! (%d)\n", vdp->cur_scanline); */
	vdp->status |= 0x04;
    } else {
	vdp->status &= ~0x04;
    }
    if (vdp->cur_scanline < 256) {
	pce_vdp_render_line(vdp);
	vdp->y_scroll++;
    }
    if (vdp->cur_scanline == 256) {
/* 	video_events(); */
	video_display_buffer();
	vdp->status |= 0x20; /* FIXME: I'm not sure about this. */
    }
    if (vdp->cur_scanline == 261) {
	vdp->cur_scanline = 0;
	vdp->y_scroll = ((vdp->regs_h[8] << 8) + vdp->regs_l[8]);
	if ((vdp->regs_l[11] & 0x7f) != vdp->last_width) {
	    vdp->last_width = vdp->regs_l[11] & 0x7f;
	    vdp->display_width = (vdp->last_width + 1) << 3;
	    video_setsize(vdp->display_width, 256);
	}
    } else {
	vdp->cur_scanline++;
    }
    return ((vdp->regs_l[5] & 0x08) && (vdp->status & 0x20)) ||
	((vdp->regs_l[5] & 0x04) && (vdp->status & 0x04));
#if 0
    return ((vdp->regs_l[5] & 0x08) && (vdp->status & 0x20)); /* FIXME: may be wrong. */
#endif
}

pce_vdp pce_vdp_create(void)
{
    pce_vdp retval;
    
    retval = calloc(1, sizeof(struct pce_vdp));
    if (retval) {
	retval->memory = calloc(1, PCE_VDP_RAMSIZE);
	if (retval->memory) {
	    video_setsize(256, 256);
	    video_setpal(64, pce_palbase_red, pce_palbase_green, pce_palbase_blue);

	    retval->last_width = 31;
	    retval->display_width = 256;
	    retval->last_height = 0;
	    retval->regs_l[11] = 31;
	} else {
	    free(retval);
	    retval = NULL;
	}
    }
    
    if (!retval) {
	deb_printf("pce_vdp_create(): out of memory.\n");
    }
    
    return retval;
}

/*
 * $Log: pce_vdp.c,v $
 * Revision 1.50  2000/09/18 03:50:27  nyef
 * re-inserted the line of code I removed in 1.49 (broke puzznic)
 *
 * Revision 1.49  2000/08/15 00:58:39  nyef
 * removed a useless-looking line of code from the vblank end code
 *
 * Revision 1.48  2000/02/20 00:53:37  nyef
 * removed some test code I accidentally left in when fixing the clipping
 *
 * Revision 1.47  2000/02/20 00:45:39  nyef
 * rebuilt sprite drawing routines to clip to the right edge of the screen
 *
 * Revision 1.46  2000/02/19 23:45:02  nyef
 * converted to use typedefs from types.h
 *
 * Revision 1.45  2000/02/19 23:29:25  nyef
 * moved struct pce_vdp in from pce_vdp.h
 * removed all function pointers from struct pce_vdp
 *
 * Revision 1.44  2000/02/19 23:11:01  nyef
 * cleaned up sprite drawing a bit
 *
 * Revision 1.43  2000/02/15 03:58:08  nyef
 * cleaned up MSB_FIRST support in background renderer
 *
 * Revision 1.42  2000/02/06 22:28:03  nyef
 * fixed to clear RCR bit of status when not on RCR scanline
 *
 * Revision 1.41  2000/02/06 03:55:35  nyef
 * fixed pce_vce_read() to return the high byte when address 5 is read
 *
 * Revision 1.40  2000/01/09 18:17:48  nyef
 * cleaned up the sprite renderer
 * fixed the sprite renderer to clip to the left edge of the screen
 *
 * Revision 1.39  2000/01/09 04:02:54  nyef
 * Fixed some V-scroll problems relating to raster splits
 *
 * Revision 1.38  1999/11/23 03:23:49  nyef
 * changed background renderer to use the tiledraw interface
 *
 * Revision 1.37  1999/11/23 02:53:50  nyef
 * removed non-tilecache renderer
 *
 * Revision 1.36  1999/08/15 15:43:08  nyef
 * added a tilecache system
 *
 * Revision 1.35  1999/08/15 03:50:27  nyef
 * fixed sprite renderer to work on MSB_FIRST systems
 *
 * Revision 1.34  1999/08/07 16:32:24  nyef
 * fixed background renderer to work on MSB_FIRST systems
 *
 * Revision 1.33  1999/06/20 14:54:22  nyef
 * fixed pce_vdp_render_background() to not v-wrap the playfield too soon
 *
 * Revision 1.32  1999/05/23 17:08:37  nyef
 * added sprite background priority control
 *
 * Revision 1.31  1999/05/23 16:25:34  nyef
 * fixed some background color problems
 *
 * Revision 1.30  1999/05/23 15:19:37  nyef
 * fixed some v-scroll problems
 *
 * Revision 1.29  1999/04/04 20:38:56  nyef
 * cleaned up the sprite renderer somewhat
 *
 * Revision 1.28  1999/03/14 13:29:05  nyef
 * fixed sprite display order
 *
 * Revision 1.27  1999/03/14 13:19:33  nyef
 * fixed reversed sprites
 *
 * Revision 1.26  1999/03/06 15:06:32  nyef
 * fixed virtual background map size
 *
 * Revision 1.25  1999/03/06 05:43:48  nyef
 * masked satb pointer down to valid range
 * added horizontal-reversed sprite support
 *
 * Revision 1.24  1999/03/03 02:46:25  nyef
 * added preliminary horizontal resolution control
 *
 * Revision 1.23  1999/02/21 18:15:50  nyef
 * added preliminary vertical scroll
 *
 * Revision 1.22  1999/02/21 17:52:52  nyef
 * added fine horizontal scroll
 *
 * Revision 1.21  1999/02/20 21:35:59  nyef
 * added preliminary course horizontal scroll
 *
 * Revision 1.20  1999/02/20 21:29:15  nyef
 * added preliminary Raster Counter support
 *
 * Revision 1.19  1999/02/20 20:38:35  nyef
 * changed background tile mask from 0xfff to 0x7ff
 *
 * Revision 1.18  1999/02/08 02:07:37  nyef
 * fixed sprite pattern and data write address masks
 *
 * Revision 1.17  1999/02/08 00:43:21  nyef
 * offset the sprites by -32x-64 so they appear in the right place
 *
 * Revision 1.16  1999/02/07 23:33:57  nyef
 * added preliminary sprite renderer
 *
 * Revision 1.15  1999/02/07 18:51:23  nyef
 * cleaned up vram write address increment logic
 * fixed virtual background size logic
 *
 * Revision 1.14  1999/02/07 18:39:27  nyef
 * added call to video_setsize() in pce_vdp_create()
 *
 * Revision 1.13  1999/02/06 03:37:32  nyef
 * removed call to video_events()
 *
 * Revision 1.12  1999/02/03 02:31:19  nyef
 * added preliminary support for memory-width register (reg 9)
 *
 * Revision 1.11  1999/02/03 02:17:23  nyef
 * added support for VRAM reads
 *
 * Revision 1.10  1999/02/02 16:21:07  nyef
 * disabled debug output of status reads
 *
 * Revision 1.9  1999/02/01 04:00:30  nyef
 * changed to only render lines < 256
 *
 * Revision 1.8  1999/01/31 21:19:50  nyef
 * removed some annoying debug output
 *
 * Revision 1.7  1999/01/31 20:33:26  nyef
 * fixed tile layer order in pce_vdp_render_background()
 *
 * Revision 1.6  1999/01/31 06:04:49  nyef
 * added very preliminary video renderer.
 * also added vce palette emulation (at 3 bpp less than the real system).
 *
 * Revision 1.5  1999/01/31 00:16:08  nyef
 * added preliminary implimentation of palette ram
 *
 * Revision 1.4  1999/01/30 23:26:11  nyef
 * added dummy routines for VCE I/O.
 *
 * Revision 1.3  1999/01/30 23:09:33  nyef
 * redesigned VDP I/O interface.
 *
 * Revision 1.2  1999/01/30 19:31:56  nyef
 * added some debug output for memory access.
 * changed pce_vdp_periodic() to interrupt by scanline rather
 * than status flag.
 *
 * Revision 1.1  1999/01/17 02:12:48  nyef
 * Initial revision
 *
 */
