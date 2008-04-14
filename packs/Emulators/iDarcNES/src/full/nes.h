/*
 * nes.h
 *
 * NES specific header.
 */

/* $Id: nes.h,v 1.13 2000/10/05 08:51:02 nyef Exp $ */

#ifndef NES_H
#define NES_H

#include "tool.h"
#include "types.h"

/* direct access to the NES PRG ROM bank handling for FDS */
extern u8 *bank_8;     /* Rom Page */
extern u8 *bank_A;     /* Rom Page */
extern u8 *bank_C;     /* Rom Page */
extern u8 *bank_E;     /* Rom Page */

/* PRG memory mapping */
void nesprg_map_4k(int bank, int page);
void nesprg_map_8k(int bank, int page);
void nesprg_map_16k(int bank, int page);
void nesprg_map_32k(int bank, int page);

typedef struct nes_mapper *nes_mapper;

typedef u8 (*readhook_t)(nes_mapper mapper, u16 address);
typedef void (*writehook_t)(nes_mapper mapper, u16 address, u8 data);

extern readhook_t nes_bank4_read_hook;
extern readhook_t nes_bank6_read_hook;
extern writehook_t nes_bank4_write_hook;
extern writehook_t nes_bank6_write_hook;

typedef struct nes_rom {
    rom_file file;
    u8 *prg_data;
    u8 *chr_data;
    int prg_size;
    int chr_size;
    int mapper;
    int uses_battery;
    int mirror_vertical;
} *nes_rom;

#endif /* NES_H */

/*
 * $Log: nes.h,v $
 * Revision 1.13  2000/10/05 08:51:02  nyef
 * removed as many obsolete internal interface definitions as possible
 *
 * Revision 1.12  2000/10/05 08:28:35  nyef
 * fixed the idempotency code to not violate ANSI quite so flagrantly
 *
 * Revision 1.11  2000/10/05 06:53:28  nyef
 * added new PRG-ROM mapping interface
 *
 * Revision 1.10  2000/09/16 23:04:10  nyef
 * removed the read_* macros and the rom header interpreting macros
 *
 * Revision 1.9  2000/09/16 22:49:08  nyef
 * added mirror_vertical field to nes_rom structure
 *
 * Revision 1.8  2000/09/16 22:45:15  nyef
 * added a uses_battery field to the nes_rom structure
 *
 * Revision 1.7  2000/09/16 22:14:14  nyef
 * moved read_* macros in from tool.h
 *
 * Revision 1.6  2000/02/19 19:59:07  nyef
 * moved nes_mapper typedef in from mappers.h
 * changed to use definitions from typed.h
 * changed bank 4 and 6 read and write hooks for new mapper interface
 *
 * Revision 1.5  1999/11/20 05:33:35  nyef
 * fixed to work with new rom loading interface
 * rebuild rom file handling
 *
 * Revision 1.4  1999/04/17 20:08:57  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.3  1999/02/19 02:09:41  nyef
 * added hooks for reading and writing banks 4 and 6
 *
 * Revision 1.2  1998/12/31 03:53:14  nyef
 * added support for 8-bit mapper ids.
 *
 * Revision 1.1  1998/07/11 22:18:06  nyef
 * Initial revision
 *
 */
