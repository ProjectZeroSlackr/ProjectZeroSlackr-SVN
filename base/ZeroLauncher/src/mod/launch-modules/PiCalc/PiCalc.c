/*
 * Last updated: March 14, 2008
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

#include "pz.h"

extern PzWindow *new_terminal_window_with();

static PzModule *module;

static PzWindow *new_picalc_window(void)
{
	const char *const cmd[] = {"Terminal.sh", NULL};
	return new_terminal_window_with(pz_module_get_datapath(module, "Terminal.sh"), (char *const *)cmd);
}

static void init_launch() 
{
	module = pz_register_module ("PiCalc", 0);
	pz_menu_add_action_group ("/Tools/PiCalc", "Reference", new_picalc_window);
}

PZ_MOD_INIT (init_launch)
