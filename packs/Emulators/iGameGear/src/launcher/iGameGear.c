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

#define FRAMESKIP	1
#define LANG		2
#define SCALING		3

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension;
static const char *path, *dir;
static const char *frameskip_options[] = {"None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 0};
static const char *locale_options[] = {"Overseas", "Japan", 0};
static const char *scaling_options[] = {"Centre", "Scale", "Stretch", 0};

static int check_ext(const char *file)
{
	return (check_file_ext(file, ".sms") ||
			check_file_ext(file, ".gg")
			);
}

static PzWindow *load_file(const char *file)
{
	char frameskip[4];
	snprintf(frameskip, 4, "%i",
		pz_get_int_setting(config, FRAMESKIP));
	char locale[8];
	if (pz_get_int_setting(config, LANG)==1) {
		snprintf(locale, 8, "--japan");
	} else {
		locale[0] = '\0';
	}
	char scaling[8];
	switch (pz_get_int_setting(config, SCALING))
	{
		case 0: snprintf(scaling, 8, "center"); break;
		case 1: snprintf(scaling, 8, "scale"); break;
		case 2: snprintf(scaling, 8, "stretch"); break;
	}
	const char *const cmd[] = {
		"Launch.sh",
		file,
		frameskip,
		scaling,
		locale,
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

static PzWindow *browse_roms()
{
	chdir(dir);
	return open_directory_title(dir, "iGameGear Roms");
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Emulators/iGameGear/ReadMe from Keripo.txt");
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
	module = pz_register_module("iGameGear", cleanup);
	path = "/opt/Emulators/iGameGear/Launch/Launch.sh";
	dir = "/opt/Emulators/iGameGear/Roms";
	
	config = pz_load_config("/opt/Emulators/iGameGear/Conf/Launch.conf");
	if (!pz_get_setting(config, FRAMESKIP)) pz_set_int_setting (config, FRAMESKIP, 5);
	if (!pz_get_setting(config, LANG)) pz_set_int_setting (config, LANG, 0);
	if (!pz_get_setting(config, SCALING)) pz_set_int_setting (config, SCALING, 0);
	
	pz_menu_add_stub_group("/Emulators/iGameGear", "Handheld");
	pz_menu_add_action_group("/Emulators/iGameGear/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/iGameGear/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Emulators/iGameGear/Roms", "Browse", browse_roms);
	pz_menu_add_setting_group("/Emulators/iGameGear/Frameskip", "~Settings", FRAMESKIP, config, frameskip_options);
	pz_menu_add_setting_group("/Emulators/iGameGear/Locale", "~Settings", LANG, config, locale_options);
	pz_menu_add_setting_group("/Emulators/iGameGear/Scaling", "~Settings", SCALING, config, scaling_options);
	pz_menu_sort("/Emulators/iGameGear");
	
	browser_extension.name = N_("Open with iGameGear");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
