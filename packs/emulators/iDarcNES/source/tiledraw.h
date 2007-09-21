/*
 * tiledraw.h
 *
 * drawing routines for tilecached VDPs
 */

/* $Id: tiledraw.h,v 1.3 1999/11/23 03:21:56 nyef Exp $ */

#ifndef TILEDRAW_H
#define TILEDRAW_H
void _tiledraw_setup();
void tiledraw_8(unsigned char *cur_vbp, unsigned char **tiles, unsigned char **palettes, int num_tiles, int finescroll);

#endif /* TILEDRAW_H */

/*
 * $Log: tiledraw.h,v $
 * Revision 1.3  1999/11/23 03:21:56  nyef
 * changed to use new tiledraw interface
 *
 * Revision 1.2  1999/11/23 01:42:35  nyef
 * added file description comment (oops)
 *
 * Revision 1.1  1999/11/23 01:41:17  nyef
 * Initial revision
 *
 */
