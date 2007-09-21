/*
 * apple2_vdp.h
 *
 * emulation of the Apple ][ video display
 */

/* $Id: apple2_vdp.h,v 1.1 2000/01/01 04:12:27 nyef Exp $ */

#ifndef APPLE2_VDP_H
#define APPLE2_VDP_H

#include "types.h"

typedef struct apple2_vdp *apple2_vdp;

void a2v_io(apple2_vdp vdp, int addr);

void a2v_periodic(apple2_vdp vdp);

apple2_vdp a2v_init(u8 *system_memory);

#endif /* APPLE2_VDP_H */

/*
 * $Log: apple2_vdp.h,v $
 * Revision 1.1  2000/01/01 04:12:27  nyef
 * Initial revision
 *
 */
