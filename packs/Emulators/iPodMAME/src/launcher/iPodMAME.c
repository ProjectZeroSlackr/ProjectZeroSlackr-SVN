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

static PzModule *module;
static const char *path, *dir;

static PzWindow *load_file(const char *file)
{
	const char *const cmd[] = {"Launch.sh", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->name); // MAME uses the romset's name as the argument and not path
}

static PzWindow *browse_romsets()
{
	return open_directory_title_mod(dir, "iPodMAME Romsets",
		check_is_dir, load_file_handler);
}

static PzWindow *fastlaunch()
{
	pz_exec(path);
	return NULL;
}

static void init_launch() 
{
	module = pz_register_module("iPodMAME", 0);
	path = "/opt/Emulators/iPodMAME/Launch/Launch.sh";
	dir = "/opt/Emulators/iPodMAME/Roms";
	
	pz_menu_add_stub_group("/Emulators/iPodMAME", "Arcade");
	pz_menu_add_action_group("/Emulators/iPodMAME/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/iPodMAME/Romsets", "Browse", browse_romsets);
	pz_menu_sort("/Emulators/iPodMAME");
}

PZ_MOD_INIT(init_launch)
