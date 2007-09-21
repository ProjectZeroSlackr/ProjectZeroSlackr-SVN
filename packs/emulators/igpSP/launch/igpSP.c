/*
 * Minimal module that selects a rom file using ZeroLauncher's modified file browser,
 * then executes it using ZeroLauncher's modified execution function.
 * This is used for launching applications that use roms/files as parameters.
 * The file browser menu handle checks the file extension using ZeroLauncher's
 * built-in extension checking functions, then executes it like above.
 * A "FastLaunch" function is also included for quickly launching the binary
 * with from a script file via the original "pz_exec" function.
 *
 * ~Keripo
 *
 */

#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

PzModule *module;

// Note: some of these are modified functions that are built
// into ZeroLauncher to save space, thus this module will
// only work on ZeroLauncher

// modules.c
extern void new_fastlaunch_window();

// browser.c 
extern int check_file_ext_3();
extern void pz_exec_mod();

// filebrowser.c
extern TWindow *open_directory();

// File extension checker for browser
static int check_ext_match(const char* file) {
	return (
		check_file_ext_3(file, ".GBA")
	);
}

// File executer for browser
static void load_file(const char* file)
{
	// Note: syntax:
	// ch_dir, file_path, file, arg1-9
	// Don't forget the commas!
	FILE *fp;
	if((fp=fopen("/root/Files/igpSP/Bios/gba_bios.bin", "r"))==NULL)
	{
		pz_message_title("No BIOS found!", "No \"gba_bios.bin\" found in \"/root/Files/igpSP/Bios/\".");
	} else {
		pz_exec_mod( "/root/Packs/igpSP",
				"/root/Packs/igpSP/igpSP",
				"igpSP",
				file,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
		);
	}
}

// Rom browser
static PzWindow *browse_roms()
{
	return open_directory("/root/Files/igpSP/Roms", "igpSP Roms", 0);
}

// "FastLaunch" script launch
static void fastlaunch_window()
{
	FILE *fp;
	if((fp=fopen("/root/Files/igpSP/Bios/gba_bios.bin", "r"))==NULL)
	{
		pz_message_title("No BIOS found!", "No \"gba_bios.bin\" found in \"/root/Files/igpSP/Bios/\".");
	} else {
		new_fastlaunch_window(module);
	}
}

static void init_launch() 
{
	// Registration
	module = pz_register_module("igpSP", 0);
	
	// Menus
	pz_menu_add_action("/Emulators/igpSP/FastLaunch", fastlaunch_window);
	pz_menu_add_action("/Emulators/igpSP/Roms", browse_roms);
	
	// File handling
	pz_browser_set_handler(check_ext_match, load_file);
}

PZ_MOD_INIT(init_launch)
