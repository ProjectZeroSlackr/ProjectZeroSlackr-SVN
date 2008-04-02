/*
 * apple2_vdp.c
 *
 * emulation of the Apple ][ video display
 */

/* $Id: apple2_vdp.c,v 1.7 2000/03/16 03:41:52 nyef Exp $ */

#include <stdlib.h>
#include <string.h>

#include "video.h"
#include "ui.h"
#include "tool.h"
#include "apple2_vdp.h"

#define BLINK_TIMER_INTERVAL 22

int a2v_palbase_red[8] = {
    0, 0, 230, 255, 0, 230, 0, 255,
};

int a2v_palbase_green[8] = {
    0, 255, 0, 255, 0, 230, 0, 255,
};

int a2v_palbase_blue[8] = {
    0, 0, 230, 255, 0, 0, 255, 255,
};

struct apple2_vdp {
    u8 *memory;
    u8 *chargen;
    u16 scanline;
    int is_textmode;
    int is_hires;
    int is_mixed;
    int is_primary;
    int blink_on;
    int blink_timer;
};

void a2v_io(apple2_vdp vdp, int addr)
{
    switch (addr & 7) {
    case 0: /* $c050 */
	vdp->is_textmode = 0;
	break;

    case 1: /* $c051 */
	vdp->is_textmode = 1;
	break;

    case 2: /* $c052 */
	vdp->is_mixed = 0;
	break;

    case 3: /* $c053 */
	vdp->is_mixed = 1;
	break;

    case 4: /* $c054 */
	vdp->is_primary = 1;
	break;

    case 5: /* $c055 */
	vdp->is_primary = 0;
	break;

    case 6: /* $c056 */
	vdp->is_hires = 0;
	break;

    case 7: /* $c057 */
	vdp-> is_hires = 1;
	break;
    }
}

int a2v_text_membase[24] = {
    0x000, 0x080, 0x100, 0x180, 0x200, 0x280, 0x300, 0x380,
    0x028, 0x0a8, 0x128, 0x1a8, 0x228, 0x2a8, 0x328, 0x3a8,
    0x050, 0x0d0, 0x150, 0x1d0, 0x250, 0x2d0, 0x350, 0x3d0,
};

void a2v_render_text(apple2_vdp vdp)
{
    u8 *cur_vbp;
    u8 *membase;
    int i;
    u8 chardata;
    
    cur_vbp = video_get_vbp(vdp->scanline);

    membase = vdp->memory;
    
    if (vdp->is_primary) {
	membase += 0x400;
    } else {
	membase += 0x800;
    }

    membase += a2v_text_membase[vdp->scanline >> 3];
    
    for (i = 0; i < 40; i++) {
	chardata = vdp->chargen[((membase[i] & 0x3f) << 3) + (vdp->scanline & 7)];
	if (!(membase[i] & 0x80)) {
	    if ((!(membase[i] & 0x40)) || (vdp->blink_on)) {
		chardata ^= 0xff;
	    }
	}
	*cur_vbp++ = vid_pre_xlat[((chardata >> 1) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 2) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 3) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 4) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 5) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 6) & 1)? 7: 0];
	*cur_vbp++ = vid_pre_xlat[((chardata >> 7) & 1)? 7: 0];
    }
}

void a2v_finalize_graphics(u8 *vbp)
{
    u8 *cur_vbp;
    u8 last_pel;
    int i;

    cur_vbp = vbp;
    last_pel = 0;
    
    for (i = 0; i < 280; i++) {
	if (cur_vbp[i] & 3) {
	    if (last_pel) {
		last_pel = vid_pre_xlat[7];
		cur_vbp[i] = last_pel;
		last_pel = 1;
	    } else {
		cur_vbp[i] = vid_pre_xlat[cur_vbp[i]];
		last_pel = 1;
	    }
	} else {
	    cur_vbp[i] = vid_pre_xlat[cur_vbp[i]];
	    last_pel = 0;
	}
    }
}

void a2v_render_hires(apple2_vdp vdp)
{
    u8 *cur_vbp;
    u8 *saved_vbp;
    u8 *membase;
    u8 palbase;
    u8 chardata;
    int i;

    cur_vbp = video_get_vbp(vdp->scanline);

    membase = vdp->memory;
    membase += a2v_text_membase[vdp->scanline >> 3];
    membase += (vdp->scanline & 7) << 10;

    if (vdp->is_primary) {
	membase += 0x2000;
    } else {
	membase += 0x4000;
    }

    saved_vbp = cur_vbp;

    for (i = 0; i < 0x28; i++) {
	chardata = membase[i];
	palbase = (chardata & 0x80) >> 5;

	if (!(i & 1)) {
	    *cur_vbp++ = ((chardata << 1) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 1) & 1) | palbase;
	    *cur_vbp++ = ((chardata >> 1) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 3) & 1) | palbase;
	    *cur_vbp++ = ((chardata >> 3) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 5) & 1) | palbase;
	    *cur_vbp++ = ((chardata >> 5) & 2) | palbase;
	} else {
	    *cur_vbp++ = ((chardata) & 1) | palbase;
	    *cur_vbp++ = ((chardata) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 2) & 1) | palbase;
	    *cur_vbp++ = ((chardata >> 2) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 4) & 1) | palbase;
	    *cur_vbp++ = ((chardata >> 4) & 2) | palbase;
	    *cur_vbp++ = ((chardata >> 6) & 1) | palbase;
	}
    }

    a2v_finalize_graphics(saved_vbp);
}

