/*
 * tool.c
 *
 * a program to aide in hacking FF3j
 */

/* $Id: tool.c,v 1.32 2001/02/27 04:18:06 nyef Exp $ */

#include <stdio.h>
#include <stdlib.h>
/* #include <unistd.h> */
/* #include <fcntl.h> */
/* #include <sys/stat.h> */
#include <ctype.h>
#include <string.h>

#include "machdep.h"
#include "ui.h"
#include "video.h"
#include "tool.h"
#include "system.h"

unsigned long system_flags;

rom_file read_romimage(char *filename)
{
    FILE *file;
    int numread;
    int error;
    rom_file retval;

    file = NULL;
    
    retval = malloc(sizeof(*retval));

    if (retval) {
	retval->filename = NULL;
	retval->data = NULL;
	error = 0;
    } else {
	error = 1;
    }
    
    if (!error) {
	file = fopen(filename, "rb");
	error = !file;
    }
  
    if (!error) {
	/* FIXME: ignoring the return value from fseek() isn't good */
	fseek(file, 0, SEEK_END);
	retval->size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	retval->filename = strdup(filename);
	retval->data = malloc(retval->size);
	error = !(retval->data && retval->filename);
    }

    if (!error) {
	numread = fread(retval->data, 1, retval->size, file);
	error = (numread != retval->size);
    }

    if (error && retval) {
	if (retval->filename) {
	    free(retval->filename);
	}
	if (retval->data) {
	    free(retval->data);
	}
	free(retval);
	retval = NULL;
    }

    if (file) {
	fclose(file);
    }
    
    return retval;
}

void free_romfile(rom_file romfile)
{
    if (!romfile) {
	deb_printf("free_romfile(): warning: NULL romfile.\n");
	return;
    }

    if (romfile->filename) {
	free(romfile->filename);
    }
    if (romfile->data) {
	free(romfile->data);
    }
    free(romfile);
}

char *get_battery_filename(const char *filename)
{
    char *retval;
    int length;
    int last_period;

    last_period = 0;
    for (length = 0; filename[length]; length++) {
	if (filename[length] == '.') {
	    last_period = length;
	}
    }

    if (last_period == 0) {
	last_period = length;
    }

    retval = malloc(last_period + 4);
    if (!retval) {
	return NULL;
    }

    strncpy(retval, filename, last_period);

    retval[last_period++] = '.';
    retval[last_period++] = 's';
    retval[last_period++] = 'a';
    retval[last_period++] = 'v';
    retval[last_period++] = '\0';
    
    return retval;
}

rom_file init_battery_file(rom_file romfile, int size)
{
    FILE *file;
    rom_file retval;

    file = NULL;
    
    retval = malloc(sizeof(*retval));

    if (!retval) {
	return NULL;
    }

    retval->filename = get_battery_filename(romfile->filename);
    
    retval->data = malloc(size);
    retval->size = size;

    /* FIXME: retval->data and retval->filename can both be NULL right now */

    file = fopen(retval->filename, "rb");

    if (!file) {
	/* No file? just return an empty buffer */
	return retval;
    }

    /* FIXME: error handling? */
    fread(retval->data, 1, retval->size, file);

    fclose(file);
    
    return retval;
}

void save_battery_file(rom_file batteryfile)
{
    FILE *batfile;
    
    deb_printf("attempting to save battery file.\n");
    
    /* FIXME: error checking might be nice */
    
    batfile = fopen(batteryfile->filename, "wb");
    
    if (batfile != NULL) {
	fwrite(batteryfile->data, batteryfile->size, 1, batfile);
	fclose(batfile);
    } else {
	deb_printf("could not open battery file.\n");
    }
}

