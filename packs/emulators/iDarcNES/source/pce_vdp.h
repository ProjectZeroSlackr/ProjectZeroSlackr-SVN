/*
 * pce_vdp.h
 *
 * HuC6270/HuC6260 emulation.
 */

#ifndef PCE_VDP_H
#define PCE_VDP_H

/* $Id: pce_vdp.h,v 1.7 2000/02/19 23:28:45 nyef Exp $ */

typedef struct pce_vdp *pce_vdp;

unsigned char pce_vce_read(pce_vdp vdp, unsigned short addr);
void pce_vce_write(pce_vdp vdp, unsigned short addr, unsigned char data);
unsigned char pce_vdp_read(pce_vdp vdp, unsigned short addr);
void pce_vdp_write(pce_vdp vdp, unsigned short addr, unsigned char data);
int pce_vdp_periodic(pce_vdp vdp);

pce_vdp pce_vdp_create(void);

#endif /* PCE_VDP_H */

/*
 * $Log: pce_vdp.h,v $
 * Revision 1.7  2000/02/19 23:28:45  nyef
 * moved struct pce_vdp out to pce_vdp.c
 * added function prototypes for vdp access routines
 *
 * Revision 1.6  1999/05/23 15:19:32  nyef
 * fixed some v-scroll problems
 *
 * Revision 1.5  1999/03/03 02:47:03  nyef
 * added hooks for resolution control to struct pce_vdp
 *
 * Revision 1.4  1999/01/31 01:04:07  nyef
 * fixed palette_xlat array to be the correct size
 *
 * Revision 1.3  1999/01/31 00:16:14  nyef
 * added preliminary implimentation of palette ram
 *
 * Revision 1.2  1999/01/30 23:09:44  nyef
 * redesigned VDP I/O interface.
 *
 * Revision 1.1  1999/01/17 02:12:51  nyef
 * Initial revision
 *
 */
