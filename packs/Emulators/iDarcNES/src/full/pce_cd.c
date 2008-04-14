/*
 * pce_cd.c
 *
 * PC-Engine cd support.
 */

/* $Id: pce_cd.c,v 1.34 2000/09/09 02:15:15 nyef Exp $ */

#include <stdlib.h>

#ifdef PCE_CD_SUPPORT

#include "ui.h"
#include "cd.h"

#define CD_BLOCK_OFFSET 150
#define CD_SECS 60
#define CD_FRAMES 75

/*
 * FIXME: This code consists mainly of hacks piled on top of hacks without
 * any real organization. There is also no real consistent theory of operation
 * involved here. When the CD system is understood well enough to present a
 * usable theory of operation, this code should be rewritten.
 */

unsigned char binbcd[0x100] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
};

unsigned char bcdbin[0x100] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0,0,0,0,0,0,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0,0,0,0,0,0,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0,0,0,0,0,0,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0,0,0,0,0,0,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0,0,0,0,0,0,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0,0,0,0,0,0,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0,0,0,0,0,0,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0,0,0,0,0,0,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0,0,0,0,0,0,
    0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0,0,0,0,0,0,
};

int pce_cd_init;

void pce_cd_initialize(void);

unsigned char *pce_cd_read_buffer;
unsigned char pce_cd_dirinfo[4];

u8 pce_cd_statusbuffer[10];

struct {
    u8 args_left;
    u8 args[10];
    u8 cur_arg;
} pce_cd_command;

unsigned long pce_cd_read_datacnt;

unsigned long pce_cd_sectoraddy;

unsigned char pce_cd_sectorbuffer[2048];
unsigned char pce_cd_sectorcnt;

u32 pce_cd_playstart;

unsigned char pce_cd_1800;

unsigned char pce_cd_1801;

unsigned char pce_cd_1802;
unsigned char pce_cd_1803;
unsigned char pce_cd_1804;

unsigned char pce_cd_adpcm_memory[0x10000];
unsigned short pce_cd_adpcm_rptr;
unsigned short pce_cd_adpcm_wptr;
unsigned short pce_cd_adpcm_length;
unsigned short pce_cd_adpcm_data16;
unsigned char pce_cd_adpcm_readbuf;
unsigned char pce_cd_180b;
unsigned char pce_cd_180d;

/*
 *  convert logical_block_address to m-s-f_number (3 bytes only)
 *  lifted from the cdrom test program in the Linux kernel docs.
 *  hacked up to convert to BCD.
 */
void lba2msf(int lba, u8 *msf)
{
    lba += CD_BLOCK_OFFSET;
    msf[0] = binbcd[lba / (CD_SECS*CD_FRAMES)];
    lba %= CD_SECS*CD_FRAMES;
    msf[1] = binbcd[lba / CD_FRAMES];
    msf[2] = binbcd[lba % CD_FRAMES];
}

u32 msf2lba(u8 *msf)
{
    u32 retval;
    
    retval = 0;
    
    retval += bcdbin[msf[0]];
    retval *= CD_SECS;
    retval += bcdbin[msf[1]];
    retval *= CD_FRAMES;
    retval += bcdbin[msf[2]];

    retval -= CD_BLOCK_OFFSET;
    
    return retval;
}

void pce_cd_set_sector_address(void)
{
    pce_cd_sectoraddy = pce_cd_command.args[1] << 16;
    pce_cd_sectoraddy += pce_cd_command.args[2] << 8;
    pce_cd_sectoraddy += pce_cd_command.args[3];

    cd_set_sector_address(pce_cd_sectoraddy);
}

void pce_cd_read_sector(void)
{
/*     deb_printf("pce_cd: loading sector.\n"); */

    pce_cd_sectoraddy++; /* Is this really nessecary? */

    cd_read_next_sector(pce_cd_sectorbuffer);

    pce_cd_read_datacnt = 2047; /* 2048 bytes */
    pce_cd_read_buffer = pce_cd_sectorbuffer;
    pce_cd_sectorcnt--;
}

