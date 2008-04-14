/*
 * snd_allegro.c
 *
 * allegro sound interface
 */

/* $Id: snd_allegro.c,v 1.2 1999/10/31 14:43:02 nyef Exp $ */

#include <allegro.h>

#include "ui.h"
#include "snd.h"

AUDIOSTREAM *sound_stream;

void snd_output_4_waves(int samples, u8 *wave1, u8 *wave2, u8 *wave3, u8 *wave4)
{
    unsigned char *final_wave;
    int i;

    if (sound_stream) {
	final_wave = NULL;
    
	while (!final_wave) { /* Wheee... Primitive Framesync. :-) */
	    final_wave = get_audio_stream_buffer(sound_stream);
	}
    
	for (i = 0; i < samples; i++) {
	    final_wave[i] = (wave1[i] + wave2[i] + wave3[i] + wave4[i]) >> 2;
	}
    
	free_audio_stream_buffer(sound_stream);
    }
}

void snd_init(void)
{
    sound_stream = NULL;
}

int snd_open(int samples_per_sync, int sample_rate)
{
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
	deb_printf("Error initializing sound system\n%s\n", allegro_error);
	return 0;
    } else {
	sound_stream = play_audio_stream(samples_per_sync, 8, sample_rate, 255, 128);
	if (!sound_stream) {
	    deb_printf("unable to create audio stream.\n");
	    return 0;
	}
    }
    return 1;
}

void snd_close(void)
{
    if (sound_stream) {
	stop_audio_stream(sound_stream);
    }
}

/*
 * $Log: snd_allegro.c,v $
 * Revision 1.2  1999/10/31 14:43:02  nyef
 * fixed to compile without warnings
 * fixed to only stop the audio stream if we actually had one
 *
 * Revision 1.1  1999/10/31 02:37:46  nyef
 * Initial revision
 *
 */
