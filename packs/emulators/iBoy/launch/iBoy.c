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

// NOTE FOR IBOY
// Latest version (0.7.4) doesn't support rom files as command-line arguments
// Also don't have source to recompile
// When have time, ask for source. Till then, just a "Coming soon" popup
 
 
#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

PzModule *module;
//static ttk_menu_item browser_launch;

// Note: some of these are modified functions that are built
// into ZeroLauncher to save space, thus this module will
// only work on ZeroLauncher

// modules.c
extern void new_fastlaunch_window();

// browser.c 
//extern int check_file_ext_2();
//extern int check_file_ext_3();
extern void pz_exec_mod();

// filebrowser.c
//extern TWindow *open_directory();

// File extension checker for browser
/*
int check_ext_match(const char* file) {
	return (
		check_file_ext_2(file, ".GB") || 
		check_file_ext_3(file, ".GBC") 
	);
}
*/

// File executer for browser
/*
void load_file(const char* file)
{
	// Note: syntax:
	// ch_dir, file_path, file, arg1-9
	pz_exec_mod( "/root/Packs/iBoy",
			"/root/Packs/iBoy/iBoy",
			"iBoy",
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
*/

// Rom browser
/*
PzWindow *browse_roms()
{
	return open_directory("/root/Files/iBoy/Roms", "iBoy Roms", 0);
}
*/


// "FastLaunch" script launch
static void fastlaunch_window()
{
	new_fastlaunch_window(module);
}

// Temporary pop-up; see above
static void dummy_browse_roms()
{
	pz_message("Sorry ; / Not implemented yet. Coming soon!");
}

static void init_launch() 
{
	// Registration
	module = pz_register_module("iBoy", 0);
	
	// Menus
	pz_menu_add_action("/Emulators/iBoy/FastLaunch", fastlaunch_window);
	//pz_menu_add_action("/Emulators/iBoy/Roms", browse_roms);
	pz_menu_add_action("/Emulators/iBoy/Roms", dummy_browse_roms);
	
	// File handling
	/*
	pz_browser_set_handler(check_ext_match, load_file);
	*/
}

PZ_MOD_INIT(init_launch)
