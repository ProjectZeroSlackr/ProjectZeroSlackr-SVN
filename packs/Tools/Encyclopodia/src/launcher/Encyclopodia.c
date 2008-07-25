/*
 * Last updated: July 22, 2008
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
	pz_exec_kill("/opt/Tools/Encyclopodia/Launch/Launch.sh");
	return NULL;
}

static void init_launch() 
{
	module = pz_register_module ("Encyclopodia", 0);
	pz_menu_add_action_group ("/Tools/Encyclopodia", "Reference", fastlaunch);
}

PZ_MOD_INIT (init_launch)
