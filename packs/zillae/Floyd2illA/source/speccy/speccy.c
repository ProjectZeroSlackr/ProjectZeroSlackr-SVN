/*
 * Copyright (C) 2005 Matthew Westcott
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdio.h>

#include "../pz.h"
#include "speccy.h"
#include "Z80.h"
#include "48_rom.h"

#define FRAME_TSTATES 69888	/* number of t-states in one frame */

static GR_WINDOW_ID speccy_wid;
static GR_GC_ID speccy_gc;
static GR_TIMER_ID speccy_timer_id;

static unsigned char mem[0x10000];
static Z80 z80;
static int flash_phase; /* counts up from 0 to 31, incrementing each frame -
	when bit 4 is set, flashing attributes are inverted */

static SPECCY_GAME speccy_game;

static int row_map[192];
static int col_map[256];
static GR_PIXELVAL palette[16];

static GR_COLOR palette_greyscales[16] = {
	BLACK, BLACK, GRAY, GRAY, LTGRAY, LTGRAY, WHITE, WHITE,
	BLACK, BLACK, GRAY, GRAY, LTGRAY, LTGRAY, WHITE, WHITE
};

static GR_COLOR palette_colours[16] = {
	MWRGB(0x00, 0x00, 0x00), MWRGB(0x00, 0x00, 0xc0), MWRGB(0xc0, 0x00, 0x00), MWRGB(0xc0, 0x00, 0xc0),
	MWRGB(0x00, 0xc0, 0x00), MWRGB(0x00, 0xc0, 0xc0), MWRGB(0xc0, 0xc0, 0x00), MWRGB(0xc0, 0xc0, 0xc0),
	MWRGB(0x00, 0x00, 0x00), MWRGB(0x00, 0x00, 0xff), MWRGB(0xff, 0x00, 0x00), MWRGB(0xff, 0x00, 0xff),
	MWRGB(0x00, 0xff, 0x00), MWRGB(0x00, 0xff, 0xff), MWRGB(0xff, 0xff, 0x00), MWRGB(0xff, 0xff, 0xff),
};

byte DebugZ80(register Z80 *R)
{
	printf("running at address %d [%d]\n", R->PC.W, mem[R->PC.W]);
	getchar();
	return 1;
}

static void draw_speccy_screen()
{
	int yblock, ychar, ypix, ywin, xchar, xpix, xwin;
	unsigned char *attrib_line_start = mem + 0x5800;
	unsigned char *pixmap_block_start = mem + 0x4000;
	unsigned char *pixmap_line_start, *pixmap_pixrow_start, *pixmap, *attrib;
	GR_PIXELVAL ink, paper;

	for (yblock = 0; yblock < 192; yblock += 64) {
		pixmap_line_start = pixmap_block_start;
		for (ychar = 0; ychar < 64; ychar += 8) {
			pixmap_pixrow_start = pixmap_line_start;
			for (ypix = 0; ypix < 8; ypix++) {
				ywin = row_map[yblock + ychar + ypix];
				if (ywin != -1) {
					/* plot this row */
					pixmap = pixmap_pixrow_start;
					attrib = attrib_line_start;
					for (xchar = 0; xchar < 256; xchar += 8) {
						if ((flash_phase & 0x10) && (*attrib & 0x80)) {
							/* invert ink and paper */
							ink = palette[(*attrib & 0x78) >> 3];
							paper = palette[(*attrib & 0x40) >> 3 | (*attrib & 0x07)];
						} else {
							paper = palette[(*attrib & 0x78) >> 3];
							ink = palette[(*attrib & 0x40) >> 3 | (*attrib & 0x07)];
						}
						for (xpix = 0; xpix < 8;xpix++) {
							xwin = col_map[xchar + xpix];
							if (xwin != -1) {
								/* plot pixel at (xwin, ywin), colour paper or ink */
								GrSetGCForegroundPixelVal(speccy_gc, (*pixmap & (1 << (xpix ^ 0x07))) ? ink : paper);
								GrPoint(speccy_wid, speccy_gc, xwin, ywin);
								/* printf("plotting at %d, %d\n", xpix, ypix); */
							}
						}
						pixmap++;
						attrib++;
					}
				}
				pixmap_pixrow_start += 0x0100;
			}
			pixmap_line_start += 0x20;
			attrib_line_start += 0x20;
		}
		pixmap_block_start += 0x0800;
	}
}

