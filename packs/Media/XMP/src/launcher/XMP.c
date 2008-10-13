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

/* Supported formats. 82 in total. */
static const char *formats[] = { 
	".xm" , ".mod", ".flt", ".st" , ".m15", ".it" , 
	".s3m", ".stm", ".stx", ".mtm", ".ice", ".imf", 
	".ptm", ".mdl", ".ult", ".liq", ".psm", ".amf", 
	".mmd0",".mmd1",".mmd2",".mmd3",".med", ".med3", 
	".med4",".dmf", ".rtm", ".pt3", ".tcb", ".dt" , 
	".gtk", ".dtt", ".mgt", ".arch",".dsym",".digi", 
	".dbm", ".emod",".okt", ".sfx", ".far", ".umx", 
	".stim",".mtp", ".ims", ".669", ".fnk", ".funk", 
	".amd", ".rad", ".hsc", ".alm", ".ac1d",".fchs", 
	".fcm", ".fuzz",".kris",".ksm", ".mp" , ".p18a", 
	".p10c",".pru1",".pru2",".pha", ".wn" , ".unic", 
	".tp3", ".xann",".di" , ".eu" , ".p4x", ".p40", 
	".p41", ".p60a",".np1", ".np2", ".np3", ".zen", 
	".crb", ".tdd", ".gmc", ".gdm" };

static int check_ext(const char *file)
{
	int i;
	for (i = 0; i < 82; i++) {
		if (check_file_ext(file, formats[i]) == 1) return 1;
	}
	return 0;
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

static PzWindow *browse_mods()
{
	chdir(dir);
	return open_directory_title(dir, "XMP Modules");
}

static PzWindow *fastlaunch()
{
	return load_file(NULL);
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Media/XMP/ReadMe from Keripo.txt");
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("XMP", cleanup);
	path = "/opt/Media/XMP/Launch/Launch.sh";
	dir = "/opt/Media/XMP/Mods";
	
	pz_menu_add_stub_group("/Media/XMP", "Music");
	pz_menu_add_action_group("/Media/XMP/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Media/XMP/~ReadMe", "#FastLaunch", readme);
	pz_menu_add_action_group("/Media/XMP/Modules", "Browse", browse_mods);
	pz_menu_sort("/Media/XMP");

	browser_extension.name = N_("Open with XMP");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)
