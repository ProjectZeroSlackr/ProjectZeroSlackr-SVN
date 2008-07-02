/*
 * Last updated: July 2, 2008
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
#include <unistd.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>

static int ipod_mixer;
static int volume_current;
static int ipod_volume;

static void ipod_update_volume() {
	if (volume_current != ipod_volume) {
		int vol;
		volume_current = ipod_volume;
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
#include <unistd.h>
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
	// Causes screwy characters to appear
	//tcsetattr(console, TCSAFLUSH, &stored_settings);
	close(console);
}

/* == loop() functions == */

void ipod_init()
{
	ipod_init_sound();
	ipod_init_input();
}

void ipod_update()
{
	int input;
	input = KEYCODE(ipod_get_keypress());
	switch (input) {
		// Scrol wheel not used for volume due to SBAGen's slow main loop
		case KEY_REWIND:
			ipod_volume -= 2;
			if (ipod_volume < 0)
				ipod_volume = 0; // Negative volume DNE!
			ipod_update_volume();
			break;
		case KEY_FORWARD:
			ipod_volume += 2;
			if (ipod_volume > 70)
				ipod_volume = 70; // To be safe - 70 is VERY loud
			ipod_update_volume();
			break;
		case KEY_ACTION:
		case KEY_MENU:
			exit(0);
			break;
		default:
			break;
	}
}

void ipod_exit()
{
	ipod_exit_sound();
	ipod_exit_input();
}