void pce_cd_dump_command_buffer(char *command_type)
{
    deb_printf("pce_cd: %s command:"
	       " %02x %02x %02x %02x %02x"
	       " %02x %02x %02x %02x %02x.\n",
	       command_type,
	       pce_cd_command.args[0], pce_cd_command.args[1],
	       pce_cd_command.args[2], pce_cd_command.args[3],
	       pce_cd_command.args[4], pce_cd_command.args[5],
	       pce_cd_command.args[6], pce_cd_command.args[7],
	       pce_cd_command.args[8], pce_cd_command.args[9]);
}

void pce_cd_command_0x00(void)
{
    /* reset */

    pce_cd_1800 = 0xd8;
}

void pce_cd_command_0x03(void)
{
    /* some sort of status/error query */

    pce_cd_dump_command_buffer("query");
    deb_printf("this shouldn't be happening, report it.\n");

    pce_cd_read_buffer = pce_cd_statusbuffer;
    pce_cd_read_datacnt = 9; /* 10 bytes */
}

void pce_cd_command_0x08(void)
{
    /* read sectors */
	
    pce_cd_sectorcnt = pce_cd_command.args[4];
    pce_cd_set_sector_address();
    pce_cd_read_sector();
}

void pce_cd_command_0xd8(void)
{
    /* play audio */
    /* addresses are BCD encoded. */

    pce_cd_dump_command_buffer("play");

    /* NOTE: this is a nasty hack */
    if (pce_cd_command.args[9] == 0x40) {
	pce_cd_playstart = msf2lba(&pce_cd_command.args[2]);
	pce_cd_1800 = 0xd8;
	return;
    }

    cd_play_track(bcdbin[pce_cd_command.args[2]]);
    
    pce_cd_1800 = 0xd8;
}

void pce_cd_command_0xd9(void)
{
    u32 playstop;

    /* first byte on the stop is 0x01 if track should repeat. */
    
    pce_cd_dump_command_buffer("play");

    if (pce_cd_command.args[9] != 0x40) {
	deb_printf("pce_cd: play assumption broken, ignoring stop command.\n");
	pce_cd_1800 = 0xd8;
	return;
    }

    playstop = msf2lba(&pce_cd_command.args[2]);
    cd_play_lba(pce_cd_playstart, playstop, (pce_cd_command.args[1] == 0x01));
    
    pce_cd_1800 = 0xd8;
}
 
void pce_cd_command_0xde(void)
{
    /* get directory (TOC) info */
    pce_cd_read_buffer = pce_cd_dirinfo;
    pce_cd_read_datacnt = 3; /* 4 bytes */
    
    if (pce_cd_command.args[1] == 0x00) {
	/* get number of tracks on CD */
	cd_get_tocheader(&pce_cd_dirinfo[0], &pce_cd_dirinfo[1]);
	pce_cd_dirinfo[2] = 0x00;
	pce_cd_dirinfo[3] = 0x00;
    } else if (pce_cd_command.args[1] == 0x02) {
	/* get TOC entry for a given track */
	u32 lba_addr;
	
	cd_get_tocentry(bcdbin[pce_cd_command.args[2]], &lba_addr, &pce_cd_dirinfo[3]);
	lba2msf(lba_addr, pce_cd_dirinfo);
    }
}

void pce_cd_handle_command_final(void)
{
    if (pce_cd_command.args[0] == 0x00) {
	pce_cd_command_0x00();
    } else if (pce_cd_command.args[0] == 0x03) {
	pce_cd_command_0x03();
    } else if (pce_cd_command.args[0] == 0x08) {
	pce_cd_command_0x08();
    } else if (pce_cd_command.args[0] == 0xd8) {
	pce_cd_command_0xd8();
    } else if (pce_cd_command.args[0] == 0xd9) {
	pce_cd_command_0xd9();
    } else if (pce_cd_command.args[0] == 0xde) {
	pce_cd_command_0xde();
    } else {
	/* unknown command */
	pce_cd_dump_command_buffer("unknown");
	pce_cd_1800 = 0xd8;
    }
}

void pce_cd_handle_command_arg(void)
{
    pce_cd_command.args[pce_cd_command.cur_arg++] = pce_cd_1801;
    pce_cd_command.args_left--;
    if (pce_cd_command.args_left) {
	pce_cd_1800 = 0xd0;
    } else {
	pce_cd_1800 = 0xc8;
	pce_cd_handle_command_final();
    }
}

