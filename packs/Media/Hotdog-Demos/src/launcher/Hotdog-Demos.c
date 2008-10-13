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
static const char *path, *path_imgbits, *dir_imgbits;

// Common
static PzWindow *exec_demo(const char *folder, const char *binary)
{
	const char *const cmd[] = {
		"Launch.sh",
		folder,
		binary,
		NULL
	};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

// hotdog demos
static PzWindow *hd_pong()
{
	return exec_demo("HD-Pong", "hd-pong");
}
static PzWindow *anim_demo()
{
	return exec_demo("Anim-Demo", "anim-demo");
}
static PzWindow *select_demo()
{
	return exec_demo("Select-Demo", "select-demo");
}
static PzWindow *prim_demo()
{
	return exec_demo("Prim-Demo", "prim-demo");
}

// hotdog-ex demos
static PzWindow *bolly()
{
	return exec_demo("Hotdog-EX", "bolly");
}
static PzWindow *bouncy()
{
	return exec_demo("Hotdog-EX", "bouncy");
}
static PzWindow *spinner()
{
	return exec_demo("Hotdog-EX", "spinner");
}

// imagebits
static int check_ext(const char *file)
{
	return check_file_ext(file, ".png");
}
static PzWindow *load_file(const char *file)
{
	const char *const cmd[] = {"Imagebits.sh", file, NULL};
	pz_execv(
		path_imgbits,
		(char *const *)cmd
	);
	return NULL;
}
static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file((const char *)(item->data)); // Recast for browser_mod
}
static PzWindow *browse_imagebits()
{
	return open_directory_title_mod(
		dir_imgbits, "imagebits PNGs",
		check_ext, load_file_handler);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Media/Hotdog-Demos/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("Hotdog-Demos", 0);
	path = "/opt/Media/Hotdog-Demos/Launch/Launch.sh";
	path_imgbits = "/opt/Media/Hotdog-Demos/Launch/Imagebits.sh";
	dir_imgbits = "/opt/Media/Hotdog-Demos/Hotdog-EX/PNGs";
	
	pz_menu_add_stub_group("/Media/Hotdog-Demos", "Demos");
	pz_menu_add_action_group("/Media/Hotdog-Demos/ReadMe", "#Info", readme);
	// hotdog demos
	pz_menu_add_action_group("/Media/Hotdog-Demos/HD-Pong", "Interactive", hd_pong);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Anim-Demo", "Interactive", anim_demo);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Select-Demo", "Interactive", select_demo);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Prim-Demo", "Static", prim_demo);
	// hotdog-ex demos
	pz_menu_add_action_group("/Media/Hotdog-Demos/Bouncy", "Interactive", bouncy);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Bolly", "Static", bolly);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Spinner", "Static", spinner);
	pz_menu_add_action_group("/Media/Hotdog-Demos/Imagebits", "Static", browse_imagebits);
	pz_menu_sort("/Media/Hotdog-Demos");
	
	browser_extension.name = N_("View with Imagebits");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action(check_ext, &browser_extension);
}

PZ_MOD_INIT (init_launch)