struct {
    int type;
    char *file_suffix;
} system_detect[] = {
    {ST_NES,       ".nes"},
    {ST_NES,       ".fds"},
    {ST_MASTERSYS, ".sms"},
    {ST_GAMEGEAR,  ".gg"},
    {ST_COLECO,    ".col"},
#if 0
    {ST_GAMEBOY,   ".gb"},
#endif
    {ST_GENESIS,   ".smd"},
    {ST_GENESIS,   ".bin"},
    {ST_PCENGINE,  ".pce"},
    {ST_SG1000,    ".sg"},
    {ST_SC3000,    ".sc"},
    {ST_NONE, NULL}, /* Must be last entry */
};

int guess_system(rom_file romfile)
{
    int i;
    int len_filename;
    int len_suffix;
    int string_offset;

    len_filename = strlen(romfile->filename);

    for (i = 0; system_detect[i].type != ST_NONE; i++) {
	len_suffix = strlen(system_detect[i].file_suffix);
	string_offset = len_filename - len_suffix;
	if (string_offset < 0) {
	    continue;
	}
	if (!strcasecmp(romfile->filename + string_offset, system_detect[i].file_suffix)) {
	    return system_detect[i].type;
	}
    }
    return ST_NONE;
}

typedef void (*sysrun_t)(rom_file romfile);

struct {
    int type;
    sysrun_t activate;
} systems[] = {
    {ST_NES,       nes_run},
    {ST_PCENGINE,  pce_run},
    {ST_MASTERSYS, sms_run},
    {ST_GAMEGEAR,  gg_run},
/*     {ST_GENESIS,   genesis_run}, */
    {ST_COLECO,    cv_run},
    {ST_SG1000,    sg1k_run},
    {ST_SC3000,    sc3k_run},
    {ST_APPLE2,    apple2_run},
    {ST_MSX,       msx_run},
    {ST_NONE,      NULL}, /* Nust be last entry */
};

void activate_system(int system_type, rom_file romfile)
{
    int i;

    for (i = 0; systems[i].type != ST_NONE; i++) {
	if (systems[i].type == system_type) {
	    systems[i].activate(romfile);
	    return;
	}
    }

    deb_printf("unknown system type.\n");
}

struct {
    int type;
    char *name;
} system_names[] = {
    {ST_NES,       "nes"},
    {ST_NES,       "nintendo"},
    {ST_MASTERSYS, "sms"},
    {ST_MASTERSYS, "mastersystem"},
    {ST_GAMEGEAR,  "gg"},
    {ST_GAMEGEAR,  "gamegear"},
    {ST_COLECO,    "coleco"},
    {ST_COLECO,    "colecovision"},
    {ST_COLECO,    "cv"},
#if 0
    {ST_GAMEBOY,   "gameboy"},
    {ST_GAMEBOY,   "gb"},
#endif
    {ST_GENESIS,   "genesis"},
    {ST_GENESIS,   "megadrive"},
    {ST_GENESIS,   "md"},
    {ST_PCENGINE,  "pce"},
    {ST_PCENGINE,  "pc-engine"},
    {ST_PCENGINE,  "pcengine"},
    {ST_PCENGINE,  "tg16"},
    {ST_PCENGINE,  "turbografx"},
    {ST_PCENGINE,  "turbografx-16"},
    {ST_SG1000,    "sg"},
    {ST_SG1000,    "sg1000"},
    {ST_SG1000,    "sg1k"},
    {ST_SC3000,    "sc"},
    {ST_SC3000,    "sc3000"},
    {ST_SC3000,    "sc3k"},
    {ST_APPLE2,    "apple2"},
    {ST_APPLE2,    "apple"},
    {ST_APPLE2,    "a2"},
    {ST_MSX,       "msx"},
    {ST_MSX,       "msx1"},
    {ST_NONE, NULL}, /* Must be last entry */
};

int parse_system_name(char *name)
{
    int i;

    for (i = 0; system_names[i].type != ST_NONE; i++) {
	if (!strcasecmp(system_names[i].name, name)) {
	    return system_names[i].type;
	}
    }

    return ST_NONE;
}