void pce_cd_handle_command(void)
{
    if (!pce_cd_init) {
	pce_cd_initialize();
    }
    
    if (!pce_cd_command.args_left) {
	pce_cd_command.cur_arg = 0;
	pce_cd_command.args_left = 10;
    }

    pce_cd_handle_command_arg();
}

void pce_cd_check_load_next_sector()
{
    if (pce_cd_sectorcnt) {
	pce_cd_read_sector();
    }
}

u8 pce_cd_read_1801(void)
{
    unsigned char retval;

    if (!pce_cd_read_buffer) {
	return 0x00;
    }

    retval = *pce_cd_read_buffer++;
    
    if (!pce_cd_read_datacnt) {
	pce_cd_read_buffer = 0;
    }

    return retval;
}

void pce_cd_handle_readack(void)
{
    if (!pce_cd_read_datacnt) {
	pce_cd_check_load_next_sector();
	if (!pce_cd_read_datacnt) {
	    if (pce_cd_1800 & 0x10) {
		pce_cd_1800 |= 0x20;
	    } else {
		pce_cd_1800 |= 0x10;
	    }
	}
    } else {
	pce_cd_read_datacnt--;
    }
}

u8 pce_cd_read_1808(void)
{
    unsigned char retval;

    retval = pce_cd_read_1801();
    
    pce_cd_handle_readack();
    
    return retval;
}

unsigned char pce_cd_read(unsigned short addr)
{
    if (!pce_cd_init) {
	pce_cd_initialize();
    }
    
    if (addr == 0x1800) {
	return pce_cd_1800;
    } else if (addr == 0x1801) {
	return pce_cd_read_1801();
    } else if (addr == 0x1802) {
/* 	deb_printf("pce_cd: $1802 read, returning 0x%02x.\n", pce_cd_1802); */
	return pce_cd_1802;
    } else if (addr == 0x1803) {
	unsigned char retval;
	retval = pce_cd_1803;
	pce_cd_1803 = 0x00;
/*  	deb_printf("pce_cd: $1803 read, returning 0x%02x.\n", retval); */
	return retval;
    } else if (addr == 0x1804) {
	deb_printf("pce_cd: $1804 read, returning 0x%02x.\n", pce_cd_1804);
	return pce_cd_1804;
    } else if (addr == 0x1808) {
	return pce_cd_read_1808();
    } else if (addr == 0x180a) {
	unsigned char retval;
	
	retval = pce_cd_adpcm_readbuf;
	pce_cd_adpcm_readbuf = pce_cd_adpcm_memory[pce_cd_adpcm_rptr++];
	return retval;
    } else if (addr == 0x180b) {
	return pce_cd_180b;
    } else if (addr == 0x180c) {
	return 0x01; /* FIXME: a real value would be good here */
    } else if (addr == 0x180d) {
	return pce_cd_180d;
#if 1
    } else if (addr == 0x18c1) {
	return 0xaa;
    } else if (addr == 0x18c2) {
	return 0x55;
    } else if (addr == 0x18c3) {
/*  	deb_printf("pce_cd: memory fake.\n"); */
	return 0x03;
#endif
#if 0
    } else if (addr == 0x18c5) {
	return 0xaa;
    } else if (addr == 0x18c6) {
	return 0x55;
    } else if (addr == 0x18c7) {
/*  	deb_printf("pce_cd: memory fake.\n"); */
	return 0x03;
#endif
    } else {
	deb_printf("pce_cd: unknown address read ($%04hx), returning 0x00.\n", addr);
	return 0x00;
    }
}

