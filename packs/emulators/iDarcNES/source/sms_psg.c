/*
 * sms_psg.c
 *
 * SMS sound emulation system
 */

/* $Id: sms_psg.c,v 1.14 2000/10/05 07:45:39 nyef Exp $ */

#include <unistd.h>

#include "ui.h"
#include "sms_psg.h"
#include "snd.h"

/* #define SOUND_LOGGING */ /* for kevtris */

struct {
    int lastchan; /* for frequency writes */
    unsigned char vols[4];
    unsigned char freqls[4];
    unsigned char freqhs[4];
    unsigned long index[4];
    unsigned long skip[4];
    unsigned long shiftreg;
} sms_psg;

/*
 * sms_psg_vsync does the actual sound rendering.
 */

#ifdef SOUND

unsigned char wave_buffers[4][735]; /* 44100 / 60 = 735 samples per sync */

#endif

#ifdef SOUND
unsigned long sms_psg_magic;
unsigned int sms_psg_samples_per_sync;
unsigned int sms_psg_sample_rate;

/*
 * magic number calculation: (NOTE: the figure 125000 below is probably wrong)
 *
 * 44100 / 125000 = 0.3528
 * 32 / 0.3528 = 90.702947
 * 90.702947 * 2^16 = 5944308.3
 * 5944308 as hex = 0x5ab3f4
 */

/*
 * new magic number calculation: (This one is derived from the system clock)
 *
 * 3.579545 / 32 = 0.1118607
 * 44100 / 111860.7 = 0.3942403
 * 32 / 0.3942403 = 81.168769
 * 81.168769 * 2^16 = 5319476.4
 * 5319476 as hex = 0x512b34
 */

struct sms_psg_quality_data {
    unsigned long magic;
    unsigned int samples_per_sync;
    unsigned int sample_rate;
} sms_psg_qual[] = {
/*     {0xb567e800, 367, 22050}, */
/*     {0x5ab3f400, 735, 44100}, */
    {0xa2566800, 367, 22050},
    {0x512b3400, 735, 44100},
};

/* NOTE: these routines use 8.24 bit fixed point math in places. */

/* FIXME: some frequency values are not handled well by this system. */

void sms_psg_calc_freqs(int channel)
{
    unsigned int freq;
    
    if (channel != 3) {
	freq = (sms_psg.freqhs[channel] << 4) + sms_psg.freqls[channel];
    } else {
	if ((sms_psg.freqls[3] & 3) != 3) {
	    freq = (0x10 << (sms_psg.freqls[3] & 3));
	} else {
	    freq = ((sms_psg.freqhs[2] << 4) + sms_psg.freqls[2]);
	}
	sms_psg.shiftreg = 0x1f;
    }
    
    if (freq) {
	sms_psg.skip[channel] = sms_psg_magic / freq;
    } else {
	sms_psg.skip[channel] = 0;
    }

    if ((channel == 2) && ((sms_psg.freqls[3] & 3) == 3)) {
	freq = ((sms_psg.freqhs[2] << 4) + sms_psg.freqls[2]);
	if (freq) {
	    sms_psg.skip[3] = sms_psg_magic / freq;
	} else {
	    sms_psg.skip[3] = 0;
	}
    }
}
#endif

void sms_psg_write(unsigned char data)
{
#ifdef SOUND_LOGGING
    deb_printf("%02x\n", data);
#endif
#ifdef SOUND
    if (data & 0x80) {
	if (data & 0x10) {
	    /* volume */
	    sms_psg.vols[(data >> 5) & 3] = (15 - (data & 15)) * 0x11;
	} else {
	    /* freq low */
	    sms_psg.lastchan = (data >> 5) & 3;
	    sms_psg.freqls[sms_psg.lastchan] = data & 15;
	    sms_psg_calc_freqs(sms_psg.lastchan);
	}
    } else {
	/* freq high */
	sms_psg.freqhs[sms_psg.lastchan] = data & 0x3f;
	sms_psg_calc_freqs(sms_psg.lastchan);
    }
#endif
}

#ifdef SOUND
void sms_psg_pulse(int channel)
{
    int i;

    for (i = 0; i < sms_psg_samples_per_sync; i++) {
	sms_psg.index[channel] += sms_psg.skip[channel];
	sms_psg.index[channel] &= 0x1fffffff;
	wave_buffers[channel][i] = (sms_psg.index[channel] & 0x10000000)? sms_psg.vols[channel]: 0;
    }
}

