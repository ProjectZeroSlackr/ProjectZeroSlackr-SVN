/*
 * nes_psg.h
 *
 * NES sound emulation
 */

/* $Id: nes_psg.h,v 1.6 2000/10/02 14:18:54 nyef Exp $ */

#ifndef NES_PSG_H
#define NES_PSG_H

#include "types.h"

void nes_psg_init(void);
void nes_psg_done(void);

void nes_psg_frame(void);

typedef void (*psg_writefunc)(u8 value);

extern psg_writefunc sound_regs[16];

void nes_psg_write_control(u8 value);

/*
 * nes_psg_quality is used to control the sound playback rate.
 * anything < 1 disables sound.
 * 1 is 22050 Hz.
 * 2 is 44100 Hz.
 * anything > 2 is 44100 Hz.
 * these values subject to change without notice.
 */
extern int nes_psg_quality;

#endif /* NES_PSG_H */

/*
 * $Log: nes_psg.h,v $
 * Revision 1.6  2000/10/02 14:18:54  nyef
 * rewrote interface
 *
 * Revision 1.5  2000/05/07 02:12:31  nyef
 * added "extern" to some variable declarations
 *
 * Revision 1.4  1999/02/14 18:28:18  nyef
 * added a function pointer array for writing the sound regs
 * added a function prototype for writing the control reg
 *
 * Revision 1.3  1998/12/06 04:18:22  nyef
 * added preliminary sound quality (playback rate) support.
 *
 * Revision 1.2  1998/08/30 20:36:42  nyef
 * added vsync hook procedure.
 *
 * Revision 1.1  1998/08/22 00:24:40  nyef
 * Initial revision
 *
 */
