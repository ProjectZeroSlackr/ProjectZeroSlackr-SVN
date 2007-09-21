/*
 * machdep.h
 *
 * machine dependancies
 */

/* $Id: machdep.h,v 1.1 1998/07/11 22:18:34 nyef Exp $ */

#ifndef __MACHDEP_H__
#define __MACHDEP_H__


/* DJGPP needs O_BINARY, but Linux doesn't use it */
#ifndef O_BINARY
#define O_BINARY 0
#endif

/* some platforms have an underbar prefix for asm labels accessible from C */
#ifdef ASM_UNDERBARS
#define SYMNAME(sym) _##sym
#define SYMLABEL(sym) _##sym##:
#else
#define SYMNAME(sym) sym
#define SYMLABEL(sym) sym##:
#endif

#endif /* __MACHDEP_H__ */

/*
 * $Log: machdep.h,v $
 * Revision 1.1  1998/07/11 22:18:34  nyef
 * Initial revision
 *
 */
