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

static PzModule *module;
static ttk_menu_item browser_extension;
static const char *path, *dir;

static int check_ext(const char *file)
{
	return (
		check_file_ext(file, ".gbc") ||
		check_file_ext(file, ".gb")
		);
}

static PzWindow *load_file(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	const char *const cmd[] = {"Launch.sh", f, d, NULL};
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
	return open_directory_title(dir, "iBoy Roms");
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Emulators/iBoy/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("iBoy", cleanup);
	path = "/opt/Emulators/iBoy/Launch/Launch.sh";
	dir = "/opt/Emulators/iBoy/Roms";
	
	pz_menu_add_stub_group("/Emulators/iBoy", "Handheld");
	pz_menu_add_action_group("/Emulators/iBoy/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Emulators/iBoy/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Emulators/iBoy/Roms", "Browse", browse_roms);
	pz_menu_sort("/Emulators/iBoy");
	
	browser_extension.name = N_("Open with iBoy");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
