/*
 * Last updated: Aug 3, 2008
 * ~Keripo
 *
 * atari_ipod_hotdog.c - iPodLinux specific, optimized port code
 *
 * Copyright (C) 2008 Keripo
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

// Includes all the #includes and #defines for my convenience ^_^
#include "hotdog.h"
#include "atari_ipod.h"

/* video */

static int IPOD_HW_VER, IPOD_LCD_TYPE;
static int IPOD_WIDTH, IPOD_HEIGHT;
static int scale_x[320], scale_y[240];
static uint16 *ipod_screen;
uint16 colour_palette[256];

/* joystick */

int kbd_joy_0_enabled = TRUE;
int kbd_joy_1_enabled = FALSE;

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

static int joystick, trigger;
static int console;
static struct termios stored_settings;

static int ipod_get_keypress()
{
	int press = 0;
	if (read(console, &press, 1) != 1)
		return KEY_NULL;
	return press;
}

static int ipod_get_keytouch()
{
	int touch;
	
	touch = 0xff;
	if (IPOD_HW_VER != 0x4 && IPOD_HW_VER != 0x3) { // Not mini 1G or 3G
		int in, st;
		in = inl(0x7000C140);
		st = ((in & 0xff000000) >> 24);
		
		touch = 0xff;
		if (st == 0xc0)
			touch = (in & 0x007F0000 ) >> 16;
	}
	
	// See http://ipodlinux.org/wiki/Key_Chart
	// The +6 is for rounding
	if (touch != 0xff) {
		touch += 6;
		touch /= 12;
		if(touch > 7)
			touch = 0;
		return touch;
	} else {
		return TOUCH_NULL;
	}
}

void ipod_init_input()
{
	struct termios new_settings;
	console = open("/dev/console", O_RDONLY | O_NONBLOCK);
	tcgetattr(console, &stored_settings);
	
	new_settings = stored_settings;
	new_settings.c_lflag &= ~(ICANON | ECHO | ISIG);
	new_settings.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON | BRKINT);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 0;
	
	tcsetattr(console, TCSAFLUSH, &new_settings);
	ioctl(console, KDSKBMODE, K_MEDIUMRAW);
}

void ipod_exit_input()
{
	close(console);
}

int Atari_Keyboard()
{
	joystick = STICK_CENTRE;
	trigger = 1;
	key_consol = CONSOL_NONE;
	
	int input;
	
	if (!ui_is_active) {
	input = ipod_get_keytouch();
		switch(input) {
			case TOUCH_NULL:
				break;
			case TOUCH_U:
				joystick = STICK_FORWARD;
				break;
			case TOUCH_UR:
				// Nothing?
				break;
			case TOUCH_R:
				joystick = STICK_RIGHT;
				break;
			case TOUCH_DR:
				key_consol &= (~CONSOL_START);
				break;
			case TOUCH_D:
				joystick = STICK_BACK;
				break;
			case TOUCH_DL:
				key_consol &= (~CONSOL_SELECT);
				break;
			case TOUCH_L:
				joystick = STICK_LEFT;
				break;
			case TOUCH_UL:
				key_consol &= (~CONSOL_OPTION);
				break;
			default:
				break;
		}
	}
	
	input = ipod_get_keypress();
	if (input != KEY_NULL) {
		if (KEYSTATE(input)) { // Key up/lifted
			if (ui_is_active) {
				input = KEYCODE(input);
				if (input == KEY_HOLD || input == KEY_ACTION)
					Atari_DisplayScreen(); // Menu changed
			}
			return AKEY_NONE;
		} else {
			input = KEYCODE(input);
			switch (input) { // In numeric order for speed
				case KEY_REWIND:
					return AKEY_LEFT;
					break;
				case SCROLL_R:
					if (!ui_is_active) {
						return AKEY_NONE;
					} else {
						return AKEY_x; // "l" skips to a menu item
					}
					break;
				case KEY_ACTION:
					if (!ui_is_active)
						trigger = 0;
					return AKEY_RETURN;
					break;
				case KEY_PLAY:
					return AKEY_DOWN;
					break;
				case KEY_FORWARD:
					return AKEY_RIGHT;
					break;
				case KEY_HOLD:
					if (!ui_is_active)
						return AKEY_UI;
					else
						return AKEY_ESCAPE;
					break;
				case SCROLL_L:
					if (!ui_is_active) {
						return AKEY_NONE;
					} else {
						return AKEY_y; // "r" skips to a menu item
					}
					break;
				case KEY_MENU: 
					return AKEY_UP;
					break;
				default:
					return AKEY_NONE;
					break;
			}
		}
	}
	return AKEY_NONE;
}

