/*
 * Last updated: July 16, 2008
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

/*
 * Key press and touch input code based on iBoy
 * Scroll mod and (rapid fire) code borrowed from courtc (Courtney Calvin)
 */

#include "ipod_common.h"
#include "ipod_input.h"

// Note: Most of this code is from iBoy but will not work for 4Gs and mini 1Gs
// as they handle wheel touches differently (see iBoy's kb.c for more info)

extern void enter_menu();
extern int IPOD_HW_VER;

int ipod_rapid_fire;
int ipod_map_triggers;
int ipod_menu_scrolling;

static int console;
static struct termios stored_settings;

static int trigger_pressed = 0;
static u32 pressed_buttons = 0;

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
	// No support for monochrome iPods - see ipod_video.c
	/*
	if (IPOD_HW_VER != 0x4 && IPOD_HW_VER != 0x3) { // mini 1G or 3G
		int in, st;
		in = inl(0x7000C140);
		st = ((in & 0xff000000) >> 24);
		
		touch = 0xff;
		if (st == 0xc0)
			touch = (in & 0x007F0000 ) >> 16;
	}
	*/
	
	// See http://ipodlinux.org/Key_Chart
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

u32 ipod_update_ingame_input()
{
	int input;
	u32 touched_buttons, key;
	
	// Note: No way so far of detecting lifting of touches, meaning
	// no multi-touch unfortunately ; / (thus, new variable every time)
	touched_buttons = BUTTON_NONE;
	input = ipod_get_keytouch();
	switch(input) {
		case TOUCH_NULL:
			break;
		case TOUCH_U:
			touched_buttons |= BUTTON_UP;
			break;
		case TOUCH_UR:
			if (ipod_rapid_fire) {
				if (RAPID_FIRE(RAPID_FIRE_NUM))
					touched_buttons |= BUTTON_A;
			} else {
				touched_buttons |= BUTTON_A;
			}
			break;
		case TOUCH_R:
			if (!ipod_map_triggers || !trigger_pressed)
				touched_buttons |= BUTTON_RIGHT;
			break;
		case TOUCH_DR:
			touched_buttons |= BUTTON_START;
			break;
		case TOUCH_D:
			touched_buttons |= BUTTON_DOWN;
			break;
		case TOUCH_DL:
			touched_buttons |= BUTTON_SELECT;
			break;
		case TOUCH_L:
			if (!ipod_map_triggers || !trigger_pressed)
				touched_buttons |= BUTTON_LEFT;
			break;
		case TOUCH_UL:
			if (ipod_rapid_fire) {
				if (RAPID_FIRE(RAPID_FIRE_NUM))
					touched_buttons |= BUTTON_B;
			} else {
				touched_buttons |= BUTTON_B;
			}
			break;
		default:
			break;
	}
	input = ipod_get_keypress();
	while (input != KEY_NULL) {
		if (KEYSTATE(input)) { // Key up/lifted
			input = KEYCODE(input);
			switch (input) { // In numeric order for speed
				case KEY_REWIND:
					if (ipod_map_triggers) {
						pressed_buttons &=~BUTTON_L;
						trigger_pressed = 0;
					} else {
						pressed_buttons &=~BUTTON_LEFT;
						pressed_buttons &=~BUTTON_B;
					}
					break;
				case SCROLL_R:
					break;
				case KEY_ACTION:
					pressed_buttons &=~BUTTON_A;
					break;
				case KEY_PLAY:
					pressed_buttons &=~BUTTON_DOWN;
					pressed_buttons &=~BUTTON_B;
					break;
				case KEY_FORWARD:
					if (ipod_map_triggers) {
						pressed_buttons &=~BUTTON_R;
						trigger_pressed = 0;
					} else {
						pressed_buttons &=~BUTTON_RIGHT;
						pressed_buttons &=~BUTTON_B;
					}
					break;
				case KEY_HOLD:
					break;
				case SCROLL_L:
					break;
				case KEY_MENU: 
					pressed_buttons &=~BUTTON_UP;
					pressed_buttons &=~BUTTON_B;
					break;
				default:
					break;
			}
		} else { // Key down/pressed
			input = KEYCODE(input);
			switch (input) { // In numeric order for speed	
				case KEY_REWIND:
					if (ipod_map_triggers) {
						pressed_buttons |= BUTTON_L;
						trigger_pressed = 1;
					} else {
						pressed_buttons |= BUTTON_LEFT;
						pressed_buttons |= BUTTON_B;
					}
					break;
				case SCROLL_R:
					break;
				case KEY_ACTION:
					pressed_buttons |= BUTTON_A;
					break;
				case KEY_PLAY:
					pressed_buttons |= BUTTON_DOWN;
					pressed_buttons |= BUTTON_B;
					break;
				case KEY_FORWARD:
					if (ipod_map_triggers) {
						pressed_buttons |= BUTTON_R;
						trigger_pressed = 1;
					} else {
						pressed_buttons |= BUTTON_RIGHT;
						pressed_buttons |= BUTTON_B;
					}
					break;
				case KEY_HOLD:
					enter_menu();
					break;
				case SCROLL_L:
					break;
				case KEY_MENU: 
					pressed_buttons |= BUTTON_UP;
					pressed_buttons |= BUTTON_B;
					break;
				default:
					break;
			}
		}
		input = ipod_get_keypress();
	}
	
	key = touched_buttons;
	key |= pressed_buttons;
	return key;
}

gui_action_type ipod_update_menu_input()
{
	// Call here as settings only get changed in the menu
	// and menu updating is fast enough anyway
	ipod_update_settings();
	
	int input;
	gui_action_type new_gui_action;
	
	new_gui_action = CURSOR_NONE;
	input = ipod_get_keypress();
	if (input != KEY_NULL) {
		if (!KEYSTATE(input)) {
			ipod_clear_screen();
			input = KEYCODE(input);
			switch (input) {
				case KEY_REWIND:
					new_gui_action = CURSOR_LEFT;
					break;
				case SCROLL_R:
					if (ipod_menu_scrolling) {
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							new_gui_action = CURSOR_DOWN;
					}
					break;
				case KEY_ACTION:
					new_gui_action = CURSOR_SELECT;
					break;
				case KEY_PLAY:
					if (!ipod_menu_scrolling) {
						new_gui_action = CURSOR_DOWN;
					}
					break;
				case KEY_FORWARD:
					new_gui_action = CURSOR_RIGHT;
					break;
				case KEY_HOLD:
					if (!ipod_menu_scrolling) {
						new_gui_action = CURSOR_EXIT;
					}
					break;
				case SCROLL_L:
					if (ipod_menu_scrolling) {
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							new_gui_action = CURSOR_UP;
					}
					break;
				case KEY_MENU:
					if (ipod_menu_scrolling) {
						new_gui_action = CURSOR_EXIT;
					} else {
						new_gui_action = CURSOR_UP;
					}
					break;
				default:
					break;
			}
		}
	}
	
	return new_gui_action;
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
	// Causes weird characters to appear due to HD_LCD_Quit
	//tcsetattr(console, TCSAFLUSH, &stored_settings);
	close(console);
}

