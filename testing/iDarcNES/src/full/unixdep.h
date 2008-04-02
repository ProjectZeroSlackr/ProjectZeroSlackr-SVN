/*
 * unixdep.h
 *
 * attempt to identify unix-type system
 */

#ifndef UNIXDEP_H
#define UNIXDEP_H

/* $Id: unixdep.h,v 1.1 2000/06/25 17:04:35 nyef Exp $ */

#include <sys/param.h>

#ifdef __FreeBSD__
#define SYSTEM_FREEBSD
#endif

#ifdef __NetBSD__
#define SYSTEM_NETBSD
#endif

#ifdef __OpenBSD__
#define SYSTEM_OPENBSD
#endif

/* whoever defined this symbol as an unadorned lowercase should be slapped */
#ifdef linux
#define SYSTEM_LINUX
#endif

#endif /* UNIXDEP_H */

/*
 * $Log: unixdep.h,v $
 * Revision 1.1  2000/06/25 17:04:35  nyef
 * Initial revision
 *
 */
