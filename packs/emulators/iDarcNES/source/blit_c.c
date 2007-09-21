/*
 * blit_c.c
 *
 * C version of blitters.S
 */

/* $Id: blit_c.c,v 1.1 1999/08/15 02:17:19 nyef Exp $ */

#include "blitters.h"

void blit_2_8(unsigned char pattern0, unsigned char pattern1,
	      unsigned char count, unsigned char skip,
	      const unsigned char *colors, unsigned char *vbp)
{
    unsigned char chunks_0;
    unsigned char chunks_1;

    chunks_0 = ((pattern1 >> 1) & 0x55) | (pattern0 & 0xaa);
    chunks_1 = (pattern1 & 0x55) | ((pattern0 << 1) & 0xaa);

    switch (skip) {
    case 0:
	if (chunks_0 & 0xc0) *vbp = colors[(chunks_0 >> 6) & 3];
	vbp++;
	if (!--count) break;
    case 1:
	if (chunks_1 & 0xc0) *vbp = colors[(chunks_1 >> 6) & 3];
	vbp++;
	if (!--count) break;
    case 2:
	if (chunks_0 & 0x30) *vbp = colors[(chunks_0 >> 4) & 3];
	vbp++;
	if (!--count) break;
    case 3:
	if (chunks_1 & 0x30) *vbp = colors[(chunks_1 >> 4) & 3];
	vbp++;
	if (!--count) break;
    case 4:
	if (chunks_0 & 0x0c) *vbp = colors[(chunks_0 >> 2) & 3];
	vbp++;
	if (!--count) break;
    case 5:
	if (chunks_1 & 0x0c) *vbp = colors[(chunks_1 >> 2) & 3];
	vbp++;
	if (!--count) break;
    case 6:
	if (chunks_0 & 0x03) *vbp = colors[chunks_0 & 3];
	vbp++;
	if (!--count) break;
    case 7:
	if (chunks_1 & 0x03) *vbp = colors[chunks_1 & 3];
    }
}

void blit_2_8_rev(unsigned char pattern0, unsigned char pattern1,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp)
{
    unsigned char chunks_0;
    unsigned char chunks_1;

    chunks_0 = ((pattern1 >> 1) & 0x55) | (pattern0 & 0xaa);
    chunks_1 = (pattern1 & 0x55) | ((pattern0 << 1) & 0xaa);

    switch (skip) {
    case 0:
	if (chunks_1 & 0x03) *vbp = colors[chunks_1 & 3];
	vbp++;
	if (!--count) break;
    case 1:
	if (chunks_0 & 0x03) *vbp = colors[chunks_0 & 3];
	vbp++;
	if (!--count) break;
    case 2:
	if (chunks_1 & 0x0c) *vbp = colors[(chunks_1 >> 2) & 3];
	vbp++;
	if (!--count) break;
    case 3:
	if (chunks_0 & 0x0c) *vbp = colors[(chunks_0 >> 2) & 3];
	vbp++;
	if (!--count) break;
    case 4:
	if (chunks_1 & 0x30) *vbp = colors[(chunks_1 >> 4) & 3];
	vbp++;
	if (!--count) break;
    case 5:
	if (chunks_0 & 0x30) *vbp = colors[(chunks_0 >> 4) & 3];
	vbp++;
	if (!--count) break;
    case 6:
	if (chunks_1 & 0xc0) *vbp = colors[(chunks_1 >> 6) & 3];
	vbp++;
	if (!--count) break;
    case 7:
	if (chunks_0 & 0xc0) *vbp = colors[(chunks_0 >> 6) & 3];
    }
}

void blit_4_8(unsigned char pattern0, unsigned char pattern1,
	      unsigned char pattern2, unsigned char pattern3,
	      unsigned char count, unsigned char skip,
	      const unsigned char *colors, unsigned char *vbp)
{
    unsigned char pixel;
    int i;
    
    pattern0 <<= skip;
    pattern1 <<= skip;
    pattern2 <<= skip;
    pattern3 <<= skip;
    
    for (i = 0; i < count; i++) {
	pixel = ((pattern0 & 0x80) >> 4) | ((pattern1 & 0x80) >> 5) |
	    ((pattern2 & 0x80) >> 6) | ((pattern3 & 0x80) >> 7);
	
	*vbp++ = colors[pixel];
	
	pattern0 <<= 1;
	pattern1 <<= 1;
	pattern2 <<= 1;
	pattern3 <<= 1;
    }
}

void blit_4_8_rev(unsigned char pattern0, unsigned char pattern1,
		  unsigned char pattern2, unsigned char pattern3,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp)
{
    unsigned char pixel;
    int i;
    
    pattern0 >>= skip;
    pattern1 >>= skip;
    pattern2 >>= skip;
    pattern3 >>= skip;
    
    for (i = 0; i < count; i++) {
	pixel = ((pattern0 & 0x01) << 3) | ((pattern1 & 0x01) << 2) |
	    ((pattern2 & 0x01) << 1) | (pattern3 & 0x01);
	
	*vbp++ = colors[pixel];
	
	pattern0 >>= 1;
	pattern1 >>= 1;
	pattern2 >>= 1;
	pattern3 >>= 1;
    }
}

void blit_4_8_czt(unsigned char pattern0, unsigned char pattern1,
		  unsigned char pattern2, unsigned char pattern3,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp)
{
    unsigned char pixel;
    int i;
    
    pattern0 <<= skip;
    pattern1 <<= skip;
    pattern2 <<= skip;
    pattern3 <<= skip;
    
    for (i = 0; i < count; i++) {
	pixel = ((pattern0 & 0x80) >> 4) | ((pattern1 & 0x80) >> 5) |
	    ((pattern2 & 0x80) >> 6) | ((pattern3 & 0x80) >> 7);

	if (pixel) *vbp = colors[pixel];

	vbp++;
	
	pattern0 <<= 1;
	pattern1 <<= 1;
	pattern2 <<= 1;
	pattern3 <<= 1;
    }
}

void blit_4_8_czt_rev(unsigned char pattern0, unsigned char pattern1,
		      unsigned char pattern2, unsigned char pattern3,
		      unsigned char count, unsigned char skip,
		      const unsigned char *colors, unsigned char *vbp)
{
    unsigned char pixel;
    int i;
    
    pattern0 >>= skip;
    pattern1 >>= skip;
    pattern2 >>= skip;
    pattern3 >>= skip;
    
    for (i = 0; i < count; i++) {
	pixel = ((pattern0 & 0x01) << 3) | ((pattern1 & 0x01) << 2) |
	    ((pattern2 & 0x01) << 1) | (pattern3 & 0x01);
	
	if (pixel) *vbp = colors[pixel];

	vbp++;
	
	pattern0 >>= 1;
	pattern1 >>= 1;
	pattern2 >>= 1;
	pattern3 >>= 1;
    }
}

/*
 * $Log: blit_c.c,v $
 * Revision 1.1  1999/08/15 02:17:19  nyef
 * Initial revision
 *
 */
