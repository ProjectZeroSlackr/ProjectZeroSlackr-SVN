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

#include "pz.h"

static PzModule *module;
static const char *path;

static PzWindow *fastlaunch()
{
	pz_exec_kill(path);
	return NULL;
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Base/ZeroLauncher/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module ("ZeroLauncher", 0);
	path = "/opt/Base/ZeroLauncher/Launch/Launch.sh";
	pz_menu_add_stub_group("/Zillae/ZeroLauncher", "#PZ2 Based");
	pz_menu_add_action_group("/Zillae/ZeroLauncher/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Zillae/ZeroLauncher/~ReadMe", "#FastLaunch", readme);
	pz_menu_sort("/Zillae/ZeroLauncher");
}

PZ_MOD_INIT (init_launch)
