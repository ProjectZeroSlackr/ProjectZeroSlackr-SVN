/*
 * Last updated: March 17, 2008
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

extern void pz_execv();
extern const char *get_dirname();

static char romdir[256];
static PzModule *module;

static PzWindow *load_romset(const char *romset, const char *files[], int num)
{
	int i;
	char file[256];
	for (i = 0; i < num; i++) {
		snprintf(file, 256, "%s/%s/%s", romdir, romset, files[i]);
		if (!(access(file, F_OK) == 0)) {
			pz_error("Unable to load romset %s; missing file %s",
				romset, files[i]);
			return NULL;
		}
	}
	chdir((const char *)pz_module_get_datapath(module, ""));
	const char *const path = pz_module_get_datapath(module, "../iPodMAME");
	const char *const cmd[] = {"iPodMAME", romset, NULL};
	pz_execv(
		path,
		(char *const *)cmd
		);
	return NULL;
}

// Romsets and files directly from romlist.ipl.txt

static const char *pacman_files[] = {
	"pacman.6e",	"pacman.6f",	"pacman.6h",	"pacman.6j",
	"pacman.5e",	"pacman.5f"
};
static PzWindow *pacman()
{
	return load_romset("pacman", pacman_files, 6);
}

static const char *pacmod_files[] = {
	"6e.mod",	"pacman.6f",	"6h.mod",	"pacman.6j",
	"5e",		"5f"
};
static PzWindow *pacmod()
{
	return load_romset("pacmod", pacmod_files, 6);
}

static const char *matrxpac_files[] = {
	"matrxpac.6e",	"matrxpac.6f",	"matrxpac.6h",	"matrxpac.6j",
	"matrxpac.5e",	"matrxpac.5f"
};
static PzWindow *matrxpac()
{
	return load_romset("matrxpac", matrxpac_files, 6);
}

static const char *hellopac_files[] = {
	"hellopac.6e",	"hellopac.6f",	"hellopac.6h",	"hellopac.6j",
	"hellopac.5e",	"hellopac.5f"
};
static PzWindow *hellopac()
{
	return load_romset("hellopac", hellopac_files, 6);
}

static const char *namcopac_files[] = {
	"namcopac.6e",	"namcopac.6f",	"namcopac.6h",	"namcopac.6j",
	"namcopac.5e",	"namcopac.5f"
};
static PzWindow *namcopac()
{
	return load_romset("namcopac", namcopac_files, 6);
}

static const char *hangly_files[] = {
	"hangly.6e",	"hangly.6f",	"hangly.6h",	"hangly.6j",
	"hangly.5e",	"hangly.5f"
};
static PzWindow *hangly()
{
	return load_romset("hangly", hangly_files, 6);
}

static const char *puckman_files[] = {
	"namcopac.6e",	"namcopac.6f",	"namcopac.6h",	"namcopac.6j",
	"pacman.5e",	"pacman.5f"
};
static PzWindow *puckman()
{
	return load_romset("puckman", puckman_files, 6);
}

static const char *piranha_files[] = {
	"pr1.cpu",	"pr2.cpu",	"pr3.cpu",	"pr4.cpu",
	"pr5.cpu",	"pr7.cpu",	"pr6.cpu",	"pr8.cpu"
};
static PzWindow *piranha()
{
	return load_romset("piranha", piranha_files, 8);
}

static const char *mspacman_files[] = {
	"boot1",	"boot2",	"boot3",	"boot4",
	"boot5",	"boot6",	"5e",		"5f"
};
static PzWindow *mspacman()
{
	return load_romset("mspacman", mspacman_files, 8);
}
static PzWindow *mspacmab()
{
	return load_romset("mspacmab", mspacman_files, 8);
}
static PzWindow *aarmada()
{
	return load_romset("aarmada", mspacman_files, 8);
}

static const char *crush_files[] = {
	"CR1",		"CR5",		"CR2",		"CR6",
	"CR3",		"CR7",		"CR4",		"CR8",
	"CRA",		"CRC",		"CRB",		"CRD"
};
static PzWindow *crush()
{
	return load_romset("crush", crush_files, 12);
}

static const char *pengo_files[] = {
	"pengopop.u8",	"pengopop.u7",	"pengopop.u15",	"pengopop.u14",
	"pengopop.u21",	"pengopop.u20",	"pengopop.u32",	"pengopop.u31",
	"pengopop.u92",	"pengopop.105"
};
static PzWindow *pengo()
{
	return load_romset("pengo", pengo_files, 10);
}

static const char *penta_files[] = {
	"penta.u8",	"penta.u7",	"penta.u15",	"penta.u14",
	"penta.u21",	"penta.u20",	"penta.u32",	"penta.u31",
	"penta.u92",	"penta.105"
};
static PzWindow *penta()
{
	return load_romset("penta", penta_files, 10);
}

static const char *pengo2u_files[] = {
	"pengo.u8",	"pengo.u7",	"pengo.u15",	"pengo.u14",
	"ep5124.21",	"pengo.u20",	"ep5126.32",	"pengo.u31",
	"ep1640.92",	"ep1695.105"
};
static PzWindow *pengo2u()
{
	return load_romset("pengo2u", pengo2u_files, 10);
}

static const char *mspac2_files[] = {
	"0000.bin",	"8000.bin",	"5e",		"5f"
};
static PzWindow *mspac2()
{
	return load_romset("mspac2", mspac2_files, 4);
}

static const char *nibbler_files[] = {
	"g-0960-52.ic12", "g-0960-48.ic7",  "g-0960-49.ic8",  "g-0960-50.ic9",
	"g-0960-51.ic10", "g-0960-53.ic14", "g-0960-54.ic15", "g-0960-55.ic16",
	"g-0960-56.ic17", "g-0960-53.ic14", "g-0960-57.ic50", "g-0960-58.ic51"
};
static PzWindow *nibbler()
{
	return load_romset("nibbler", nibbler_files, 12);
}

static const char *fantasy_files[] = {
	"5.12",		"1.7",		"2.8",		"3.9",
	"4.10",		"ic14.cpu",	"ic15.cpu",	"8.16",
	"9.17",		"ic14.cpu",	"fs10ic50.bin",	"fs11ic51.bin"
};
static PzWindow *fantasy()
{
	return load_romset("fantasy", fantasy_files, 12);
}

static const char *zaxxon_files[] = {
	"zaxxon.3",	"zaxxon.2",	"zaxxon.1",	"zaxxon.15",
	"zaxxon.14",	"zaxxon.13",	"zaxxon.12",	"zaxxon.11",
	"zaxxon.6",	"zaxxon.5",	"zaxxon.4",	"zaxxon.8",
	"zaxxon.7",	"zaxxon.10",	"zaxxon.9"
};
static PzWindow *zaxxon()
{
	return load_romset("zaxxon", zaxxon_files, 15);
}

static const char *congo_files[] = {
	"congo1.bin",	"congo2.bin",	"congo3.bin",	"congo4.bin",
	"congo5.bin",	"congo16.bin",	"congo15.bin",	"congo12.bin",
	"congo14.bin",	"congo11.bin",	"congo13.bin",	"congo10.bin",
	"congo9.bin",	"congo8.bin",	"congo6.bin",	"congo6.bin",
	"congo7.bin",	"congo7.bin"
};
static PzWindow *congo()
{
	return load_romset("congo", congo_files, 18);
}

static const char *centiped_files[] = {
	"centiped.307",	"centiped.308",	"centiped.309",	"centiped.310",
	"centiped.310",	"centiped.211",	"centiped.212"
};
static PzWindow *centiped()
{
	return load_romset("centiped", centiped_files, 7);
}

static const char *milliped_files[] = {
	"milliped.104",	"milliped.103",	"milliped.102",	"milliped.101",
	"milliped.101",	"milliped.106",	"milliped.107"
};
static PzWindow *milliped()
{
	return load_romset("milliped", milliped_files, 7);
}

static const char *frogger_files[] = {
	"frogger.ic5",	"frogger.ic6",	"frogger.ic7",	"frogger.ic8",
	"frogger.606",	"frogger.607",	"frogger.608",	"frogger.609",
	"frogger.610"
};
static PzWindow *frogger()
{
	return load_romset("frogger", frogger_files, 9);
}

static const char *frogsega_files[] = {
	"frogger.ic5",	"frogger.ic6",	"frogger.ic7",	"frogger.606",
	"frogger.607",	"frogger.608",	"frogger.609",	"frogger.610"
};
static PzWindow *frogsega()
{
	return load_romset("frogsega", frogsega_files, 8);
}

static const char *gberet_files[] = {
	"577l03.10c",	"577l02.8c",	"577l01.7c",	"577l07.3f",
        "577l06.5e",	"577l05.4e",	"577l08.4f",	"577l04.3e"
};
static PzWindow *gberet()
{
	return load_romset("gberet", gberet_files, 8);
}
static PzWindow *rushatck()
{
	return load_romset("rushatck", gberet_files, 8);
}

static const char *mrdo_files[] = {
	"a4-01.bin",	"c4-02.bin",	"e4-03.bin",	"f4-04.bin",
        "s8-09.bin",	"u8-10.bin",	"r8-08.bin",	"n8-07.bin",
        "h5-05.bin",	"k5-06.bin"
};
static PzWindow *mrdo()
{
	return load_romset("mrdo", mrdo_files, 10);
}

static const char *ladybug_files[] = {
	"lb1.cpu",	"lb2.cpu",	"lb3.cpu",	"lb4.cpu",
        "lb5.cpu",	"lb6.cpu",	"lb9.vid",	"lb10.vid",
        "lb8.cpu",	"lb7.cpu"
};
static PzWindow *ladybug()
{
	return load_romset("ladybug", ladybug_files, 8);
}

static PzWindow *fastlaunch()
{
	pz_exec(pz_module_get_datapath(module, "FastLaunch.sh"));
	return NULL;
}

static void init_launch() 
{
	module = pz_register_module("iPodMAME", 0);

	sprintf(romdir, "%s", pz_module_get_datapath(module, "../Roms"));
	
	pz_menu_add_stub_group("/Emulators/iPodMAME", "Arcade");
	pz_menu_add_action_group("/Emulators/iPodMAME/FastLaunch", "Launching", fastlaunch);

	pz_menu_add_action_group("/Emulators/iPodMAME/pacman", "Romsets", pacman);
	pz_menu_add_action_group("/Emulators/iPodMAME/pacmod", "Romsets", pacmod);
	pz_menu_add_action_group("/Emulators/iPodMAME/matrxpac", "Romsets", matrxpac);
	pz_menu_add_action_group("/Emulators/iPodMAME/hellopac", "Romsets", hellopac);
	pz_menu_add_action_group("/Emulators/iPodMAME/namcopac", "Romsets", namcopac);
	pz_menu_add_action_group("/Emulators/iPodMAME/hangly", "Romsets", hangly);
	pz_menu_add_action_group("/Emulators/iPodMAME/puckman", "Romsets", puckman);
	pz_menu_add_action_group("/Emulators/iPodMAME/piranha", "Romsets", piranha);
	pz_menu_add_action_group("/Emulators/iPodMAME/mspacman", "Romsets", mspacman);
	pz_menu_add_action_group("/Emulators/iPodMAME/mspacmab", "Romsets", mspacmab);
	pz_menu_add_action_group("/Emulators/iPodMAME/aarmada", "Romsets", aarmada);
	pz_menu_add_action_group("/Emulators/iPodMAME/crush", "Romsets", crush);
	pz_menu_add_action_group("/Emulators/iPodMAME/pengo", "Romsets", pengo);
	pz_menu_add_action_group("/Emulators/iPodMAME/penta", "Romsets", penta);
	pz_menu_add_action_group("/Emulators/iPodMAME/pengo2u", "Romsets", pengo2u);
	pz_menu_add_action_group("/Emulators/iPodMAME/mspac2", "Romsets", mspac2);
	pz_menu_add_action_group("/Emulators/iPodMAME/nibbler", "Romsets", nibbler);
	pz_menu_add_action_group("/Emulators/iPodMAME/fantasy", "Romsets", fantasy);
	pz_menu_add_action_group("/Emulators/iPodMAME/zaxxon", "Romsets", zaxxon);
	pz_menu_add_action_group("/Emulators/iPodMAME/congo", "Romsets", congo);
	pz_menu_add_action_group("/Emulators/iPodMAME/centiped", "Romsets", centiped);
	pz_menu_add_action_group("/Emulators/iPodMAME/milliped", "Romsets", milliped);
	pz_menu_add_action_group("/Emulators/iPodMAME/frogger", "Romsets", frogger);
	pz_menu_add_action_group("/Emulators/iPodMAME/frogsega", "Romsets", frogsega);
	pz_menu_add_action_group("/Emulators/iPodMAME/gberet", "Romsets", gberet);
	pz_menu_add_action_group("/Emulators/iPodMAME/rushatck", "Romsets", rushatck);
	pz_menu_add_action_group("/Emulators/iPodMAME/mrdo", "Romsets", mrdo);
	pz_menu_add_action_group("/Emulators/iPodMAME/ladybug", "Romsets", ladybug);

	pz_menu_sort("/Emulators/iPodMAME");
}

PZ_MOD_INIT(init_launch)
