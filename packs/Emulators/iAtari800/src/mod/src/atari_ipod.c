// Last updated: July 31, 2008
// ~Keripo

/*
 * atari_ipod.c - iPodLinux specific port code, based on SDL port
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

#include "config.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Atari800 includes */
#include "input.h"
#include "colours.h"
#include "monitor.h"
#include "platform.h"
#include "ui.h"
#include "screen.h"
#include "pokeysnd.h"
#include "gtia.h"
#include "antic.h"
#include "devices.h"
#include "cpu.h"
#include "memory.h"
#include "pia.h"
#include "log.h"
#include "util.h"

/* you can set that variables in code, or change it when emulator is running
   I am not sure what to do with sound_enabled (can't turn it on inside
   emulator, probably we need two variables or command line argument) */
#ifdef SOUND
static int sound_enabled = TRUE;
static int sound_flags = 0;
static int sound_bits = 8;
#endif
static int SDL_ATARI_BPP = 0;	/* 0 - autodetect */
static int FULLSCREEN = 1;
static int BW = 0;
static int WIDTH_MODE = 1;
#define SHORT_WIDTH_MODE 0
#define DEFAULT_WIDTH_MODE 1
#define FULL_WIDTH_MODE 2

/* you need to uncomment this to turn on fps counter */

/* #define FPS_COUNTER = 1 */

#ifdef SOUND
/* sound */
#define FRAGSIZE        10		/* 1<<FRAGSIZE is size of sound buffer */
static int SOUND_VOLUME = SDL_MIX_MAXVOLUME / 4;
static int dsprate = 44100;
#endif

/* video */
SDL_Surface *MainScreen = NULL;
SDL_Color colors[256];			/* palette */
Uint16 Palette16[256];			/* 16-bit palette */
Uint32 Palette32[256];			/* 32-bit palette */

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

int Atari_Configure(char *option, char *parameters)
{
	return FALSE;
}

void Atari_ConfigSave(FILE *fp)
{
}

char *joy_0_description(char *buffer, int maxsize)
{
	snprintf(buffer, maxsize, " (L=%s R=%s U=%s D=%s B=%s)",
			SDL_GetKeyName(KBD_STICK_0_LEFT),
			SDL_GetKeyName(KBD_STICK_0_RIGHT),
			SDL_GetKeyName(KBD_STICK_0_UP),
			SDL_GetKeyName(KBD_STICK_0_DOWN),
			SDL_GetKeyName(KBD_TRIG_0)
	);
	return buffer;
}

char *joy_1_description(char *buffer, int maxsize)
{
	return '\0'; // No second joystick used
}

#ifdef SOUND

void Sound_Pause()
{
	if (sound_enabled) {
		/* stop audio output */
		SDL_PauseAudio(1);
	}
}

void Sound_Continue()
{
	if (sound_enabled) {
		/* start audio output */
		SDL_PauseAudio(0);
	}
}

void Sound_Update()
{
	/* fake function */
}

#endif /* SOUND */

