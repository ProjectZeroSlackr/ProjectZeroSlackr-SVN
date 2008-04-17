/*
 * Last updated: March 17, 2008
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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern void pz_execv();
extern int check_file_ext();
extern TWindow *open_directory_title();

#define LAUNCHER 1

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension_iDoom;
static ttk_menu_item browser_extension_iFreeDoom;

static const char * launcher_options[] = {"iDoom", "iFreedoom", 0};

static int check_ext(const char* file)
{
	return check_file_ext(file, ".wad");	
}

static PzWindow *load_file_iDoom(const char *file)
{
	const char *const path = pz_module_get_datapath(module, "../iDoom");
	const char *const cmd[] = {"iDoom", "-file", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_iFreeDoom(const char *file)
{
	const char *const path = pz_module_get_datapath(module, "../iFreeDoom");
	const char *const cmd[] = {"iFreeDoom", "-file", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_iDoom()
{
	return load_file_iDoom(NULL);
}

static PzWindow *load_iFreeDoom()
{
	return load_file_iFreeDoom(NULL);
}

static PzWindow *load_file(const char* file)
{
	pz_save_config(config);
	if (pz_get_int_setting(config, LAUNCHER) == 0) {
		return load_file_iDoom(file);
	} else {
		return load_file_iFreeDoom(file);
	}
}

static PzWindow *load_file_handler_iDoom(ttk_menu_item * item)
{
	load_file_iDoom(item->data);
	return 0;
}

static PzWindow *load_file_handler_iFreeDoom(ttk_menu_item * item)
{
	load_file_iFreeDoom(item->data);
	return 0;
}


static PzWindow *browse_doom_pwads()
{
	const char *const path = pz_module_get_datapath(module, "../PWADs/Doom");
	chdir(path);
	pz_set_int_setting (config, LAUNCHER, 0);
	return open_directory_title(path, "Doom PWADs");
}

static PzWindow *browse_doom2_pwads()
{
	const char *const path = pz_module_get_datapath(module, "../PWADs/DoomII");
	chdir(path);
	pz_set_int_setting (config, LAUNCHER, 1);
	return open_directory_title(path, "Doom II PWADs");
}

static PzWindow *fastlaunch()
{
	pz_exec(pz_module_get_datapath(module, "FastLaunch.sh"));
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

	config = pz_load_config(pz_module_get_datapath(module, "../Conf/Launcher.conf"));
	if (!pz_get_setting(config, LAUNCHER))
		pz_set_int_setting (config, LAUNCHER, 0);

	pz_menu_add_stub_group("/Media/iDoom", "Games");
	pz_menu_add_action_group("/Media/iDoom/FastLaunch", "Launching", fastlaunch);
	pz_menu_add_action_group("/Media/iDoom/iDoom", "Launching", load_iDoom);
	pz_menu_add_action_group("/Media/iDoom/iFreeDoom", "Launching", load_iFreeDoom);
	pz_menu_add_action_group("/Media/iDoom/Doom PWADs", "PWADs", browse_doom_pwads);
	pz_menu_add_action_group("/Media/iDoom/Doom II PWads", "PWADs", browse_doom2_pwads);
	pz_menu_add_setting_group("/Media/iDoom/Launcher", "Settings", LAUNCHER, config, launcher_options);
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
