/*
 * Last updated: Jun 25, 2008
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

static PzModule *module;

static void init_launch()
{
	module = pz_register_module("CmdLine-Tools", 0);
	
	char path[256];
	sprintf(
		path, "%s:%s",
		getenv("PATH"),
		PATH
		);	
	setenv("PATH", path, 1);
	setenv("JOHN", JOHN, 1);
}

PZ_MOD_INIT(init_launch)
