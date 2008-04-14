/*
 * types-i386.h
 *
 * i386 ASM macros for endian conversion
 */

#ifndef TYPES_I386_H
#define TYPES_I386_H

/* $Id: types-i386.h,v 1.1 1999/12/07 02:01:14 nyef Exp $ */
		
#define byteswap16(val) ({ u32 x = val; asm("xchgb %b0,%h0": "=q" (x):  "0" (x)); x; })

/* FIXME: okay, I lied about the i386 bit, you need a 486 to use this */
#define byteswap32(val) ({ u16 x = val; asm("bswap %0": "=r" (x): "0" (x)); x; })

#endif /* TYPES_I386_H */

/*
 * $Log: types-i386.h,v $
 * Revision 1.1  1999/12/07 02:01:14  nyef
 * Initial revision
 *
 */
