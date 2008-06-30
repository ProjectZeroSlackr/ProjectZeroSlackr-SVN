/*
 * Last updated: Jun 29, 2008
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

 
/* == Volume code == */

#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>

static int ipod_mixer;
static int ipod_volume;
static int volume_current = -1;

static void ipod_update_volume()
{
	if (volume_current != ipod_volume) {
		volume_current = ipod_volume;
		int vol;
		vol = volume_current << 8 | volume_current;
		ioctl(ipod_mixer, SOUND_MIXER_WRITE_PCM, &vol);
	}
}

static void ipod_init_sound()
{
	ipod_mixer = open("/dev/mixer", O_RDWR);
	ipod_volume = 50; // Good default
	ipod_update_volume();
}

static void ipod_exit_sound()
{
	close(ipod_mixer);
}


/* == Input code == */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>

#define KEY_MENU	50 // Up
#define KEY_PLAY	32 // Down
#define KEY_REWIND	17 // Left
#define KEY_FORWARD	33 // Right
#define KEY_ACTION	28 // Select
#define KEY_HOLD	35 // Exit
#define SCROLL_L	38 // Counter-clockwise
#define SCROLL_R	19 // Clockwise
#define KEY_NULL	-1 // No key event

#define KEYCODE(a)	(a & 0x7f) // Use to get keycode of scancode.
#define KEYSTATE(a)	(a & 0x80) // Check if key is pressed or lifted

static int console;
static struct termios stored_settings;

static int ipod_get_keypress()
{
	int press = 0;
	if (read(console, &press, 1) != 1)
		return KEY_NULL;
	return press;
}

static void ipod_init_input()
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

static void ipod_exit_input()
{
	tcsetattr(console, TCSAFLUSH, &stored_settings);
	close(console);
}


/* == Main loop == */

void ipod_init_volume_control()
{
	ipod_init_sound();
	ipod_init_input();
	
	int input, exit;
	exit = 0;
	while (exit != 1) {
		input = KEYCODE(ipod_get_keypress());
		switch (input) {
			case SCROLL_L:
				ipod_volume--;
				if (ipod_volume < 0)
					ipod_volume = 0; // Negative volume DNE!
				ipod_update_volume();
				break;
			case KEY_ACTION:
				exit = 1;
				break;
			case SCROLL_R:
				ipod_volume++;
				if (ipod_volume > 70)
					ipod_volume = 70; // To be safe - 70 is VERY loud
				ipod_update_volume();
				break;
			case KEY_MENU:
				exit = 1;
				break;			
			default:
				break;		
		}
	}
	
	ipod_exit_sound();
	ipod_exit_input();
}