/*
 * $Log: tool.c,v $
 * Revision 1.32  2001/02/27 04:18:06  nyef
 * disabled genesis support
 *
 * Revision 1.31  2000/11/25 15:01:39  nyef
 * added activation entries for the MSX
 *
 * Revision 1.30  2000/11/23 16:23:51  nyef
 * added detect and activation entries for the SC3000
 *
 * Revision 1.29  2000/09/24 00:44:11  nyef
 * added some error trapping code to free_romfile()
 *
 * Revision 1.28  2000/09/23 02:57:47  nyef
 * added a function to free rom_files
 *
 * Revision 1.27  2000/05/06 21:50:17  nyef
 * fixed to compile with C++-fixed tool.h
 *
 * Revision 1.26  2000/03/06 01:26:51  nyef
 * added parse_system_name() to convert a string to a system_type
 *
 * Revision 1.25  2000/02/15 03:40:24  nyef
 * rewrote the battery file handling to not use the cheap hack it was before
 *
 * Revision 1.24  2000/02/15 03:22:04  nyef
 * removed the old implementation of guess_system()
 *
 * Revision 1.23  2000/01/01 04:12:05  nyef
 * added activation entry for the Apple ][
 *
 * Revision 1.22  1999/12/07 00:52:58  nyef
 * made the cheap hack used for the battery filename endian-independant
 *
 * Revision 1.21  1999/12/06 00:33:31  nyef
 * added battery file support
 *
 * Revision 1.20  1999/12/05 00:47:05  nyef
 * cleaned up a bit I missed when adding the new type system
 *
 * Revision 1.19  1999/11/28 17:47:01  nyef
 * fixed type in new system_guess() implementation ('smd', not 'smb')
 *
 * Revision 1.18  1999/11/27 20:09:01  nyef
 * added detect and activation entries for the SG1000
 *
 * Revision 1.17  1999/11/27 18:59:15  nyef
 * built a new version of guess_system() that detects based on filenames
 *
 * Revision 1.16  1999/11/27 18:34:04  nyef
 * added gamegear support to activate_system()
 *
 * Revision 1.15  1999/11/20 05:26:35  nyef
 * redesigned rom loading interface
 * refactored to use standard C file routines in read_romimage()
 * refactored activate_system()
 *
 * Revision 1.14  1999/06/08 01:48:02  nyef
 * added colecovision support (the rom detect is really lame, though)
 *
 * Revision 1.13  1999/04/17 22:24:41  nyef
 * added genesis support.
 *
 * Revision 1.12  1999/02/20 20:12:07  nyef
 * added minimum size checks to SMS/GG detection (fixes small PCE roms)
 * changed most if not all strncmp()s to memcmp()s
 *
 * Revision 1.11  1999/02/14 06:18:19  nyef
 * added detection of FDS images
 *
 * Revision 1.10  1999/01/30 17:53:02  nyef
 * reduced the 0x4000s in the genesis/pce detect to 0x2000 (this doesn't
 * affect the genesis detect much, and fixes the pce detect to catch some
 * roms produced with the MagicKit assembler).
 *
 * Revision 1.9  1999/01/01 06:26:50  nyef
 * added support for Master System.
 *
 * Revision 1.8  1998/12/21 02:58:59  nyef
 * added char * romfilename to save the name of the rom file.
 *
 * Revision 1.7  1998/10/11 20:34:33  nyef
 * added SMS/GG detection to read_romimage()
 * added preliminary GB detection to read_romimage()
 *
 * Revision 1.6  1998/08/02 13:42:40  nyef
 * changed every printf to deb_printf for debug console.
 *
 * Revision 1.5  1998/08/01 01:07:44  nyef
 * fixed stupid mistake in activate_system().
 *
 * Revision 1.4  1998/08/01 01:03:20  nyef
 * moved main() out to the video interfaces. retained some of the logic in
 * activate_system().
 *
 * Revision 1.3  1998/07/12 23:07:07  nyef
 * added a romsize variable and fixed read_romimage() to set it
 *
 * Revision 1.2  1998/07/12 20:52:06  nyef
 * enabled PC-Engine emulation
 *
 * Revision 1.1  1998/07/11 22:19:09  nyef
 * Initial revision
 *
 */
