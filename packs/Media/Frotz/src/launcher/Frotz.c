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

#include "browser-ext.h"

#define LAUNCHER 1

static PzModule *module;
static PzConfig *config;
static const char *path_frotz, *path_dfrotz, *dir;
static const char * launcher_options[] = {"Frotz", "DFrotz", 0};

static int ncurses_check(ttk_menu_item *item)
{
	return HAVE_NCURSES;
}

static PzWindow *load_file(const char *file)
{
	if (ncurses_check(NULL) && pz_get_int_setting(config, LAUNCHER) == 0) {
		const char *const cmd[] = {
			"Terminal-frotz.sh", file, NULL};
		return new_terminal_window_with(
			path_frotz,
			(char *const *)cmd
		);
	} else {
		const char *const cmd[] = {
			"Terminal-dfrotz.sh", file, NULL};
		return new_terminal_window_with(
			path_dfrotz,
			(char *const *)cmd
		);
	}
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file((const char *)item->data); // Recast for browser_mod
}

static PzWindow *browse_games()
{
	return open_directory_title_mod(
		dir, "Z-Machine Games",
		check_is_file, load_file_handler);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Media/Frotz/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_save_config(config);
	pz_free_config(config);
	config = 0;
}

static void init_launch() 
{
	module = pz_register_module("Frotz", cleanup);
	path_frotz = "/opt/Media/Frotz/Launch/Terminal-frotz.sh";
	path_dfrotz = "/opt/Media/Frotz/Launch/Terminal-dfrotz.sh";
	dir = "/opt/Media/Frotz/Games";
	
	config = pz_load_config("/opt/Media/Frotz/Conf/Launcher.conf");
	if (!pz_get_setting(config, LAUNCHER))
		pz_set_int_setting (config, LAUNCHER, 0);
	
	pz_menu_add_stub_group("/Media/Frotz", "Text Games");
	pz_menu_add_action_group("/Media/Frotz/~ReadMe", "#Info", readme);
	pz_menu_add_action_group("/Media/Frotz/Games", "Browse", browse_games);
	pz_menu_add_setting_group("/Media/Frotz/Launcher", "~Settings", LAUNCHER, config, launcher_options);
	pz_get_menu_item("/Media/Frotz/Launcher")->visible = ncurses_check;
	pz_menu_sort("/Media/Frotz");
}

PZ_MOD_INIT (init_launch)
