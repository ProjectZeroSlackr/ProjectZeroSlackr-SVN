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

static PzWindow *fastlaunch()
{
	pz_warning("Note: %s requires large amounts of memory and may not work here.", "Encyclopodia");
	pz_warning("You should try launching %s from Loader2 instead (see the \"loader.cfg\" file).", "Encyclopodia");
	int launch = pz_errdialog(_("Launch app?"), _("If you are using a large library, you will get bzip errors. Do you wish to continue?"), 2, 0, _("No"), _("Yes"));
	if (launch == 0) return NULL;
	pz_exec_kill("/opt/Tools/Encyclopodia/Launch/Launch.sh");
	return NULL;
}

static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Tools/Encyclopodia/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("Encyclopodia", 0);
	pz_menu_add_stub_group("/Tools/Encyclopodia", "Reference");
	pz_menu_add_action_group("/Tools/Encyclopodia/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Tools/Encyclopodia/~ReadMe", "#FastLaunch", readme);
	pz_menu_sort("/Tools/Encyclopodia");
}

PZ_MOD_INIT (init_launch)
