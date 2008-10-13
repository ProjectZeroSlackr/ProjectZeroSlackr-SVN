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
	return check_file_ext(file, ".sid");
}

static int mpd_check()
{
	if (MPD_ACTIVE) {
		pz_error("MPD is active. Please kill MPD first before running %s", "SBaGen");
		return 1;
	} else {
		return 0;
	}
}

static PzWindow *load_file(const char *file)
{
	if (mpd_check())
		return NULL;
	const char *const cmd[] = {"Launch.sh", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
		);
	return NULL;
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}

static PzWindow *browse_sids()
{
	chdir(dir);
	return open_directory_title(dir, "TinySID SIDs");
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Media/TinySID/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("TinySID", cleanup);
	path = "/opt/Media/TinySID/Launch/Launch.sh";
	dir = "/opt/Media/TinySID/SIDs";
	
	pz_menu_add_stub_group("/Media/TinySID", "Music");
	pz_menu_add_action_group("/Media/TinySID/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Media/TinySID/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Media/TinySID/SIDs", "Browse", browse_sids);
	pz_menu_sort("/Media/TinySID");

	browser_extension.name = N_("Open with TinySID");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
