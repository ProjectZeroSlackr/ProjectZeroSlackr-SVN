/*
 * types.h
 *
 * system-independant names and endian conversion macros for ordinal types
 */

#ifndef TYPES_H
#define TYPES_H

/* $Id: types.h,v 1.1 1999/12/07 02:01:23 nyef Exp $ */

#ifdef MACH_TYPES
#include MACH_TYPES
#endif

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned long u32;
typedef signed long s32;

/*
 * WARNING: these endian macros _must_ evaluate their arguments only once.
 *
 * if, for a given compiler, this is impossible, use procedures.
 */

/* WARNING: the default implementation of the endian macros are gcc-specific */

/* NOTE: ASM versions should be defined in MACH_TYPES (whatever that is) */

#ifndef byteswap16
#define byteswap16(val) ({ u16 x = val; x = (x << 8) | (x >> 8); x; })
#endif

#ifndef byteswap32
#define byteswap32(val) ({ u32 x = val; x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24); x; })
#endif

#ifdef MSB_FIRST
#ifdef LSB_FIRST
#define ENDIAN_ERROR
#else
#define htol16(x) byteswap16(x)
#define htol32(x) byteswap32(x)
#define htom16(x) (x)
#define htom32(x) (x)
#endif
#endif

#ifdef LSB_FIRST
#define htol16(x) (x)
#define htol32(x) (x)
#define htom16(x) byteswap16(x)
#define htom32(x) byteswap32(x)
#else
#ifndef MSB_FIRST
#define ENDIAN_ERROR
#endif
#endif

#ifdef ENDIAN_ERROR
#error One (but not both) of MSB_FIRST or LSB_FIRST must be defined. Check the Makefile.
#endif

#define ltoh16(x) htol16(x)
#define ltoh32(x) htol32(x)
#define mtoh16(x) htom16(x)
#define mtoh32(x) htom32(x)

#endif /* TYPES_H */

/*
 * $Log: types.h,v $
 * Revision 1.1  1999/12/07 02:01:23  nyef
 * Initial revision
 *
 */
