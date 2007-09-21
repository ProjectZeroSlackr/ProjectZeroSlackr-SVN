/*
 * This is an additional header containing all modified file names or paths.
 * Any source file using strings or "define"s will refer to this file.
 * Just makes modifications nice and easy for me : )
 *
 * Any other modifications can be found by searching for "KERIPO" or "KERIPO MOD".
 * All modifications include the original lines, just commented out.
 * In addition, I added a variable in the Makefile that easily allows you to change
 * all the outputted file's name (instead of "podzilla, podzilla.o, etc.).
 *
 * Note that if you want to have the default scheme symlink somewhere else other
 * than "/usr/share/schemes/default.cs", you will have to manually hex edit the
 * final binary even if you make the modifications here. The same goes with the
 * default font list reading at "/usr/share/fonts/fonts.lst". This is a hard-coded
 * path that is part of TTK - unless you want to recompile TTK with the changed
 * lines every time, it's much easier to just hex-edit a binary. ; P
 *
 * ~Keripo
 *
 * Last updated: August 19, 2007 
 */

///////////////////////////
// Main
///////////////////////////

// String name
// "podzilla"
#define PZNAME "Podzilla2-Mod"

// Main title name
// "podzilla"
#define PZTITLE "Podzilla2-Mod"

// Main configuration file
// "/etc/podzilla/podzilla.conf
#define CONFIG_FILE "Conf/Podzilla2-Mod.conf"

// Menu configuration file (is this even used???)
// "/etc/podzilla/menu.conf"
#define MENUCONF_CONFIG_FILE "Conf/Podzilla2-Mod-Menu.conf"

// Oops error file
// "podzilla.oops"
#define PZOOPS "Misc/Podzilla2-Mod.oops"

// Message file
// "msg.inf"
#define PZMSG "Misc/Podzilla2-Mod-Msg.inf"


///////////////////////////
// Schemes
///////////////////////////

// Schemes directory
// "/usr/share/schemes/"
#define SCHEMESDIR "/usr/share/schemes/"
// Note: not re-defined since location is symlinked

// Default scheme
// "mono.cs"
#define DEFSCHEME "skywards.cs"

// Default symlinked scheme directory
// SCHEMESDIR
#define SYMLINKSCHEMEDIR "/sandbox/"

// Default scheme symlink
// "default.cs"
#define SYMLINKDEFSCHEME "pz2-mod-default.cs"
 
///////////////////////////
// Modules
///////////////////////////

// Modules load directory
// "/usr/lib"
#define MODULEDIR "/usr/lib"
// Note: not re-defined since location is symlinked

// Modules and packs extension
// ".mod.o"
#define MODULESEXTENSION ".mod.o"

// Modules info file
// "Module"
#define MODULE_INF_FILE "Module"

// Modules configuration main directory
// "/etc/podzilla"
#define CONFDIR "/sandbox"

// Modules configuration sub-directory
// "/modules"
#define SUBCONFDIR "/pz2mods"

