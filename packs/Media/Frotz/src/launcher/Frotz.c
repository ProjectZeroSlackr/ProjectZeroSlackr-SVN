/*
 * Last updated: Aug 4, 2008
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
	const char *const cmd[] = {
		"Terminal.sh", file, NULL};
	return new_terminal_window_with(
		path,
		(char *const *)cmd
	);
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}

static PzWindow *browse_games()
{
	return open_directory_title_mod(dir, "Z-Machine Games",
		check_is_file, load_file_handler);
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Media/Frotz/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("Frotz", 0);
	path = "/opt/Media/Frotz/Launch/Terminal.sh";
	dir = "/opt/Media/Frotz/Games";
	
	pz_menu_add_stub_group("/Media/Frotz", "Games");
	pz_menu_add_action_group("/Media/Frotz/~ReadMe", "#Info", readme);
	pz_menu_add_action_group("/Media/Frotz/Games", "Browse", browse_games);
	pz_menu_sort("/Media/Frotz");
}

PZ_MOD_INIT (init_launch)
