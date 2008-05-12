/*
 * Last updated: May 11, 2008
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

#define SOUND 1

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension;
static const char
*path_no_sound, *path_with_sound,
*path_bios, *dir;
static const char * sound_options[] = {"Off", "On", 0};

static int check_ext(const char *file)
{
	return check_file_ext(file, ".gba");	
}

static int check_bios()
{
	FILE *bios;
	bios = fopen(path_bios, "r");
	if (bios == NULL) {
		pz_message_title("No GBA BIOS found!", "No \"gba_bios.bin\" found in \"Data/\"!");
		return 0;
	} else {
		return 1;
	}
}

static PzWindow *load_file_no_sound(const char *file)
{
	if (check_bios() == 1) {
		const char *const cmd[] = {"Launch-no-sound.sh", file, NULL};
		pz_execv(
			path_no_sound,
			(char *const *)cmd
		);
	}
	return NULL;
}

static PzWindow *load_file_with_sound(const char *file)
{
	if (check_bios() == 1) {
		const char *const cmd[] = {"Launch-with-sound.sh", file, NULL};
		pz_execv(
			path_with_sound,
			(char *const *)cmd
		);
	}
	return NULL;
}

static PzWindow *load_file(const char *file)
{
	pz_save_config(config);
	if (pz_get_int_setting(config, SOUND) == 0) {
		return load_file_no_sound(file);
	} else {
		return load_file_with_sound(file);
	}
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}

static PzWindow *browse_roms()
{
	chdir(dir);
	return open_directory_title(dir, "igpSP Roms");
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
	pz_save_config(config);
}

static void init_launch() 
{
	module = pz_register_module("igpSP", cleanup);
	path_no_sound = "/opt/Emulators/igpSP/Launch/Launch-no-sound.sh";
	path_with_sound = "/opt/Emulators/igpSP/Launch/Launch-with-sound.sh";
	path_bios = "/opt/Emulators/igpSP/Data/gba_bios.bin";
	dir = "/opt/Emulators/igpSP/Roms";

	config = pz_load_config("/opt/Emulators/igpSP/Conf/Sound.conf");
	if (!pz_get_setting(config, SOUND))
		pz_set_int_setting (config, SOUND, 0);

	pz_menu_add_stub_group("/Emulators/igpSP", "Handheld");
	pz_menu_add_action_group("/Emulators/igpSP/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/igpSP/Roms", "Browse", browse_roms);
	pz_menu_add_setting_group("/Emulators/igpSP/Sound", "~Settings", SOUND, config, sound_options);
	pz_menu_sort("/Emulators/igpSP");
	
	browser_extension.name = N_("Open with igpSP");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
