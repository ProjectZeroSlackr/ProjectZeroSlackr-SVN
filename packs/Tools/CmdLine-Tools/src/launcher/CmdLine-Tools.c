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

#define PATH "/opt/Tools/CmdLine-Tools/Bin"
#define JOHN "/opt/Tools/CmdLine-Tools/John"
#define chmod_path "/opt/Tools/CmdLine-Tools/Launch/chmod-Terminal.sh"
#define file_path "/opt/Tools/CmdLine-Tools/Launch/file-Terminal.sh"
#define info_path "/opt/Tools/CmdLine-Tools/Launch/info-Terminal.sh"
#define lolcode_path "/opt/Tools/CmdLine-Tools/Launch/lol-Terminal.sh"
#define lolcode_dir "/opt/Tools/CmdLine-Tools/LOLCODE"

static PzModule *module;
static ttk_menu_item
chmod_browser_extension,
dos2unix_browser_extension,
vi_browser_extension,
lolcode_browser_extension;

static int check_is_text_file(const char* file)
{
	return (
		check_file_ext(file, ".txt") ||
		check_file_ext(file, ".log") ||
		check_file_ext(file, ".nfo") ||
		check_file_ext(file, ".ini") ||
		check_file_ext(file, ".cfg") ||
		check_file_ext(file, ".conf")
	);
}

static PzWindow *chmod_load_file(const char *file)
{
	const char *const cmd[] = {"chmod-Terminal.sh", file, NULL};
	return new_terminal_window_with(
		chmod_path,
		(char *const *)cmd
	);
}

static PzWindow *chmod_load_file_handler(ttk_menu_item *item)
{
	return chmod_load_file(item->data);
}

static PzWindow *dos2unix_load_file(const char *file)
{
	const char *const cmd[] = {"file-Terminal.sh", "dos2unix", file, NULL};
	return new_terminal_window_with(
		file_path,
		(char *const *)cmd
	);
}

static PzWindow *dos2unix_load_file_handler(ttk_menu_item *item)
{
	return dos2unix_load_file(item->data);
}

static PzWindow *vi_load_file(const char *file)
{
	const char *const cmd[] = {"file-Terminal.sh", "vi", file, NULL};
	return new_terminal_window_with(
		file_path,
		(char *const *)cmd
	);
}

static PzWindow *vi_load_file_handler(ttk_menu_item *item)
{
	return vi_load_file(item->data);
}

static PzWindow *info()
{
	const char *const cmd[] = {"info-Terminal.sh", NULL};
	return new_terminal_window_with(
		info_path,
		(char *const *)cmd
	);
}

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

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Tools/CmdLine-Tools/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_browser_remove_handler(lolcode_check_ext);
}

static void init_launch()
{
	module = pz_register_module("CmdLine-Tools", cleanup);
	char path[256];
	sprintf(
		path, "%s:%s",
		getenv("PATH"),
		PATH
		);	
	setenv("PATH", path, 1);
	setenv("JOHN", JOHN, 1);
	
	pz_menu_add_stub_group("/Tools/CmdLine-Tools", "Utilities");
	pz_menu_add_action_group("/Tools/CmdLine-Tools/LOLCODE FastLaunch",
		"#FastLaunch", lolcode_fastlaunch);
	pz_menu_add_action_group("/Tools/CmdLine-Tools/System Information", "#FastLaunch", info);
	pz_menu_add_action_group("/Tools/CmdLine-Tools/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Tools/CmdLine-Tools/LOLCODE Scripts", "Browse LOLCODE", browse_lolcode);
	pz_menu_sort("/Tools/CmdLine-Tools");
	
	chmod_browser_extension.name = N_("Set full permissions");
	chmod_browser_extension.makesub = chmod_load_file_handler;
	pz_browser_add_action(check_nothing, &chmod_browser_extension);
	
	dos2unix_browser_extension.name = N_("Convert to UNIX format");
	dos2unix_browser_extension.makesub = dos2unix_load_file_handler;
	pz_browser_add_action(check_is_text_file, &dos2unix_browser_extension);
	
	vi_browser_extension.name = N_("Open with vi");
	vi_browser_extension.makesub = vi_load_file_handler;
	pz_browser_add_action(check_is_text_file, &vi_browser_extension);
	
	lolcode_browser_extension.name = N_("Interpret LOLCODE");
	lolcode_browser_extension.makesub = lolcode_load_file_handler;
	pz_browser_add_action(lolcode_check_ext, &lolcode_browser_extension);
	pz_browser_set_handler(lolcode_check_ext, lolcode_load_file);
}

PZ_MOD_INIT(init_launch)
