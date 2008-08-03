// Last updated: Aug 3, 2008
// ~Keripo

/*
 * atari_ipod_sdl.c - iPodLinux specific port code, based on SDL port
 *
 * Copyright (c) 2008 Keripo
 * Copyright (c) 2001-2002 Jacek Poplawski
 * Copyright (C) 2001-2005 Atari800 development team (see DOC/CREDITS)
 *
 * This file is part of the Atari800 emulator project which emulates
 * the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
 *
 * Atari800 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atari800 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atari800; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// Includes all the #includes and #defines for my convenience ^_^
#include <SDL.h>
#include "atari_ipod.h"

/* video */

SDL_Surface *MainScreen = NULL;
SDL_Color colors[256];			/* palette */
Uint16 Palette16[256];			/* 16-bit palette */

/* keyboard */

Uint8 *kbhits;

/* joystick */

int kbd_joy_0_enabled = TRUE;
int kbd_joy_1_enabled = FALSE;

int KBD_STICK_0_UP		= SDLK_m;		// Menu
int KBD_STICK_0_DOWN	= SDLK_d;		// Pause/play
int KBD_STICK_0_LEFT	= SDLK_w;		// Rewind
int KBD_STICK_0_RIGHT	= SDLK_f;		// Fastforward
int KBD_TRIG_0			= SDLK_RETURN;	// Centre

/* dummied functions */

int Atari_Configure(char *option, char *parameters)
{
	return FALSE;
}

void Atari_ConfigSave(FILE *fp)
{
}

char *joy_0_description(char *buffer, int maxsize)
{
	return '\0';
}

char *joy_1_description(char *buffer, int maxsize)
{
	return '\0';
}

/* input */

int Atari_Keyboard()
{
	static int lastkey = AKEY_NONE, key_pressed = 0;	
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			lastkey = event.key.keysym.sym;
			key_pressed = 1;
			break;
		case SDL_KEYUP:
			lastkey = event.key.keysym.sym;
			key_pressed = 0;
			break;
		case SDL_QUIT:
			return AKEY_EXIT;
			break;
		}
	}
	else if (!key_pressed)
		return AKEY_NONE;
	
	kbhits = SDL_GetKeyState(NULL);

	if (kbhits == NULL) {
		Aprint("oops, kbhits is NULL!");
		Aflushlog();
		exit(-1);
	}
	
	key_consol = CONSOL_NONE;
	
	if (key_pressed == 0)
		return AKEY_NONE;
	
	// Use wheel scrolling for select and option due to lack of input keys
	static int scroll_count_l = 0, scroll_count_r = 0;
	#define SCROLL_MOD_NUM 50 // 100 / 2
	#define SCROLL_MOD_L(n) \
		({ \
			int use = 0; \
			if (++scroll_count_l >= n) { \
				scroll_count_l -= n; \
				use = 1; \
			} \
			if (scroll_count_r > 10) { \
				scroll_count_r--; \
			} \
			(use == 1); \
		})
	#define SCROLL_MOD_R(n) \
		({ \
			int use = 0; \
			if (++scroll_count_r >= n) { \
				scroll_count_r -= n; \
				use = 1; \
			} \
			if (scroll_count_l > 10) { \
				scroll_count_l--; \
			} \
			(use == 1); \
		})
	#define CLEAR_SCROLL() \
		({ \
			scroll_count_l = 0; \
			scroll_count_r = 0; \
		})
	
	// Key presses for in-game control
	if (!ui_is_active && kbd_joy_0_enabled) {
		if (lastkey == KBD_STICK_0_UP ||
			lastkey == KBD_STICK_0_DOWN ||
			lastkey == KBD_STICK_0_LEFT ||
			lastkey == KBD_STICK_0_RIGHT ||
			lastkey == KBD_TRIG_0) {
			key_pressed = 0;
			CLEAR_SCROLL();
			return AKEY_NONE;
		}
	}
	
	// Other input
	switch (lastkey) {
		// Enter/exit emulator menu
		case SDLK_h: // Hold switch
			key_pressed = 0;
			CLEAR_SCROLL();
			if (!ui_is_active)
				return AKEY_UI;
			else
				return AKEY_ESCAPE;
		// Scrolling used for start/select due to lack of input buttons
		case SDLK_l: // Scroll left
			if (!ui_is_active) {
				if (SCROLL_MOD_L(SCROLL_MOD_NUM)) {
					key_consol &= (~CONSOL_SELECT);
					key_pressed = 0;
					CLEAR_SCROLL();
				}
				return AKEY_NONE;
			} else {
				return AKEY_y; // "l" skips to a menu item
			}
		case SDLK_r: // Scroll right
			if (!ui_is_active) {
				if (SCROLL_MOD_R(SCROLL_MOD_NUM)) {
					key_consol &= (~CONSOL_START);
					key_pressed = 0;
					CLEAR_SCROLL();
				}
				return AKEY_NONE;
			} else {
				return AKEY_x; // "r" skips to a menu item
			}
		// All of the below are for the emulator's menu
		case SDLK_m: // Menu
			return AKEY_UP;
		case SDLK_d: // Pause/play
			return AKEY_DOWN;
		case SDLK_w: // Rewind
			return AKEY_LEFT;
		case SDLK_f: // Forward
			return AKEY_RIGHT;
		case SDLK_RETURN: // Centre
			return AKEY_RETURN;
		default:
			return AKEY_NONE;
	}
}

