/*
 * gen_vdp.c
 *
 * genesis custom VDP emulation.
 */

/* $Id: gen_vdp.c,v 1.5 1999/12/24 16:52:23 nyef Exp $ */

#include <stdlib.h>
#include "types.h"
#include "gen_vdp.h"
#include "ui.h"
#include "video.h"
#include "blitters.h"

/* manifest constants and flag defines */

#define GEN_VDP_RAMSIZE 0x10000

#define TF_ADDRWRITE 1
#define TF_PALETTE 2


/* data structure definition */

struct gen_vdp {
    unsigned short flags;
    unsigned short readahead;
    unsigned short addrsave;
    unsigned short status;
    unsigned char *memory;
    unsigned char regs[24];
    unsigned char palette[128];
    unsigned char palette_xlat[64];
    unsigned long address;
    unsigned short cur_scanline;
    unsigned char linecounter;
};


/* Internal prototypes */

void gen_vdp_handle_dma(gen_vdp vdp);


/* Palette definition */

int gen_palbase_red[64] = {
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

int gen_palbase_green[64] = {
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
};

int gen_palbase_blue[64] = {
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

unsigned short gen_vdp_readport0(gen_vdp vdp)
{
    unsigned char retval;

    if (vdp->address & 0x4000) {
	    retval = vdp->palette[(vdp->address++) & 0x3f];
	    vdp->address &= 0x403f;
    } else {
	retval = vdp->readahead;
	vdp->readahead = vdp->memory[vdp->address++];
	vdp->address &= 0x3fff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void gen_vdp_writeport0(gen_vdp vdp, unsigned short data)
{
/*     deb_printf("gen_vdp: data write 0x%04hx\n", data); */
    if (vdp->flags & TF_PALETTE) {
/* 	deb_printf("vdp: palette data write 0x%04hx.\n", data); */
	*((unsigned short *)(&vdp->palette[vdp->address])) = data;
	if (1) {
	    unsigned char tmp;

	    tmp = (vdp->palette[((vdp->address) & 0x7f) | 1] << 3) & 0x30;
	    tmp |= (vdp->palette[(vdp->address) & 0x7e] >> 4) & 0x0c;
	    tmp |= (vdp->palette[(vdp->address) & 0x7e] >> 2) & 0x03;

	    vdp->palette_xlat[((vdp->address) >> 1) & 0x3f] = vid_pre_xlat[tmp];
	}
	vdp->address += 2;
	vdp->address &= 0x7f;
    } else {
	vdp->readahead = data;
	*((unsigned short *)(&vdp->memory[vdp->address])) = data;
	vdp->address += 2;
	vdp->address &= 0xffff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
}


/* port 1 (status/address/palette/registers) emulation */

unsigned short gen_vdp_readport1(gen_vdp vdp)
{
    unsigned short retval;

    retval = vdp->status;
    vdp->status &= 0xdfff;
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void gen_vdp_writeport1(gen_vdp vdp, unsigned short data)
{
    if ((data & 0xc000) && (vdp->flags & TF_ADDRWRITE)) {
	deb_printf("vdp: control write during addr sequence.\n");
    }
    
    if ((data & 0xc000) == 0x8000) {
	if (((data & 0x3f00) >> 8) > 23) {
	    deb_printf("vdp: regwrite 0x%02x to outrange reg %d (ignored).\n", data & 0xff, (data & 0x3f00) >> 8);
	} else {
	    deb_printf("vdp: regwrite 0x%02x to reg %d.\n", data & 0xff, (data & 0x3f00) >> 8);
	    vdp->regs[(data & 0x3f00) >> 8] = data & 0xff;
	}
	vdp->flags &= ~TF_ADDRWRITE;
    } else if ((data & 0xc000) == 0xc000) {
	deb_printf("gen_vdp: palette address write? 0x%04hx.\n", data);
	vdp->addrsave = data;
	vdp->flags |= TF_ADDRWRITE;
    } else if ((data & 0xc000) == 0x4000) {
	deb_printf("gen_vdp: vram address write? 0x%04hx.\n", data);
	vdp->addrsave = data;
	vdp->flags |= TF_ADDRWRITE;
    } else if (vdp->flags & TF_ADDRWRITE) {
	deb_printf("gen_vdp: control write 0x%04hx\n", data);
	vdp->address = (data << 14) | (vdp->addrsave & 0x3fff);
	if (vdp->addrsave & 0x8000) {
	    vdp->flags |= TF_PALETTE;
	} else {
	    vdp->flags &= ~TF_PALETTE;
	}
	if (vdp->address & 0x200000) {
	    gen_vdp_handle_dma(vdp);
	}
	deb_printf("gen_vdp: address set 0x%x.\n", vdp->address);
	vdp->address &= 0xffff; /* FIXME: this is incredibly wrong */
	vdp->flags &= ~TF_ADDRWRITE;
    } else {
	deb_printf("gen_vdp: unexpected control write 0x%04hx\n", data);
    }
}


/* DMA */

void gen_vdp_handle_dma(gen_vdp vdp)
{
    unsigned short length;
    unsigned long source;
    
    deb_printf("gen_vdp: DMA requested?\n");

    length = vdp->regs[19] + (vdp->regs[20] << 8);
    source = vdp->regs[21] + (vdp->regs[22] << 8) + (vdp->regs[23] << 16);
    deb_printf("gen_vdp: length 0x%04hx words, source 0x%06lx.\n", length, source << 1);
    if (vdp->regs[23] & 0x80) {
	deb_printf("VRAM transfer not supported.\n");
    }
    if (vdp->regs[23] & 0x40) {
	deb_printf("unknown DMA control bit.\n");
    }
    if (source < 0x8000) { /* FIXME: Cheating Like All Noodles */
	extern unsigned short *genesis_rom;
	int i;

	vdp->address &= 0xffff;
	
	for (i = 0; i < length; i++) {
	    *((unsigned short *)&vdp->memory[vdp->address]) = mtoh16(genesis_rom[source++]);
	    vdp->address += 2;
	}
    }
}


/* video rendering */

void gen_vdp_render_background(gen_vdp vdp, unsigned char *cur_vbp, int behind)
{
    unsigned short *cur_pos;
    unsigned short cur_tile;
    unsigned long tiledata;
    int cur_scanline;
    int cur_pattern_line;
    int scroll;
    int finescroll;
    int i;

#if 0
    cur_scanline = vdp->cur_scanline + vdp->regs[9];
    while (cur_scanline > 223) {
	cur_scanline -= 224;
    }
#else
    cur_scanline = vdp->cur_scanline;
#endif
    cur_pattern_line = cur_scanline & 7;
/*     cur_pos = (unsigned short *)&vdp->memory[(vdp->regs[2] & 14) << 10]; */
    cur_pos = (unsigned short *)&vdp->memory[0xe000];
/*     cur_pos += (cur_scanline >> 3) << 5; */
#if 0
    if ((vdp->regs[0] & 0x40) && (cur_scanline < 16)) {
	scroll = 0;
	finescroll = 0;
    } else {
	scroll = 32 - ((vdp->regs[8] + 7) >> 3);
	finescroll = (8 - (vdp->regs[8] & 7)) & 7;
    }
#else
    scroll = 0;
    finescroll = 0;
#endif
    
/*     for (i = 0; i < (finescroll? 33: 32); i++) { */
    for (i = 0; i < 32; i++) {
	cur_tile = cur_pos[(i + scroll) & 0x1f];
	if (cur_tile & 0x1000) {
	    tiledata = (((unsigned long *)vdp->memory)[((cur_tile & 0x7ff) << 3) + ((7 - cur_pattern_line))]);
	} else {
	    tiledata = (((unsigned long *)vdp->memory)[((cur_tile & 0x7ff) << 3) + (cur_pattern_line)]);
	}

/* 	deb_printf("vdp: td 0x%08x.\n", tiledata); */
#if 0
	tiledata = 0; /* FIXME: wrong */
#endif
	
#if 0
	if ((!(cur_tile & 0x8000)) == (!behind)) {
	    cur_vbp += (i == 0)? 8 - finescroll: 8;
	} else {
#endif
#if 1
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 12) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 8) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 4) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 28) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 24) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 20) & 15];
	    *cur_vbp++ = vdp->palette_xlat[(tiledata >> 16) & 15];
#else
	    if (i == 0) {
		if (cur_tile & 0x800) {
/* 		    blit_4_8_rev(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8-finescroll, finescroll, &vdp->palette_xlat[0], cur_vbp); */
		} else {
/* 		    blit_4_8(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8-finescroll, finescroll, &vdp->palette_xlat[0], cur_vbp); */
		}
		cur_vbp += (i == 0)? 8 - finescroll: 8;
	    } else if (i == 32) {
		if (cur_tile & 0x800) {
/* 		    blit_4_8_rev(tiledata[3], tiledata[2], tiledata[1], tiledata[0], finescroll, 0, &vdp->palette_xlat[0], cur_vbp); */
		} else {
/* 		    blit_4_8(tiledata[3], tiledata[2], tiledata[1], tiledata[0], finescroll, 0, &vdp->palette_xlat[0], cur_vbp); */
		}
	    } else {
		if (cur_tile & 0x800) {
/* 		    blit_4_8_rev(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, &vdp->palette_xlat[0], cur_vbp); */
		} else {
/* 		    blit_4_8(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, &vdp->palette_xlat[0], cur_vbp); */
		}
		cur_vbp += 8;
	    }
#endif
#if 0
	}
#endif
    }
}

