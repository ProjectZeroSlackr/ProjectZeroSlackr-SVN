/*
 * Last updated: Aug 11, 2008
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
static ttk_menu_item
browser_extension_file, browser_extension_folder;
static const char
*path, *path_iv_file, *path_iv_folder, *dir_iv;

// Common
static PzWindow *exec_demo(const char *binary)
{
	const char *const cmd[] = {
		"Launch.sh",
		binary,
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
	return NULL;
}

// TinyGL demos
static PzWindow *gears()
{
	return exec_demo("Gears");
}
static PzWindow *mech()
{
	return exec_demo("Mech");
}
static PzWindow *spin()
{
	return exec_demo("Spin");
}
static PzWindow *texobj()
{
	return exec_demo("Texobj");
}

// IV
static int check_ext(const char *file)
{
	return (
		check_file_ext(file, ".jpg") ||
		check_file_ext(file, ".png")
		);
}
static PzWindow *load_file(const char *file)
{
	const char *const cmd[] = {"ImageViewer-File.sh", file, NULL};
	pz_execv(
		path_iv_file,
		(char *const *)cmd
	);
	return NULL;
}
static PzWindow *load_folder(const char *file)
{
	const char *const cmd[] = {"ImageViewer-Folder.sh", file, NULL};
	pz_execv(
		path_iv_folder,
		(char *const *)cmd
	);
	return NULL;
}
static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}
static PzWindow *load_folder_handler(ttk_menu_item *item)
{
	return load_folder(item->data);
}
static PzWindow *browse_pics()
{
	return open_directory_title(dir_iv, "Pictures");
}
static PzWindow *view_all_pics()
{
	return load_folder(dir_iv);
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Media/TinyGL-Demos/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("TinyGL-Demos", cleanup);
	path = "/opt/Media/TinyGL-Demos/Launch/Launch.sh";
	path_iv_file = "/opt/Media/TinyGL-Demos/Launch/ImageViewer-File.sh";
	path_iv_folder = "/opt/Media/TinyGL-Demos/Launch/ImageViewer-Folder.sh";
	dir_iv = "/opt/Media/TinyGL-Demos/Pics";
	
	pz_menu_add_stub_group("/Media/TinyGL-Demos", "Demos");
	pz_menu_add_action_group("/Media/TinyGL-Demos/ReadMe", "#Info", readme);
	pz_menu_add_action_group("/Media/TinyGL-Demos/Gears", "Demos", gears);
	pz_menu_add_action_group("/Media/TinyGL-Demos/Mech", "Demos", mech);
	pz_menu_add_action_group("/Media/TinyGL-Demos/Spin", "Demos", spin);
	pz_menu_add_action_group("/Media/TinyGL-Demos/Texobj", "Demos", texobj);
	pz_menu_add_action_group("/Media/TinyGL-Demos/ImageViewer", "Viewer", browse_pics);
	pz_menu_add_action_group("/Media/TinyGL-Demos/View All Pics", "Viewer", view_all_pics);
	pz_menu_sort("/Media/TinyGL-Demos");
	
	browser_extension_file.name = N_("Open with ImageViewer");
	browser_extension_file.makesub = load_file_handler;
	pz_browser_add_action(check_ext, &browser_extension_file);
	browser_extension_folder.name = N_("Slideshow with ImageViewer");
	browser_extension_folder.makesub = load_folder_handler;
	pz_browser_add_action(check_is_dir, &browser_extension_folder);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT (init_launch)