static void SetPalette()
{
	SDL_SetPalette(MainScreen, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
}

void CalcPalette()
{
	int i, rgb;
	float y;
	Uint32 c;
	if (BW == 0)
		for (i = 0; i < 256; i++) {
			rgb = colortable[i];
			colors[i].r = (rgb & 0x00ff0000) >> 16;
			colors[i].g = (rgb & 0x0000ff00) >> 8;
			colors[i].b = (rgb & 0x000000ff) >> 0;
		}
	else
		for (i = 0; i < 256; i++) {
			rgb = colortable[i];
			y = 0.299 * ((rgb & 0x00ff0000) >> 16) +
				0.587 * ((rgb & 0x0000ff00) >> 8) +
				0.114 * ((rgb & 0x000000ff) >> 0);
			colors[i].r = y;
			colors[i].g = y;
			colors[i].b = y;
		}
	for (i = 0; i < 256; i++) {
		c =
			SDL_MapRGB(MainScreen->format, colors[i].r, colors[i].g,
					   colors[i].b);
		switch (MainScreen->format->BitsPerPixel) {
		case 16:
			Palette16[i] = (Uint16) c;
			break;
		case 32:
			Palette32[i] = (Uint32) c;
			break;
		}
	}

}

void ModeInfo()
{
	char bwflag, fullflag, width, joyflag;
	if (BW)
		bwflag = '*';
	else
		bwflag = ' ';
	if (FULLSCREEN)
		fullflag = '*';
	else
		fullflag = ' ';
	switch (WIDTH_MODE) {
	case FULL_WIDTH_MODE:
		width = 'f';
		break;
	case DEFAULT_WIDTH_MODE:
		width = 'd';
		break;
	case SHORT_WIDTH_MODE:
		width = 's';
		break;
	default:
		width = '?';
		break;
	}
	joyflag = ' ';
	Aprint("Video Mode: %dx%dx%d", MainScreen->w, MainScreen->h,
		   MainScreen->format->BitsPerPixel);
	Aprint("[%c] FULLSCREEN  [%c] BW  [%c] WIDTH MODE  [%c] JOYSTICKS SWAPPED",
		 fullflag, bwflag, width, joyflag);
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

void SetNewVideoMode(int w, int h, int bpp)
{
	SetVideoMode(w, h, bpp);

	SDL_ATARI_BPP = MainScreen->format->BitsPerPixel;
	if (bpp == 0) {
		Aprint("detected %dbpp", SDL_ATARI_BPP);
		if ((SDL_ATARI_BPP != 8) && (SDL_ATARI_BPP != 16)
			&& (SDL_ATARI_BPP != 32)) {
			Aprint("it's unsupported, so setting 8bit mode (slow conversion)");
			SetVideoMode(w, h, 8);
		}
	}

	SetPalette();

	SDL_ShowCursor(SDL_DISABLE);	/* hide mouse cursor */

	ModeInfo();
}

void SwitchFullscreen()
{
	FULLSCREEN = 1 - FULLSCREEN;
	SetNewVideoMode(MainScreen->w, MainScreen->h,
					MainScreen->format->BitsPerPixel);
	Atari_DisplayScreen();
}

void SwitchWidth()
{
	WIDTH_MODE++;
	if (WIDTH_MODE > FULL_WIDTH_MODE)
		WIDTH_MODE = SHORT_WIDTH_MODE;
	SetNewVideoMode(MainScreen->w, MainScreen->h,
					MainScreen->format->BitsPerPixel);
	Atari_DisplayScreen();
}

void SwitchBW()
{
	BW = 1 - BW;
	CalcPalette();
	SetPalette();
	ModeInfo();
}

#ifdef SOUND
void SDL_Sound_Update(void *userdata, Uint8 *stream, int len)
{
	Uint8 dsp_buffer[2 << FRAGSIZE]; /* x2, because 16bit buffers */
	if (len > 1 << FRAGSIZE)
		len = 1 << FRAGSIZE;
	Pokey_process(dsp_buffer, len);
	if (sound_bits == 8)
		SDL_MixAudio(stream, dsp_buffer, len, SOUND_VOLUME);
	else
		SDL_MixAudio(stream, dsp_buffer, 2 * len, SOUND_VOLUME);
}

void SDL_Sound_Initialise(int *argc, char *argv[])
{
	int i, j;
	SDL_AudioSpec desired, obtained;

	for (i = j = 1; i < *argc; i++) {
		if (strcmp(argv[i], "-sound") == 0)
			sound_enabled = TRUE;
		else if (strcmp(argv[i], "-nosound") == 0)
			sound_enabled = FALSE;
		else if (strcmp(argv[i], "-audio16") == 0) {
			Aprint("audio 16bit enabled");
			sound_flags |= SND_BIT16;
			sound_bits = 16;
		}
		else if (strcmp(argv[i], "-dsprate") == 0)
			dsprate = Util_sscandec(argv[++i]);
		else {
			if (strcmp(argv[i], "-help") == 0) {
				Aprint("\t-sound           Enable sound");
				Aprint("\t-nosound         Disable sound");
				Aprint("\t-dsprate <rate>  Set DSP rate in Hz");
				sound_enabled = FALSE;
			}
			argv[j++] = argv[i];
		}
	}
	*argc = j;

	if (sound_enabled) {
		desired.freq = dsprate;
		if (sound_bits == 8)
			desired.format = AUDIO_U8;
		else if (sound_bits == 16)
			desired.format = AUDIO_U16;
		else {
			Aprint("unknown sound_bits");
			Atari800_Exit(FALSE);
			Aflushlog();
		}

		desired.samples = 1 << FRAGSIZE;
		desired.callback = SDL_Sound_Update;
		desired.userdata = NULL;
		desired.channels = 1;

		if (SDL_OpenAudio(&desired, &obtained) < 0) {
			Aprint("Problem with audio: %s", SDL_GetError());
			Aprint("Sound is disabled.");
			sound_enabled = FALSE;
			return;
		}

		Pokey_sound_init(FREQ_17_EXACT, dsprate, 1, sound_flags);
		SDL_PauseAudio(0);
	}
}
#endif /* SOUND */

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
		case SDL_VIDEORESIZE:
			SetNewVideoMode(event.resize.w, event.resize.h, MainScreen->format->BitsPerPixel);
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

void Atari_Initialise(int *argc, char *argv[])
{
	int i, j;
	int width, height, bpp;
	int help_only = FALSE;

	//width = ATARI_WIDTH;
	//height = ATARI_HEIGHT;
	//bpp = SDL_ATARI_BPP;

	for (i = j = 1; i < *argc; i++) {
		if (strcmp(argv[i], "-width") == 0) {
			width = Util_sscandec(argv[++i]);
			Aprint("width set", width);
		}
		else if (strcmp(argv[i], "-height") == 0) {
			height = Util_sscandec(argv[++i]);
			Aprint("height set");
		}
		else if (strcmp(argv[i], "-bpp") == 0) {
			bpp = Util_sscandec(argv[++i]);
			Aprint("bpp set");
		}
		else if (strcmp(argv[i], "-fullscreen") == 0) {
			FULLSCREEN = 1;
		}
		else if (strcmp(argv[i], "-windowed") == 0) {
			FULLSCREEN = 0;
		}
		else {
			if (strcmp(argv[i], "-help") == 0) {
				help_only = TRUE;
				Aprint("\t-width <num>     Host screen width");
				Aprint("\t-height <num>    Host screen height");
				Aprint("\t-bpp <num>       Host color depth");
				Aprint("\t-fullscreen      Run fullscreen");
				Aprint("\t-windowed        Run in window");
			}
			argv[j++] = argv[i];
		}
	}
	*argc = j;

	i = SDL_INIT_VIDEO;
#ifdef SOUND
	if (!help_only)
		i |= SDL_INIT_AUDIO;
#endif
	if (SDL_Init(i) != 0) {
		Aprint("SDL_Init FAILED");
		Aprint(SDL_GetError());
		Aflushlog();
		exit(-1);
	}
	atexit(SDL_Quit);

#ifdef SOUND
	SDL_Sound_Initialise(argc, argv);
#endif

	if (help_only)
		return;		/* return before changing the gfx mode */

	// Autodetect screen dimension
	SDL_Rect **modes;
    modes = SDL_ListModes(NULL, 0);
    if (modes == (SDL_Rect **)0){
        fprintf(stderr, "No Video mode available.\n");
        exit(-1);
    }
	width = modes[0]->w;
	height = modes[0]->h;
	bpp = SDL_ATARI_BPP;

	SetNewVideoMode(width, height, bpp);
	CalcPalette();
	SetPalette();

	Aprint("video initialized");

	SDL_EnableUNICODE(1);
}

int Atari_Exit(int run_monitor)
{
	int restart;
	int original_fullscreen = FULLSCREEN;

	if (run_monitor) {
		/* disable graphics, set alpha mode */
		if (FULLSCREEN) {
			SwitchFullscreen();
		}
#ifdef SOUND
		Sound_Pause();
#endif
		restart = monitor();
#ifdef SOUND
		Sound_Continue();
#endif
	}
	else {
		restart = FALSE;
	}

	if (restart) {
		/* set up graphics and all the stuff */
		if (original_fullscreen != FULLSCREEN) {
			SwitchFullscreen();
		}
		return 1;
	}

	SDL_Quit();

	Aflushlog();

	return restart;
}

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
	switch (MainScreen->format->BitsPerPixel) {
	case 8:
		while (i > 0) {
			memcpy(start32, screen, width);
			screen += ATARI_WIDTH;
			start32 += pitch4;
			i--;
		}
		break;
	case 16:
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
		break;
	case 32:
		while (i > 0) {
			pos = width - 1;
			while (pos > 0) {
				c = screen[pos];
				quad = Palette32[c];
				start32[pos] = quad;
				pos--;
			}
			screen += ATARI_WIDTH;
			start32 += pitch4;
			i--;
		}
		break;
	default:
		Aprint("unsupported color depth %d", MainScreen->format->BitsPerPixel);
		Aprint("please set SDL_ATARI_BPP to 8 or 16 and recompile atari_sdl");
		Aflushlog();
		exit(-1);
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

	switch (MainScreen->format->BitsPerPixel) {
	case 8:
		while (i > 0) {
			x = (width + jumped) << 16;
			pos = w4;
			yy = ATARI_WIDTH * (y >> 16);
			while (pos >= 0) {
				quad = (ss[yy + (x >> 16)] << 24);
				x = x - dx;
				quad += (ss[yy + (x >> 16)] << 16);
				x = x - dx;
				quad += (ss[yy + (x >> 16)] << 8);
				x = x - dx;
				quad += (ss[yy + (x >> 16)] << 0);
				x = x - dx;

				start32[pos] = quad;
				pos--;

			}
			start32 += pitch4;
			y = y + dy;
			i--;
		}
		break;
	case 16:
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
		break;
	case 32:
		while (i > 0) {
			x = (width + jumped) << 16;
			pos = w1;
			yy = ATARI_WIDTH * (y >> 16);
			while (pos >= 0) {

				c = ss[yy + (x >> 16)];
				quad = Palette32[c];
				x = x - dx;
				start32[pos] = quad;
				pos--;

			}
			start32 += pitch4;
			y = y + dy;
			i--;
		}

		break;
	default:
		Aprint("unsupported color depth %d", MainScreen->format->BitsPerPixel);
		Aprint("please set SDL_ATARI_BPP to 8 or 16 or 32 and recompile atari_sdl");
		Aflushlog();
		exit(-1);
	}
}

void Atari_DisplayScreen()
{
	int width, jumped;

	switch (WIDTH_MODE) {
	case SHORT_WIDTH_MODE:
		width = ATARI_WIDTH - 2 * 24 - 2 * 8;
		jumped = 24 + 8;
		break;
	case DEFAULT_WIDTH_MODE:
		width = ATARI_WIDTH - 2 * 24;
		jumped = 24;
		break;
	case FULL_WIDTH_MODE:
		width = ATARI_WIDTH;
		jumped = 0;
		break;
	default:
		Aprint("unsupported WIDTH_MODE");
		Aflushlog();
		exit(-1);
		break;
	}

	if (MainScreen->w == width && MainScreen->h == ATARI_HEIGHT) {
		DisplayWithoutScaling((UBYTE *) atari_screen, jumped, width);
	}
	else {
		DisplayWithScaling((UBYTE *) atari_screen, jumped, width);
	}
	SDL_Flip(MainScreen);
}

void SDL_Atari_PORT(Uint8 *s0, Uint8 *s1)
{
	int stick0, stick1;
	stick0 = stick1 = STICK_CENTRE;

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
	*s1 = stick1;
}

void SDL_Atari_TRIG(Uint8 *t0, Uint8 *t1)
{
	int trig0, trig1;
	trig0 = trig1 = 1;
	if (kbd_joy_0_enabled) {
		trig0 = kbhits[KBD_TRIG_0] ? 0 : 1;
	}
	*t0 = trig0;
	*t1 = trig1;
}

void CountFPS()
{
	static int ticks1 = 0, ticks2, shortframes;
	if (ticks1 == 0)
		ticks1 = SDL_GetTicks();
	ticks2 = SDL_GetTicks();
	shortframes++;
	if (ticks2 - ticks1 > 1000) {
		ticks1 = ticks2;
		Aprint("%d fps", shortframes);
		shortframes = 0;
	}
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

/*
vim:ts=4:sw=4:
*/
