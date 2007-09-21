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

// For the directory check
#include <dirent.h>

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

static int sound = 1;

// File extension checker for browser
// Note: There is a check for iGameGear since iGameGear
// handles SMS emulation better than iDarcNES

static int check_ext_match(const char* file) {
	if (check_file_ext_3(file, ".NES") == 1) {
		sound = 1;
		return 1;
	} else if (opendir("/root/Packs/iGameGear") == NULL && check_file_ext_3(file, ".SMS") == 1) {
		// Sega Master System emu's sound doesn't work at the moment
		sound = 0;
		return 1;
	} else {
		sound = 1;
		return 0;
	}
}

// File executer for browser
static void load_file(const char* file)
{
	// Note: syntax:
	// ch_dir, file_path, file, arg1-9
	if (sound == 1) {
		pz_exec_mod( "/root/Packs/iDarcNES",
				"/root/Packs/iDarcNES/iDarcNES",
				"iDarcNES",
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
	} else {
		pz_exec_mod( "/root/Packs/iDarcNES",
				"/root/Packs/iDarcNES/iDarcNES",
				"iDarcNES",
				file,
				"--nosound",
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
	return open_directory("/root/Files/iDarcNES/Roms", "iDarcNES Roms", 0);
}

// "FastLaunch" script launch
static void fastlaunch_window()
{
	new_fastlaunch_window(module);
}

static void init_launch() 
{
	// Registration
	module = pz_register_module("iDarcNES", 0);
	
	// Menus
	pz_menu_add_action("/Emulators/iDarcNES/FastLaunch", fastlaunch_window);
	pz_menu_add_action("/Emulators/iDarcNES/Roms", browse_roms);
	
	// File handling
	pz_browser_set_handler(check_ext_match, load_file);
}

PZ_MOD_INIT(init_launch)
