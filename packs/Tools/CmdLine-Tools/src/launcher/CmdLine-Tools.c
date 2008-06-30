/*
 * Last updated: Jun 30, 2008
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
static ttk_menu_item lolcode_browser_extension;
static const char *lolcode_path, *lolcode_dir, *PATH, *JOHN;

static int lolcode_check_ext(const char* file)
{
	return check_file_ext(file, ".lol");
}

static PzWindow *lolcode_load_file(const char *file)
{
	const char *const cmd[] = {"lol-Terminal.sh", file, NULL};
	return new_terminal_window_with(
		lolcode_path,
		(char *const *)cmd
	);
}

static PzWindow *lolcode_load_file_handler(ttk_menu_item *item)
{
	return lolcode_load_file(item->data);
}

static PzWindow *lolcode_fastlaunch() {
	return lolcode_load_file(NULL);
}

static PzWindow *browse_lolcode()
{
	return open_directory_title(lolcode_dir, "LOLCODE Scripts");
}

static void cleanup()
{
	pz_browser_remove_handler(lolcode_check_ext);
}

static void init_launch()
{
	module = pz_register_module("CmdLine-Tools", cleanup);
	PATH = "/opt/Tools/CmdLine-Tools/Bin";
	JOHN = "/opt/Tools/CmdLine-Tools/John";
	lolcode_path = "/opt/Tools/CmdLine-Tools/Launch/lol-Terminal.sh";
	lolcode_dir = "/opt/Tools/CmdLine-Tools/LOLCODE";
	
	char path[256];
	sprintf(
		path, "%s:%s",
		getenv("PATH"),
		PATH
		);	
	setenv("PATH", path, 1);
	setenv("JOHN", JOHN, 1);
	
	pz_menu_add_stub_group("/Tools/LOLCODE", "Development");
	pz_menu_add_action_group("/Tools/LOLCODE/#FastLaunch",
		"#FastLaunch", lolcode_fastlaunch);
	pz_menu_add_action_group("/Tools/LOLCODE/Scripts", "Browse", browse_lolcode);
	pz_menu_sort("/Tools/LOLCODE");
	
	lolcode_browser_extension.name = N_("Interpret LOLCODE");
	lolcode_browser_extension.makesub = lolcode_load_file_handler;
	pz_browser_add_action (lolcode_check_ext, &lolcode_browser_extension);
	pz_browser_set_handler(lolcode_check_ext, lolcode_load_file);
}

PZ_MOD_INIT(init_launch)
