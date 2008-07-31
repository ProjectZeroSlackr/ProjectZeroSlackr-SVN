/*
 * Last updated: July 31, 2008
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

#define REFRESH	1

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension;
static const char *path, *dir;
static const char *refresh_options[] = {"4", "5", "6", "7", "8", "9", "10", 0};

// Note that Atari files could also be in .exe or .bin or other formats
// This is only used for the default handler
static int check_ext(const char *file)
{
	return (check_file_ext(file, ".atr") ||
			check_file_ext(file, ".dcm") ||
			check_file_ext(file, ".xfd")
			);
}

static PzWindow *load_file(const char *file)
{
	char refresh_rate[4];
	snprintf(refresh_rate, 4, "%i",
		(pz_get_int_setting(config, REFRESH) + 4));
	const char *const cmd[] = {
		"Launch.sh",
		file,
		refresh_rate,
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}

static PzWindow *browse_disks()
{
	return open_directory_title_mod(dir, "Atari Disk Images",
		check_is_file, load_file_handler);
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Emulators/iAtari800/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_save_config(config);
	pz_free_config(config);
	config = 0;
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("iAtari800", cleanup);
	path = "/opt/Emulators/iAtari800/Launch/Launch.sh";
	dir = "/opt/Emulators/iAtari800/Disks";
	
	config = pz_load_config("/opt/Emulators/iAtari800/Conf/Launch.conf");
	if (!pz_get_setting(config, REFRESH))
		pz_set_int_setting (config, REFRESH, 6);
	
	pz_menu_add_stub_group("/Emulators/iAtari800", "Console");
	pz_menu_add_action_group("/Emulators/iAtari800/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/iAtari800/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Emulators/iAtari800/Disk Images", "Browse", browse_disks);
	pz_menu_add_setting_group("/Emulators/iAtari800/Refresh rate", "~Settings", REFRESH, config, refresh_options);
	pz_menu_sort("/Emulators/iAtari800");
	
	// These do not cover all formats supported by Atari800 but just ones with unique Atari file extensions
	browser_extension.name = N_("Open with iAtari800");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
