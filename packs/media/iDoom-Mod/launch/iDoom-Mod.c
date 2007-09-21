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
 
// Note: For iDoom, there are three different launching methods in treating
// the doom.wad (Doom I), doom1.wad (Shareware), and doom2.wad (Doom II) files.
// There is also a testing mode to try all three methods.

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

// Note: declared here to save space
const static char dir[22] = "/root/Packs/iDoom-Mod";
const static char bin[32] = "/root/Packs/iDoom-Mod/iDoom-Mod";
const static char x[10] = "iDoom-Mod";

// Default test launch method
static int launch_method = 3;

// File extension checker for browser
static int check_ext_match(const char* file) {
	return (check_file_ext_3(file, ".WAD"));
}

// File executer for browser
static void test_load_file(const char* file)
{
	// Try all three methods
	pz_exec_mod( dir,
			bin,
			x,
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
	pz_exec_mod( dir,
			bin,
			x,
			"",
			file,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
	);
	pz_exec_mod( dir,
			bin,
			x,
			"",
			"",
			file,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
	);
}

static void load_file(const char* file)
{	
	// Note: syntax:
	// ch_dir, file_path, file, arg1-9
	switch (launch_method) {
	case 0:
		// shareware as "doom1.wad"
		pz_exec_mod( dir,
				bin,
				x,
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
		break;
	case 1:
		// doom 1 as "doom.wad"
		pz_exec_mod( dir,
				bin,
				x,
				"",
				file,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
		);
		break;
	case 2:
		// doom 2 as "doom2.wad"
		pz_exec_mod( dir,
				bin,
				x,
				"",
				"",
				file,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
		);
		break;
	default:
		test_load_file(file);
		break;
	}
}

// Wad browser
static PzWindow *browse_wads_0()
{
	launch_method = 0;
	return open_directory("/root/Files/iDoom-Mod/Shareware", "Shareware Wads", 0);
}

static PzWindow *browse_wads_1()
{
	launch_method = 1;
	return open_directory("/root/Files/iDoom-Mod/DoomI", "Doom I Wads", 0);
}

static PzWindow *browse_wads_2()
{
	launch_method = 2;
	return open_directory("/root/Files/iDoom-Mod/DoomII", "Doom II Wads", 0);
}

static PzWindow *browse_wads_3()
{
	// None of choices, thus default
	launch_method = 3;
	return open_directory("/root/Files/iDoom-Mod/Testing", "Testing Wads", 0);
}

// "FastLaunch" script launch
static void fastlaunch_window()
{
	new_fastlaunch_window(module);
}

static void init_launch() 
{
	// Registration
	module = pz_register_module("iDoom-Mod", 0);
	
	// Menus
	pz_menu_add_action("/Media/Games/iDoom-Mod/FastLaunch", fastlaunch_window);
	pz_menu_add_action("/Media/Games/iDoom-Mod/Shareware", browse_wads_0);
	pz_menu_add_action("/Media/Games/iDoom-Mod/Doom I", browse_wads_1);
	pz_menu_add_action("/Media/Games/iDoom-Mod/Doom II", browse_wads_2);

	// Check if the Testing folder exists
	if (opendir("/root/Files/iDoom-Mod/Testing") != NULL) {
		pz_menu_add_action("/Media/Games/iDoom-Mod/Testing", browse_wads_3);
	}
	
	// File handling
	pz_browser_set_handler(check_ext_match, load_file);
}

PZ_MOD_INIT(init_launch)
