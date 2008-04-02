/*
 * sms9918.h
 *
 * sms custom vdp emulation.
 */

#ifndef SMS9918_H
#define SMS9918_H

/* $Id: sms9918.h,v 1.11 1999/12/07 02:05:55 nyef Exp $ */

#include "types.h"

typedef struct sms9918 *sms9918;

u8 sms9918_readport0(sms9918 vdp);
u8 sms9918_readport1(sms9918 vdp);
u8 sms9918_readscanline(sms9918 vdp);

void sms9918_writeport0(sms9918 vdp, u8 data);
void sms9918_writeport1(sms9918 vdp, u8 data);

int sms9918_periodic(sms9918 vdp);

sms9918 sms9918_create(int is_gamegear);

#endif /* SMS9918_H */

/*
 * $Log: sms9918.h,v $
 * Revision 1.11  1999/12/07 02:05:55  nyef
 * fixed a couple spots missed in the type system conversion
 *
 * Revision 1.10  1999/12/07 02:02:15  nyef
 * changed to use new types interface
 *
 * Revision 1.9  1999/11/26 16:42:01  nyef
 * published the actual routine names for what was hiding behind procpointers
 * moved the vdp data structure out to sms9918.c
 *
 * Revision 1.8  1999/11/26 15:50:33  nyef
 * added some variables to support some strength reductions in the renderer
 *
 * Revision 1.7  1999/03/12 02:12:22  nyef
 * added is_gamegear parameter to sms9918_create()
 *
 * Revision 1.6  1999/03/12 01:42:47  nyef
 * doubled palette memory size for GameGear
 *
 * Revision 1.5  1999/01/11 02:24:35  nyef
 * added palette_xlat[] to struct sms9918 for new blitters.
 *
 * Revision 1.4  1999/01/10 00:02:01  nyef
 * fixed cur_scanline to be proper length.
 *
 * Revision 1.3  1999/01/09 23:10:00  nyef
 * added support for reading the current scanline.
 *
 * Revision 1.2  1999/01/08 02:24:43  nyef
 * added linecounter for line interrupts.
 *
 * Revision 1.1  1999/01/07 03:17:15  nyef
 * Initial revision
 *
 */
