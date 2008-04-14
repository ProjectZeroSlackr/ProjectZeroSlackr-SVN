/*
 * system.h
 *
 * definitions for system emulation control
 */

/* $Id: system.h,v 1.12 2000/11/25 15:02:00 nyef Exp $ */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "tool.h"

#define ST_NONE         0
#define ST_NES          1
#define ST_PCENGINE     2 /* also the CD versions */
#define ST_GENESIS      3
#define ST_MASTERSYS    4
#define ST_GAMEGEAR     5
#define ST_GAMEBOY      6
#define ST_SNES         7
#define ST_LYNX         8
#define ST_JAGUAR       9
#define ST_SEGACD      10
#define ST_APPLE2      11
#define ST_COLECO      12
#define ST_MSX         13
#define ST_SG1000      14
#define ST_SC3000      15
#define ST_MACINTOSH   20
#define ST_PLAYSTATION 30
#define ST_N64         31
#define ST_SATURN      32

void nes_run(rom_file romfile);
void pce_run(rom_file romfile);
void sms_run(rom_file romfile);
void gg_run(rom_file romfile);
void genesis_run(rom_file romfile);
void cv_run(rom_file romfile);
void sg1k_run(rom_file romfile);
void sc3k_run(rom_file romfile);
void apple2_run(rom_file romfile);
void msx_run(rom_file romfile);

#endif /* __SYSTEM_H__ */

/*
 * $Log: system.h,v $
 * Revision 1.12  2000/11/25 15:02:00  nyef
 * added prototype for msx_run()
 *
 * Revision 1.11  2000/11/23 16:23:03  nyef
 * added system define for SC3000
 * added prototype for sc3k_run()
 *
 * Revision 1.10  2000/05/29 19:28:35  nyef
 * removed disused definition of system_type
 *
 * Revision 1.9  2000/01/01 04:11:26  nyef
 * added prototype for apple2_run()
 *
 * Revision 1.8  1999/11/27 20:08:44  nyef
 * added system define for SG1000
 * added prototype for sg1k_run()
 *
 * Revision 1.7  1999/11/27 18:33:18  nyef
 * added a separate system type for the gamegear
 * added prototype for gg_run()
 *
 * Revision 1.6  1999/11/20 05:29:18  nyef
 * fixed to work with new rom loading interface
 *
 * Revision 1.5  1999/06/08 01:49:04  nyef
 * added prototype for cv_run()
 * moved mac, psx, n64, and saturn type codes up by 10
 * added codes for apple2, colecovision, and msx
 *
 * Revision 1.4  1999/04/17 22:24:15  nyef
 * added prototype for genesis_run().
 *
 * Revision 1.3  1999/01/01 06:27:13  nyef
 * added prototype for sms_run().
 *
 * Revision 1.2  1998/07/26 14:20:06  nyef
 * added system defines for Master System, GameBoy, SNES, Lynx, Jaguar,
 *   SegaCD, Macintosh, Playstation and N64.
 *
 * Revision 1.1  1998/07/11 22:19:03  nyef
 * Initial revision
 *
 */