void a2v_render_lores(apple2_vdp vdp)
{
    u8 *cur_vbp;
    u8 *saved_vbp;
    u8 *membase;
    int i;
    u8 chardata;
    
    cur_vbp = video_get_vbp(vdp->scanline);

    membase = vdp->memory;
    
    if (vdp->is_primary) {
	membase += 0x400;
    } else {
	membase += 0x800;
    }

    membase += a2v_text_membase[vdp->scanline >> 3];
    
    saved_vbp = cur_vbp;

    for (i = 0; i < 40; i++) {
	chardata = membase[i];
	if (vdp->scanline & 4) {
	    chardata >>= 4;
	} else {
	    chardata &= 15;
	}

	if (i & 1) {
	    *cur_vbp++ = (chardata >> 3) & 1;
	    *cur_vbp++ = (chardata << 1) & 2;
	    *cur_vbp++ = (chardata >> 1) & 1;
	    *cur_vbp++ = (chardata >> 1) & 2;
	    *cur_vbp++ = (chardata >> 3) & 1;
	    *cur_vbp++ = (chardata << 1) & 2;
	    *cur_vbp++ = (chardata >> 1) & 1;
	} else {
	    *cur_vbp++ = (chardata << 1) & 2;
	    *cur_vbp++ = (chardata >> 1) & 1;
	    *cur_vbp++ = (chardata >> 1) & 2;
	    *cur_vbp++ = (chardata >> 3) & 1;
	    *cur_vbp++ = (chardata << 1) & 2;
	    *cur_vbp++ = (chardata >> 1) & 1;
	    *cur_vbp++ = (chardata >> 1) & 2;
	}
    }

    a2v_finalize_graphics(saved_vbp);
}

void a2v_periodic(apple2_vdp vdp)
{
    if (vdp->scanline < 192) {
	if (vdp->is_textmode) {
	    a2v_render_text(vdp);
	} else if ((vdp->scanline >= 160) && (vdp->is_mixed)) {
	    a2v_render_text(vdp);
	} else if (vdp->is_hires) {
	    a2v_render_hires(vdp);
	} else {
	    a2v_render_lores(vdp);
	}
    } else if (vdp->scanline == 262) {
	video_display_buffer();
	vdp->scanline = 0;
	if (!--vdp->blink_timer) {
	    vdp->blink_timer = BLINK_TIMER_INTERVAL;
	    vdp->blink_on ^= 1;
	}
	return;
    }
    
    vdp->scanline++;
}

void a2v_load_chargen(apple2_vdp vdp)
{
    rom_file romfile;

    vdp->chargen = NULL;
    
    romfile = read_romimage("applecg.bin");
    
    if (!romfile) {
	deb_printf("unable to load character generator.\n");
	return;
    }

    vdp->chargen = romfile->data;

    /* FIXME: memory leak */
}

apple2_vdp a2v_init(u8 *system_memory)
{
    apple2_vdp retval;

    retval = malloc(sizeof(struct apple2_vdp));

    if (!retval) {
	return NULL;
    }

    a2v_load_chargen(retval);

    if (!retval->chargen) {
	free(retval);
	return NULL;
    }
    
    retval->memory = system_memory;

    retval->blink_timer = BLINK_TIMER_INTERVAL;
    retval->blink_on = 0;

    video_setsize(280, 192);

    video_setpal(8, a2v_palbase_red, a2v_palbase_green, a2v_palbase_blue);

    return retval;
}

/*
 * $Log: apple2_vdp.c,v $
 * Revision 1.7  2000/03/16 03:41:52  nyef
 * cleaned up and fixed a minor problem with the hires mode
 * added preliminary support for lores graphics mode
 *
 * Revision 1.6  2000/03/13 03:34:08  nyef
 * fixed hires mode to display colors properly (more or less)
 *
 * Revision 1.5  2000/03/12 01:18:33  nyef
 * removed some annoying debug output from the I/O handler
 *
 * Revision 1.4  2000/03/12 01:00:55  nyef
 * added preliminary hires modes
 *
 * Revision 1.3  2000/03/08 04:40:12  nyef
 * added inverse and blinking text modes
 *
 * Revision 1.2  2000/03/08 04:08:34  nyef
 * fixed textmode to use white instead of green text
 *
 * Revision 1.1  2000/01/01 04:12:20  nyef
 * Initial revision
 *
 */
