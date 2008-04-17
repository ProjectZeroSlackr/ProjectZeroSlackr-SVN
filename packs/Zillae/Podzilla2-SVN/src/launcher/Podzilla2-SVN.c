/*
 * Last updated: Apr 17, 2008
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

extern void pz_exec();

#define HIDE_ALL 1
#define HIDE_ARCADE 2
#define HIDE_DEV 3
#define HIDE_MEDIA 4
#define HIDE_MISC 5
#define HIDE_TOOLS 6
#define HIDE_UNSORTED 7

static PzModule *module;
static PzConfig *config;
static const char *on_off_options[] = {"Off", "On", 0};

static PzWindow *fastlaunch()
{
	pz_exec(pz_module_get_datapath(module, "FastLaunch.sh"));
	return NULL;
}
static PzWindow *launch_all()
{
	pz_exec(pz_module_get_datapath(module, "Launch/All.sh"));
	return NULL;
}
static PzWindow *launch_arcade()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Arcade.sh"));
	return NULL;
}
static PzWindow *launch_dev()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Dev.sh"));
	return NULL;
}
static PzWindow *launch_media()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Media.sh"));
	return NULL;
}
static PzWindow *launch_misc()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Misc.sh"));
	return NULL;
}
static PzWindow *launch_tools()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Tools.sh"));
	return NULL;
}
static PzWindow *launch_unsorted()
{
	pz_exec(pz_module_get_datapath(module, "Launch/Unsorted.sh"));
	return NULL;
}

static void cleanup()
{
	pz_save_config(config);
}

static void init_launch() 
{
	module = pz_register_module ("Podzilla2-SVN", cleanup);

	config = pz_load_config(pz_module_get_datapath(module, "../Conf/Hide.conf"));
	if (!pz_get_setting(config, HIDE_ALL)) pz_set_int_setting (config, HIDE_ALL, 0);
	if (!pz_get_setting(config, HIDE_ARCADE)) pz_set_int_setting (config, HIDE_ARCADE, 0);
	if (!pz_get_setting(config, HIDE_DEV)) pz_set_int_setting (config, HIDE_DEV, 0);
	if (!pz_get_setting(config, HIDE_MEDIA)) pz_set_int_setting (config, HIDE_MEDIA, 0);
	if (!pz_get_setting(config, HIDE_MISC)) pz_set_int_setting (config, HIDE_MISC, 0);
	if (!pz_get_setting(config, HIDE_TOOLS)) pz_set_int_setting (config, HIDE_TOOLS, 0);
	if (!pz_get_setting(config, HIDE_UNSORTED)) pz_set_int_setting (config, HIDE_UNSORTED, 0);
	
	pz_menu_add_stub_group ("/Zillae/Podzilla2-SVN", "#PZ2 Based");
		pz_menu_add_action_group("/Zillae/Podzilla2-SVN/#Fastlaunch",	"#Normal", fastlaunch);
	if (pz_get_int_setting(config, HIDE_ALL) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/All Modules",	"#Normal", launch_all);
	if (pz_get_int_setting(config, HIDE_ARCADE) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Arcade Only",	"Lite", launch_arcade);
	if (pz_get_int_setting(config, HIDE_DEV) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Dev Only",	"Lite", launch_dev);
	if (pz_get_int_setting(config, HIDE_MEDIA) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Media Only",	"Lite", launch_media);
	if (pz_get_int_setting(config, HIDE_MISC) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Misc Only",	"Lite", launch_misc);
	if (pz_get_int_setting(config, HIDE_TOOLS) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Tools Only",	"Lite", launch_tools);
	if (pz_get_int_setting(config, HIDE_UNSORTED) == 0)
		pz_menu_add_action_group ("/Zillae/Podzilla2-SVN/Unsorted Only","Lite", launch_unsorted);
	
	pz_menu_add_stub_group ("/Zillae/Podzilla2-SVN/Settings", "Settings");
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide All Modules",	"Normal", HIDE_ALL, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Arcade Only",	"Lite", HIDE_ARCADE, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Dev Only",	"Lite", HIDE_DEV, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Media Only",	"Lite", HIDE_MEDIA, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Misc Only",	"Lite", HIDE_MISC, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Tools Only",	"Lite", HIDE_TOOLS, config, on_off_options);
	pz_menu_add_setting_group("/Zillae/Podzilla2-SVN/Settings/Hide Unsorted Only",	"Lite", HIDE_UNSORTED, config, on_off_options);
	
	pz_menu_sort ("/Zillae/Podzilla2-SVN");
}

PZ_MOD_INIT (init_launch)
