/*
 * snd.h
 *
 * os-dependant sound routines
 */

/* $Id: snd.h,v 1.2 2000/01/01 02:38:11 nyef Exp $ */

#ifndef SND_H
#define SND_H

#include "types.h"

void snd_init(void);
int snd_open(int samples_per_sync, int sample_rate);
void snd_close(void);
void snd_output_4_waves(int samples, u8 *wave1, u8 *wave2, u8 *wave3, u8 *wave4);

#endif /* SND_H */

/*
 * $Log: snd.h,v $
 * Revision 1.2  2000/01/01 02:38:11  nyef
 * changed to use new types interface
 *
 * Revision 1.1  1999/10/31 02:37:52  nyef
 * Initial revision
 *
 */
