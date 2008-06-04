/*
 * Last updated: May 31, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo, Various
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

#include "ipod_common.h"

extern u32 sound_fd;
extern u32 sound_frequency;

int ipod_mixer;
int ipod_volume;

static int volume_current = -1;

void ipod_update_volume()
{
	if (volume_current != ipod_volume) {
		volume_current = ipod_volume;
		int vol;
		vol = volume_current << 8 | volume_current;
		ioctl(ipod_mixer, SOUND_MIXER_WRITE_PCM, &vol);
	}
}

void ipod_init_sound()
{
	sound_fd = open("/dev/dsp", O_WRONLY);
	ipod_mixer = open("/dev/mixer", O_RDWR);
	
	ipod_volume = 90; // Default volume level - place this into some config file
	ipod_update_volume();
	
	int channels;
	channels = 1; // Mono MUCH better and smoother than stereo!
	ioctl(sound_fd, SNDCTL_DSP_CHANNELS, &channels);
	ioctl(sound_fd, SNDCTL_DSP_SPEED, &sound_frequency);
}

void ipod_exit_sound()
{
	close(sound_fd);
	close(ipod_mixer);
}