int Atari_PORT(int num)
{
	if (num == 0) {
		UBYTE a, b;
		a = joystick;
		b = STICK_CENTRE;
		return (b << 4) | (a & 0x0f);
	}
	return 0xff;
}

int Atari_TRIG(int num)
{
	return trigger;
}

/* palette */

void ipod_calc_palette()
{	
	int i, r, g, b, rgb;
	for (i = 0; i < 256; i++) {
		rgb = colortable[i];
		r = (rgb & 0x00ff0000) >> 16;
		g = (rgb & 0x0000ff00) >> 8;
		b = (rgb & 0x000000ff) >> 0;
		colour_palette[i] = RGB565(r, g, b);
	}
}

/* video blitting and cop */

static int smooth;
static int use_cop;
static int ipod_scale_type;

typedef void (*ipod_update_screen_type)();
static ipod_update_screen_type ipod_update_screen_funct;

void ipod_rescale()
{
	int i, offset_x, offset_y, offset_s;
	offset_x = 64; // These offsets are for omitting the added black border; these values are from
	offset_y = 44; // experimentation on an iPod nano - need to check if they work on other models
	offset_s = 0; // For SCALED
	switch (ipod_scale_type) {
		case FULLSCREEN: // Stretch and cropped
			for (i = 0; i < IPOD_WIDTH; i++)
				scale_x[i] =
					i * (ATARI_WIDTH - offset_x) / IPOD_WIDTH
					+ (offset_x + 8) * IPOD_WIDTH / ATARI_WIDTH;
					// +8 determined experimentally - need to check on other iPod models
			for (i = 0; i < IPOD_HEIGHT; i++)
				scale_y[i] = (
					i * (ATARI_HEIGHT - offset_y) / IPOD_HEIGHT
					+ (offset_y) * IPOD_HEIGHT / ATARI_HEIGHT
					) * ATARI_WIDTH;
			break;
		case SCALED: // Scale to width and scropped
			/*
			  * Note that by not setting those between 0 to offset_s and offset_s to IPOD_HEIGHT, those pixels
			  * all default to the very first pixel of atari_screen (usually a background colour). While I can easily
			  * add code for checking the boundaries and setting those pixels instead to black, Atari games seldomly
			  * change backgrounds and so it isn't worth it over speed (slowdown due to the extra checks).
			  * As well, note that (ATARI_WIDTH - 48) is used as actual Atari dimensions are 336 x 240, not 384 x 240
			  */
			offset_s = (IPOD_HEIGHT - ATARI_HEIGHT * IPOD_WIDTH / (ATARI_WIDTH - 48)) / 2;
			for (i = 0; i < IPOD_WIDTH; i++)
				scale_x[i] =
					i * (ATARI_WIDTH - offset_x) / IPOD_WIDTH
					+ (offset_x + 8) * IPOD_WIDTH / ATARI_WIDTH;
			for (i = offset_s; i < IPOD_HEIGHT - offset_s; i++)
				scale_y[i] = (
					(i - offset_s) * ((ATARI_WIDTH - 48) - offset_x) / IPOD_WIDTH
					+ (offset_y) * IPOD_WIDTH / (ATARI_WIDTH - 48)
					) * ATARI_WIDTH;
			break;
		case UNCROPPED: // Stretched and uncropped
			for (i = 0; i < IPOD_WIDTH; i++)
				scale_x[i] =
					i * (ATARI_WIDTH) / IPOD_WIDTH;
			for (i = 0; i < IPOD_HEIGHT; i++)
				scale_y[i] = (
					i * (ATARI_HEIGHT) / IPOD_HEIGHT
					) * ATARI_WIDTH;
			break;
	}
}

void ipod_update_screen()
{
	uint8 *screen = (UBYTE *) atari_screen;
	
	int x, y, p_ipod, p_src;
	for (y = 0; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			ipod_screen[p_ipod] = colour_palette[screen[p_src]];
		}
	}
	HD_LCD_Update(ipod_screen, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
}

