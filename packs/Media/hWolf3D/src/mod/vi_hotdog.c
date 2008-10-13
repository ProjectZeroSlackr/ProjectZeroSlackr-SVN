/*
 * Last updated: Oct 12, 2008
 * ~Keripo
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

#include "wl_def.h"
#include "vi_hotdog.h"

static uint16 *ipod_screen;
static uint16 ipod_palette[256];
static uint8 ipod_palette_r[256];;
static uint8 ipod_palette_g[256];;
static uint8 ipod_palette_b[256];;
static int scale_x[320], scale_y[240];
static int IPOD_HW_VER, IPOD_LCD_TYPE;
uint32 IPOD_WIDTH, IPOD_HEIGHT;

byte *gfxbuf = NULL;
int in_menu = 0;

extern void keyboard_handler(int code, int press);
extern void HD_LCD_Init();
extern void HD_LCD_GetInfo (int *hw_ver, int *lcd_width, int *lcd_height, int *lcd_type);
extern void HD_LCD_Update (void *fb, int x, int y, int w, int h);
extern void HD_LCD_Quit();

int main (int argc, char *argv[])
{
	return WolfMain(argc, argv);
}

void DisplayTextSplash(byte *text);

/*
==========================
=
= Quit
=
==========================
*/

void Quit(const char *error)
{
	memptr screen = NULL;

	if (!error || !*error) {
		CA_CacheGrChunk(ORDERSCREEN);
		screen = grsegs[ORDERSCREEN];
		WriteConfig();
	} else if (error) {
		CA_CacheGrChunk(ERRORSCREEN);
		screen = grsegs[ERRORSCREEN];
	}
	
	ShutdownId();
	
	if (screen) {
		//DisplayTextSplash(screen);
	}
	
	if (error && *error) {
		fprintf(stderr, "Quit: %s\n", error);
		exit(EXIT_FAILURE);
 	}
	exit(EXIT_SUCCESS);
}

void VL_WaitVBL(int vbls)
{
	unsigned long last = get_TimeCount() + vbls;
	while (last > get_TimeCount()) ;
}

void VW_UpdateScreen()
{
	int x, y, p_ipod, p_src;
	uint16 p1, p2, p3;
	for (y = 0; y < IPOD_HEIGHT; y++) {
		for (x = 0; x < IPOD_WIDTH; x++) {
			p_ipod = x + y * IPOD_WIDTH;
			p_src = scale_x[x] + scale_y[y];
			p1 = ipod_palette[gfxbuf[p_src]]; // Pixel
			p2 = ipod_palette[gfxbuf[p_src + 1]]; // Pixel to the right
			p3 = ipod_palette[gfxbuf[p_src + vwidth]]; // Pixel underneath
			ipod_screen[p_ipod] = blend_pixels_4_RGB565(p1, p2, p3);
		}
	}
	HD_LCD_Update(ipod_screen, 0, 0, IPOD_WIDTH, IPOD_HEIGHT);
}


/*
=======================
=
= VL_Startup
=
=======================
*/

static int console;
static struct termios stored_settings;

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

void ipod_set_scaling()
{
	int i;
	for (i = 0; i < IPOD_WIDTH; i++)
		scale_x[i] = i * vwidth / IPOD_WIDTH;
	for (i = 0; i < IPOD_HEIGHT; i++)
		scale_y[i] = (i * vheight / IPOD_HEIGHT) * vwidth;
}

void VL_Startup()
{
	HD_LCD_Init();
	HD_LCD_GetInfo(&IPOD_HW_VER, &IPOD_WIDTH, &IPOD_HEIGHT, &IPOD_LCD_TYPE);
	vwidth = 320;
	vheight = 200;
	
	ipod_screen = malloc(IPOD_WIDTH * IPOD_HEIGHT * 2);
	gfxbuf = malloc(320 * 200);
	
	ipod_init_input();
	ipod_set_scaling();
}

/*
=======================
=
= VL_Shutdown
=
=======================
*/


void ipod_exit_input()
{
	close(console);
}

void VL_Shutdown()
{
	ipod_exit_input();
	free(ipod_screen);
	free(gfxbuf);
	HD_LCD_Quit();
}

