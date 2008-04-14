/*
 * palette.c
 *
 * generic palette code
 */

/* $Id: palette.c,v 1.1 2000/08/04 23:34:48 nyef Exp $ */

#include <stdlib.h>
#include "palette.h"
#include "types.h"
#include "video.h"

#ifndef PALETTE_DEPTH
#error PALETTE_DEPTH not defined, should be set by makefile
#endif

#define APPEND(a, b) a##b
#define XAPPEND(a, b) APPEND(a, b)
#define PALETTE_TYPE XAPPEND(u, PALETTE_DEPTH)
#define NEW_PALETTE_FUNCTION XAPPEND(new_palette_, PALETTE_DEPTH)
#define SET_PALETTE_FUNCTION XAPPEND(set_palette_, PALETTE_DEPTH)

void SET_PALETTE_FUNCTION(palette this, int index, int value)
{
    ((PALETTE_TYPE *)this->base_palette)[index] = ((PALETTE_TYPE *)this->translate)[value];
}

palette NEW_PALETTE_FUNCTION(int num_palettes, int palette_size)
{
    palette retval;
    int i;

    retval = malloc(sizeof(*retval) + (num_palettes * palette_size * sizeof(PALETTE_TYPE)) + (num_palettes * sizeof(void *)));

    if (!retval) {
	return NULL;
    }

    retval->set = SET_PALETTE_FUNCTION;
    retval->translate = vid_pre_xlat;
    retval->base_palette = &retval->palettes[num_palettes];

    for (i = 0; i < num_palettes; i++) {
	retval->palettes[i] = &(((PALETTE_TYPE *)retval->base_palette)[palette_size * i]);
    }
    
    return retval;
}

/*
 * $Log: palette.c,v $
 * Revision 1.1  2000/08/04 23:34:48  nyef
 * Initial revision
 *
 */
