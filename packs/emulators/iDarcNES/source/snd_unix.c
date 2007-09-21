/*
 * snd_unix.c
 *
 * UNIX(tm) sound interface (mainly OSS)
 */

/* $Id: snd_unix.c,v 1.4 2000/06/25 17:04:32 nyef Exp $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "cop.h"
#include "ui.h"
#include "snd.h"
#include "unixdep.h"

#include <sys/soundcard.h>
#define SOUND_DEVICE "/dev/dsp"
#ifdef SYSTEM_LINUX
#endif

#ifdef SYSTEM_FREEBSD
#include <machine/soundcard.h>
#define SOUND_DEVICE "/dev/dsp"
#endif

#ifdef SYSTEM_NETBSD
#include <sys/audioio.h>
#define SOUND_DEVICE "/dev/sound"
#endif

#ifdef SYSTEM_OPENBSD
#error No sound support on OpenBSD systems yet
#endif

#define MAGIC 2048
unsigned short final_wave[8192];//=0x40015030;//+182*4*2+4;
int waveptr=0;
int wavflag=0;
int sound_fd=0;
int mixer=0;
int samples=0;
int sound_pl=0;
u8 *wave1;
u8 *wave2;
u8 *wave3;
u8 *wave4;
void snd_output_4_waves_()
{
    int i;
//    while (sound_pl!=0);
    	{

	for (i = 0; i < samples; i++) {
	    final_wave[waveptr] = (wave1[i] + wave2[i] +
				   wave3[i] + wave4[i])<<5;
	    waveptr++;
//	    if (waveptr>=1024) waveptr=0;
/*   if (waveptr == MAGIC) {
		waveptr = 0;
		wavflag = 2;
	    } else if (waveptr == 1024) {
		wavflag = 1;
	    }*/
	}
    }
	samples=0;
	sound_pl=0;
}
int getsamples()
{
	return samples;
}
extern int volume;
void snd_output_4_waves(int _samples, u8 *_wave1, u8 *_wave2, u8 *_wave3, u8 *_wave4)
{
	/*

    while (((inl(IBOY_COP_STATUS) & (IBOY_COP_LINE_REQ2|IBOY_COP_LINE_REQ)) != 0))
	 */
	samples=_samples;
	wave1=_wave1;
	wave2=_wave2;
	wave3=_wave3;
	wave4=_wave4;
     
/*    while ((inl(IBOY_COP_STATUS) & (IBOY_COP_LINE_REQ)) != 0);// && (samples!=0))
    if (((inl(IBOY_COP_STATUS) & IBOY_COP_LINE_REQ) == 0) && (samples!=0))
    {
	outl(inl(IBOY_COP_STATUS) | IBOY_COP_LINE_REQ2, IBOY_COP_STATUS); 
	outl(inl(IBOY_COP_STATUS) | IBOY_COP_LINE_REQ, IBOY_COP_STATUS); 
	sound_pl++;
    }*/
    //while ((inl(IBOY_COP_STATUS) & (IBOY_COP_LINE_REQ)) != 0);// && (samples!=0))
//    if (samples!=0) { snd_output_4_waves_(); samples=0; } 
		
	snd_output_4_waves_();
	sound_pl=0;
    if (waveptr>90*4) {
	    int n,rem;
	    rem=waveptr*2;
	    
	//n=write(sound_fd, &final_wave[(MAGIC-rem)], rem);
//        while ((inl(IBOY_COP_STATUS) & (IBOY_COP_LINE_REQ2)) != 0);// && (samples!=0))
	if (rem!=0)
	{
	n=write(sound_fd, &final_wave[waveptr*2-rem],rem);
	if (n>0) { rem-=n; }
	}
        waveptr = 0;
    }
}
void snd_init(void)
{
    sound_fd = 0;
    int i=0;
    for (i=0; i<8192; i++) final_wave[i]=0;
}

int snd_open(int samples_per_sync, int sample_rate)
{
    int tmp;
    int result;
    int sound_rate;
    int sound_frag;

    waveptr = 0;
    wavflag = 0;
    mixer=open("/dev/mixer", O_RDWR); 
    printf("opening "SOUND_DEVICE"...");
    sound_fd = open(SOUND_DEVICE, O_WRONLY);
    if (sound_fd < 0) {
	perror("failed");
	sound_fd = 0;
	return 0;
    } else {
	printf("done.\n");
    }
    
/*    printf("setting unsigned 16bit format...");
    tmp = AFMT_S16_LE;
    result = ioctl(sound_fd, SNDCTL_DSP_SETFMT, &tmp);
    if (result < 0) {
	perror("failed");
	close(sound_fd);
	sound_fd = 0;
	return 0;
    } else {
	printf("done.\n");
    }
    
    printf("setting mono mode...");
    tmp = 0;
	    result = ioctl(sound_fd, SNDCTL_DSP_CHANNELS, &tmp);
    if (result < 0) {
	perror("failed");
	close(sound_fd);
	sound_fd = 0;
	return 0;
    } else {
	printf("done.\n");
    }
*/    
    sound_rate = sample_rate;
    printf("setting sound rate to %dHz...", sound_rate);
    result = ioctl(sound_fd, SNDCTL_DSP_SPEED, &sound_rate);
    if (result < 0) {
	perror("failed");
	close(sound_fd);
	sound_fd = 0;
	return 0;
    } else {
	printf("got %dHz...done.\n", sound_rate);
    }

    /* high word of sound_frag is number of frags, low word is frag size */
/*    sound_frag = 0x00010400;
    printf("setting soundfrags...");
    result = ioctl(sound_fd, SNDCTL_DSP_SETFRAGMENT, &sound_frag);
    if (result < 0) {
	perror("failed");
	close(sound_fd);
	sound_fd = 0;
	return 0;
    } else {
	printf("done.\n");
    }*/

    return 1;
}

void snd_close(void)
{
    if (sound_fd) {
	close(sound_fd);
    }
}

/*
 * $Log: snd_unix.c,v $
 * Revision 1.4  2000/06/25 17:04:32  nyef
 * fixed to automatically detect system type
 *
 * Revision 1.3  2000/02/14 02:04:13  nyef
 * addded (untested) support for NetBSD
 *
 * Revision 1.2  1999/12/24 16:52:42  nyef
 * changed to compile clean on FreeBSD
 *
 * Revision 1.1  1999/10/31 02:37:58  nyef
 * Initial revision
 *
 */
