/*
 * blitters.h
 *
 * miscelaneous blitters
 */

#ifndef BLITTERS_H
#define BLITTERS_H

/* $Id: blitters.h,v 1.2 1999/06/13 20:46:31 nyef Exp $ */

void blit_2_8(unsigned char pattern0, unsigned char pattern1,
	      unsigned char count, unsigned char skip,
	      const unsigned char *colors, unsigned char *vbp);
void blit_2_8_rev(unsigned char pattern0, unsigned char pattern1,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp);

void blit_4_8(unsigned char pattern0, unsigned char pattern1,
	      unsigned char pattern2, unsigned char pattern3,
	      unsigned char count, unsigned char skip,
	      const unsigned char *colors, unsigned char *vbp);
void blit_4_8_rev(unsigned char pattern0, unsigned char pattern1,
		  unsigned char pattern2, unsigned char pattern3,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp);
void blit_4_8_czt(unsigned char pattern0, unsigned char pattern1,
		  unsigned char pattern2, unsigned char pattern3,
		  unsigned char count, unsigned char skip,
		  const unsigned char *colors, unsigned char *vbp);
void blit_4_8_czt_rev(unsigned char pattern0, unsigned char pattern1,
		      unsigned char pattern2, unsigned char pattern3,
		      unsigned char count, unsigned char skip,
		      const unsigned char *colors, unsigned char *vbp);

#endif /* BLITTERS_H */

/*
 * $Log: blitters.h,v $
 * Revision 1.2  1999/06/13 20:46:31  nyef
 * fixed to squash errors when using constant palettes (for tile caches)
 *
 * Revision 1.1  1999/01/11 02:22:27  nyef
 * Initial revision
 *
 */
