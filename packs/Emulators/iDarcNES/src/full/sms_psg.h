/*
 * sms_psg.h
 *
 * SMS sound emulation
 */

/* $Id: sms_psg.h,v 1.1 1999/04/28 13:45:32 nyef Exp $ */

#ifndef SMS_PSG_H
#define SMS_PSG_H


void sms_psg_init(void);
void sms_psg_done(void);

void sms_psg_vsync(void);

void sms_psg_write(unsigned char data);

/*
 * nes_psg_quality is used to control the sound playback rate.
 * anything < 1 disables sound.
 * 1 is 22050 Hz.
 * 2 is 44100 Hz.
 * anything > 2 is 44100 Hz.
 * these values subject to change without notice.
 */
extern int nes_psg_quality;


#endif /* SMS_PSG_H */

/*
 * $Log: sms_psg.h,v $
 * Revision 1.1  1999/04/28 13:45:32  nyef
 * Initial revision
 *
 */
