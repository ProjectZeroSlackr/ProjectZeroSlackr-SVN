/*
 * Minimal module that uses ZeroLauncher's built-in "FastLaunch" function that executes
 * a script file via the original "pz_exec" function.
 * 
 * ~Keripo
 *
 */

#include "pz.h"

static PzModule *module;

// modules.c
extern void new_fastlaunch_window();

// "FastLaunch" script launch
static void fastlaunch_window()
{
	new_fastlaunch_window(module);
}

static void init_launch() 
{
    module = pz_register_module ("Podzilla0-Lite", 0);
    pz_menu_add_action ("/Zillae/Podzilla0-Lite", fastlaunch_window);
}

PZ_MOD_INIT (init_launch)
