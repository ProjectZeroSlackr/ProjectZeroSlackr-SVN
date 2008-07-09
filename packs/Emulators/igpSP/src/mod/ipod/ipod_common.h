/*
 * Last updated: July 9, 2008
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
 * Source code for these files borrowed from various places including:
 * - iBoy: http://sourceforge.net/projects/iboy
 * - igpSP: http://ipodlinux.org/Igpsp (Zaphod's original port)
 * - ithread: http://svn.so2.sytes.net/repos/ipod/ithread/
 * - libipod: https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/libipod/
 * - Rockbox: svn://svn.rockbox.org/rockbox/trunk/firmware
 * - iniParser: http://ndevilla.free.fr/iniparser/
 * See the comments of each file for more details
 */

#include "../common.h"
#include "hotdog.h"

#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>


/* ipod_hw.c */

void ipod_init_hw();
void ipod_exit_hw();
void ipod_toggle_backlight();
//void ipod_update_contrast(); // No support for monochrome iPods - see ipod_video.c
void ipod_update_brightness();
void ipod_update_cpu_speed();


/* ipod_cop.c */

#define outl(a, b) \
	(*(volatile unsigned int *)(b) = (a)) \

#define inl(a) \
	(*(volatile unsigned int *)(a)) \

void ipod_init_cop();
void ipod_exit_cop();
void ipod_cop_execute(void (*function)());
void ipod_cop_update_screen();


/* ipod_video.c */

void ipod_init_video();
void ipod_exit_video();
void ipod_clear_screen();
void ipod_update_scale_type();
void ipod_update_screen();


/* ipod_input.c */

void ipod_init_input();
void ipod_exit_input();
u32 ipod_update_ingame_input();
gui_action_type ipod_update_menu_input();


#ifndef NOSOUND

/* ipod_sound.c */

void ipod_init_sound();
void ipod_exit_sound();
void ipod_update_volume();

#endif

/* ipod_conf.c */
void ipod_init_conf();
void ipod_exit_conf();
void ipod_update_settings();

