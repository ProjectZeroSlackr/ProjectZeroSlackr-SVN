/*
 * mappers.h
 *
 * Mapper emulation defines
 */

/* $Id: mappers.h,v 1.9 2000/10/05 08:54:37 nyef Exp $ */

#ifndef MAPPERS_H
#define MAPPERS_H

#include "cal.h"
#include "nes.h"
#include "nes_ppu.h"
#include "types.h"

/* mapper interface definition */

typedef void (* mapwrite_t)(nes_mapper mapper, u16 address, u8 value);
typedef void (* maphsync_t)(nes_mapper mapper, cal_cpu cpu, int display_active);

struct nes_mapper {
    mapwrite_t write;
    maphsync_t hsync;
};

nes_mapper create_mapper(nes_ppu ppu, nes_rom romfile);
int mapper_supported(int mapper);

#endif /* MAPPERS_H */

/*
 * $Log: mappers.h,v $
 * Revision 1.9  2000/10/05 08:54:37  nyef
 * fixed the idempotency code to not violate ANSI quite so flagrantly
 *
 * Revision 1.8  2000/06/07 00:39:25  nyef
 * deleted all mention of vsync routines
 *
 * Revision 1.7  2000/02/19 19:55:44  nyef
 * moved nes_mapper typedef out to nes.h
 * changed to use types.h definitions
 *
 * Revision 1.6  1999/11/20 05:36:00  nyef
 * rebuilt romfile handling
 *
 * Revision 1.5  1999/11/14 06:59:55  nyef
 * changed to use new mapper interface
 *
 * Revision 1.4  1999/10/21 23:35:53  nyef
 * changed to use new mapper hblank interface
 *
 * Revision 1.3  1998/12/15 01:50:29  nyef
 * changed mapper hsync and vsync routines to conform to new spec.
 *
 * Revision 1.2  1998/08/23 22:42:12  nyef
 * added hooks for calling mappers on vsync and hsync.
 *
 * Revision 1.1  1998/07/11 22:18:30  nyef
 * Initial revision
 *
 */
