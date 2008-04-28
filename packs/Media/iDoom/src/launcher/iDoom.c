/*
 * Last updated: Apr 27, 2008
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
static ttk_menu_item browser_extension_iDoom;
static ttk_menu_item browser_extension_iFreeDoom;
static const char *path_iDoom, *path_iFreeDoom, *dir_I, *dir_II;
static const char * launcher_options[] = {"iDoom", "iFreedoom", 0};

static int check_ext(const char *file)
{
	return check_file_ext(file, ".wad");	
}

static PzWindow *load_file_iDoom(const char *file)
{
	const char *const cmd[] = {"Launch-iDoom.sh", file, NULL};
	pz_execv(
		path_iDoom,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_iFreeDoom(const char *file)
{
	const char *const cmd[] = {"Launch-iFreeDoom.sh", file, NULL};
	pz_execv(
		path_iFreeDoom,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file(const char *file)
{
	pz_save_config(config);
	if (pz_get_int_setting(config, LAUNCHER) == 0) {
		return load_file_iDoom(file);
	} else {
		return load_file_iFreeDoom(file);
	}
}

static PzWindow *load_file_handler_iDoom(ttk_menu_item *item)
{
	load_file_iDoom(item->data);
	return 0;
}

static PzWindow *load_file_handler_iFreeDoom(ttk_menu_item *item)
{
	load_file_iFreeDoom(item->data);
	return 0;
}


static PzWindow *browse_doom_pwads()
{
	chdir(dir_I);
	pz_set_int_setting (config, LAUNCHER, 0);
	return open_directory_title(dir_I, "Doom PWADs");
}

static PzWindow *browse_doom2_pwads()
{
	chdir(dir_II);
	pz_set_int_setting (config, LAUNCHER, 1);
	return open_directory_title(dir_II, "Doom II PWADs");
}

static PzWindow *fastlaunch()
{
	pz_exec(path_iDoom);
	return NULL;
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
	pz_save_config(config);
}

static void init_launch() 
{
	module = pz_register_module("iDoom", cleanup);
	path_iDoom = "/opt/Media/iDoom/Launch/Launch-iDoom.sh";
	path_iFreeDoom = "/opt/Media/iDoom/Launch/Launch-iFreeDoom.sh";
	dir_I = "/opt/Media/iDoom/PWADs/Doom";
	dir_II = "/opt/Media/iDoom/PWADs/DoomII";

	config = pz_load_config("/opt/Media/iDoom/Conf/Launcher.conf");
	if (!pz_get_setting(config, LAUNCHER))
		pz_set_int_setting (config, LAUNCHER, 0);

	pz_menu_add_stub_group("/Media/iDoom", "Games");
	pz_menu_add_action_group("/Media/iDoom/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Media/iDoom/Doom PWADs", "Browse", browse_doom_pwads);
	pz_menu_add_action_group("/Media/iDoom/Doom II PWads", "Browse", browse_doom2_pwads);
	pz_menu_add_setting_group("/Media/iDoom/Launcher", "~Settings", LAUNCHER, config, launcher_options);
	pz_menu_sort("/Media/iDoom");
	
	browser_extension_iDoom.name = N_("Open with iDoom");
	browser_extension_iDoom.makesub = load_file_handler_iDoom;
	pz_browser_add_action (check_ext, &browser_extension_iDoom);
	browser_extension_iFreeDoom.name = N_("Open with iFreeDoom");
	browser_extension_iFreeDoom.makesub = load_file_handler_iFreeDoom;
	pz_browser_add_action (check_ext, &browser_extension_iFreeDoom);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