void pce_cd_write(unsigned short addr, unsigned char data)
{
    if (addr == 0x1800) {
/*  	deb_printf("pce_cd: $1800 = 0x%02x.\n", data); */
	if (data == 0x81) {
	    pce_cd_1800 = 0xd0;
	}
    } else if (addr == 0x1801) {
/*  	deb_printf("pce_cd: $1801 = 0x%02x.\n", data); */
	if (data == 0x81) {
	    pce_cd_1800 = 0x40;
	}
	pce_cd_1801 = data;
    } else if (addr == 0x1802) {
/* 	deb_printf("pce_cd: $1802 = 0x%02x.\n", data); */
	if ((!(pce_cd_1802 & 0x80)) && (data & 0x80)) {
	    pce_cd_1800 &= ~0x40;
	} else if ((pce_cd_1802 & 0x80) && (!(data & 0x80))) {
	    pce_cd_1800 |= 0x40;
	    if (pce_cd_1800 & 0x08) {
		if (pce_cd_1800 & 0x20) {
		    pce_cd_1800 &= ~0x80;
		} else {
		    pce_cd_handle_readack();
		}
	    } else {
		pce_cd_handle_command();
	    }
	}
	pce_cd_1802 = data;
    } else if (addr == 0x1804) {
	deb_printf("pce_cd: $1804 = 0x%02x.\n", data);
	pce_cd_1804 = data;
    } else if (addr == 0x1808) {
	pce_cd_adpcm_data16 &= 0xff00; /* FIXME: may be wrong */
	pce_cd_adpcm_data16 |= data;
    } else if (addr == 0x1809) {
	pce_cd_adpcm_data16 &= 0x00ff; /* FIXME: may be wrong */
	pce_cd_adpcm_data16 |= (data << 8);
    } else if (addr == 0x180a) {
	pce_cd_adpcm_readbuf = data; /* may be wrong */
	pce_cd_adpcm_memory[pce_cd_adpcm_wptr++] = data;
    } else if (addr == 0x180b) {
/*  	deb_printf("pce_cd: adpcm control 1 = 0x%02x.\n", data); */
	pce_cd_180b = data;
	if (data & 0x02) {
	    deb_printf("pce_cd: AD_TRANS dma enable.\n");
	    /* FIXME: This is CLAN code */
	    {
		unsigned char tmp;

		while (pce_cd_read_buffer) {
		    tmp = pce_cd_read(0x1808);
		    pce_cd_write(0x180a, tmp);
		}
	    }
	    pce_cd_1803 |= 0x20;
	}
    } else if (addr == 0x180d) {
/*  	deb_printf("pce_cd: adpcm control 2 = 0x%02x.\n", data); */
	if (data & 0x01) {
	    /* FIXME: The full sequence is 0->3->2->0 on the low 2 bits */
/*  	    deb_printf("pce_cd: adpcm wptr set 0x%04hx.\n", pce_cd_adpcm_data16); */
	    pce_cd_adpcm_wptr = pce_cd_adpcm_data16;
	}
	if ((!(data & 0x08)) && (pce_cd_180d & 0x08)) {
	    /* clocking it in on the downwards edge may well be a hack */
/*  	    deb_printf("pce_cd: adpcm rptr set 0x%04hx.\n", pce_cd_adpcm_data16); */
	    pce_cd_adpcm_rptr = pce_cd_adpcm_data16;
	}
	if (data & 0x10) {
/*  	    deb_printf("pce_cd: adpcm length set 0x%04hx.\n", pce_cd_adpcm_data16); */
	    pce_cd_adpcm_length = pce_cd_adpcm_data16;
	}
	if (data & 0x20) {
	    deb_printf("pce_cd: adpcm play (don't you just wish).\n");
	    data &= ~0x20; /* a bit of a hack */
	}
	if (data & 0x80) {
	    deb_printf("pce_cd: adpcm reset.\n");
	}
	pce_cd_180d = data;
    } else {
	deb_printf("pce_cd: unknown address write ($%04hx = 0x%02x).\n", addr, data);
    }
}

void pce_cd_initialize(void)
{
    extern unsigned char *pce_mmap[256];
    int i;
    
    deb_printf("pce_cd: initializing.\n");

    cd_init();

#if 1
    pce_mmap[0x68] = malloc(0x30000);
    for (i = 0x69; i < 0x80; i++) {
	pce_mmap[i] = pce_mmap[i-1] + 0x2000;
    }
#endif
    
    pce_mmap[0x80] = malloc(0x10000);
    for (i = 0x81; i < 0x88; i++) {
	pce_mmap[i] = pce_mmap[i-1] + 0x2000;
    }
    
    pce_cd_init = 1;
}

#endif /* PCE_CD_SUPPORT */

