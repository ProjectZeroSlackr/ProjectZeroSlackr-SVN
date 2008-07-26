/*
 * Last updated: July 26, 2008
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

static PzWindow *load_game(const char *folder)
{
	pz_warning("Note: %s requires large amounts of memory and may not work here.", "ONScripter");
	pz_warning("You should try launching %s from Loader2 instead (see the \"loader.cfg\" file).", "ONScripter");
	const char *const cmd[] = {"Launch.sh", folder, NULL};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_game_handler(ttk_menu_item *item)
{
	return load_game((const char *)(item->data)); // Recast for browser_mod
}

static PzWindow *browse_novels()
{
	return open_directory_title_mod(dir, "Visual Novels",
		check_is_dir, load_game_handler);
}

static PzWindow *fastlaunch()
{
	return load_game(NULL);
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Media/ONScripter/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("ONScripter", 0);
	path = "/opt/Media/ONScripter/Launch/Launch.sh";
	dir = "/opt/Media/ONScripter/VisualNovels";
	
	pz_menu_add_stub_group("/Media/ONScripter", "Games");
	pz_menu_add_action_group("/Media/ONScripter/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Media/ONScripter/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Media/ONScripter/Visual Novels", "Browse", browse_novels);
	pz_menu_sort("/Media/ONScripter");
}

PZ_MOD_INIT(init_launch)
