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

#define HIDE_ALL		1
#define HIDE_ARCADE		2
#define HIDE_DEV		3
#define HIDE_MEDIA		4
#define HIDE_MISC		5
#define HIDE_TOOLS		6
#define HIDE_UNSORTED	7

static PzModule *module;
static PzConfig *config;
static const char *path;
static const char *on_off_options[] = {"Off", "On", 0};

static PzWindow *fastlaunch()
{
	pz_exec_kill(path);
	return NULL;
}
static PzWindow *launch_all()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
	return NULL;
}
static PzWindow *launch_arcade()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Arcade",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}
static PzWindow *launch_dev()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Dev",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}
static PzWindow *launch_media()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Media",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}
static PzWindow *launch_misc()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Misc",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}
static PzWindow *launch_tools()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Tools",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}
static PzWindow *launch_unsorted()
{
	const char *const cmd[] = {
		"Launch.sh",
		"/usr/lib/All:/usr/lib/Unsorted",
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Zillae/Podzilla2-SVN/ReadMe from Keripo.txt");
}


static void cleanup()
{
	pz_save_config(config);
	pz_free_config(config);
	config = 0;
}

static void init_launch() 
{
	module = pz_register_module("Podzilla2-SVN", cleanup);
	path = "/opt/Zillae/Podzilla2-SVN/Launch/Launch.sh";

	config = pz_load_config("/opt/Zillae/Podzilla2-SVN/Conf/Hide.conf");
	if (!pz_get_setting(config, HIDE_ALL)) pz_set_int_setting (config, HIDE_ALL, 0);
	if (!pz_get_setting(config, HIDE_ARCADE)) pz_set_int_setting (config, HIDE_ARCADE, 0);
	if (!pz_get_setting(config, HIDE_DEV)) pz_set_int_setting (config, HIDE_DEV, 0);
	if (!pz_get_setting(config, HIDE_MEDIA)) pz_set_int_setting (config, HIDE_MEDIA, 0);
	if (!pz_get_setting(config, HIDE_MISC)) pz_set_int_setting (config, HIDE_MISC, 0);
	if (!pz_get_setting(config, HIDE_TOOLS)) pz_set_int_setting (config, HIDE_TOOLS, 0);
	if (!pz_get_setting(config, HIDE_UNSORTED)) pz_set_int_setting (config, HIDE_UNSORTED, 0);
	
	pz_menu_add_stub_group("/Zillae/Podzilla2-SVN", "#PZ2 Based");
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/#Fastlaunch",	"#Normal", fastlaunch);
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/~ReadMe",		"#Normal", readme);
	if (pz_get_int_setting(config, HIDE_ALL) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/All Modules",	"#Normal", launch_all);
	if (pz_get_int_setting(config, HIDE_ARCADE) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Arcade",	"Lite", launch_arcade);
	if (pz_get_int_setting(config, HIDE_DEV) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Dev",	"Lite", launch_dev);
	if (pz_get_int_setting(config, HIDE_MEDIA) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Media",	"Lite", launch_media);
	if (pz_get_int_setting(config, HIDE_MISC) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Misc",	"Lite", launch_misc);
	if (pz_get_int_setting(config, HIDE_TOOLS) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Tools",	"Lite", launch_tools);
	if (pz_get_int_setting(config, HIDE_UNSORTED) == 0)
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/Unsorted","Lite", launch_unsorted);
	
	pz_menu_add_stub_group("/Zillae/Podzilla2-SVN/Hide Settings", "~Settings");
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide All Modules",	"#Normal", HIDE_ALL, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Arcade",	"Lite", HIDE_ARCADE, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Dev",	"Lite", HIDE_DEV, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Media",	"Lite", HIDE_MEDIA, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Misc",	"Lite", HIDE_MISC, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Tools",	"Lite", HIDE_TOOLS, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Hide Settings/Hide Unsorted",	"Lite", HIDE_UNSORTED, config, on_off_options);
	pz_menu_sort("/Zillae/Podzilla2-SVN");
}

PZ_MOD_INIT (init_launch)
