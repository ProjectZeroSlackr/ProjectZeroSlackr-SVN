/*
 * Last updated: Oct 12, 2008
 * ~Keripo
 *  
 * Copyright (C) 2007 Keripo
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

static PzWindow *new_moon_buggy_window(void)
{
	const char *const cmd[] = {"Terminal.sh", NULL};
	return new_terminal_window_with(
		"/opt/Media/Moon-Buggy/Launch/Terminal.sh",
		(char *const *)cmd
	);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Media/Moon-Buggy/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("Moon-Buggy", 0);
	pz_menu_add_stub_group("/Media/Moon-Buggy", "Text Games");
	pz_menu_add_action_group("/Media/Moon-Buggy/#Terminal launch", "#FastLaunch", new_moon_buggy_window);
	pz_menu_add_action_group("/Media/Moon-Buggy/~ReadMe", "#FastLaunch", readme);
	pz_menu_sort("/Media/Moon-Buggy");
}

PZ_MOD_INIT (init_launch)