void gen_vdp_render_sprites(gen_vdp vdp, unsigned char *cur_vbp)
{
    unsigned char *sprite_table;
    unsigned char *tiledata[8];
    unsigned short cur_tile;
    unsigned char sprite_x[8];
    int cur_pattern_line;
    int sprite_8x16;
    int i;
    int k;

    k = 0;
/*     sprite_table = &vdp->memory[(vdp->regs[5] & 0x7e) << 7]; */
    sprite_8x16 = (vdp->regs[1] & 2)? 16: 8;

    sprite_table = 0; /* FIXME: wrong */

    for (i = 0; (i < 64) && (k < 8); i++) {
	if ((vdp->cur_scanline >= sprite_table[i]) &&
	    (vdp->cur_scanline < (sprite_table[i] + sprite_8x16))) {
	    cur_pattern_line = vdp->cur_scanline - sprite_table[i];

	    cur_tile = sprite_table[128 + (i << 1) + 1] | ((vdp->regs[6] & 4) << 6);
	    if (vdp->regs[1] & 2) {
		cur_tile &= 0x1fe;
	    }
/* 	    tiledata[k] = &vdp->memory[(cur_tile << 5) + (cur_pattern_line << 2)]; */
	    sprite_x[k] = sprite_table[128 + (i << 1) + 0];
	    k++;
	} else if (sprite_table[i] == 208) {
	    break;
	}
    }
    for (i = k - 1; i >= 0; i--) {
	blit_4_8_czt(tiledata[i][3], tiledata[i][2],
		     tiledata[i][1], tiledata[i][0],
		     8, 0, &vdp->palette_xlat[16],
		     &cur_vbp[sprite_x[i]]);
    }
}