void SDL_Atari_PORT(Uint8 *s0, Uint8 *s1)
{
	int stick0;
	stick0 = STICK_CENTRE;

	if (kbd_joy_0_enabled) {
		if (kbhits[KBD_STICK_0_LEFT])
			stick0 = STICK_LEFT;
		if (kbhits[KBD_STICK_0_RIGHT])
			stick0 = STICK_RIGHT;
		if (kbhits[KBD_STICK_0_UP])
			stick0 = STICK_FORWARD;
		if (kbhits[KBD_STICK_0_DOWN])
			stick0 = STICK_BACK;
	}
	*s0 = stick0;
	*s1 = STICK_CENTRE;
}

void SDL_Atari_TRIG(Uint8 *t0, Uint8 *t1)
{
	int trig0;
	trig0 = 1;
	if (kbd_joy_0_enabled) {
		trig0 = kbhits[KBD_TRIG_0] ? 0 : 1;
	}
	*t0 = trig0;
	*t1 = 1;
}

int Atari_PORT(int num)
{
	if (num == 0) {
		UBYTE a, b;
		SDL_Atari_PORT(&a, &b);
		return (b << 4) | (a & 0x0f);
	}
	return 0xff;
}

int Atari_TRIG(int num)
{
	UBYTE a, b;
	SDL_Atari_TRIG(&a, &b);
	switch (num) {
	case 0:
		return a;
	case 1:
		return b;
	default:
		break;
	}
	return 1;
}

/* palette */

