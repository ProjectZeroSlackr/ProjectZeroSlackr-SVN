/*
 * Last updated: May 22, 2008
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

/*
 * Source code for these files borrowed from various places including:
 * iBoy: http://sourceforge.net/projects/iboy
 * igpSP: http://ipodlinux.org/Igpsp (Zaphod's original port)
 * ithread: http://svn.so2.sytes.net/repos/ipod/ithread/
 * libipod: https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/libipod/
 *
 */

#include "../common.h"

#include <ctype.h>
#include <fcntl.h>
#include <termios.h>

#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>


/* ipod_hw.c */

// Postscalar values: CPU speed = (24 / 8) * postscalar
// 81MHz is the MAXIMUM the iPod will go without screwing up
// 75MHz is iPodLinux's default, 66MHz is Apple OS's default
#define CPU_33MHz	11
#define CPU_45MHz	15
#define CPU_66MHz	22
#define CPU_75MHz	25
#define CPU_78MHz	26
#define CPU_81MHz	27

#define BACKLIGHT_OFF	0
#define BACKLIGHT_ON	1

void ipod_init_hw();
void ipod_exit_hw();
void ipod_toggle_backlight();
void ipod_update_contrast();
void ipod_update_cpu_speed();


/* ipod_cop.c */

#define outl(a, b) \
	(*(volatile unsigned int *)(b) = (a)) \

#define inl(a) \
	(*(volatile unsigned int *)(a)) \

#define ipod_cop_operation(op) \
	(ipod_cop_execute((void (*)())(op))) \

void ipod_init_cop();
void ipod_exit_cop();
void ipod_cop_update_screen();
void ipod_cop_execute(void (*function)());


/* ipod_video.c */

void ipod_init_video();
void ipod_exit_video();
void ipod_update_screen();
void ipod_update_scale_type();
void ipod_clear_screen(u16 color);


/* ipod_input.c */

void ipod_init_input();
void ipod_exit_input();
u32 ipod_update_ingame_input();
gui_action_type ipod_update_menu_input();

