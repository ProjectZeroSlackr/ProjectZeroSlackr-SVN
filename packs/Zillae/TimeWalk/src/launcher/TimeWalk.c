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

#define DEFAULT "podzilla"

static PzModule *module;
static const char *path;

// Common
static PzWindow *exec_zilla(const char *folder, const char *binary)
{
	const char *const cmd[] = {
		"Launch.sh",
		folder,
		binary,
		NULL
	};
	pz_execv_kill(
		path,
		(char *const *)cmd
	);
	return NULL;
}

// Non-standards
static PzWindow *iPod_Desktop()
{
	// Special since it needs nano-X started
	pz_exec_kill("/opt/Zillae/TimeWalk/Launch/iPod-Desktop.sh");
	return NULL;
}
static PzWindow *Owen_OS()
{
	const char *const cmd[] = {"Owen-OS.sh", NULL};
	return new_terminal_window_with(
		"/opt/Zillae/TimeWalk/Launch/Owen-OS.sh",
		(char *const *)cmd
	);
}
static PzWindow *Margarelon()
{
	return exec_zilla("Margarelon", "margarelon");
}

// PZ0 Specials
static PzWindow *iTanks()
{
	return exec_zilla("iTanks", DEFAULT);
}
static PzWindow *MikModPodzilla()
{
	return exec_zilla("MikModPodzilla", "mikpodzilla");
}
static PzWindow *pdPod()
{
	return exec_zilla("pdPod", DEFAULT);
}

// PZ0 Features
static PzWindow *Keyman_pz0()
{
	return exec_zilla("Keyman", DEFAULT);
}
static PzWindow *Keyman_floyd()
{
	return exec_zilla("Keyman", "floydzilla");
}
static PzWindow *NXSnake()
{
	return exec_zilla("NXSnake", DEFAULT);
}
static PzWindow *Othello()
{
	return exec_zilla("Othello", DEFAULT);
}
static PzWindow *Sudoku()
{
	return exec_zilla("Sudoku", DEFAULT);
}
static PzWindow *Tuxchess()
{
	return exec_zilla("Tuxchess", DEFAULT);
}
static PzWindow *Video_podzilla()
{
	return exec_zilla("Video-podzilla", DEFAULT);
}

// PZ0 Jonrelays
static PzWindow *podzilla_fbk()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-fbk");
}
static PzWindow *podzilla_mc()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-mc");
}
static PzWindow *podzilla_ti()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti");
}
static PzWindow *podzilla_ti2()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti2");
}
static PzWindow *podzilla_ti3()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti3");
}
static PzWindow *podzilla_ti4()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti4");
}
static PzWindow *podzilla_ti5()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti5");
}
static PzWindow *podzilla_ti6()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti6");
}
static PzWindow *podzilla_ti7()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti7");
}
static PzWindow *podzilla_ti8()
{
	return exec_zilla("podzilla-jonrelay", "podzilla-ti8");
}
static PzWindow *relayzilla()
{
	return exec_zilla("podzilla-jonrelay", "relayzilla");
}

// PZ0 Translates
static PzWindow *podzilla_Dutch()
{
	return exec_zilla("podzilla-Dutch", "podzilla_dutch");
}
static PzWindow *Jpodzilla()
{
	return exec_zilla("Jpodzilla", DEFAULT);
}

// PZ0 Custom Builds
static PzWindow *Funzilla()
{
	return exec_zilla("Funzilla", "Funzilla");
}
static PzWindow *Yankeezilla()
{
	return exec_zilla("Yankeezilla", DEFAULT);
}

// Info
static PzWindow *welcome()
{
	return pz_new_textview_window(
		"/opt/Zillae/TimeWalk/Launch/Welcome.txt");
}
static PzWindow *missing()
{
	return pz_new_textview_window(
		"/opt/Zillae/TimeWalk/Launch/Missing.txt");
}
static PzWindow *readme()
{
	return pz_new_textview_window(
		"/opt/Zillae/TimeWalk/ReadMe from Keripo.txt");
}

