/*
 * Last updated: March 14, 2008
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

static PzModule *module;
static ttk_menu_item browser_extension;

static int check_ext(const char* file) {
	return check_file_ext(file, ".mvpd");
}

static PzWindow *load_file(const char *file)
{
	const char *const path = pz_module_get_datapath(module, "MV_Player-CLI");
	const char *const cmd[] = {"MV_Player-CLI", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_handler(ttk_menu_item * item)
{
	load_file(item->data);
	return 0;
}

static PzWindow *browse_vids()
{
	const char *const path = pz_module_get_datapath(module, "Videos");
	chdir(path);
	return open_directory_title(path, "MvPD Videos");
}

static PzWindow *fastlaunch()
{
	pz_exec(pz_module_get_datapath(module, "FastLaunch.sh"));
	return NULL;
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("MvPD", cleanup);
	
	pz_menu_add_stub_group("/Media/MvPD", "Video");
	pz_menu_add_action_group("/Media/MvPD/FastLaunch", "Launching", fastlaunch);
	pz_menu_add_action_group("/Media/MvPD/Videos", "Launching", browse_vids);
	pz_menu_sort("/Media/MvPD");
	
	browser_extension.name = N_("Open with MvPD");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
