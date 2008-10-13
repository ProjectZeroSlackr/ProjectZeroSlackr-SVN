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

#define SCALING		1
#define SMOOTHING	2
#define COP			3
#define REFRESH		4

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension;
static const char *path, *dir;
static const char *scaling_options[] = {"Fullscreen", "Scaled", "Uncropped", 0};
static const char *off_on_options[] = {"Off", "On", 0};
static const char *refresh_options[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 0};

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
	char scaling[12];
	switch (pz_get_int_setting(config, SCALING))
	{
		case 0: snprintf(scaling, 12, "%s", "-fullscreen"); break;
		case 1: snprintf(scaling, 12, "%s", "-scaled"); break;
		case 2: snprintf(scaling, 12, "%s", "-uncropped"); break;
	}
	char smoothing[14];
	if (pz_get_int_setting(config, SMOOTHING) == 1) {
		snprintf(smoothing, 14, "%s", "-smoothing");
	} else {
		snprintf(smoothing, 14, "%s", "-no-smoothing");
	}
	char cop[8];
	if (pz_get_int_setting(config, COP) == 1) {
		snprintf(cop, 8, "%s", "-cop");
	} else {
		snprintf(cop, 8, "%s", "-no-cop");
	}
	char refresh_rate[4];
	snprintf(refresh_rate, 4, "%i",
		(pz_get_int_setting(config, REFRESH) + 1));
	
	const char *const cmd[] = {
		"Launch.sh",
		file,
		scaling,
		smoothing,
		cop,
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
	return pz_new_textview_window(
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
	if (!pz_get_setting(config, SCALING))
		pz_set_int_setting (config, SCALING, 1); // Scaled
	if (!pz_get_setting(config, SMOOTHING))
		pz_set_int_setting (config, SMOOTHING, 1); // On
	if (!pz_get_setting(config, COP))
		pz_set_int_setting (config, COP, 1); // On
	if (!pz_get_setting(config, REFRESH))
		pz_set_int_setting (config, REFRESH, 4); // Almost full speed without being choppy
	
	pz_menu_add_stub_group("/Emulators/iAtari800", "Console");
	pz_menu_add_action_group("/Emulators/iAtari800/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/iAtari800/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Emulators/iAtari800/Disk Images", "Browse", browse_disks);
	pz_menu_add_setting_group("/Emulators/iAtari800/Scale type", "~Settings",
		SCALING, config, scaling_options);
	pz_menu_add_setting_group("/Emulators/iAtari800/Smoothing", "~Settings",
		SMOOTHING, config, off_on_options);
	pz_menu_add_setting_group("/Emulators/iAtari800/Use COP", "~Settings",
		COP, config, off_on_options);
	pz_menu_add_setting_group("/Emulators/iAtari800/Refresh rate", "~Settings",
		REFRESH, config, refresh_options);
	pz_menu_sort("/Emulators/iAtari800");
	
	// These do not cover all formats supported by Atari800 but just ones with unique Atari file extensions
	browser_extension.name = N_("Open with iAtari800");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