// Init
static void init_launch() 
{
	module = pz_register_module("TimeWalk", 0);
	path = "/opt/Zillae/TimeWalk/Launch/Launch.sh";
	
	// Group
	pz_menu_add_stub_group("/~TimeWalk", "#ZeroSlackr");
	
	// Info
	pz_menu_add_action_group("/~TimeWalk/#Welcome",			"#Info",		welcome);
	pz_menu_add_action_group("/~TimeWalk/Missing",			"#Info",		missing);
	pz_menu_add_action_group("/~TimeWalk/~ReadMe",			"#Info",		readme);
	
	// Non-standard
	pz_menu_add_action_group("/~TimeWalk/iPod-Desktop",		"Non-standard", iPod_Desktop);
	pz_menu_add_action_group("/~TimeWalk/Owen-OS",			"Non-standard", Owen_OS);
	pz_menu_add_action_group("/~TimeWalk/Margarelon",		"Non-standard", Margarelon);
	
	// PZ0 Specials
	pz_menu_add_action_group("/~TimeWalk/iTanks",			"PZ0 Specials",	iTanks);
	pz_menu_add_action_group("/~TimeWalk/MikModPodzilla",		"PZ0 Specials",	MikModPodzilla);
	pz_menu_add_action_group("/~TimeWalk/pdPod",			"PZ0 Specials",	pdPod);
	
	// PZ0 Features
	pz_menu_add_action_group("/~TimeWalk/Keyman (PZ0)",		"PZ0 Featured",	Keyman_pz0);
	pz_menu_add_action_group("/~TimeWalk/Keyman (Floyd)",		"PZ0 Featured",	Keyman_floyd);
	pz_menu_add_action_group("/~TimeWalk/NXSnake",			"PZ0 Featured",	NXSnake);
	pz_menu_add_action_group("/~TimeWalk/Othello",			"PZ0 Featured",	Othello);
	pz_menu_add_action_group("/~TimeWalk/Sudoku",			"PZ0 Featured",	Sudoku);
	pz_menu_add_action_group("/~TimeWalk/Tuxchess",			"PZ0 Featured",	Tuxchess);
	pz_menu_add_action_group("/~TimeWalk/Video-podzilla",		"PZ0 Featured",	Video_podzilla);
	
	// PZ0 Jonrelay's
	pz_menu_add_action_group("/~TimeWalk/podzilla-fbk",		"PZ0 Jonrelay",	podzilla_fbk);
	pz_menu_add_action_group("/~TimeWalk/podzilla-mc",		"PZ0 Jonrelay",	podzilla_mc);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti",		"PZ0 Jonrelay",	podzilla_ti);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti2",		"PZ0 Jonrelay",	podzilla_ti2);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti3",		"PZ0 Jonrelay",	podzilla_ti3);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti4",		"PZ0 Jonrelay",	podzilla_ti4);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti5",		"PZ0 Jonrelay",	podzilla_ti5);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti6",		"PZ0 Jonrelay",	podzilla_ti6);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti7",		"PZ0 Jonrelay",	podzilla_ti7);
	pz_menu_add_action_group("/~TimeWalk/podzilla-ti8",		"PZ0 Jonrelay",	podzilla_ti8);
	pz_menu_add_action_group("/~TimeWalk/relayzilla",		"PZ0 Jonrelay",	relayzilla);
	
	// PZ0 Translates
	pz_menu_add_action_group("/~TimeWalk/podzilla-Dutch",		"PZ0 Translations", podzilla_Dutch);
	pz_menu_add_action_group("/~TimeWalk/Jpodzilla",		"PZ0 Translations", Jpodzilla);
	
	// PZ0 Custom Builds
	pz_menu_add_action_group("/~TimeWalk/Funzilla",			"PZ0 Custom Builds", Funzilla);
	pz_menu_add_action_group("/~TimeWalk/Yankeezilla",		"PZ0 Custom Builds", Yankeezilla);

	// Sort
	pz_menu_sort("/~TimeWalk");
}

PZ_MOD_INIT (init_launch)
