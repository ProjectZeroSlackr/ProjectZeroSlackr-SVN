/*
 * nes_psg.c
 *
 * NES sound emulation system
 */

/* $Id: nes_psg.c,v 1.50 2000/11/18 14:56:57 nyef Exp $ */

#include "cal.h"
#include "ui.h"
#include "nes_psg.h"
#include "snd.h"
#include "types.h"

/*
 * sound register data
 */

#ifdef SOUND
struct channel { u8 a; u8 b; u8 c; u8 d; };

struct channel nes_psg_c1;
struct channel nes_psg_c2;
struct channel nes_psg_c3;
struct channel nes_psg_c4;

u8 nes_psg_control;
#endif

/*
 * a psg write function is of the form c#$ where # is the channel number and
 *   $ is the channel register starting from a
 */

#ifdef SOUND
#define PSG_WRITEFUNC(channel, reg) \
void nes_psg_write_##channel##reg(u8 value) \
{ \
    nes_psg_##channel.reg = value; \
}
#else
#define PSG_WRITEFUNC(channel, reg) \
void nes_psg_write_##channel##reg(u8 value) \
{ \
}
#endif

void nes_psg_write_c1d(u8);
void nes_psg_write_c2d(u8);
void nes_psg_write_c3d(u8);

PSG_WRITEFUNC(c1, a); PSG_WRITEFUNC(c1, b); PSG_WRITEFUNC(c1, c);
PSG_WRITEFUNC(c2, a); PSG_WRITEFUNC(c2, b); PSG_WRITEFUNC(c2, c);
PSG_WRITEFUNC(c3, a); PSG_WRITEFUNC(c3, b); PSG_WRITEFUNC(c3, c);
PSG_WRITEFUNC(c4, a); PSG_WRITEFUNC(c4, b); PSG_WRITEFUNC(c4, c);
PSG_WRITEFUNC(c4, d);

#ifndef SOUND
PSG_WRITEFUNC(c1, d);
PSG_WRITEFUNC(c2, d);
PSG_WRITEFUNC(c3, d);
#endif

psg_writefunc sound_regs[16] = {
    nes_psg_write_c1a, nes_psg_write_c1b, nes_psg_write_c1c, nes_psg_write_c1d,
    nes_psg_write_c2a, nes_psg_write_c2b, nes_psg_write_c2c, nes_psg_write_c2d,
    nes_psg_write_c3a, nes_psg_write_c3b, nes_psg_write_c3c, nes_psg_write_c3d,
    nes_psg_write_c4a, nes_psg_write_c4b, nes_psg_write_c4c, nes_psg_write_c4d,
};

void nes_psg_write_control(u8 value)
{
#ifdef SOUND
    nes_psg_control = value;
#endif
}

int nes_psg_quality;

#ifdef SOUND
/*
 * Sync occurs every 7467 CPU cycles.
 * Therefore, at the rate calculated
 * for 44.1kHz (our highest rate),
 * there are 182 samples per sync.
 */
static u8 wave_buffers[4][182];
//u8 *wave_buffers_0=0x40015030;
//u8 *wave_buffers_1=0x40015030+182;
//u8 *wave_buffers_2=0x40015030+182*2;
//u8 *wave_buffers_3=0x40015030+182*3;
unsigned long nes_psg_pulse_magic;
unsigned long nes_psg_triangle_magic;
unsigned long nes_psg_noise_magic;
unsigned int nes_psg_samples_per_sync;
unsigned int nes_psg_cycles_per_sample;
unsigned int nes_psg_sample_rate;

struct nes_psg_quality_data {
    unsigned long pulse_magic;
    unsigned long triangle_magic;
    unsigned long noise_magic;
    unsigned int samples_per_sync;
    unsigned int cycles_per_sample;
    unsigned int sample_rate;
} nes_psg_qual[] = {
    {0xa2567000, 0x512b3800, 0x512b3800,  91, 82, 22050},
    {0x512b3800, 0x289d9c00, 0x289d9c00, 182, 41, 44100},
};

/* NOTE: these routines use 8.24 bit fixed point math in places. */

/* FIXME: some frequency values are not handled well by this system. */