/* ======================================================================== */

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette(const byte *palette)
{
	VL_WaitVBL(1);
	
	int i;
	for ( i = 0; i < 256; i++ ) {
		ipod_palette_r[i] = ((int)palette[i * 3 + 0] * 255) / 63;
		ipod_palette_g[i] = ((int)palette[i * 3 + 1] * 255) / 63;
		ipod_palette_b[i] = ((int)palette[i * 3 + 2] * 255) / 63;
		ipod_palette[i] = RGB565(
			ipod_palette_r[i],
			ipod_palette_g[i],
			ipod_palette_b[i]
			);
	}
}

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette(byte *palette)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		palette[i * 3 + 0] = ipod_palette_r[i] >> 2;
		palette[i * 3 + 1] = ipod_palette_g[i] >> 2;
		palette[i * 3 + 2] = ipod_palette_b[i] >> 2;
	}
}

static int ipod_get_keypress()
{
	int press = 0;
	if (read(console, &press, 1) != 1)
		return KEY_NULL;
	return press;
}

void INL_Update()
{
	int input = ipod_get_keypress();
	while (input != KEY_NULL) {
		if (KEYSTATE(input)) { // Key up/lifted
			input = KEYCODE(input);
			switch (input) { // In numeric order for speed
				case KEY_REWIND:
					keyboard_handler(sc_LeftArrow, 0);
					break;
				case KEY_ACTION:
					if (in_menu) {
						keyboard_handler(sc_Enter, 0);
					} else {
						keyboard_handler(sc_Alt, 0);
						keyboard_handler(sc_Space, 0);
					}
					break;
				case KEY_REC:
				case KEY_PLAY:
					if (in_menu)
						keyboard_handler(sc_DownArrow, 0);
					else
						keyboard_handler(sc_Control, 0);
					break;
				case KEY_FORWARD:
					keyboard_handler(sc_RightArrow, 0);
					break;
				case KEY_POWER:
				case KEY_HOLD:
					keyboard_handler(sc_Escape, 0);
					break;
				case KEY_MENU: 
					keyboard_handler(sc_UpArrow, 0);
					break;
				default:
					keyboard_handler(sc_None, 0);
					break;
			}
		} else { // Key down/pressed
			input = KEYCODE(input);
			switch (input) { // In numeric order for speed	
				case KEY_REWIND:
					keyboard_handler(sc_LeftArrow, 1);
					break;
				case SCROLL_R:
					if (in_menu) {
						keyboard_handler(sc_X, 1); // For game saving
						keyboard_handler(sc_Y, 0);
					}
					break;
				case KEY_ACTION:
					if (in_menu) {
						keyboard_handler(sc_Enter, 1);
					} else {
						keyboard_handler(sc_Space, 1); // Open door
						keyboard_handler(sc_Alt, 1); // Strafing
					}
					break;
				case KEY_REC:
				case KEY_PLAY:
					if (in_menu)
						keyboard_handler(sc_DownArrow, 1);
					else
						keyboard_handler(sc_Control, 1); // Fire
					break;
				case KEY_FORWARD:
					keyboard_handler(sc_RightArrow, 1);
					break;
				case KEY_POWER:
				case KEY_HOLD:
					keyboard_handler(sc_Escape, 1);
					break;
				case SCROLL_L:
					if (in_menu) {
						keyboard_handler(sc_Y, 1); // For game saving
						keyboard_handler(sc_Y, 0);
					}
					break;
				case KEY_MENU:
					keyboard_handler(sc_UpArrow, 1);
					break;
				default:
					keyboard_handler(sc_None, 1);
					break;
			}
		}
		input = ipod_get_keypress();
	}	
}

void IN_GetMouseDelta(int *dx, int *dy)
{
	// Dummy
}

byte IN_MouseButtons()
{
	// Dummy
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartJoy() - Detects & auto-configures the specified joystick
//					The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
boolean INL_StartJoy(word joy)
{
	// Dummy
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
void INL_ShutJoy(word joy)
{
	// Dummy
}

/*
===================
=
= IN_JoyButtons
=
===================
*/

byte IN_JoyButtons()
{
	// Dummy
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyAbs(word joy,word *xp,word *yp)
{
	// Dummy
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void INL_GetJoyDelta(word joy,int *dx,int *dy)
{
	// Dummy
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
word INL_GetJoyButtons(word joy)
{
	// Dummy
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//      IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//              to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
	// Dummu
}
