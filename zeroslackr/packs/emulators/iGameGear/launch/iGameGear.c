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

static PzModule * module;

// Note: some of these are modified functions that are built
// into ZeroLauncher to save space, thus this module will
// only work on ZeroLauncher

// modules.c
extern void new_fastlaunch_window();

// browser.c 
extern int check_file_ext_2();
extern int check_file_ext_3();
extern void pz_exec_mod();

// filebrowser.c
extern TWindow *open_directory();

// Parameter options
static PzConfig * config;

#define FRAMESKIP_VALUE 1
#define DISPLAY_TYPE 2

static const char * frameskip_options[] = {"None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 0};
static const char * display_options[] = {"Centre", "Scale", "Stretch", 0};

static void save_settings()
{
	pz_save_config(config);
}

// File extension checker for browser

static int check_ext_match(const char* file) {
	return (check_file_ext_2(file, ".GG") || 
			check_file_ext_3(file, ".SMS") ||
			check_file_ext_3(file, ".ZIP")
			);
}

// File executer for browser
static void load_file(const char* file)
{
	// Set parameters
	char frameskip_value[4];
	char display_type[8];
	
	sprintf(frameskip_value, "%i", pz_get_int_setting(config, FRAMESKIP_VALUE));
	switch (pz_get_int_setting(config, DISPLAY_TYPE))
	{
		// Centre
		case 0: sprintf(display_type, "center"); break;
		// Scale
		case 1: sprintf(display_type, "scale"); break;
		// Stretch
		case 2: sprintf(display_type, "stretch"); break;
		// Default
		default: sprintf(display_type, "center"); break;
	}
	save_settings();
	
	// Note: syntax:
	// ch_dir, file_path, file, arg1-9
	pz_exec_mod( "/root/Packs/iGameGear",
			"/root/Packs/iGameGear/iGameGear",
			"iGameGear",
			file,
			"--fskip",
			frameskip_value,
			"--display",
			display_type,
			NULL,
			NULL,
			NULL,
			NULL
	);
}

// Rom browser
static PzWindow *browse_roms()
{
	return open_directory("/root/Files/iGameGear/Roms", "iGameGear Roms", 0);
}

// "FastLaunch" script launch
static void fastlaunch_window()
{
	new_fastlaunch_window(module);
}

static void init_launch() 
{
	// Registration
	module = pz_register_module("iGameGear", save_settings);
	config = pz_load_config(pz_module_get_cfgpath(module,"parameters.conf"));
	
	// Menus
	pz_menu_add_action("/Emulators/iGameGear/FastLaunch", fastlaunch_window);
	pz_menu_add_action("/Emulators/iGameGear/Roms", browse_roms);
	
	// File handling
	pz_browser_set_handler(check_ext_match, load_file);
	
	// Parameter defaults
	// frameskip of 4 and centre display
	if (!pz_get_setting(config, FRAMESKIP_VALUE)) pz_set_int_setting (config, FRAMESKIP_VALUE, 4);
	if (!pz_get_setting(config, DISPLAY_TYPE))  pz_set_int_setting (config, DISPLAY_TYPE, 0);	
	
	// Parameter options
	pz_menu_add_setting("/Emulators/iGameGear/Frameskip", FRAMESKIP_VALUE, config, frameskip_options);
	pz_menu_add_setting("/Emulators/iGameGear/Display", DISPLAY_TYPE, config, display_options);
	
}

PZ_MOD_INIT(init_launch)
