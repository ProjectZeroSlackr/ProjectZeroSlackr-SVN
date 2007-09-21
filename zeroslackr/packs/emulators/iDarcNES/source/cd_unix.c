/*
 * cd_unix.c
 *
 * UNIX(tm) CDROM interface
 */

/* $Id: cd_unix.c,v 1.6 2000/08/20 01:25:53 nyef Exp $ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

#include "types.h"
#include "cd.h"
#include "unixdep.h"

#ifdef SYSTEM_LINUX
#include <linux/cdrom.h>
#define CDROM_DEVICE "/dev/cdrom"
#endif

#ifdef SYSTEM_FREEBSD
#include <sys/cdio.h>
#define CDROM_DEVICE "/dev/acd1c"
#define LBA_CONVERT(x) (ntohl(x))
#endif

#ifdef SYSTEM_NETBSD
#include <sys/cdio.h>
#define CDROM_DEVICE "/dev/rcd0d"
#define LBA_CONVERT(x) (x)
#endif

#ifdef SYSTEM_OPENBSD
#error No CD support on OpenBSD systems yet
#endif

int cd_drive;

void cd_set_sector_address(u32 sector)
{
    int result;

    result = lseek(cd_drive, (sector) * 2048, SEEK_SET);
    if (result < 0) {
	perror("cd_unix: seeking to CD sector");
    }
}

void cd_read_next_sector(u8 *buf)
{
    int result;

    result = read(cd_drive, buf, 2048);
    if (result < 0) {
	perror("cd_unix: reading CD data");
    }
}

void cd_play_track(u8 track)
{
#ifdef SYSTEM_FREEBSD
    struct ioc_play_track playtrack;

    /* FIXME: CD keeps playing after program quits */

    /* NOTE: The FreeBSD 4.0 atapi-cd driver ignores the *_index elements */
    playtrack.start_track = track;
    playtrack.start_index = 1;
    playtrack.end_track = track + 1;
    playtrack.end_index = 1;

    ioctl(cd_drive, CDIOCPLAYTRACKS, &playtrack);
#endif
}

void cd_play_lba(u32 from, u32 to, int repeat)
{
#ifdef SYSTEM_FREEBSD
    struct ioc_play_blocks playblocks;

    /* FIXME: ignores the repeat flag */
    /* FIXME: CD keeps playing after program quits */

#if 0
    printf("play CD starting from %ld, stopping at %ld and %srepeating.\n",
	   from, to, repeat? "": "not ");
#endif
    
    playblocks.blk = from;
    playblocks.len = to - from;

    ioctl(cd_drive, CDIOCPLAYBLOCKS, &playblocks);
#endif
}

#ifdef SYSTEM_LINUX
void cd_get_tocheader(u8 *start_track, u8 *stop_track)
{
    /* pretend that there are 4 tracks on this CD */
    /* FIXME: real implementation please? */
    *start_track = 1;
    *stop_track = 4;
}

void cd_get_tocentry(u8 track, u32 *lba, u8 *flags)
{
    int result;
    struct cdrom_tocentry cd_tocentry;
    
    cd_tocentry.cdte_track = track;
    cd_tocentry.cdte_format = CDROM_LBA;
    
    result = ioctl(cd_drive, CDROMREADTOCENTRY, &cd_tocentry);
    if (result < 0) {
	perror("cd_unix: reading tocentry");
/*  	deb_printf("cd_unix: tocread failure.\n"); */
    } else {
	*lba = cd_tocentry.cdte_addr.lba;
	*flags = cd_tocentry.cdte_ctrl; /* FIXME: may want cdte.adr << 4 in here as well */
    }
}

#else
void cd_get_tocheader(u8 *start_track, u8 *stop_track)
{
    struct ioc_toc_header toc_header;
    int result;
    
    result = ioctl(cd_drive, CDIOREADTOCHEADER, &toc_header);
    if (result < 0) {
	perror("cd_unix: reading tocheader");
    } else {
	*start_track = toc_header.starting_track;
	*stop_track = toc_header.ending_track;
    }	
}

void cd_get_tocentry(u8 track, u32 *lba, u8 *flags)
{
    int result;
    struct cd_toc_entry toc_entry;
    struct ioc_read_toc_entry cd_tocentry;
    
    cd_tocentry.starting_track = track;
    cd_tocentry.address_format = CD_LBA_FORMAT;
    cd_tocentry.data_len = sizeof(toc_entry);
    cd_tocentry.data = &toc_entry;

    result = ioctl(cd_drive, CDIOREADTOCENTRYS, &cd_tocentry);
    if (result < 0) {
	perror("cd_unix: reading tocentry");
/*  	deb_printf("cd_unix: tocread failure.\n"); */
    } else {
	*lba = LBA_CONVERT(toc_entry.addr.lba);
	*flags = toc_entry.control; /* FIXME: may want addr_type << 4 in here as well */
    }
}
#endif

void cd_init(void)
{
    cd_drive = open(CDROM_DEVICE, O_RDONLY);
    if (cd_drive < 0) {
	perror("cd_unix: opening CD drive");
	cd_drive = 0;
    } else {
	/* Do something? */
    }
}

/*
 * $Log: cd_unix.c,v $
 * Revision 1.6  2000/08/20 01:25:53  nyef
 * renamed cd_play() to cd_play_msf(), and added cd_play_track()
 *
 * Revision 1.5  2000/08/05 01:43:04  nyef
 * added support for playing CD audio
 *
 * Revision 1.4  2000/06/25 17:04:27  nyef
 * fixed to automatically detect system type
 *
 * Revision 1.3  2000/03/15 03:48:04  nyef
 * cleaned up some comments
 *
 * Revision 1.2  2000/02/20 06:55:35  nyef
 * fixed to hopefully compile on Linux
 *
 * Revision 1.1  2000/02/14 02:09:03  nyef
 * Initial revision
 *
 *
 */
