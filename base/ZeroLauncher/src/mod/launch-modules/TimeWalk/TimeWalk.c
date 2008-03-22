/*
 * Last updated: March 14, 2008
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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

extern void pz_exec();

#define DEFAULT "podzilla"

static PzModule *module;
static char path[256];

// Common

static PzWindow *exec_zilla(const char *folder, const char *binary)
{
	chdir(pz_module_get_datapath(module, folder));
	sprintf(path, "%s/%s", folder, binary);
	pz_exec(pz_module_get_datapath(module, path));
	return NULL;
}


// Non-standards

static PzWindow *iPod_Desktop()
{
	// Special since it needs nano-X started
	pz_exec(pz_module_get_datapath(module, "iPod-Desktop.sh"));
	return NULL;
}

static PzWindow *Owen_OS()
{
	return exec_zilla("Owen-OS", "owenipod");
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
	pz_message_title(
		"Welcome Part 1 of 7",
		"Welcome to TimeWalk! All you see here are "
		"historical zillae built back when iPodLinux "
		"was still young and new."
		);
	pz_message_title(
		"Welcome Part 2 of 7",
		"Some of these custom builds "
		"demonstrating PZ0 progress while others "
		"integrate unofficial features."
		);	
	pz_message_title(
		"Welcome Part 3 of 7",
		"A few of them are alternate interfaces "
		"separate from podzilla legacy."
		);
	pz_message_title(
		"Welcome Part 4 of 7",
		"Many of these are incomplete and probably "
		"WILL NOT WORK on most iPod models."
		);
	pz_message_title(
		"Welcome Part 5 of 7",
		"There are still a large number of historical zillae "
		"missing from this collection; please see \"Missing\" "
		"for the list."
		);
	pz_message_title(
		"Welcome Part 6 of 7",
		"The ZeroSlackr versions are all mostly hex edited "
		"in an attempt to be self-contained but are not "
		"all fully done."
		);
	pz_message_title(
		"Welcome Part 7 of 7",
		"If you find important paths that need "
		"changing, please contact me. "
		"Apart from that, enjoy! ~Keripo"
		);
	return NULL;
}

static PzWindow *missing()
{
	pz_message_title(
		"Missing Part 1 of 6",
		"Due to iPodLinux's long history of development, "
		"there are many zillae for which "
		"download links have long died. "
	);
	pz_message_title(
		"Missing Part 2 of 6",
		"Here is a list of missing zillae "
		"(if you happen to have "
		"any of them, please contact me): "
	);
	pz_message_title(
		"Missing Part 3 of 6",
		"iPodVX, Pokezilla, illzilla, PureZilla, "
		"king-zilla, multipodzilla."
	);
	pz_message_title(
		"Missing Part 4 of 6",
		"Here is a list of unconfirmed zillae "
		"(if you can confirm their past "
		"existance, please do): "
	);
	pz_message_title(
		"Missing Part 5 of 6",
		"vanillazilla, MarioZilla, Canuckzilla. "
	);
	pz_message_title(
		"Missing Part 6 of 6",
		"If you have/know  any of the previously "
		"listed zillae, please contact me so "
		"TimeWalk can be completed ; ) ~Keripo"
	);
	return NULL;
}

// Init!!!

static void init_launch() 
{
	module = pz_register_module ("TimeWalk", 0);
	
	// Group!
	pz_menu_add_stub_group ("/~TimeWalk", "#ZeroSlackr");
	
	// Info
	pz_menu_add_action_group ("/~TimeWalk/#Welcome",	"#Info",	welcome);
	pz_menu_add_action_group ("/~TimeWalk/Missing",		"#Info",	missing);
	
	// Non-standard
	pz_menu_add_action_group ("/~TimeWalk/iPod-Desktop",	"Non-standard", iPod_Desktop);
	pz_menu_add_action_group ("/~TimeWalk/Owen-OS",		"Non-standard", Owen_OS);
	pz_menu_add_action_group ("/~TimeWalk/Margarelon",	"Non-standard", Margarelon);
	
	// PZ0 Specials
	pz_menu_add_action_group ("/~TimeWalk/iTanks",		"PZ0 Specials",	iTanks);
	pz_menu_add_action_group ("/~TimeWalk/MikModPodzilla",	"PZ0 Specials",	MikModPodzilla);
	pz_menu_add_action_group ("/~TimeWalk/pdPod",		"PZ0 Specials",	pdPod);
	
	// PZ0 Features
	pz_menu_add_action_group ("/~TimeWalk/Keyman (PZ0)",	"PZ0 Features",	Keyman_pz0);
	pz_menu_add_action_group ("/~TimeWalk/Keyman (Floyd)",	"PZ0 Features",	Keyman_floyd);
	pz_menu_add_action_group ("/~TimeWalk/NXSnake",		"PZ0 Feature",	NXSnake);
	pz_menu_add_action_group ("/~TimeWalk/Othello",		"PZ0 Feature",	Othello);
	pz_menu_add_action_group ("/~TimeWalk/Sudoku",		"PZ0 Feature",	Sudoku);
	pz_menu_add_action_group ("/~TimeWalk/Tuxchess",	"PZ0 Feature",	Tuxchess);
	pz_menu_add_action_group ("/~TimeWalk/Video-podzilla",	"PZ0 Feature",	Video_podzilla);
	
	// PZ0 Jonrelay's
	pz_menu_add_action_group ("/~TimeWalk/podzilla-fbk",	"PZ0 Jonrelay",	podzilla_fbk);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-mc",	"PZ0 Jonrelay",	podzilla_mc);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti",	"PZ0 Jonrelay",	podzilla_ti);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti2",	"PZ0 Jonrelay",	podzilla_ti2);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti3",	"PZ0 Jonrelay",	podzilla_ti3);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti4",	"PZ0 Jonrelay",	podzilla_ti4);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti5",	"PZ0 Jonrelay",	podzilla_ti5);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti6",	"PZ0 Jonrelay",	podzilla_ti6);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti7",	"PZ0 Jonrelay",	podzilla_ti7);
	pz_menu_add_action_group ("/~TimeWalk/podzilla-ti8",	"PZ0 Jonrelay",	podzilla_ti8);
	pz_menu_add_action_group ("/~TimeWalk/relayzilla",	"PZ0 Jonrelay",	relayzilla);
	
	// PZ0 Translates
	pz_menu_add_action_group ("/~TimeWalk/podzilla-Dutch",	"PZ0 Translations", podzilla_Dutch);
	pz_menu_add_action_group ("/~TimeWalk/Jpodzilla",	"PZ0 Translations", Jpodzilla);
	
	// PZ0 Custom Builds
	pz_menu_add_action_group ("/~TimeWalk/Funzilla",	"PZ0 Custom Builds", Funzilla);
	pz_menu_add_action_group ("/~TimeWalk/Yankeezilla",	"PZ0 Custom Builds", Yankeezilla);

	// Sort!!
	pz_menu_sort("/~TimeWalk");
}

PZ_MOD_INIT (init_launch)
