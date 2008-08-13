/*
 * Last updated: Aug 13, 2008
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

static int check_file(const char *file, const char *game)
{
	FILE *f;
	f = fopen(file, "r");
	if (f == NULL) {
		pz_error("Data files missing. Unable to start %s.",	game);
		return 0;
	} else {
		pz_warning("Game saving does not yet work. %s",
			"Please do not try to save/load games.");
		fclose(f);
		return 1;
	}
}

static PzWindow *wolf3d_shareware()
{
	if (check_file(
		"/opt/Media/hWolf3D/Wolf3D-Shareware/vswap.wl1",
		"Wolfenstein 3D (Shareware)") == 1) {
		pz_exec_kill(
			"/opt/Media/hWolf3D/Launch/Launch-Wolf3D-Shareware.sh"
			);
	}
	return NULL;
}

static PzWindow *wolf3d_retail()
{
	if (check_file(
		"/opt/Media/hWolf3D/Wolf3D-Retail/vswap.wl1",
		"Wolfenstein 3D (Retail)") == 1) {
		pz_exec_kill(
			"/opt/Media/hWolf3D/Launch/Launch-Wolf3D-Retail.sh"
			);
	}
	return NULL;
}

static PzWindow *sod_shareware()
{
	if (check_file(
		"/opt/Media/hWolf3D/SoD-Shareware/vswap.sdm",
		"Spear of Destiny (Shareware)") == 1) {
		pz_exec_kill(
			"/opt/Media/hWolf3D/Launch/Launch-SoD-Shareware.sh"
			);
	}
	return NULL;
}

static PzWindow *sod_retail()
{
	if (check_file(
		"/opt/Media/hWolf3D/SoD-Retail/vswap.sdm",
		"Spear of Destiny (Retail)") == 1) {
		pz_exec_kill(
			"/opt/Media/hWolf3D/Launch/Launch-SoD-Retail.sh"
			);
	}
	return NULL;
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Media/hWolf3D/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module ("hWolf3D", 0);
	pz_menu_add_stub_group("/Media/hWolf3D", "Games");
	pz_menu_add_action_group("/Media/hWolf3D/#Wolfenstein-3D (Shareware)",
		"#FastLaunch", wolf3d_shareware);
	pz_menu_add_action_group("/Media/hWolf3D/#Wolfenstein-3D (Retail)",
		"#FastLaunch", wolf3d_retail);
	pz_menu_add_action_group("/Media/hWolf3D/Spear of Destiny (Shareware)",
		"#FastLaunch", sod_shareware);
	pz_menu_add_action_group("/Media/hWolf3D/Spear of Destiny (Retail)",
		"#FastLaunch", sod_retail);
	pz_menu_add_action_group("/Media/hWolf3D/~ReadMe", "#FastLaunch", readme);
	pz_menu_sort("/Media/hWolf3D");
}

PZ_MOD_INIT (init_launch)