void gen_vdp_render_line(gen_vdp vdp)
{
    unsigned char *video_buffer;

    video_buffer = video_get_vbp(vdp->cur_scanline);
    
    if (vdp->regs[1] & 0x40) {
	gen_vdp_render_background(vdp, video_buffer, 1);
/* 	gen_vdp_render_sprites(vdp, video_buffer); */
/* 	gen_vdp_render_background(vdp, video_buffer, 0); */
    } else {
	/* FIXME: floodfill with black */
    }
}


/* periodic task */

int gen_vdp_periodic(gen_vdp vdp)
{
    /* FIXME: does not account for overscan. */
    if (vdp->cur_scanline < 192) {
	gen_vdp_render_line(vdp);
    } else if (vdp->cur_scanline == 192) {
	video_display_buffer();
	vdp->status |= 0x2000;
    }
    if (vdp->cur_scanline == 261) {
	vdp->cur_scanline = 0;
	vdp->linecounter = vdp->regs[10];
	vdp->status = 0;
    } else {
	vdp->cur_scanline++;
#if 0
	if ((vdp->cur_scanline < 192) && !vdp->linecounter--) {
	    vdp->status |= 0x40;
	    vdp->linecounter = vdp->regs[10];
	}
#endif
    }
#if 1
    return (((vdp->status & 0x2000) && (vdp->regs[1] & 0x20)));
/* 	    || */
/* 	    ((vdp->status & 0x40) && (vdp->regs[0] & 0x10))); */
#endif
/*     return 0; */
}


/* initialization */

gen_vdp gen_vdp_create(void)
{
    gen_vdp retval;

    retval = calloc(1, sizeof(struct gen_vdp));
    if (retval) {
	retval->memory = calloc(1, GEN_VDP_RAMSIZE);
	if (retval->memory) {
	    video_setsize(256, 192); /* FIXME: may be worng */
	    video_setpal(64, gen_palbase_red, gen_palbase_green, gen_palbase_blue);
	} else {
	    free(retval);
	    retval = NULL;
	}
    }
    
    if (!retval) {
	deb_printf("gen_vdp_create(): out of memory.\n");
    }
    
    return retval;
}

/*
 * $Log: gen_vdp.c,v $
 * Revision 1.5  1999/12/24 16:52:23  nyef
 * converted to use the new type interface
 *
 * Revision 1.4  1999/12/04 16:14:04  nyef
 * moved the vdp data structure in from gen_vdp.h
 * stripped out all procpointers in the vdp data structure
 * cleaned up some dead code
 *
 * Revision 1.3  1999/11/30 03:34:14  nyef
 * added preliminary support for DMA transfers (hacked up interface)
 *
 * Revision 1.2  1999/09/08 01:04:25  nyef
 * changed status register to be 16 bit
 * added preliminary support for vblank interrupts
 *
 * Revision 1.1  1999/04/17 22:24:52  nyef
 * Initial revision
 *
 */
