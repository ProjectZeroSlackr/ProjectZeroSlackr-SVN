/*
 * Audio Generator
 * Copyright (C) 2005 Dilip Panicker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * problems I know about: 
 * 	audible samp error clicks at high frequencies
 * Any bugs please mail me:
 * onglipo (at) gmail.com
 *
 * Key use:
 * wheel -> [frequency | amplitude | note ] up/down
 * action -> [frequency inc/dec by 1/10/100 Hz, note inc/dec by 1 semitone/ 5 semitones]
 * fwd/rev -> mode change between [freq | volume | notes]
 * play/pause -> play/pause
 * menu -> quit
 * 
 * Thanks to filcab for suggestions and parts of code
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include "ipod.h"
#include "pz.h"
#include "oss.h"
#include "generator.h"	

static GR_WINDOW_ID generator_wid;
static GR_GC_ID generator_gc;
static dsp_st dspz;

static short *lookup_table; //quarter wave store
void make_lookup_table()
{
	int i, fd, version;
	double minOmega ;
	char buf[16];
	int len;
	char filename[64];

	strcpy(filename, SINE_TABLE_DIR);
	strcat(filename, SINE_TABLE_FILE);
	lookup_table = (short *) malloc(MAX_LOOKUP_SAMPLES*sizeof(short));
	if(lookup_table == NULL) {
		pz_error("Out of memory!");
		generator_destruct();
	}
	fd = open(filename, O_RDONLY);
	if (fd>=0) {	//if file exists, read it
		if (read(fd, &version, sizeof(int)) == sizeof(int)) {
			if (version == SINE_TABLE_VERSION) {
				len = read(fd, lookup_table, MAX_LOOKUP_SAMPLES*sizeof(short));
				close(fd);
				if(len == MAX_LOOKUP_SAMPLES*sizeof(short))
					return; 
			}
		}
       }
	mkdir(SINE_TABLE_DIR, 0777);
	fd = open(filename, O_RDWR |O_CREAT | O_TRUNC);
	for (i = 0; i < MAX_LOOKUP_SAMPLES; i++) {
		minOmega = (M_PI_2* i)/MAX_LOOKUP_SAMPLES; 
		lookup_table[i] = ((1.0+sin(minOmega))*ZERO_SAMPLE_VALUE);
		if((i%64)==0)	{
			sprintf(buf, "sin:%d", i);
			generator_draw_text(buf);
		}
	}
	if (fd != -1) {
		version = SINE_TABLE_VERSION;
		write(fd, &version, sizeof(int));
		write(fd, lookup_table, sizeof(short)* MAX_LOOKUP_SAMPLES);
		close(fd); 
	}
}
// to speed up sin(x) calculation, we use a look-up table
// ipod would die otherwise, calculating this in real time
short fast_sine(int sample_number) // takes 0 - SAMPLING_RATE-1
{
	if(sample_number < QUARTER_WAVE) // Quadrant Q1
		return(lookup_table[sample_number]);
	if (sample_number < HALF_WAVE) // Q2 
		return(lookup_table[HALF_WAVE-sample_number-1]);
	// > 180, we need the -ve portion (0 - ZERO_SAMPLE_VALUE)
	if(sample_number == HALF_WAVE)
		return(ZERO_SAMPLE_VALUE);
	return ((2*ZERO_SAMPLE_VALUE) - fast_sine(HALF_WAVE -(sample_number-HALF_WAVE)-1)) ;
}
//****************************************************************************
// DRAWING FUNCTIONS
// UI is mostly from metronome by Stephen Lawrence (Iklop)
//****************************************************************************

// Draws number sent over; clears the entire screen before drawing
// Status: Done
void generator_draw_number( int num )
{
	int i;	//used for looping	
	int width = (num > 999) ? 66 : (num > 99) ? 49 : (num > 9) ? 32 :  15;	// width of drawn numbers
	int offset = (screen_info.cols - width) / 2;	// x pos of numbers
	int top = 10;	// y pos of numbers
	char str[sizeof(int)+1];
	
	GrSetGCForeground( generator_gc, WHITE );
	GrFillRect( generator_wid, generator_gc, 0, top, screen_info.cols, 25 );
	
	sprintf( str, "%d", num );
	
	for( i=0; i<4; i++ ) {
		switch( str[i] ) {
			case '0':	generator_zero( offset, top );		break;	
			case '1':	generator_one( offset, top );		break;
			case '2':	generator_two( offset, top );		break;
			case '3':	generator_three( offset, top );		break;
			case '4':	generator_four( offset, top );		break;
			case '5':	generator_five( offset, top );		break;
			case '6':	generator_six( offset, top );		break;
			case '7':	generator_seven( offset, top );		break;
			case '8':	generator_eight( offset, top );		break;
			case '9':	generator_nine( offset, top );		break;
			case '-':	generator_dash( offset, top );		break;			
		}

		offset += 17;	//increment the offset of one letter width + space
	}
}


//displays the type of wave being played or PAUSED
void generator_draw_text(char *text )
{
	int width;
	
	width = strlen(text)*7; 			

	//erase the previous 
	GrSetGCForeground( generator_gc, WHITE );
	GrFillRect( generator_wid, generator_gc, 0, 40, screen_info.cols, 30 );
	
	//draw  
	GrSetGCForeground( generator_gc, BLACK );
	GrText( generator_wid, generator_gc, (screen_info.cols-width)/2, 60, text, -1, GR_TFASCII );

}

void generator_draw_volume()
{

	//erase just the volume bar (prevent flickering)
	GrSetGCForeground( generator_gc, WHITE);
	GrFillRect( generator_wid, generator_gc, 10, screen_info.rows - HEADER_TOPLINE - 16, screen_info.cols - 20, 8 );

	if( generator_mode==GENERATOR_MODE_VOLUME) {
		//get the volume
		int volume = dsp_get_volume(&dspz);	
		//calculate the bar length for the volume
		int volume_length = ((screen_info.cols - 22) * volume) / 100; 

		//draw the volume border
		GrSetGCForeground( generator_gc, BLACK);
		GrRect( generator_wid, generator_gc, 10, screen_info.rows - HEADER_TOPLINE - 16, screen_info.cols - 20, 8 );
		
		//draw the volume inside
		GrSetGCForeground( generator_gc, GRAY);		
		GrFillRect( generator_wid, generator_gc, 11, screen_info.rows - HEADER_TOPLINE - 15, volume_length, 6 );
	}
}

void generator_display_refresh()
{
	char buf[18];
	char tmp[10];

	generator_draw_number( generator_freq ); // always disply current frequency
	generator_draw_volume();	
	if (generator_paused)	
		strcpy(buf, "PAUSE ");
	else
		strcpy(buf, "PLAY ");
	switch (generator_mode) {
		case GENERATOR_MODE_FREQ:
			sprintf(tmp, "TONE +%.3d", generator_freq_delta);
			break;
		case GENERATOR_MODE_VOLUME:
			strcpy(tmp, "VOLUME");
			break;
		case GENERATOR_MODE_NOTES:
			sprintf(tmp, "%s +%.1d", generator_note_names[generator_cur_note], generator_notes_delta);
			break;
		default:
			break;
	}
	strcat(buf, tmp);
	generator_draw_text(buf);
}


// Called on exposure event
static void generator_do_draw()
{
    pz_draw_header( "Tone Generator" );
    generator_display_refresh();
}


static short wave_buffer [MAX_SAMPLES+1];
// number of samples calculated = SAMPLINGRATE/freq samples less
// which gives integeir # of  full waves
// n = prepares 1 sin wave data for freq in wave_buffer
// returns # of samples in the buffer 
// this should have a table look up for sin(x) : tbd
int make_wave_buffer(int freq)
{

	int i, nsamples, theta, waves_per_buffer	;
	double  samples_per_wave; 

	samples_per_wave = 1.0*SAMPLING_RATE / freq; // not integral!
	waves_per_buffer = (MAX_SAMPLES/samples_per_wave); // integer
	nsamples = (samples_per_wave * waves_per_buffer)+0.5; 
	
	for (i=0; i< nsamples; i++) {
		theta  = 1.0*SAMPLING_RATE*(double) i/samples_per_wave;
		if(theta >= SAMPLING_RATE)
			theta %= SAMPLING_RATE;
		wave_buffer[i] = fast_sine(theta);
	}
	return nsamples;
}


//****************************************************************************
// MAIN PROGRAM FUNCTIONS
//****************************************************************************

void generator_construct()
{
	int frag;
	dsp_open(&dspz, DSP_LINEOUT);
    	frag = (16 << 16) | 8;	// 16 fragments of 256 bytes each : Not sure if this is implemented for ipod
    	ioctl (dspz.dsp, SNDCTL_DSP_SETFRAGMENT, &frag);	//for better realtime response 
	dsp_setup(&dspz, 1, SAMPLING_RATE);

	generator_freq = START_FREQ;
	generator_mode = GENERATOR_MODE_FREQ;
	generator_quit_app = 0 ;
	generator_paused = 1;
	make_lookup_table();	//make fast_sine() table
	generator_display_refresh();
}

void generator_button_lr(int dir) //dir = +1 for right, -1 for left, 0 will refresh all variables;
{
	switch( generator_mode ) {
		case GENERATOR_MODE_VOLUME: 
    			dsp_vol_change(&dspz, dir*3);
    			break;
    		
    		case GENERATOR_MODE_FREQ: 
   			generator_freq += dir * generator_freq_delta;
			if(generator_freq > FREQUENCY_MAX)
				generator_freq = FREQUENCY_MAX;
			if(generator_freq < FREQUENCY_MIN)
				generator_freq = FREQUENCY_MIN;
    			break;
		case GENERATOR_MODE_NOTES: 
			generator_cur_note = (dir*generator_notes_delta+generator_cur_note+GENERATOR_NUM_NOTES) % GENERATOR_NUM_NOTES;
			generator_freq = generator_notes[generator_cur_note];
			break;     		
	}
	generator_display_refresh();
}


void generator_button_play()
{
	generator_paused = generator_paused ? 0 : 1 ; 
 	generator_display_refresh();
	if (!generator_paused)	
		generate_tone();
}

void generator_button_action()
{
	switch(generator_mode) {
		 case GENERATOR_MODE_FREQ: 
			generator_freq_delta =	(generator_freq_delta == 1) ? 10 : 
						(generator_freq_delta == 10) ? 100 : 1;
			generator_display_refresh();	
			break;
		case GENERATOR_MODE_NOTES:
			generator_notes_delta =	(generator_notes_delta == 1) ? 5 :  1;
			generator_display_refresh();	
		case GENERATOR_MODE_VOLUME:
		default:
		;
	}
}



void generator_button_forward()
{
	generator_mode = (generator_mode+1) % GENERATOR_NUM_MODES;
	generator_button_lr(0); //refresh frequency etc...
}

void generator_button_rewind()
{
	generator_mode = (generator_mode - 1 + GENERATOR_NUM_MODES) % GENERATOR_NUM_MODES;
	generator_button_lr(0);	
}

void generator_destruct()
{
	if(lookup_table != NULL) 
		free(lookup_table);
	dsp_close(&dspz);
	pz_close_window( generator_wid );
	GrDestroyGC( generator_gc );
}
void generator_button_menu()
{
	if(!generator_paused) {
		generator_paused = 1;
		generator_quit_app = 1;
		return ; 
	}
	generator_destruct();
}
void generate_tone()
{
	int nsamples;
	GR_EVENT event;
	int freq = 0;
//	struct audio_buf_info info;
//	char buf[20];

	while(!generator_paused) {
		if(generator_freq != freq) {
			freq = generator_freq;
			nsamples = make_wave_buffer(generator_freq); 
//add wave types here - tbd
		}
// we need to use ioctl to get available space and ensure we dont write too much to improve response
// times while changing freq/notes. to be done
//        	ioctl(dspz.dsp, SNDCTL_DSP_GETOSPACE, &info);
//		sprintf(buf, "f:%d, s/f: %d", info.fragstotal, info.fragsize);
//		sprintf(buf, "afs: %d, ab: %d", info.fragments, info.bytes);
//		generator_draw_text(buf);
		
		dsp_write( &dspz, wave_buffer, sizeof(short)*nsamples);	// dont write block
		if(GrPeekEvent(&event)) {
repeat:
			GrGetNextEventTimeout(&event, 1000);
			if (event.type != GR_EVENT_TYPE_TIMEOUT) {
				pz_event_handler(&event);
				if (GrPeekEvent(&event)) 
					goto repeat;
			}
		}
	}
	if (generator_quit_app)	// menu button in the middle of a play
		generator_button_menu();//get out of the app
		// better way to get out of the event handler - tbd
}

//****************************************************************************
// EVENT HANDLERS
//****************************************************************************

// Nornal Mode Event Handler
// Status: Done
static int generator_normal_handle( GR_EVENT *event )
{
	int ret = 0;
	
    switch (event->type) {
    	case GR_EVENT_TYPE_KEY_DOWN:
	        switch (event->keystroke.ch) {
		        case IPOD_WHEEL_CLOCKWISE: 
				generator_button_lr(1);
				break;
       			case IPOD_WHEEL_ANTICLOCKWISE:
				generator_button_lr(-1);
				break;
       			case IPOD_BUTTON_PLAY:
				generator_button_play();
				break;
       			case IPOD_BUTTON_ACTION:
				generator_button_action();
				break;
       			case IPOD_BUTTON_MENU:
				generator_button_menu();
				break;
			case IPOD_BUTTON_FORWARD:
				generator_button_forward();
				break;
       			case IPOD_BUTTON_REWIND:
				generator_button_rewind();
				break;
       			case IPOD_SWITCH_HOLD: 
       			default:
       			    ret |= KEY_UNUSED;
       			    break;
       		}
	        break;
        
    	case GR_EVENT_TYPE_KEY_UP:
    	    switch (event->keystroke.ch) {
    		    default:
    		        ret |= KEY_UNUSED;
    		        break;
    	    }
	
	    default:
    	    ret |= EVENT_UNUSED; //unused event
    }
    return ret;
}


// Main Event Handler, decides which mode to go into
// Status: Done
static int generator_handle_event( GR_EVENT *event )
{
    int ret = 0;

	ret = generator_normal_handle( event );
	
	return ret;
	
}



//***************************************************************************
// PROGRAM ENTERS HERE
//***************************************************************************

// Creates window
// Status: Done
void new_generator_window (void)
{
	// Get a copy of the root graphics context
	generator_gc = pz_get_gc( 1 );   
	
	// Set foreground color
	GrSetGCUseBackground( generator_gc, GR_FALSE );
	GrSetGCForeground( generator_gc, BLACK);

	// Open the window
	generator_wid = pz_new_window (0, HEADER_TOPLINE + 1,
				screen_info.cols,  
				screen_info.rows - (HEADER_TOPLINE + 1),
				generator_do_draw,
				generator_handle_event);

	// Select the types of events you need for your window:
	GrSelectEvents( generator_wid, GR_EVENT_MASK_EXPOSURE | 
				GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP );

	// Map the window to the screen:
	GrMapWindow( generator_wid );
	generator_construct();
}



//******************************************************
// BIG PRETTY NUMBERS
//******************************************************
void generator_zero( int x, int y )
{
	GR_POINT black[] = {
		{4+x,1+y}, {5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {2+x,3+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {1+x,5+y}, {2+x,5+y}, {3+x,5+y}, {4+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {13+x,5+y}, {1+x,6+y}, {2+x,6+y}, {3+x,6+y}, {11+x,6+y}, {12+x,6+y}, {13+x,6+y}, {1+x,7+y}, {2+x,7+y}, {3+x,7+y}, {11+x,7+y}, {12+x,7+y}, {13+x,7+y}, {0+x,8+y}, {1+x,8+y}, {2+x,8+y}, {3+x,8+y}, {11+x,8+y}, {12+x,8+y}, {13+x,8+y}, {0+x,9+y}, {1+x,9+y}, {2+x,9+y}, {3+x,9+y}, {11+x,9+y}, {12+x,9+y}, {13+x,9+y}, {14+x,9+y}, {0+x,10+y}, {1+x,10+y}, {2+x,10+y}, {3+x,10+y}, {11+x,10+y}, {12+x,10+y}, {13+x,10+y}, {14+x,10+y}, {0+x,11+y}, {1+x,11+y}, {2+x,11+y}, {3+x,11+y}, {11+x,11+y}, {12+x,11+y}, {13+x,11+y}, {14+x,11+y}, {0+x,12+y}, {1+x,12+y}, {2+x,12+y}, {3+x,12+y}, {11+x,12+y}, {12+x,12+y}, {13+x,12+y}, {0+x,13+y}, {1+x,13+y}, {2+x,13+y}, {3+x,13+y}, {11+x,13+y}, {12+x,13+y}, {13+x,13+y}, {14+x,13+y}, {0+x,14+y}, {1+x,14+y}, {2+x,14+y}, {3+x,14+y}, {11+x,14+y}, {12+x,14+y}, {13+x,14+y}, {1+x,15+y}, {2+x,15+y}, {3+x,15+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {1+x,16+y}, {2+x,16+y}, {3+x,16+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {1+x,17+y}, {2+x,17+y}, {3+x,17+y}, {4+x,17+y}, {10+x,17+y}, {11+x,17+y}, {12+x,17+y}, {13+x,17+y}, {1+x,18+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {9+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {4+x,21+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,0+y}, {1+x,4+y}, {5+x,4+y}, {13+x,4+y}, {4+x,6+y}, {10+x,6+y}, {0+x,7+y}, {14+x,7+y}, {14+x,8+y}, {14+x,12+y}, {14+x,14+y}, {0+x,15+y}, {14+x,15+y}, {0+x,16+y}, {10+x,16+y}, {13+x,18+y}, {2+x,20+y}
	};
	
	GR_POINT grey[] = {
		{5+x,0+y}, {6+x,0+y}, {8+x,0+y}, {9+x,0+y}, {3+x,1+y}, {11+x,1+y}, {2+x,2+y}, {12+x,2+y}, {1+x,3+y}, {13+x,3+y}, {6+x,4+y}, {8+x,4+y}, {0+x,5+y}, {0+x,6+y}, {14+x,6+y}, {4+x,7+y}, {10+x,7+y}, {10+x,14+y}, {4+x,15+y}, {10+x,15+y}, {4+x,16+y}, {14+x,16+y}, {0+x,17+y}, {9+x,17+y}, {6+x,18+y}, {8+x,18+y}, {1+x,19+y}, {13+x,19+y}, {12+x,20+y}, {3+x,21+y}, {11+x,21+y}, {5+x,22+y}, {6+x,22+y}, {7+x,22+y}, {8+x,22+y}, {9+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 163, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 17, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 36, grey );
	
}


void generator_one( int x, int y )
{
	GR_POINT black[] = {
		{5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {7+x,4+y}, {8+x,4+y}, {9+x,4+y}, {10+x,4+y}, {7+x,5+y}, {8+x,5+y}, {9+x,5+y}, {10+x,5+y}, {7+x,6+y}, {8+x,6+y}, {9+x,6+y}, {10+x,6+y}, {7+x,7+y}, {8+x,7+y}, {9+x,7+y}, {10+x,7+y}, {7+x,8+y}, {8+x,8+y}, {9+x,8+y}, {10+x,8+y}, {7+x,9+y}, {8+x,9+y}, {9+x,9+y}, {10+x,9+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {7+x,12+y}, {8+x,12+y}, {9+x,12+y}, {10+x,12+y}, {7+x,13+y}, {8+x,13+y}, {9+x,13+y}, {10+x,13+y}, {7+x,14+y}, {8+x,14+y}, {9+x,14+y}, {10+x,14+y}, {7+x,15+y}, {8+x,15+y}, {9+x,15+y}, {10+x,15+y}, {7+x,16+y}, {8+x,16+y}, {9+x,16+y}, {10+x,16+y}, {7+x,17+y}, {8+x,17+y}, {9+x,17+y}, {10+x,17+y}, {7+x,18+y}, {8+x,18+y}, {9+x,18+y}, {10+x,18+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{3+x,3+y}, {3+x,4+y}, {4+x,4+y}, {5+x,4+y}, {6+x,4+y}
	};
	
	GR_POINT grey[] = {
		{4+x,1+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 92, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 5, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 1, grey );
}

void generator_two( int x, int y )
{
	GR_POINT black[] = {
		{5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {5+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {2+x,5+y}, {3+x,5+y}, {4+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {13+x,5+y}, {1+x,6+y}, {2+x,6+y}, {3+x,6+y}, {4+x,6+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {13+x,6+y}, {1+x,7+y}, {2+x,7+y}, {3+x,7+y}, {4+x,7+y}, {10+x,7+y}, {11+x,7+y}, {12+x,7+y}, {13+x,7+y}, {10+x,8+y}, {11+x,8+y}, {12+x,8+y}, {10+x,9+y}, {11+x,9+y}, {12+x,9+y}, {9+x,10+y}, {10+x,10+y}, {11+x,10+y}, {12+x,10+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {8+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {7+x,13+y}, {8+x,13+y}, {9+x,13+y}, {10+x,13+y}, {6+x,14+y}, {7+x,14+y}, {8+x,14+y}, {9+x,14+y}, {5+x,15+y}, {6+x,15+y}, {7+x,15+y}, {8+x,15+y}, {4+x,16+y}, {5+x,16+y}, {6+x,16+y}, {7+x,16+y}, {3+x,17+y}, {4+x,17+y}, {5+x,17+y}, {6+x,17+y}, {7+x,17+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {6+x,18+y}, {1+x,19+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {13+x,19+y}, {0+x,20+y}, {1+x,20+y}, {2+x,20+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {12+x,20+y}, {13+x,20+y}, {0+x,21+y}, {1+x,21+y}, {2+x,21+y}, {3+x,21+y}, {4+x,21+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}, {11+x,21+y}, {12+x,21+y}, {13+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,0+y}, {4+x,1+y}, {2+x,3+y}, {13+x,4+y}, {1+x,5+y}, {13+x,8+y}, {13+x,9+y}, {8+x,11+y}, {12+x,11+y}, {9+x,15+y}, {8+x,16+y}, {7+x,18+y}, {8+x,18+y}, {9+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {13+x,18+y}
	};
	
	GR_POINT grey[] = {
		{6+x,0+y}, {8+x,0+y}, {9+x,0+y}, {3+x,1+y}, {11+x,1+y}, {12+x,2+y}, {1+x,4+y}, {6+x,4+y}, {7+x,4+y}, {8+x,4+y}, {5+x,5+y}, {9+x,5+y}, {9+x,9+y}, {7+x,12+y}, {6+x,13+y}, {11+x,13+y}, {5+x,14+y}, {10+x,14+y}, {4+x,15+y}, {3+x,16+y}, {2+x,17+y}, {1+x,18+y}, {0+x,19+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 140, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 18, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 23, grey );
	
}

void generator_three( int x, int y )
{
	GR_POINT black[] = {
		{5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {5+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {2+x,5+y}, {3+x,5+y}, {4+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {13+x,5+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {10+x,7+y}, {11+x,7+y}, {12+x,7+y}, {9+x,8+y}, {10+x,8+y}, {11+x,8+y}, {12+x,8+y}, {6+x,9+y}, {7+x,9+y}, {8+x,9+y}, {9+x,9+y}, {10+x,9+y}, {11+x,9+y}, {6+x,10+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {6+x,11+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {8+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {12+x,12+y}, {10+x,13+y}, {11+x,13+y}, {12+x,13+y}, {13+x,13+y}, {11+x,14+y}, {12+x,14+y}, {13+x,14+y}, {14+x,14+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {14+x,15+y}, {1+x,16+y}, {2+x,16+y}, {3+x,16+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {14+x,16+y}, {1+x,17+y}, {2+x,17+y}, {3+x,17+y}, {4+x,17+y}, {11+x,17+y}, {12+x,17+y}, {13+x,17+y}, {1+x,18+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {13+x,18+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {13+x,19+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,0+y}, {4+x,1+y}, {2+x,3+y}, {9+x,4+y}, {13+x,4+y}, {1+x,5+y}, {13+x,6+y}, {13+x,7+y}, {8+x,8+y}, {12+x,9+y}, {12+x,11+y}, {6+x,12+y}, {7+x,12+y}, {13+x,12+y}, {2+x,15+y}, {4+x,16+y}, {14+x,17+y}, {9+x,18+y}, {2+x,20+y}, {12+x,20+y}, {4+x,21+y}, {11+x,21+y}
	};
	
	GR_POINT grey[] = {
		{6+x,0+y}, {8+x,0+y}, {9+x,0+y}, {11+x,1+y}, {2+x,2+y}, {12+x,2+y}, {13+x,3+y}, {1+x,4+y}, {6+x,4+y}, {7+x,4+y}, {8+x,4+y}, {5+x,5+y}, {9+x,7+y}, {13+x,8+y}, {11+x,10+y}, {9+x,13+y}, {14+x,13+y}, {10+x,14+y}, {0+x,15+y}, {1+x,15+y}, {3+x,15+y}, {4+x,15+y}, {0+x,16+y}, {0+x,17+y}, {10+x,17+y}, {6+x,18+y}, {7+x,18+y}, {8+x,18+y}, {14+x,18+y}, {1+x,19+y}, {3+x,21+y}, {5+x,22+y}, {6+x,22+y}, {7+x,22+y}, {8+x,22+y}, {9+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 133, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 22, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 36, grey );
	
}

void generator_four( int x, int y )
{
	GR_POINT black[] = {
		{9+x,0+y}, {10+x,0+y}, {11+x,0+y}, {12+x,0+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {11+x,1+y}, {12+x,1+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {12+x,2+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {7+x,4+y}, {8+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {6+x,5+y}, {7+x,5+y}, {8+x,5+y}, {9+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {5+x,6+y}, {6+x,6+y}, {7+x,6+y}, {8+x,6+y}, {9+x,6+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {5+x,7+y}, {6+x,7+y}, {7+x,7+y}, {9+x,7+y}, {10+x,7+y}, {11+x,7+y}, {12+x,7+y}, {4+x,8+y}, {5+x,8+y}, {6+x,8+y}, {7+x,8+y}, {9+x,8+y}, {10+x,8+y}, {11+x,8+y}, {12+x,8+y}, {3+x,9+y}, {4+x,9+y}, {5+x,9+y}, {6+x,9+y}, {9+x,9+y}, {10+x,9+y}, {11+x,9+y}, {12+x,9+y}, {3+x,10+y}, {4+x,10+y}, {5+x,10+y}, {9+x,10+y}, {10+x,10+y}, {11+x,10+y}, {12+x,10+y}, {2+x,11+y}, {3+x,11+y}, {4+x,11+y}, {5+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {12+x,11+y}, {1+x,12+y}, {2+x,12+y}, {3+x,12+y}, {4+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {12+x,12+y}, {1+x,13+y}, {2+x,13+y}, {3+x,13+y}, {9+x,13+y}, {10+x,13+y}, {11+x,13+y}, {12+x,13+y}, {0+x,14+y}, {1+x,14+y}, {2+x,14+y}, {3+x,14+y}, {4+x,14+y}, {5+x,14+y}, {6+x,14+y}, {7+x,14+y}, {8+x,14+y}, {9+x,14+y}, {10+x,14+y}, {11+x,14+y}, {12+x,14+y}, {13+x,14+y}, {14+x,14+y}, {0+x,15+y}, {1+x,15+y}, {2+x,15+y}, {3+x,15+y}, {4+x,15+y}, {5+x,15+y}, {6+x,15+y}, {7+x,15+y}, {8+x,15+y}, {9+x,15+y}, {10+x,15+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {14+x,15+y}, {0+x,16+y}, {1+x,16+y}, {2+x,16+y}, {3+x,16+y}, {4+x,16+y}, {5+x,16+y}, {6+x,16+y}, {7+x,16+y}, {8+x,16+y}, {9+x,16+y}, {10+x,16+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {14+x,16+y}, {9+x,17+y}, {10+x,17+y}, {11+x,17+y}, {12+x,17+y}, {9+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {12+x,20+y}, {9+x,21+y}, {10+x,21+y}, {11+x,21+y}, {12+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,2+y}, {6+x,4+y}, {5+x,5+y}, {4+x,7+y}, {8+x,7+y}, {2+x,10+y}, {6+x,10+y}, {0+x,13+y}, {4+x,13+y}, {0+x,17+y}, {1+x,17+y}, {2+x,17+y}, {3+x,17+y}, {4+x,17+y}, {5+x,17+y}, {6+x,17+y}, {7+x,17+y}, {8+x,17+y}, {13+x,17+y}, {14+x,17+y}
	};
	
	GR_POINT grey[] = {
		{8+x,0+y}, {6+x,3+y}, {4+x,6+y}, {3+x,8+y}, {2+x,9+y}, {7+x,9+y}, {1+x,11+y}, {5+x,12+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 159, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 20, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 8, grey );
	
}

void generator_five( int x, int y )
{
	GR_POINT black[] = {
		{5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {11+x,1+y}, {12+x,1+y}, {13+x,1+y}, {14+x,1+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {12+x,2+y}, {13+x,2+y}, {14+x,2+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {13+x,3+y}, {14+x,3+y}, {4+x,4+y}, {5+x,4+y}, {6+x,4+y}, {7+x,4+y}, {4+x,5+y}, {5+x,5+y}, {6+x,5+y}, {4+x,6+y}, {5+x,6+y}, {6+x,6+y}, {4+x,7+y}, {5+x,7+y}, {6+x,7+y}, {3+x,8+y}, {4+x,8+y}, {5+x,8+y}, {6+x,8+y}, {3+x,9+y}, {4+x,9+y}, {5+x,9+y}, {6+x,9+y}, {7+x,9+y}, {8+x,9+y}, {9+x,9+y}, {10+x,9+y}, {3+x,10+y}, {4+x,10+y}, {5+x,10+y}, {6+x,10+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {11+x,10+y}, {12+x,10+y}, {3+x,11+y}, {4+x,11+y}, {5+x,11+y}, {6+x,11+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {12+x,11+y}, {13+x,11+y}, {3+x,12+y}, {4+x,12+y}, {5+x,12+y}, {10+x,12+y}, {11+x,12+y}, {12+x,12+y}, {13+x,12+y}, {11+x,13+y}, {12+x,13+y}, {13+x,13+y}, {11+x,14+y}, {12+x,14+y}, {13+x,14+y}, {14+x,14+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {14+x,15+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {1+x,17+y}, {2+x,17+y}, {3+x,17+y}, {4+x,17+y}, {11+x,17+y}, {12+x,17+y}, {13+x,17+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {13+x,18+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{4+x,2+y}, {4+x,3+y}, {8+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {13+x,4+y}, {7+x,5+y}, {3+x,7+y}, {7+x,8+y}, {8+x,8+y}, {11+x,9+y}, {2+x,11+y}, {10+x,13+y}, {14+x,13+y}, {2+x,16+y}, {14+x,16+y}, {10+x,17+y}, {14+x,17+y}, {1+x,18+y}, {9+x,18+y}, {13+x,19+y}, {12+x,20+y}, {4+x,21+y}
	};
	
	GR_POINT grey[] = {
		{4+x,1+y}, {14+x,4+y}, {3+x,5+y}, {3+x,6+y}, {7+x,6+y}, {9+x,8+y}, {10+x,8+y}, {12+x,9+y}, {2+x,10+y}, {2+x,12+y}, {6+x,12+y}, {9+x,12+y}, {14+x,12+y}, {5+x,13+y}, {0+x,16+y}, {1+x,16+y}, {3+x,16+y}, {4+x,16+y}, {10+x,16+y}, {0+x,17+y}, {5+x,17+y}, {6+x,18+y}, {7+x,18+y}, {8+x,18+y}, {1+x,19+y}, {2+x,20+y}, {3+x,21+y}, {11+x,21+y}, {5+x,22+y}, {6+x,22+y}, {7+x,22+y}, {8+x,22+y}, {9+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 138, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 25, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 33, grey );
	
}

void generator_six( int x, int y )
{
	GR_POINT black[] = {
		{8+x,1+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {6+x,4+y}, {7+x,4+y}, {8+x,4+y}, {9+x,4+y}, {6+x,5+y}, {7+x,5+y}, {8+x,5+y}, {5+x,6+y}, {6+x,6+y}, {7+x,6+y}, {8+x,6+y}, {5+x,7+y}, {6+x,7+y}, {7+x,7+y}, {4+x,8+y}, {5+x,8+y}, {6+x,8+y}, {7+x,8+y}, {4+x,9+y}, {5+x,9+y}, {6+x,9+y}, {7+x,9+y}, {3+x,10+y}, {4+x,10+y}, {5+x,10+y}, {6+x,10+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {3+x,11+y}, {4+x,11+y}, {5+x,11+y}, {6+x,11+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {2+x,12+y}, {3+x,12+y}, {4+x,12+y}, {5+x,12+y}, {6+x,12+y}, {7+x,12+y}, {8+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {12+x,12+y}, {2+x,13+y}, {3+x,13+y}, {4+x,13+y}, {9+x,13+y}, {10+x,13+y}, {11+x,13+y}, {12+x,13+y}, {2+x,14+y}, {3+x,14+y}, {4+x,14+y}, {10+x,14+y}, {11+x,14+y}, {12+x,14+y}, {1+x,15+y}, {2+x,15+y}, {3+x,15+y}, {4+x,15+y}, {10+x,15+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {1+x,16+y}, {2+x,16+y}, {3+x,16+y}, {4+x,16+y}, {10+x,16+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {2+x,17+y}, {3+x,17+y}, {4+x,17+y}, {10+x,17+y}, {11+x,17+y}, {12+x,17+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {9+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,1+y}, {9+x,1+y}, {6+x,3+y}, {10+x,3+y}, {5+x,5+y}, {9+x,5+y}, {4+x,7+y}, {8+x,7+y}, {3+x,9+y}, {8+x,9+y}, {2+x,11+y}, {1+x,13+y}, {5+x,13+y}, {13+x,13+y}, {1+x,14+y}, {13+x,14+y}, {1+x,17+y}, {13+x,17+y}, {1+x,18+y}, {13+x,18+y}, {4+x,21+y}, {10+x,21+y}
	};
	
	GR_POINT grey[] = {
		{8+x,0+y}, {10+x,1+y}, {9+x,9+y}, {11+x,10+y}, {12+x,11+y}, {6+x,13+y}, {5+x,14+y}, {9+x,14+y}, {5+x,17+y}, {9+x,17+y}, {6+x,18+y}, {7+x,18+y}, {8+x,18+y}, {11+x,21+y}, {5+x,22+y}, {6+x,22+y}, {7+x,22+y}, {8+x,22+y}, {9+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 126, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 22, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 19, grey );
	
}

void generator_seven( int x, int y )
{
	GR_POINT black[] = {
		{1+x,1+y}, {2+x,1+y}, {3+x,1+y}, {4+x,1+y}, {5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {11+x,1+y}, {12+x,1+y}, {13+x,1+y}, {14+x,1+y}, {1+x,2+y}, {2+x,2+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {12+x,2+y}, {13+x,2+y}, {14+x,2+y}, {1+x,3+y}, {2+x,3+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {13+x,3+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {13+x,4+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {9+x,6+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {9+x,7+y}, {10+x,7+y}, {11+x,7+y}, {8+x,8+y}, {9+x,8+y}, {10+x,8+y}, {11+x,8+y}, {8+x,9+y}, {9+x,9+y}, {10+x,9+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {6+x,12+y}, {7+x,12+y}, {8+x,12+y}, {9+x,12+y}, {6+x,13+y}, {7+x,13+y}, {8+x,13+y}, {9+x,13+y}, {5+x,14+y}, {6+x,14+y}, {7+x,14+y}, {8+x,14+y}, {5+x,15+y}, {6+x,15+y}, {7+x,15+y}, {4+x,16+y}, {5+x,16+y}, {6+x,16+y}, {7+x,16+y}, {4+x,17+y}, {5+x,17+y}, {6+x,17+y}, {7+x,17+y}, {3+x,18+y}, {4+x,18+y}, {5+x,18+y}, {6+x,18+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {2+x,20+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {4+x,21+y}, {5+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{14+x,3+y}, {1+x,4+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {5+x,4+y}, {6+x,4+y}, {7+x,4+y}, {8+x,4+y}, {9+x,4+y}, {13+x,5+y}, {12+x,7+y}, {11+x,9+y}, {10+x,11+y}, {8+x,15+y}, {6+x,19+y}, {3+x,21+y}
	};
	
	GR_POINT grey[] = {
		{14+x,4+y}, {9+x,5+y}, {13+x,6+y}, {8+x,7+y}, {12+x,8+y}, {7+x,9+y}, {11+x,10+y}, {6+x,11+y}, {10+x,12+y}, {5+x,13+y}, {9+x,14+y}, {4+x,15+y}, {8+x,16+y}, {3+x,17+y}, {7+x,18+y}, {2+x,19+y}, {6+x,20+y}, {5+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 105, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 17, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 18, grey );
	
}

void generator_eight( int x, int y )
{
	GR_POINT black[] = {
		{4+x,1+y}, {5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {10+x,1+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {2+x,3+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {1+x,5+y}, {2+x,5+y}, {3+x,5+y}, {4+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {1+x,6+y}, {2+x,6+y}, {3+x,6+y}, {4+x,6+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {13+x,6+y}, {2+x,7+y}, {3+x,7+y}, {4+x,7+y}, {10+x,7+y}, {11+x,7+y}, {12+x,7+y}, {2+x,8+y}, {3+x,8+y}, {4+x,8+y}, {5+x,8+y}, {10+x,8+y}, {11+x,8+y}, {12+x,8+y}, {2+x,9+y}, {3+x,9+y}, {4+x,9+y}, {5+x,9+y}, {6+x,9+y}, {7+x,9+y}, {8+x,9+y}, {9+x,9+y}, {10+x,9+y}, {11+x,9+y}, {12+x,9+y}, {3+x,10+y}, {4+x,10+y}, {5+x,10+y}, {6+x,10+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {11+x,10+y}, {2+x,11+y}, {3+x,11+y}, {4+x,11+y}, {5+x,11+y}, {6+x,11+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {12+x,11+y}, {1+x,12+y}, {2+x,12+y}, {3+x,12+y}, {4+x,12+y}, {5+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {12+x,12+y}, {1+x,13+y}, {2+x,13+y}, {3+x,13+y}, {4+x,13+y}, {10+x,13+y}, {11+x,13+y}, {12+x,13+y}, {13+x,13+y}, {0+x,14+y}, {1+x,14+y}, {2+x,14+y}, {3+x,14+y}, {11+x,14+y}, {12+x,14+y}, {13+x,14+y}, {0+x,15+y}, {1+x,15+y}, {2+x,15+y}, {3+x,15+y}, {11+x,15+y}, {12+x,15+y}, {13+x,15+y}, {14+x,15+y}, {0+x,16+y}, {1+x,16+y}, {2+x,16+y}, {3+x,16+y}, {11+x,16+y}, {12+x,16+y}, {13+x,16+y}, {14+x,16+y}, {1+x,17+y}, {2+x,17+y}, {3+x,17+y}, {11+x,17+y}, {12+x,17+y}, {13+x,17+y}, {1+x,18+y}, {2+x,18+y}, {3+x,18+y}, {4+x,18+y}, {10+x,18+y}, {11+x,18+y}, {12+x,18+y}, {13+x,18+y}, {1+x,19+y}, {2+x,19+y}, {3+x,19+y}, {4+x,19+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {8+x,19+y}, {9+x,19+y}, {10+x,19+y}, {11+x,19+y}, {12+x,19+y}, {13+x,19+y}, {2+x,20+y}, {3+x,20+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {8+x,20+y}, {9+x,20+y}, {10+x,20+y}, {11+x,20+y}, {12+x,20+y}, {4+x,21+y}, {5+x,21+y}, {6+x,21+y}, {7+x,21+y}, {8+x,21+y}, {9+x,21+y}, {10+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{7+x,0+y}, {2+x,2+y}, {1+x,4+y}, {5+x,4+y}, {13+x,4+y}, {13+x,5+y}, {1+x,7+y}, {13+x,7+y}, {1+x,8+y}, {9+x,8+y}, {6+x,12+y}, {7+x,12+y}, {8+x,12+y}, {13+x,12+y}, {14+x,14+y}, {0+x,17+y}, {4+x,17+y}, {10+x,17+y}, {14+x,17+y}, {0+x,18+y}, {5+x,18+y}, {9+x,18+y}, {3+x,21+y}, {11+x,21+y}, {7+x,22+y}
	};
	
	GR_POINT grey[] = {
		{5+x,0+y}, {6+x,0+y}, {8+x,0+y}, {9+x,0+y}, {3+x,1+y}, {11+x,1+y}, {12+x,2+y}, {1+x,3+y}, {6+x,4+y}, {8+x,4+y}, {5+x,5+y}, {9+x,5+y}, {9+x,7+y}, {8+x,8+y}, {13+x,8+y}, {2+x,10+y}, {12+x,10+y}, {1+x,11+y}, {13+x,11+y}, {0+x,13+y}, {5+x,13+y}, {14+x,13+y}, {10+x,14+y}, {6+x,18+y}, {8+x,18+y}, {14+x,18+y}, {1+x,20+y}, {2+x,21+y}, {5+x,22+y}, {6+x,22+y}, {8+x,22+y}, {9+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 178, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 25, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 32, grey );
	
}

void generator_nine( int x, int y )
{
	GR_POINT black[] = {
		{5+x,1+y}, {6+x,1+y}, {7+x,1+y}, {8+x,1+y}, {9+x,1+y}, {3+x,2+y}, {4+x,2+y}, {5+x,2+y}, {6+x,2+y}, {7+x,2+y}, {8+x,2+y}, {9+x,2+y}, {10+x,2+y}, {11+x,2+y}, {2+x,3+y}, {3+x,3+y}, {4+x,3+y}, {5+x,3+y}, {6+x,3+y}, {7+x,3+y}, {8+x,3+y}, {9+x,3+y}, {10+x,3+y}, {11+x,3+y}, {12+x,3+y}, {2+x,4+y}, {3+x,4+y}, {4+x,4+y}, {5+x,4+y}, {9+x,4+y}, {10+x,4+y}, {11+x,4+y}, {12+x,4+y}, {2+x,5+y}, {3+x,5+y}, {4+x,5+y}, {10+x,5+y}, {11+x,5+y}, {12+x,5+y}, {1+x,6+y}, {2+x,6+y}, {3+x,6+y}, {4+x,6+y}, {10+x,6+y}, {11+x,6+y}, {12+x,6+y}, {13+x,6+y}, {1+x,7+y}, {2+x,7+y}, {3+x,7+y}, {4+x,7+y}, {10+x,7+y}, {11+x,7+y}, {12+x,7+y}, {13+x,7+y}, {1+x,8+y}, {2+x,8+y}, {3+x,8+y}, {4+x,8+y}, {10+x,8+y}, {11+x,8+y}, {12+x,8+y}, {2+x,9+y}, {3+x,9+y}, {4+x,9+y}, {5+x,9+y}, {9+x,9+y}, {10+x,9+y}, {11+x,9+y}, {12+x,9+y}, {2+x,10+y}, {3+x,10+y}, {4+x,10+y}, {5+x,10+y}, {6+x,10+y}, {7+x,10+y}, {8+x,10+y}, {9+x,10+y}, {10+x,10+y}, {11+x,10+y}, {12+x,10+y}, {3+x,11+y}, {4+x,11+y}, {5+x,11+y}, {6+x,11+y}, {7+x,11+y}, {8+x,11+y}, {9+x,11+y}, {10+x,11+y}, {11+x,11+y}, {4+x,12+y}, {5+x,12+y}, {6+x,12+y}, {7+x,12+y}, {8+x,12+y}, {9+x,12+y}, {10+x,12+y}, {11+x,12+y}, {7+x,13+y}, {8+x,13+y}, {9+x,13+y}, {10+x,13+y}, {7+x,14+y}, {8+x,14+y}, {9+x,14+y}, {10+x,14+y}, {7+x,15+y}, {8+x,15+y}, {9+x,15+y}, {6+x,16+y}, {7+x,16+y}, {8+x,16+y}, {9+x,16+y}, {6+x,17+y}, {7+x,17+y}, {8+x,17+y}, {5+x,18+y}, {6+x,18+y}, {7+x,18+y}, {8+x,18+y}, {5+x,19+y}, {6+x,19+y}, {7+x,19+y}, {4+x,20+y}, {5+x,20+y}, {6+x,20+y}, {7+x,20+y}, {6+x,21+y}
 	};
 	
	GR_POINT darkgrey[] = {
		{4+x,1+y}, {10+x,1+y}, {1+x,5+y}, {13+x,5+y}, {13+x,8+y}, {13+x,9+y}, {12+x,11+y}, {6+x,13+y}, {11+x,13+y}, {6+x,15+y}, {10+x,15+y}, {5+x,17+y}, {9+x,17+y}, {4+x,19+y}, {8+x,19+y}, {5+x,21+y}, {7+x,21+y}
	};
	
	GR_POINT grey[] = {
		{5+x,0+y}, {6+x,0+y}, {7+x,0+y}, {8+x,0+y}, {9+x,0+y}, {3+x,1+y}, {2+x,2+y}, {12+x,2+y}, {1+x,4+y}, {6+x,4+y}, {7+x,4+y}, {8+x,4+y}, {13+x,4+y}, {5+x,5+y}, {9+x,5+y}, {5+x,8+y}, {9+x,8+y}, {1+x,9+y}, {6+x,9+y}, {2+x,11+y}, {3+x,12+y}, {5+x,13+y}, {4+x,21+y}, {6+x,22+y}
 	};
	
	GrSetGCForeground( generator_gc, BLACK );
	GrPoints( generator_wid, generator_gc, 128, black );

	GrSetGCForeground( generator_gc, GRAY );
	GrPoints( generator_wid, generator_gc, 17, darkgrey );
		
	GrSetGCForeground( generator_gc, LTGRAY );
	GrPoints( generator_wid, generator_gc, 24, grey );
}

void generator_dash( int x, int y )
{
	GrSetGCForeground( generator_gc, BLACK );
	GrFillRect( generator_wid, generator_gc, 2+x, 10+y, 13, 4 );

}

