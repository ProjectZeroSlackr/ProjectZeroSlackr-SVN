/*
 * Last updated: Jun 26, 2008
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

// Small program for testing iPod input (keypresses).
// Not part of igpSP - compile this separately with arm-uclinux-elf-gcc
// Should work on all iPods supported by iPodLinux.

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>

#include "ipod_input.h"

static int console;
static struct termios stored_settings;

static int ipod_get_keypress()
{
	int press = 0;
	if (read(console, &press, 1) != 1)
		return KEY_NULL;
	return press;
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
	tcsetattr(console, TCSAFLUSH, &stored_settings);
	close(console);
}

int main()
{
	int input;
	char *keyname;
	
	ipod_init_input();
	
	for (;;) { // Infinite loop
		input = ipod_get_keypress();
		keyname = NULL;
		if (input != KEY_NULL) {
			if (KEYSTATE(input)) { // Key up/lifted
				input = KEYCODE(input);
				switch (input) { // In numeric order for speed
					case KEY_REWIND:
						keyname = "rewind";
						break;
					case SCROLL_R:
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							keyname = "scroll r";
						break;
					case KEY_ACTION:
						keyname = "action";
						break;
					case KEY_PLAY:
						keyname = "play";
						break;
					case KEY_FORWARD:
						keyname = "forward";
						break;
					case KEY_HOLD:
						keyname = "hold";
						break;
					case SCROLL_L:
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							keyname = "scroll l";
						break;
					case KEY_MENU: 
						keyname = "menu";
						break;
					default:
						keyname = "UNKNOWN!";
						break;
				}
				if (keyname)
					fprintf(stderr, "Lift %i: %s\n", input, keyname);
			} else { // Key down/pressed
				input = KEYCODE(input);
				switch (input) { // In numeric order for speed
					case KEY_REWIND:
						keyname = "rewind";
						break;
					case SCROLL_R:
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							keyname = "scroll r";
						break;
					case KEY_ACTION:
						keyname = "action";
						break;
					case KEY_PLAY:
						keyname = "play";
						break;
					case KEY_FORWARD:
						keyname = "forward";
						break;
					case KEY_HOLD:
						keyname = "hold";
						break;
					case SCROLL_L:
						if (SCROLL_MOD(SCROLL_MOD_NUM))
							keyname = "scroll l";
						break;
					case KEY_MENU: 
						keyname = "menu";
						break;
					default:
						keyname = "UNKNOWN!";
						break;
				}
				if (keyname)
					fprintf(stderr, "Press %i: %s\n", input, keyname);
			}
		}
	}
}