/*
 * $Log: pce_cd.c,v $
 * Revision 1.34  2000/09/09 02:15:15  nyef
 * fixed "unknown command" messages for command 0x00 in pce_cd_handle_command()
 *
 * Revision 1.33  2000/08/24 01:22:10  nyef
 * added preliminary support for the 0x03 query command
 *
 * Revision 1.32  2000/08/22 02:34:18  nyef
 * extracted pce_cd_dump_command_buffer() from the three places it was used
 *
 * Revision 1.31  2000/08/20 01:27:41  nyef
 * hacked up cd play command to support track play
 *
 * Revision 1.30  2000/08/19 23:23:19  nyef
 * split the CD command handlers out into separate routines
 *
 * Revision 1.29  2000/08/19 20:13:02  nyef
 * cleaned up CD command handling yet more
 *
 * Revision 1.28  2000/08/19 19:59:18  nyef
 * fixed bug introduced in revision 1.25
 *
 * Revision 1.27  2000/08/19 18:40:24  nyef
 * cleaned up CD command handling some more
 *
 * Revision 1.26  2000/08/19 18:22:59  nyef
 * fixed bug in TOC read command handling
 *
 * Revision 1.25  2000/08/19 17:59:00  nyef
 * cleaned up the data buffer read routines yet again
 *
 * Revision 1.24  2000/08/19 17:03:23  nyef
 * minor cleanup (function rename, etc.)
 *
 * Revision 1.23  2000/08/15 00:31:44  nyef
 * added more debug info for CD audio play commands
 *
 * Revision 1.22  2000/08/05 01:42:39  nyef
 * added support for PLAY command
 *
 * Revision 1.21  2000/05/06 23:31:09  nyef
 * changed to only compile code if PCE_CD_SUPPORT is defined
 *
 * Revision 1.20  2000/02/12 19:21:10  nyef
 * disabled some more debug messages
 *
 * Revision 1.19  2000/02/12 19:03:52  nyef
 * moved all direct CD access out to cd_*.c
 *
 * Revision 1.18  2000/02/10 03:39:35  nyef
 * fixed stupid (compile breaking) bug with last revision
 *
 * Revision 1.17  2000/02/10 03:38:21  nyef
 * fixed unknown commands to not collect args (broke half my CDs)
 *
 * Revision 1.16  2000/02/10 03:11:46  nyef
 * cleaned up CDROM data reads
 * fixed some bugs with the ADPCM system
 *
 * Revision 1.15  2000/02/08 02:32:36  nyef
 * cleaned up command handling a little more
 *
 * Revision 1.14  2000/02/07 05:28:45  nyef
 * disabled some of the more annoying debug output
 *
 * Revision 1.13  2000/02/07 05:24:32  nyef
 * cleaned up the data buffer read routines some more
 *
 * Revision 1.12  2000/02/07 04:52:00  nyef
 * started cleaning up the data buffer read routines
 *
 * Revision 1.11  2000/02/07 02:43:18  nyef
 * removed some dead code of various sorts
 *
 * Revision 1.10  2000/02/07 02:33:55  nyef
 * cleaned up cd command handling
 *
 * Revision 1.9  2000/01/16 19:53:39  nyef
 * hacked up to work on my FreeBSD system
 *
 * Revision 1.8  1999/11/24 01:19:57  nyef
 * disabled pce_cd_dump_ramblock() (not used, and no longer compiles)
 *
 * Revision 1.7  1999/09/04 01:58:39  nyef
 * removed some debug output
 *
 * Revision 1.6  1999/08/28 03:03:45  nyef
 * more ADPCM code
 *
 * Revision 1.5  1999/07/18 01:32:01  nyef
 * added more ADPCM code
 *
 * Revision 1.4  1999/07/11 17:29:04  nyef
 * added preliminary support for the ADPCM controller
 *
 * Revision 1.3  1999/06/12 20:43:57  nyef
 * added dummy implimentations of the CD play commands
 *
 * Revision 1.2  1999/06/12 05:33:53  nyef
 * fixed a problem with loading secotrs to VRAM reading 2049 bytes per
 * secotr instead of 2048
 *
 * Revision 1.1  1999/06/04 00:56:14  nyef
 * Initial revision
 *
 */
