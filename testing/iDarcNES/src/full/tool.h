/*
 * tool.h
 *
 * various definitions
 */

/* $Id: tool.h,v 1.14 2000/09/23 02:57:42 nyef Exp $ */

#ifndef TOOL_H
#define TOOL_H

extern unsigned long system_flags;
#define F_NONE    0x00000000
#define F_UNIMPL  0x00000001
#define F_BREAK   0x00000002
#define F_QUIT    0x00000004
#define F_GUI     0x00000008

typedef struct tag_rom_file {
    char *filename;
    unsigned char *data;
    int size;
} *rom_file;

rom_file read_romimage(char *filename);
void free_romfile(rom_file romfile);

void activate_system(int system_type, rom_file romfile);
int guess_system(rom_file romfile);
int parse_system_name(char *name);

rom_file init_battery_file(rom_file romfile, int size);
void save_battery_file(rom_file batteryfile);

#endif /* TOOL_H */

/*
 * $Log: tool.h,v $
 * Revision 1.14  2000/09/23 02:57:42  nyef
 * added a function to free rom_files
 *
 * Revision 1.13  2000/09/16 22:14:02  nyef
 * moved read_* macros out to nes.h
 *
 * Revision 1.12  2000/05/29 19:54:01  nyef
 * added a struct name to rom_file (tag_rom_file) to work around some
 * linkage issues with C++ and anonymous structs
 *
 * Revision 1.11  2000/05/06 21:49:45  nyef
 * fixed to compile on C++ compilers
 *
 * Revision 1.10  2000/03/06 01:28:03  nyef
 * added parse_system_name() to convert a string to a system_type
 * fixed include guards to not use ANSI C reserved identifiers.
 *
 * Revision 1.9  1999/12/06 00:33:21  nyef
 * added new battery file interface
 *
 * Revision 1.8  1999/12/04 04:34:00  nyef
 * removed inclusion of emu6502.h
 *
 * Revision 1.7  1999/11/20 05:26:42  nyef
 * redesigned rom loading interface
 *
 * Revision 1.6  1998/12/31 03:52:47  nyef
 * added macro read_long.
 *
 * Revision 1.5  1998/12/21 02:59:04  nyef
 * added char * romfilename to save the name of the rom file.
 *
 * Revision 1.4  1998/11/17 00:02:18  nyef
 * expanded system flag (F_xxx) defines to 8 digits.
 * added system flag F_GUI for use by user interface code.
 *
 * Revision 1.3  1998/08/01 18:40:59  nyef
 * added prototypes for read_romimage() and activate_system().
 *
 * Revision 1.2  1998/07/12 23:08:03  nyef
 * added romsize definition
 *
 * Revision 1.1  1998/07/11 22:19:11  nyef
 * Initial revision
 *
 */
