/*
 * Last updated: Aug 7, 2008
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

#include "pz.h"
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

// PZ2 core functions - see pz.h for more
extern int MPD_ACTIVE;
extern int HAVE_NCURSES;
extern void setup_sigchld_handler();
extern void pz_set_backlight_timer(int sec);
extern TWindow *open_directory_title(const char *filename, const char *title);

// Terminal module
extern PzWindow * new_terminal_window_with(const char *path, char *const argv[]);

// String manipulators
extern const char *get_filename(const char *file);
extern const char *get_dirname(const char *file);

// Convenient checkers
extern int check_file_ext(const char *file, const char *ext);
extern int check_is_dir(const char *file);
extern int check_is_file(const char *file);
extern int check_nothing(const char *file);

// Backlight
extern void toggle_backlight();
extern PzWindow *toggle_backlight_window();

// CPU speed
// Postscalar values: CPU speed = (24 / 8) * postscalar
// 81MHz is the MAXIMUM the iPod will go without screwing up
// 75MHz is iPodLinux's default, 66MHz is Apple OS's default
#define CPU_33MHz	11
#define CPU_45MHz	15
#define CPU_66MHz	22
#define CPU_68MHz	23
#define CPU_72MHz	24 // Underclock
#define CPU_75MHz	25 // Normal
#define CPU_78MHz	26 // Overclocked
#define CPU_81MHz	27 // Max Overclock - Unstable!
extern void set_cpu_speed(int postscalar);

// Mini-browser mod
extern TWidget *read_directory_mod(const char *dirname,
	int check(const char *file),TWindow *handler(ttk_menu_item *item));
extern TWindow *open_directory_title_mod(const char *filename, const char *title,
	int check(const char *file), TWindow *handler(ttk_menu_item *item));