/* Given an address in the bitmap part of screen memory, redraw the corresponding pixels */
static void update_bitmap_byte(word addr)
{
	int xpix, ypix, xwin, ywin, i;
	unsigned char bmp, attrib;
	GR_PIXELVAL ink, paper;

	ypix = (addr & 0x1800) >> 5 | (addr & 0x0700) >> 8 | (addr & 0x00e0) >> 2;
	ywin = row_map[ypix];
	if (ywin == -1) return; /* nothing to draw */
	bmp = mem[addr];
	xpix = (addr & 0x1f) << 3;
	attrib = mem[0x5800 | (addr & 0x1800) >> 3 | (addr & 0x00ff)];
	if ((flash_phase & 0x10) && (attrib & 0x80)) {
		/* invert ink and paper */
		ink = palette[(attrib & 0x78) >> 3];
		paper = palette[(attrib & 0x40) >> 3 | (attrib & 0x07)];
	} else {
		paper = palette[(attrib & 0x78) >> 3];
		ink = palette[(attrib & 0x40) >> 3 | (attrib & 0x07)];
	}
	for (i = 0; i < 8; i++) {
		xwin = col_map[xpix + i];
		if (xwin != -1) {
			/* plot pixel at (xwin, ywin), colour paper or ink */
			GrSetGCForegroundPixelVal(speccy_gc, (bmp & (1 << (i ^ 0x07))) ? ink : paper);
			GrPoint(speccy_wid, speccy_gc, xwin, ywin);
		}
	}
}

static void update_attribute_byte(word addr)
{
	unsigned char attrib, bmp;
	int xpix, ypix, xwin, ywin, pixaddr, i, j;
	GR_PIXELVAL ink, paper;

	attrib = mem[addr];
	if ((flash_phase & 0x10) && (attrib & 0x80)) {
		/* invert ink and paper */
		ink = palette[(attrib & 0x78) >> 3];
		paper = palette[(attrib & 0x40) >> 3 | (attrib & 0x07)];
	} else {
		paper = palette[(attrib & 0x78) >> 3];
		ink = palette[(attrib & 0x40) >> 3 | (attrib & 0x07)];
	}
	xpix = (addr & 0x1f) << 3;
	ypix = (addr & 0x03e0) >> 2;
	pixaddr = 0x4000 | (addr & 0x0300) << 3 | (addr & 0x00ff);
	for (j = 0; j < 8; j++) {
		bmp = mem[pixaddr];
		ywin = row_map[ypix];
		if (ywin != -1) {
			for (i = 0; i < 8; i++) {
				xwin = col_map[xpix + i];
				if (xwin != -1) {
					/* plot pixel at (xwin, ywin), colour paper or ink */
					GrSetGCForegroundPixelVal(speccy_gc, (bmp & (1 << (i ^ 0x07))) ? ink : paper);
					GrPoint(speccy_wid, speccy_gc, xwin, ywin);
				}
			}
		}
		ypix++;
		pixaddr += 0x0100;
	}
}

static void speccy_do_draw()
{
//	speccy_game.draw_header();
	draw_speccy_screen();
}

void WrZ80(register word Addr,register byte Value)
{
	if (!(Addr & 0xc000)) {
		/* attempted write to ROM */
	} else if (Addr < 0x5b00) {
		/* Optimisation - no screen update if byte doesn't change */
		if (mem[Addr] != Value) {
			mem[Addr] = Value;
			if (Addr < 0x5800) {
				update_bitmap_byte(Addr);
			} else {
				update_attribute_byte(Addr);
			}
		}
	} else {
		mem[Addr] = Value;
	}
}

byte RdZ80(register word Addr)
{
	return mem[Addr];
}

void OutZ80(register word Port,register byte Value)
{
	/* hand this off to the game code */
	speccy_game.out_z80(Port, Value);
}

byte InZ80(register word Port)
{
	/* hand this off to the game code */
	return speccy_game.in_z80(Port);
}

void PatchZ80(register Z80 *R)
{
}

word LoopZ80(register Z80 *R)
{
	/* printf("interrupted at address %d [%d]\n", R->PC.W, mem[R->PC.W]); */
	IntZ80(R, INT_IRQ);
	return INT_QUIT;
}

static void speccy_loop()
{
	word i;

	RunZ80(&z80);
	flash_phase = (flash_phase + 1) & 0x1f;
	if (!(flash_phase & 0x0f)) {
		/* flash effect is in transition - refresh all attribute squares with flash bit set */
		for (i = 0x5800; i < 0x5b00; i++) {
			if (mem[i] & 0x80) {
				update_attribute_byte(i);
			}
		}
	}
}

void speccy_end() {
	pz_close_window(speccy_wid);
	GrDestroyTimer(speccy_timer_id);
	GrDestroyGC(speccy_gc);
}