void sms_psg_wave_4(void)
{
#if 1
    int i;

    if (!sms_psg.shiftreg) {
	sms_psg.shiftreg = 0x1f;
    }

    for (i = 0; i < sms_psg_samples_per_sync; i++) {
	sms_psg.index[3] += sms_psg.skip[3];
	if (sms_psg.index[3] > 0x1fffffff) {
	    if (sms_psg.freqls[3] & 0x04) {
		sms_psg.shiftreg |= ((!(sms_psg.shiftreg & 1)) ^ (!(sms_psg.shiftreg & 4))) << 5;
	    } else {
		sms_psg.shiftreg |= (!(sms_psg.shiftreg & 1)) << 5;
	    }
	    sms_psg.shiftreg >>= 1;
	}
	sms_psg.index[3] &= 0x1fffffff;
	if (sms_psg.shiftreg & 1) {
	    wave_buffers[3][i] = sms_psg.vols[3];
	} else {
	    wave_buffers[3][i] = 0;
	}
    }
#endif
}

#endif

void sms_psg_vsync(void)
{
#ifdef SOUND_LOGGING
    deb_printf("frame\n");
#endif
#ifdef SOUND
    sms_psg_pulse(0);
    sms_psg_pulse(1);
    sms_psg_pulse(2);
    sms_psg_wave_4();

    snd_output_4_waves(sms_psg_samples_per_sync, wave_buffers[0], wave_buffers[1], wave_buffers[2], wave_buffers[3]);
#endif
}

void sms_psg_init(void)
{
#ifdef SOUND
    snd_init();
    
    if (nes_psg_quality > 0) {
	if (nes_psg_quality > 2) {
	    nes_psg_quality = 1;
	} else {
	    nes_psg_quality--;
	}
	sms_psg_magic = sms_psg_qual[nes_psg_quality].magic;
	sms_psg_samples_per_sync = sms_psg_qual[nes_psg_quality].samples_per_sync;
	sms_psg_sample_rate = sms_psg_qual[nes_psg_quality].sample_rate;
	
	snd_open(sms_psg_samples_per_sync, sms_psg_sample_rate);
    } else {
    }
#endif
}

void sms_psg_done(void)
{
#ifdef SOUND
    snd_close();
#endif
}

/*
 * $Log: sms_psg.c,v $
 * Revision 1.14  2000/10/05 07:45:39  nyef
 * added optional support for logging sound writes
 *
 * Revision 1.13  2000/05/06 23:25:54  nyef
 * moved #define SOUND out to the Makefile level
 *
 * Revision 1.12  2000/05/06 22:29:32  nyef
 * fixed to compile with no sound code in the executable
 *
 * Revision 1.11  1999/11/28 05:03:02  nyef
 * fixed a couple frequency bugs with the noise channel
 *
 * Revision 1.10  1999/10/31 02:37:41  nyef
 * broke all os-dependant sound code out to separate files
 *
 * Revision 1.9  1999/10/31 00:59:10  nyef
 * loop invariant code motion: moved multiply of volumes by 0x11 to
 * sms_psg_write() from sms_psg_pulse() and sms_psg_wave_4()
 *
 * Revision 1.8  1999/10/31 00:56:54  nyef
 * changed sms_psg_pulse to not use pulse_50[] from nes_psg.c
 *
 * Revision 1.7  1999/10/31 00:51:57  nyef
 * consolidated sms_psg_wave_[123]() into sms_psg_pulse()
 *
 * Revision 1.6  1999/10/31 00:24:04  nyef
 * rearranged some stuff in sms_psg_vsync()
 *
 * Revision 1.5  1999/09/25 20:21:26  nyef
 * fixed to compile properly with sound disabled
 *
 * Revision 1.4  1999/09/09 00:34:26  nyef
 * calculated a new magic number (sounds a lot closer to the real thing)
 *
 * Revision 1.3  1999/07/10 02:44:36  nyef
 * rearranged the calculation of frequencies and skip factors
 *
 * Revision 1.2  1999/05/04 23:50:30  nyef
 * fixed problem with initialization in DOS version
 *
 * Revision 1.1  1999/04/28 14:11:08  nyef
 * Initial revision
 *
 */