void ipod_update_screen_smooth()
{
	uint8 *screen = (UBYTE *) atari_screen;
	
	int x, y, p_ipod, p_src;
	uint16 p1, p2, p3, p4, p5;
	for (y = 0; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			p1 = colour_palette[screen[p_src]]; // Pixel
			p2 = colour_palette[screen[p_src + 1]]; // Pixel to the right
			p3 = colour_palette[screen[p_src - 1]]; // Pixel to the right
			p4 = colour_palette[screen[p_src + ATARI_WIDTH]]; // Pixel underneath
			p5 = colour_palette[screen[p_src - ATARI_WIDTH]]; // Pixel above
			ipod_screen[p_ipod] = blend_pixels_8_RGB565(p1, p2, p3, p4, p5);
		}
	}
	HD_LCD_Update(ipod_screen, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
}

void ipod_init_video()
{
	HD_LCD_Init();
	HD_LCD_GetInfo(&IPOD_HW_VER, &IPOD_WIDTH, &IPOD_HEIGHT, &IPOD_LCD_TYPE);
	ipod_screen = malloc(IPOD_WIDTH * IPOD_HEIGHT * 2);
	ipod_calc_palette();
	ipod_rescale();
}

void ipod_exit_video()
{
	free(ipod_screen);
	atexit(HD_LCD_Quit);
}

// Waits for main update_screen to call "ipod_cop_update_screen();"
static void ipod_cop_sync_screen()
{
	while (inl(COP_STATUS) & COP_RUNNING) // COP is active
	{
		// Get stuck in loop until told to update screen
		while ((inl(COP_STATUS) & COP_LINE_REQ) == 0);
		if (smooth) {
			ipod_update_screen_smooth();
		} else {
			ipod_update_screen();
		}
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
	ipod_update_screen_funct();
}

/* main */

void ModeInfo()
{
	char scale_type, smoothing, cop;
	
	if (ipod_scale_type == FULLSCREEN) {
		scale_type = 'f';
	} else if (ipod_scale_type == SCALED) {
		scale_type = 's';
	} else { // Uncropped
		scale_type = 'u';
	}
	if (smooth) {
		smoothing = 'y';
	} else {
		smoothing = 'n';
	}
	if (use_cop) {
		cop = 'y';
	} else {
		cop = 'n';
	}
	
	Aprint("Video Mode: %dx%dx%d", IPOD_WIDTH, IPOD_HEIGHT, 16);
	Aprint("[%c] SCALE TYPE  [%c] SMOOTHING  [%c] COP",
		 scale_type, smoothing, cop);
}

void Atari_Initialise(int *argc, char *argv[])
{
	ipod_scale_type = FULLSCREEN;
	smooth = TRUE;
	use_cop = TRUE;
	
	int i, j;
	int help_only = FALSE;
	for (i = j = 1; i < *argc; i++) {
		if (strcmp(argv[i], "-fullscreen") == 0) {
			ipod_scale_type = FULLSCREEN;
		}
		else if (strcmp(argv[i], "-scaled") == 0) {
			ipod_scale_type = SCALED;
		}
		else if (strcmp(argv[i], "-uncropped") == 0) {
			ipod_scale_type = UNCROPPED;
		}
		else if (strcmp(argv[i], "-smoothing") == 0) {
			smooth = TRUE;
		}
		else if (strcmp(argv[i], "-no-smoothing") == 0) {
			smooth = FALSE;
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
				Aprint("\t-fullscreen      Run fullscreen and cropped [default]");
				Aprint("\t-scaled          Run scaled and cropped");
				Aprint("\t-uncropped       Run fullscreen and uncropped");
				Aprint("\t-smoothing       Blend pixels [default]");
				Aprint("\t-no-smoothing    Do not blend pixels");
				Aprint("\t-cop             Use co-processor [default]");
				Aprint("\t-no-cop          Do not use co-processor");
			}
			argv[j++] = argv[i];
		}
	}
	*argc = j;
	if (help_only) return;
	
	if (use_cop) {
		ipod_update_screen_funct = ipod_cop_update_screen;
	} else {
		if (smooth)
			ipod_update_screen_funct = ipod_update_screen_smooth;
		else
			ipod_update_screen_funct = ipod_update_screen;
	}
	
	ipod_init_input();
	ipod_init_cop();
	ipod_init_video();
	ModeInfo();
}

int Atari_Exit(int run_monitor)
{
	Aflushlog();
	ipod_exit_input();
	ipod_exit_video();
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