static int speccy_handle_event(GR_EVENT *event)
{
	int ret = 0;
	static int paused = 0;

	switch (event->type) {

	case GR_EVENT_TYPE_TIMER:
		if (!paused)
			speccy_loop();
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		//case 'h': /* hold switch on */
		//	paused = 1; /* pause the loop */
		//	break;
		case 'x': /* debug */
			draw_speccy_screen();
			break;
		default:
			/* let the game decide how to handle other keypress events */
			return speccy_game.handle_key_event(event);
		}
	case GR_EVENT_TYPE_KEY_UP:
		switch (event->keystroke.ch) {
		case 'h': /* un-held */
			paused = 0;
			break;
		default:
			/* let the game decide how to handle other key release events */
			return speccy_game.handle_key_event(event);
		}
	default:
		ret |= EVENT_UNUSED; /* we didn't ask for this event, go away */
	}
	return ret;
}

void speccy_load_sna(unsigned char *sna) {
	int im_num;
	byte iff2, im;

	z80.I = *(sna++);
	z80.HL1.B.l = *(sna++); z80.HL1.B.h = *(sna++);
	z80.DE1.B.l = *(sna++); z80.DE1.B.h = *(sna++);
	z80.BC1.B.l = *(sna++); z80.BC1.B.h = *(sna++);
	z80.AF1.B.l = *(sna++); z80.AF1.B.h = *(sna++);
	z80.HL.B.l = *(sna++); z80.HL.B.h = *(sna++);
	z80.DE.B.l = *(sna++); z80.DE.B.h = *(sna++);
	z80.BC.B.l = *(sna++); z80.BC.B.h = *(sna++);
	z80.IY.B.l = *(sna++); z80.IY.B.h = *(sna++);
	z80.IX.B.l = *(sna++); z80.IX.B.h = *(sna++);
	iff2 = (*(sna++) & 0x04) ? IFF_2 : 0;
	z80.R = *(sna++);
	z80.AF.B.l = *(sna++); z80.AF.B.h = *(sna++);
	z80.SP.B.l = *(sna++); z80.SP.B.h = *(sna++);
	im_num = *(sna++);
	if (im_num == 0) {
		im = 0;
	} else if (im_num == 2) {
		im = IFF_IM2;
	} else { /* im_num == 1 */
		im = IFF_IM1;
	}
	z80.IFF = im | iff2;
	sna++; /* ignore border colour */
	memcpy(mem + 0x4000, sna, 0xc000 * sizeof(unsigned char));

	/* All data loaded. Now simulate a RETN to get PC pointing to the right place */
	z80.PC.B.l = RdZ80(z80.SP.W); WrZ80(z80.SP.W, 0); z80.SP.W++;
	z80.PC.B.h = RdZ80(z80.SP.W); WrZ80(z80.SP.W, 0); z80.SP.W++;
}

void new_speccy_window(SPECCY_GAME game)
{
	int wnd_top, wnd_left, wnd_width, wnd_height;
	int x, y, i;

	speccy_game = game;

	speccy_gc = pz_get_gc(1);   /* Get a copy of the root graphics context */

	wnd_left = 0;
	wnd_top = 0;
	wnd_width = screen_info.cols;
	wnd_height = screen_info.rows;

	/* Open the window: */
	speccy_wid = pz_new_window(wnd_left, wnd_top, wnd_width, wnd_height,
			speccy_do_draw, /* function pointer, called on an EXPOSURE event */
			speccy_handle_event); /* function pointer, called on all other selected events */

	/* set up tables mapping Spectrum coordinates to window coordinates */
	/* prefill map with -1 (to mean 'no corresponding window pixel') */
	for (x = 0; x < 256; x++) col_map[x] = -1;
	for(x = 0; x < wnd_width; x++) {
		col_map[(x * 256) / wnd_width] = x;
	}

	for (y = 0; y < 192; y++) row_map[y] = -1;
	for (y = 0; y < wnd_height; y++) {
		row_map[(y * 192) / wnd_height] = y;
	}

	/* set up palette */
	if (screen_info.cols==220) {
		/* ipod photo - use colour palette */
		for (i = 0; i < 16; i++) GrFindColor(palette_colours[i], palette + i);
	} else {
		/* ipod - use greyscale palette */
		for (i = 0; i < 16; i++) GrFindColor(palette_greyscales[i], palette + i);
	}

	/* Select the types of events you need for your window: */
	GrSelectEvents (speccy_wid,
			GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP | GR_EVENT_MASK_TIMER);

	/* Map the window to the screen: */
	GrMapWindow (speccy_wid);

	/* insert ROM image into memory */
	memcpy(mem, rom48, 0x4000 * sizeof(unsigned char));
	ResetZ80(&z80);
	z80.IPeriod = FRAME_TSTATES;

	flash_phase = 0;

	speccy_game.setup();

	speccy_do_draw();
	speccy_timer_id = GrCreateTimer (speccy_wid, 20);
}