unsigned char pulse_25[0x20] = {
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

unsigned char pulse_50[0x20] = {
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

unsigned char pulse_75[0x20] = {
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

unsigned char pulse_87[0x20] = {
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00,
};

unsigned char triangle_50[0x20] = {
    0x00, 0x11, 0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb,
    0xcc, 0xdd, 0xee, 0xff,
    0xff, 0xee, 0xdd, 0xcc,
    0xbb, 0xaa, 0x99, 0x88,
    0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x00,
};

unsigned char *pulse_waves[4] = {
    pulse_87, pulse_75, pulse_50, pulse_25,
};

unsigned char nes_psg_atl[0x20] = {
    5, 127, 10, 1, 20,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
    6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/*
 * volume envelopes
 */

struct envelope {
    u8 decay_counter;
    u8 rate_counter;
};

u8 envelope_get_volume(struct channel *channel, struct envelope *envelope)
{
    if (channel->a & 0x10) {
	return channel->a & 0x0f;
    } else {
	return envelope->decay_counter;
    }
}

void envelope_run_counter(struct channel *channel, struct envelope *envelope)
{
    if (envelope->rate_counter--) return;
    envelope->rate_counter = channel->a & 0x0f;

    if (envelope->decay_counter--) return;
    envelope->decay_counter = (channel->a & 0x20)? 0x0f: 0;
}

/*
 * frequency sweeps
 */

static int sweep_unit_active(struct channel *channel)
{
    return channel->b & 0x80;
}

static int sweep_unit_shift_count(struct channel *channel)
{
    return channel->b & 7;
}

static int sweep_unit_should_sweep_down(struct channel *channel)
{
    return channel->b & 0x08;
}

static void sweep_unit_set_frequency(struct channel *channel, u16 freq)
{
    channel->c = freq;
    channel->d &= ~7;
    channel->d |= (freq >> 8) & 7;
}

static void sweep_unit_do_sweep(struct channel *channel, u16 freq)
{
    if (sweep_unit_should_sweep_down(channel)) {
	freq -= freq >> sweep_unit_shift_count(channel);
	if (channel == &nes_psg_c1) {
	    freq--;
	}
    } else {
	freq += freq >> sweep_unit_shift_count(channel);
    }

    sweep_unit_set_frequency(channel, freq);
    
    /* FIXME: sweep unit upper bound shutdown? */
}

void run_sweep_unit(struct channel *channel, int *sweep_clock, u16 freq)
{
    if (!sweep_unit_active(channel)) return;

    if ((*sweep_clock)--) return;
    *sweep_clock = (channel->b >> 3) & 0x0e;
    
    if (!sweep_unit_shift_count(channel)) return;
    
    sweep_unit_do_sweep(channel, freq);
}

/*
 * nes_psg_frame is called every 7457 CPU cycles to do sound rendering.
 */

static u32 wave_1_index;
static int wave_1_sweep_clock;
static u8 wave_1_length_counter;
static struct envelope wave_1_envelope;

void nes_psg_write_c1d(u8 value)
{
    nes_psg_c1.d = value;
    wave_1_envelope.decay_counter = 0x0f;
    wave_1_length_counter = nes_psg_atl[value >> 3];
}

void nes_psg_wave_1(void)
{
    int i;
    u8 volume;
    u16 freq;
    u32 step;
    u8 *sample;

    volume = envelope_get_volume(&nes_psg_c1, &wave_1_envelope);
    
    freq = ((nes_psg_c1.d & 0x07) << 8) + nes_psg_c1.c;
    if (freq >= 8) { /* sweep unit lower bound shutdown */
	step = nes_psg_pulse_magic / freq;
    } else {
	step = 0;
    }

    if (!wave_1_length_counter) {
	step = 0;
    }

    if (!(nes_psg_control & 0x01)) { /* channel output enable */
	step = 0;
    }

    sample = pulse_waves[(nes_psg_c1.a >> 6)];
    
    for (i = 0; i < nes_psg_samples_per_sync; i++) {
	wave_1_index += step;
	wave_1_index &= 0x1fffffff;
	wave_buffers[0][i] = sample[wave_1_index >> 24] * volume;
    }

    envelope_run_counter(&nes_psg_c1, &wave_1_envelope);

    run_sweep_unit(&nes_psg_c1, &wave_1_sweep_clock, freq);
}

static u32 wave_2_index;
static int wave_2_sweep_clock;
static u8 wave_2_length_counter;
static struct envelope wave_2_envelope;

void nes_psg_write_c2d(u8 value)
{
    nes_psg_c2.d = value;
    wave_2_envelope.decay_counter = 0x0f;
    wave_2_length_counter = nes_psg_atl[value >> 3];
}

void nes_psg_wave_2(void)
{
    int i;
    u8 volume;
    u16 freq;
    u32 step;
    u8 *sample;

    volume = envelope_get_volume(&nes_psg_c2, &wave_2_envelope);
    
    freq = ((nes_psg_c2.d & 0x07) << 8) + nes_psg_c2.c;
    if (freq >= 8) { /* sweep unit lower bound shutdown */
	step = nes_psg_pulse_magic / freq;
    } else {
	step = 0;
    }

    if (!wave_2_length_counter) {
	step = 0;
    }

    if (!(nes_psg_control & 0x02)) { /* channel output enable */
	step = 0;
    }

    sample = pulse_waves[(nes_psg_c2.a >> 6)];
    
    for (i = 0; i < nes_psg_samples_per_sync; i++) {
	wave_2_index += step;
	wave_2_index &= 0x1fffffff;
	wave_buffers[1][i] = sample[wave_2_index >> 24] * volume;
    }

    envelope_run_counter(&nes_psg_c2, &wave_2_envelope);

    run_sweep_unit(&nes_psg_c2, &wave_2_sweep_clock, freq);
}

static u32 wave_3_index;
static u8 wave_3_length_counter;

void nes_psg_write_c3d(u8 value)
{
    nes_psg_c3.d = value;
    wave_3_length_counter = nes_psg_atl[value >> 3];
}

void nes_psg_wave_3(void)
{
    int i;
    u16 freq;
    u32 step;

    /* FIXME: Add linear counter */
    
    freq = ((nes_psg_c3.d & 0x07) << 8) + nes_psg_c3.c;
    if (freq) { /* sweep unit lower bound shutdown */
	step = nes_psg_triangle_magic / freq;
    } else {
	step = 0;
    }

    if (!wave_3_length_counter) {
	step = 0;
    }

    if (!(nes_psg_control & 0x04)) { /* channel output enable */
	step = 0;
    }

    for (i = 0; i < nes_psg_samples_per_sync; i++) {
	wave_3_index += step;
	wave_3_index &= 0x1fffffff;
	wave_buffers[2][i] = triangle_50[wave_3_index >> 24];
    }
}

void nes_psg_wave_4(void)
{
#if 0
    int i;
    int cycles;
    int event;
    unsigned char ctrl;

    ctrl = nes_psg_ctrl;
    
    cycles = 0;
    event = 0;
    
    for (i = 0; i < nes_psg_samples_per_sync; i++) {
	cycles += nes_psg_cycles_per_sample;
	while ((event < cur_event) && (psg_eventqueue[event].time < cycles)) {
	    if ((psg_eventqueue[event].type & PSGET_MASK) == PSGET_C4) {
		switch (psg_eventqueue[event].type & 3) {
		case 0:
		    nes_psg_c4a = psg_eventqueue[event].data;
		    nes_psg_c4_vol = nes_psg_c4a & 0x0f;
		    nes_psg_c4_vol |= nes_psg_c4_vol << 4;
/* 		    deb_printf("psg: c4a: vol %d.\n", nes_psg_c4a & 15); */
		    break;
		case 1:
		    nes_psg_c4b = psg_eventqueue[event].data;
/* 		    deb_printf("psg: c4b: 0x%02x.\n", nes_psg_c4b); */
		    break;
		case 2:
		    nes_psg_c4c = psg_eventqueue[event].data;
		    if (nes_psg_c4c & 0x80) {
			nes_psg_c4_sr = 0x001f;
		    } else {
			nes_psg_c4_sr = 0x01ff;
		    }
/* 		    if (nes_psg_c4c & 0x70) { */
/* 			deb_printf("psg: c4c: c4c & 0x70 == 0x%02x.\n", nes_psg_c4c & 0x70); */
/* 		    } */
		    if (nes_psg_c4c & 15) {
			nes_psg_c4_skip = nes_psg_noise_magic / (nes_psg_c4c & 15);
		    } else {
			nes_psg_c4_skip = 0;
		    }
		    nes_psg_c4_atl = nes_psg_atl[(nes_psg_c4d & 0xf8) >> 3];
/* 		    deb_printf("psg: c4: freq: %d.\n", nes_psg_c4c & 15); */
		    break;
		case 3:
		    nes_psg_c4d = psg_eventqueue[event].data;
		    if (nes_psg_c4c & 15) {
			nes_psg_c4_skip = nes_psg_noise_magic / (nes_psg_c4c & 15);
		    } else {
			nes_psg_c4_skip = 0;
		    }
		    nes_psg_c4_atl = nes_psg_atl[(nes_psg_c4d & 0xf8) >> 3];
		}
	    } else if (psg_eventqueue[event].type == PSGET_W_CTRL) {
		ctrl = psg_eventqueue[event].data;
	    }
	    
	    event++;
	}
	if (ctrl & 8) {
	    nes_psg_c4_index += nes_psg_c4_skip;
	    if (nes_psg_c4_index > 0x1fffffff) {
		if (nes_psg_c4c & 0x80) { /* FIXME: may be wrong */
		    nes_psg_c4_sr |= ((!(nes_psg_c4_sr & 1)) ^ (!(nes_psg_c4_sr & 4))) << 5;
		} else {
		    nes_psg_c4_sr |= ((!(nes_psg_c4_sr & 1)) ^ (!(nes_psg_c4_sr & 16))) << 9;
		}
		nes_psg_c4_sr >>= 1;
	    }
	    nes_psg_c4_index &= 0x1fffffff;
#if 1
	    if (nes_psg_c4_atl && (nes_psg_c4_sr & 1)) {
		wave_buffers[3][i] = nes_psg_c4_vol;
	    } else {
		wave_buffers[3][i] = 0;
	    }
#else
	    wave_buffers[3][i] = 0;
#endif
	} else {
	    wave_buffers[3][i] = 0;
	}
    }
    if (nes_psg_c4_atl) {
	nes_psg_c4_atl--;
    }

    nes_psg_ctrl_new = ctrl;
#endif
}
#endif

int length_counter_clock;

void nes_psg_frame(void)
{
#ifdef SOUND
    nes_psg_wave_1();
    nes_psg_wave_2();
    nes_psg_wave_3();
    nes_psg_wave_4();

    if (!length_counter_clock--) {
	length_counter_clock = 3; /* FIXME: This may want to be 3 */

	if (!(nes_psg_c1.a & 0x20)) {
	    if (wave_1_length_counter) {
		wave_1_length_counter--;
	    }
	}

	if (!(nes_psg_c2.a & 0x20)) {
	    if (wave_2_length_counter) {
		wave_2_length_counter--;
	    }
	}

	if (!(nes_psg_c3.a & 0x80)) {
	    if (wave_3_length_counter) {
		wave_3_length_counter--;
	    }
	}
    }
    
    snd_output_4_waves(nes_psg_samples_per_sync, wave_buffers[0], wave_buffers[1], wave_buffers[2], wave_buffers[3]);
#endif
}

void nes_psg_init(void)
{
#ifdef SOUND
    snd_init();
/*    wave_buffers[0]=wave_buffers_0;
    wave_buffers[1]=wave_buffers_1;
    wave_buffers[2]=wave_buffers_2;
    wave_buffers[3]=wave_buffers_3;*/
    nes_psg_quality=1;
    if (nes_psg_quality > 0) {
	if (nes_psg_quality > 2) {
	    nes_psg_quality = 1;
	} else {
	    nes_psg_quality--;
	}
	nes_psg_pulse_magic = nes_psg_qual[nes_psg_quality].pulse_magic;
	nes_psg_triangle_magic = nes_psg_qual[nes_psg_quality].triangle_magic;
	nes_psg_noise_magic = nes_psg_qual[nes_psg_quality].noise_magic;
	nes_psg_samples_per_sync = nes_psg_qual[nes_psg_quality].samples_per_sync;
	nes_psg_cycles_per_sample = nes_psg_qual[nes_psg_quality].cycles_per_sample;
	nes_psg_sample_rate = nes_psg_qual[nes_psg_quality].sample_rate;
	
	snd_open(nes_psg_samples_per_sync, nes_psg_sample_rate);
    } else {
    }
#endif
}

void nes_psg_done(void)
{
#ifdef SOUND
    snd_close();
#endif
}

/*
 * $Log: nes_psg.c,v $
 * Revision 1.50  2000/11/18 14:56:57  nyef
 * cleaned up envelope_run_counter() and the sweep unit code
 *
 * Revision 1.49  2000/10/30 22:24:09  nyef
 * made wave_buffers[][] static to work around linker bug
 *
 * Revision 1.48  2000/10/05 07:57:10  nyef
 * fixed another "minor" bug triggered when -DSOUND isn't present
 *
 * Revision 1.47  2000/10/05 07:52:55  nyef
 * fixed minor bug with compiling without -DSOUND
 *
 * Revision 1.46  2000/10/02 16:35:40  nyef
 * fixed the sweep unit not to run if the shift count is 0
 *
 * Revision 1.45  2000/10/02 16:32:04  nyef
 * extracted frequency sweep emulation from the pulse wave functions
 *
 * Revision 1.44  2000/10/02 16:08:13  nyef
 * moved volume envelope handling out of nes_psg_wave_?() to new functions
 *
 * Revision 1.43  2000/10/02 15:19:50  nyef
 * halved sweep clock rate (oops)
 *
 * Revision 1.42  2000/10/02 14:17:55  nyef
 * gutted and reimplemented almost everything
 *
 * Revision 1.41  2000/05/06 23:25:47  nyef
 * moved #define SOUND out to the Makefile level
 *
 * Revision 1.40  2000/05/06 22:29:11  nyef
 * fixed to compile with no sound code in the executable
 *
 * Revision 1.39  1999/10/31 14:23:53  nyef
 * fixed to compile with sound disabled
 *
 * Revision 1.38  1999/10/31 02:37:28  nyef
 * broke all os-dependant sound code out to separate files
 *
 * Revision 1.37  1999/10/31 00:23:53  nyef
 * rearranged some stuff in nes_psg_vsync()
 *
 * Revision 1.36  1999/10/30 23:58:16  nyef
 * moved the $4015 handling code from nes_psg_mix() to the wave renderers
 *
 * Revision 1.35  1999/08/07 01:05:35  nyef
 * removed O_NONBLOCK from linux sound open routine
 * (this should prevent games from going too fast)
 *
 * Revision 1.34  1999/07/24 01:36:29  nyef
 * Fixed noise channel to be far more correct
 * Enabled noise channel, it's worth it now
 *
 * Revision 1.33  1999/02/14 18:27:54  nyef
 * added a function pointer array for writing the sound regs
 *
 * Revision 1.32  1999/01/17 04:21:11  nyef
 * fixed some problems with disabling sound completely
 *
 * Revision 1.31  1998/12/28 04:26:18  nyef
 * added simple fix for "long note" problem on wave 3.
 * full volume support on wave 3 is nessecary for proper fix.
 *
 * Revision 1.30  1998/12/28 04:10:50  nyef
 * added disabled preliminary wave 4 generation.
 *
 * Revision 1.29  1998/12/27 04:57:34  nyef
 * added basic framework for wave 4 generation.
 *
 * Revision 1.28  1998/12/27 01:57:18  nyef
 * added support for volume control and looped sounds.
 *
 * Revision 1.27  1998/12/17 06:22:19  nyef
 * cleaned up wave generation functions slightly.
 *
 * Revision 1.26  1998/12/12 20:33:07  nyef
 * commented out "DONT_USE_CAL" as final step in bringing up to spec.
 *
 * Revision 1.25  1998/12/12 02:46:42  nyef
 * fixed bug in nes_psg_open_sound_dos().
 *
 * Revision 1.24  1998/12/06 04:15:56  nyef
 * added preliminary sound quality (playback rate) adjustment.
 *
 * Revision 1.23  1998/12/05 23:32:14  nyef
 * halved the frequency on the triangle waves.
 *
 * Revision 1.22  1998/12/05 18:43:03  nyef
 * active time length counters improved. Dungeon Magic sounds much better.
 *
 * Revision 1.21  1998/12/05 18:23:51  nyef
 * changed from 16.16 fixed point to 8.24 fixed point. Due to some loss of
 * precision in the conversion, the "sound constant" needs to be recalculated.
 *
 * Revision 1.20  1998/12/05 05:25:48  nyef
 * added pulse wave duty cycles. don't know how well they work.
 *
 * Revision 1.19  1998/12/04 02:55:10  nyef
 * started to bring io write functions up to spec. maintaining backward
 * compatability for ease of transition and for RockNES.
 *
 * Revision 1.18  1998/12/04 02:08:36  nyef
 * changed wave buffer 'triangle' to 'triangle_50' because allegro already
 * defines 'triangle'.
 *
 * Revision 1.17  1998/12/03 04:58:59  nyef
 * major overhaul. changed sound rendering method. system sounds a lot
 * better. channel 3 approaches triangularity for sufficiently large values
 * of 32. some hooks for duty cycles in place. active time length counters
 * improved. floating point stuff removed. improved event handling.
 *
 * Revision 1.16  1998/12/01 04:01:12  nyef
 * obtained speedup by storing the value (44100 / 111860.78) * freq in a
 * local variable instead of calculating it every time it was needed.
 *
 * Revision 1.15  1998/12/01 03:34:15  nyef
 * more doc fixes (mainly adding FIXMEs). added preliminary active time left
 * support (I don't know if I did it right). more bugfixes.
 *
 * Revision 1.14  1998/12/01 02:57:48  nyef
 * doc fix. removed useless debug code from psg write functions. added
 * preliminary wave 3 support (needs to be changed to triangle wave).
 * fixed a few bugs in the wave functions.
 *
 * Revision 1.13  1998/11/28 05:06:40  nyef
 * bashed up linux sound stuff again. reintroduced soundfrags. changed to
 * use 1024 byte writes with dual local buffers.
 *
 * Revision 1.12  1998/11/26 01:39:54  nyef
 * gratuitous changes. mainly linux latency stuff.
 *
 * Revision 1.11  1998/11/05 02:58:31  nyef
 * added preliminary emulation of psg control register.
 *
 * Revision 1.10  1998/11/05 02:29:34  nyef
 * integrated new dos code changes. sound output now works on dos.
 *
 * Revision 1.9  1998/11/04 03:25:45  nyef
 * moved the code to open the sound device from nes_psg_init() to
 * nes_psg_open_sound_linux().
 *
 * Revision 1.8  1998/11/04 03:20:16  nyef
 * added (commented out) defines for SOUND_LINUX and SOUND_DOS.
 * made inclusion of system header files required for compile with
 * sound support conditional upon definition of SOUND_LINUX.
 *
 * Revision 1.7  1998/11/04 03:10:08  nyef
 * changed all psg event type defines to hexadecimal.
 * added psg event type PSGET_SYNC.
 *
 * Revision 1.6  1998/11/03 00:43:26  nyef
 * Added preliminary wave 2 generation.
 *
 * Revision 1.5  1998/10/21 00:15:33  nyef
 * commented out the define for sound.
 * changed some of the debug output in nes_psg_wave_1().
 *
 * Revision 1.4  1998/09/19 00:48:25  nyef
 *   wave 1 generation now working (the video display and rendering must be
 * disabled for nes_ppu.c for it not to skip on my machine). started
 * instrumenting wave 1 control regs for furthur insight into wave generation.
 *
 * Revision 1.3  1998/09/02 01:25:40  nyef
 * added test output of wave 1. wave 1 generation still not working.
 *
 * Revision 1.2  1998/08/30 20:37:57  nyef
 * added event queue, filled in the write functions, and added vsync hook.
 *
 * Revision 1.1  1998/08/22 00:35:43  nyef
 * Initial revision
 *
 */