static void SetPalette()
{
	SDL_SetPalette(MainScreen, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
}

void CalcPalette()
{
	int i, rgb;
	Uint32 c;
	// BPP = 16
	for (i = 0; i < 256; i++) {
		rgb = colortable[i];
		colors[i].r = (rgb & 0x00ff0000) >> 16;
		colors[i].g = (rgb & 0x0000ff00) >> 8;
		colors[i].b = (rgb & 0x000000ff) >> 0;
	}
	for (i = 0; i < 256; i++) {
		c =	SDL_MapRGB(MainScreen->format,
			colors[i].r, colors[i].g, colors[i].b);
		Palette16[i] = (Uint16) c;
	}
}

void SetVideoMode(int w, int h, int bpp)
{
	MainScreen = SDL_SetVideoMode(w, h, bpp, SDL_FULLSCREEN);
	if (MainScreen == NULL) {
		Aprint("Setting Video Mode: %dx%dx%d FAILED", w, h, bpp);
		Aflushlog();
		exit(-1);
	}
}

void ModeInfo(); // prototype
void SetNewVideoMode(int w, int h, int bpp)
{
	SetVideoMode(w, h, bpp);
	
	if (bpp != 16) {
		Aprint("Unsupported bpp value; iAtari800 only runs on 16bpp colour iPods");
		exit(0);
	}
	
	SetPalette();
	
	SDL_ShowCursor(SDL_DISABLE);	/* hide mouse cursor */
	
	ModeInfo();
}

/* video blitting and cop */

static int use_cop;

void DisplayWithoutScaling(Uint8 *screen, int jumped, int width)
{
	register Uint32 quad;
	register Uint32 *start32;
	register Uint8 c;
	register int pos;
	register int pitch4;
	int i;

	pitch4 = MainScreen->pitch / 4;
	start32 = (Uint32 *) MainScreen->pixels;

	screen = screen + jumped;
	i = MainScreen->h;
	
	// BPP = 16
	while (i > 0) {
		pos = width - 1;
		while (pos > 0) {
			c = screen[pos];
			quad = Palette16[c] << 16;
			pos--;
			c = screen[pos];
			quad += Palette16[c];
			start32[pos >> 1] = quad;
			pos--;
		}
		screen += ATARI_WIDTH;
		start32 += pitch4;
		i--;
	}
}

void DisplayWithScaling(Uint8 *screen, int jumped, int width)
{
	register Uint32 quad;
	register int x;
	register int dx;
	register int yy;
	register Uint8 *ss;
	register Uint32 *start32;
	int i;
	int y;
	int w1, w2, w4;
	int w, h;
	int pos;
	int pitch4;
	int dy;
	Uint8 c;
	pitch4 = MainScreen->pitch / 4;
	start32 = (Uint32 *) MainScreen->pixels;

	w = (width) << 16;
	h = (ATARI_HEIGHT) << 16;
	dx = w / MainScreen->w;
	dy = h / MainScreen->h;
	w1 = MainScreen->w - 1;
	w2 = MainScreen->w / 2 - 1;
	w4 = MainScreen->w / 4 - 1;
	ss = screen;
	y = (0) << 16;
	i = MainScreen->h;
	
	// BPP = 16
	while (i > 0) {
		x = (width + jumped) << 16;
		pos = w2;
		yy = ATARI_WIDTH * (y >> 16);
		while (pos >= 0) {

			c = ss[yy + (x >> 16)];
			quad = Palette16[c] << 16;
			x = x - dx;
			c = ss[yy + (x >> 16)];
			quad += Palette16[c];
			x = x - dx;
			start32[pos] = quad;
			pos--;

		}
		start32 += pitch4;
		y = y + dy;
		i--;
	}
}

void ipod_update_screen()
{
	int width, jumped;
	width = ATARI_WIDTH;
	jumped = 0;
	if (MainScreen->w == width && MainScreen->h == ATARI_HEIGHT) {
		DisplayWithoutScaling((UBYTE *) atari_screen, jumped, width); // Will only run for 5/5.5G iPod videos
	}
	else {
		DisplayWithScaling((UBYTE *) atari_screen, jumped, width);
	}
	SDL_Flip(MainScreen);
}

static void ipod_cop_sync_screen()
{
	while (inl(COP_STATUS) & COP_RUNNING) // COP is active
	{
		// Get stuck in loop until told to update screen
		while ((inl(COP_STATUS) & COP_LINE_REQ) == 0);
		ipod_update_screen();
		outl(inl(COP_STATUS) &~ COP_LINE_REQ, COP_STATUS);
	}	
}

void ipod_cop_execute(void (*function)())
{
	outl((unsigned int)function, COP_HANDLER);
	outl(0x0, COP_CONTROL);
}

void ipod_init_cop()
{
	if (use_cop) {
		ipod_cop_clear_frameready();
		ipod_cop_execute(ipod_cop_sync_screen);	
		outl(COP_RUNNING, COP_STATUS); // Start COP?
	}
}

void Atari_DisplayScreen()
{	
	if (use_cop) {
		ipod_cop_update_screen();
	} else {
		ipod_update_screen();
	}
}

/* main */

void ModeInfo()
{
	char scale_type, cop;
	
	scale_type = 'u';
	if (use_cop) {
		cop = 'y';
	} else {
		cop = 'n';
	}
	
	Aprint("Video Mode: %dx%dx%d", MainScreen->w, MainScreen->h,
		MainScreen->format->BitsPerPixel);
	Aprint("[%c] SCALE TYPE  [%c] COP", scale_type, cop);
}

void Atari_Initialise(int *argc, char *argv[])
{
	int i, j;
	int help_only;
	help_only = FALSE;
	use_cop = TRUE;

	for (i = j = 1; i < *argc; i++) {
		if (strcmp(argv[i], "-fullscreen") == 0) {
			Aprint("-fullscreen not supported on SDL build");
		}
		else if (strcmp(argv[i], "-scaled") == 0) {
			Aprint("-scaled not supported on SDL build");
		}
		else if (strcmp(argv[i], "-uncropped") == 0) {
			// Only scale mode - no printout
		}
		else if (strcmp(argv[i], "-smoothing") == 0) {
			Aprint("-smoothing not supported on SDL build");
		}
		else if (strcmp(argv[i], "-no-smoothing") == 0) {
			Aprint("-no-smoothing not supported on SDL build");
		}
		else if (strcmp(argv[i], "-cop") == 0) {
			use_cop = TRUE;
		}
		else if (strcmp(argv[i], "-no-cop") == 0) {
			use_cop = FALSE;
		}
		else {
			if (strcmp(argv[i], "-help") == 0) {
				help_only = TRUE;
				Aprint("\t-uncropped	   Run fullscreen and uncropped [default]");
				Aprint("\t-cop		       Use co-processor [default]");
				Aprint("\t-no-cop		   Do not use co-processor");
			}
			argv[j++] = argv[i];
		}
	}
	*argc = j;

	if (help_only) return;
	
	ipod_init_cop();

	i = SDL_INIT_VIDEO;
	if (SDL_Init(i) != 0) {
		Aprint("SDL_Init FAILED");
		Aprint(SDL_GetError());
		Aflushlog();
		exit(-1);
	}
	atexit(SDL_Quit);

	// Autodetect screen dimension
	SDL_Rect **modes;
	modes = SDL_ListModes(NULL, 0);
	if (modes == (SDL_Rect **)0){
		fprintf(stderr, "No Video mode available.\n");
		exit(-1);
	}

	SetNewVideoMode(modes[0]->w, modes[0]->h, 16);	// iAtari800 only supports 16bpp colour ipods
	CalcPalette();
	SetPalette();

	SDL_EnableUNICODE(1);
}

int Atari_Exit(int run_monitor)
{
	SDL_Quit();
	Aflushlog();
	return FALSE;
}

int main(int argc, char **argv)
{
	/* initialise Atari800 core */
	if (!Atari800_Initialise(&argc, argv))
		return 3;

	/* main loop */
	for (;;) {
		key_code = Atari_Keyboard();
		Atari800_Frame();
		if (display_screen)
			Atari_DisplayScreen();
	}
}
