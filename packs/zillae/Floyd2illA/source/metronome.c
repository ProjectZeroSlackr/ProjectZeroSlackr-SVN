/*
 * Metronome
 *
 * Copyright (C) 2005 Stephen Lawrence (Iklop)
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
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "pz.h"
#include "oss.h"
#include "ipod.h"


// main window stuff
static GR_WINDOW_ID metronome_wid;
static GR_GC_ID metronome_gc;
static GR_TIMER_ID metronome_timer_id;

// sound
static dsp_st dspz;

// metronome
static struct timeval metronome_goal;	//goal time
static int metronome_bpm;		//beats per minute
static int metronome_mspb;		//milliseconds per beat
static int metronome_volume;	//volume
static int metronome_scale;		//scale
static int metronome_scale2b;	//scale to be, this keeps the main main always on track
static int metronome_tick_count;	//keeps track of ticks

#define METRONOME_MAX	208 	//maximum bpm
#define METRONOME_MIN 	40		//minimum bpm

//tap varialbes
#define METRONOME_TAP_SIZE	10
static int metronome_tap_diffs[METRONOME_TAP_SIZE];
static int metronome_tap_count;
static struct timeval metronome_tap_time;	//tap time

//modes & submodes
#define METRONOME_MODE_NORMAL	0
#define METRONOME_MODE_TAP		1
#define METRONOME_MODE_LAST		1

#define METRONOME_SUBMODE_NORMAL 	0
#define METRONOME_SUBMODE_VOLUME	1
#define METRONOME_SUBMODE_SCALE		2
#define METRONOME_SUBMODE_LAST 		2

//mode variables
static int metronome_mode;
static int metronome_submode;

//state variables
static int metronome_paused;	//paused state
static int metronome_piezo;		//piezo state





// drawing function declarations
void metronome_zero( int x, int y );
void metronome_one( int x, int y );
void metronome_two( int x, int y );
void metronome_three( int x, int y );
void metronome_four( int x, int y );
void metronome_five( int x, int y );
void metronome_six( int x, int y );
void metronome_seven( int x, int y );
void metronome_eight( int x, int y );
void metronome_nine( int x, int y );
void metronome_dash( int x, int y );
void metronome_notes( int y );



//****************************************************************************
// DRAWING FUNCTIONS
//****************************************************************************

// Draws number sent over
// Status: Good
void metronome_draw_number( int num )
{
	int i;	//used for looping	
	int width = (num > 99) ? 49 : (num > 9) ? 32 : 15;	// width of drawn numbers
	int offset = (screen_info.cols - width) / 2;	// x pos of numbers
	int top = 10;	// y pos of numbers
	char str[3];
	
	// erase the previous number
	GrSetGCForeground( metronome_gc, WHITE );
	GrFillRect( metronome_wid, metronome_gc, 0, top, screen_info.cols, 25 );
	
	//convert number to a string
	if( num==0 )
		sprintf( str, "-" );
	else	
		sprintf( str, "%d", num );
	
	//draw each number
	for( i=0; i<3; i++ )
	{
		switch( str[i] )
		{
			case '0':	metronome_zero( offset, top );		break;	
			case '1':	metronome_one( offset, top );		break;
			case '2':	metronome_two( offset, top );		break;
			case '3':	metronome_three( offset, top );		break;
			case '4':	metronome_four( offset, top );		break;
			case '5':	metronome_five( offset, top );		break;
			case '6':	metronome_six( offset, top );		break;
			case '7':	metronome_seven( offset, top );		break;
			case '8':	metronome_eight( offset, top );		break;
			case '9':	metronome_nine( offset, top );		break;
			case '-':	metronome_dash( offset, top );		break;			
		}

		offset += 17;	//increment the offset of one letter width + space
	}
}

// Draws the tempo word
// Status: Done
void metronome_draw_extra( int bpm )
{
	char extra[12];
	int width;
	
	//determine what to draw
	if( metronome_mode==METRONOME_MODE_TAP )
	{
		sprintf( extra, "%s", "tap mode" );
		width = 43; 			
	}
	else if( metronome_paused )
	{
		sprintf( extra, "%s", "paused" );
		width = 32; 			
	}
	else if( bpm<=60 )
 	{
 		sprintf( extra, "%s", "largo" );
		width = 22; 			
 	}
 	else if( bpm <= 66 )
 	{
 		sprintf( extra, "%s", "larghetto" );
		width = 40; 			 			
 	}
 	else if( bpm <= 76 )
 	{
 		sprintf( extra, "%s", "adagio" );
		width = 31; 			 			
 	}	
 	else if( bpm <= 108 )
 	{
 		sprintf( extra, "%s", "andante" );
		width = 38; 			 			
 	}
 	else if( bpm <= 120 )
 	{
 		sprintf( extra, "%s", "moderato" );
		width = 43; 			 			
 	}
 	else if( bpm <= 168 )
 	{
 		sprintf( extra, "%s", "allegro" );
		width = 30; 			 			
 	}
 	else if( bpm <= 200 )
 	{
 		sprintf( extra, "%s", "presto" );
		width = 28; 			 			
 	}
 	else
 	{
 		sprintf( extra, "%s", "prestissimo" );
		width = 50; 			 			
 	}
	
	//erase the previous extra		
	GrSetGCForeground( metronome_gc, WHITE );
	GrFillRect( metronome_wid, metronome_gc, 0, 40, screen_info.cols, 20 );
	
	//draw the extra text
	GrSetGCForeground( metronome_gc, BLACK );
	GrText( metronome_wid, metronome_gc, (screen_info.cols-width)/2, 50, extra, -1, GR_TFASCII );
}


// Draws a bpm and the extra text related to the bpm
// Status: Done
void metronome_draw_bpm( int bpm )
{
	// Draw the bpm
 	metronome_draw_number( bpm );

	// Draw extra text
	metronome_draw_extra( bpm );
}


// Draws the volume
// Status: Done
void metronome_draw_volume()
{
	//calculate the bar length for the volume
	int volume_length = ((screen_info.cols - 22) * metronome_volume) / 100; 

	//draw the volume border
	GrSetGCForeground( metronome_gc, BLACK);
	GrRect( metronome_wid, metronome_gc, 10, screen_info.rows - HEADER_TOPLINE - 16, screen_info.cols - 20, 8 );
		
	//draw the volume inside
	GrSetGCForeground( metronome_gc, GRAY);		
	GrFillRect( metronome_wid, metronome_gc, 11, screen_info.rows - HEADER_TOPLINE - 15, volume_length, 6 );
}

// Draws the scale
// Status: Needs to be Written
void metronome_draw_scale()
{
	metronome_notes(68);
}

// Draws the submode
// Status: Done
void metronome_draw_submode()
{
	//erase the submode area
	GrSetGCForeground( metronome_gc, WHITE);
	GrFillRect( metronome_wid, metronome_gc, 0, 55, screen_info.cols, screen_info.rows-55 );
	
	//only draw submod if in normal mode
	if( metronome_mode==METRONOME_MODE_NORMAL )
	{	
		//determine what submode to draw
		switch( metronome_submode )
		{
			case METRONOME_SUBMODE_VOLUME:	metronome_draw_volume();	break;
			case METRONOME_SUBMODE_SCALE:	metronome_draw_scale();		break;	
		}
	}
	
}

// Called on exposure event
// Status: Done
static void metronome_do_draw()
{
    pz_draw_header( "Metronome" );
    metronome_draw_bpm( metronome_bpm ); // the call to draw function
}



//****************************************************************************
// TICK SOUND
//****************************************************************************

typedef struct wftable {
	int nsamples;		/* number of samples in the next array */
	int samples[282];	/* array of 0..128 waveform samples */
				/* these are to be played at 44.1khz */
} wftable;

static wftable metronome_tick_waveform = {  35, { 127,127,128,129,133,139,148,159,169,170,161,146,122,99,80,73,78,89,100,109,117,118,119,118,115,114,115,122,132,146,157,160,157,147,136 }};
static short * metronome_tick_buffer;


// Creates the buffer used to make the tick sound
// Status: Done
void metronome_create_tick()
{
	int bp;
	int f1p = 0;

	//nsamps = ontime * 44100 / 1000;
	metronome_tick_buffer = (short *)malloc( sizeof( short ) * 44 );

	// play the two tones
	for( bp = 0 ; bp < 44; bp++ )
	{
		// copy over and add the two waveforms
		metronome_tick_buffer[bp] = ( metronome_tick_waveform.samples[f1p++] );
		metronome_tick_buffer[bp] = metronome_tick_buffer[bp] << 6;

		// adjust sample pointers
		if( f1p >= metronome_tick_waveform.nsamples )  
			f1p = 0;
	}
}

// Deletes the tick sound
// Status: Done
void metronome_delete_tick()
{
	int bp;
	
	for( bp=0 ; bp < 44; bp++ )
		metronome_tick_buffer[bp] = 0;

	free( metronome_tick_buffer );

}

// Makes a sound
// Status: This isn't a good way to deal with different volumes
//	I should have two different sounds instead of changing the volume so much
void metronome_tick( int level )
{
	if( metronome_piezo )
	{
		if( level==0 )
			ipod_beep();
	}
	else
	{
		int goal_vol, cur_vol, delta;
	
		//calculate the goal volume
		if( level==0 )
			goal_vol = metronome_volume;
		else
			goal_vol = metronome_volume - 20;
				
		//get the current volume
		cur_vol = dsp_get_volume( &dspz );
		
		//calculate the difference
		delta = goal_vol - cur_vol;
		
		//change the volume
		dsp_vol_change( &dspz, delta );
		
		dsp_write( &dspz, metronome_tick_buffer, 44 * sizeof(short));
	}
}


//****************************************************************************
// Initialization & De-Initialization
//****************************************************************************

// Perogram Initialization
// Status: Done
void metronome_construct()
{
	//set the goal to zero
	metronome_goal.tv_sec = 0;
	metronome_goal.tv_usec = 0;
	
	//setup the sound
	dsp_open(&dspz, DSP_LINEOUT);
	dsp_setup(&dspz, 1, 44100);
	metronome_create_tick();
	
	//get the volume
	metronome_volume = dsp_get_volume( &dspz );
	
	//initialize metronome variables
	metronome_bpm = 120;
	metronome_mspb = 60000 / metronome_bpm;
	metronome_scale = metronome_scale2b = 1;
	metronome_tick_count = 0;

	//initialize state variables
	metronome_paused = 0;
	metronome_piezo = 0;
			
	//set the mode
	metronome_mode = METRONOME_MODE_NORMAL;
	metronome_submode = METRONOME_SUBMODE_NORMAL;
}

// Program De-initilization
// Status: Done
void metronome_destruct()
{
	metronome_delete_tick();
	dsp_close(&dspz);
}

// Initializes the current main mode, should be called with a change of main mode
// Status: Check
void metronome_mode_changed()
{
	switch( metronome_mode )
	{
		case METRONOME_MODE_NORMAL:
		{
			//reset the goal timer
			metronome_goal.tv_sec = 0;
			metronome_goal.tv_usec = 0;		
			
			metronome_tick_count = 0;
			metronome_paused = 0;	// unpause
			metronome_submode = METRONOME_SUBMODE_NORMAL;	//resset the submode
			
			metronome_draw_bpm( metronome_bpm );	//draw the bpm
			metronome_draw_submode();	//erase the submode
			
			break;
		}
			
		case METRONOME_MODE_TAP:
		{
			int i;
			
			// clear the tap array	
			for( i=0; i<METRONOME_TAP_SIZE; i++ )
				metronome_tap_diffs[i] = 0;
			
			//reset the tap timer
			metronome_tap_time.tv_sec = 0;
			metronome_tap_time.tv_usec = 0;
			
			metronome_tap_count = 0; // set the tap count to zero
	
			metronome_draw_bpm( 0 );	//draw a dash and say 'tap mode'
			metronome_draw_submode();	//erase the submode
			
			break;
		}
	}

	metronome_draw_submode();	
}

//****************************************************************************
// Main Program Functions
//****************************************************************************

// Determines whether a tick sound should be made
// Status: Done
void metronome_check_ticks()
{
	struct timeval now;
	int secdiff;
	int usecdiff;
	int diff;
	
	//get the current time
	gettimeofday( &now, NULL );
	
	// set the goal to now if the goal is zero
	if( metronome_goal.tv_sec==0 && metronome_goal.tv_usec==0 )
	{
		metronome_goal = now;
		diff = 0;
	}
	else
	{
		//calculate the difference between now and the goal		
		secdiff = (metronome_goal.tv_sec - now.tv_sec) * 1000;
		usecdiff = (metronome_goal.tv_usec - now.tv_usec ) / 1000;
		diff = secdiff + usecdiff;			
	}
		
	if( diff <= 0 )	
	{
		int usec = 0;
		
		//make a sound
		metronome_tick( metronome_tick_count );
		
		if( metronome_tick_count==0 )
		{
			metronome_scale = metronome_scale2b;	
		}
				
		//increase the tick count
		if( metronome_tick_count >= metronome_scale - 1 )
			metronome_tick_count = 0;
		else
			metronome_tick_count++;

		//calculate the next time there should be a tick    
		usec = (metronome_goal.tv_usec + (metronome_mspb * 1000 / metronome_scale));
		metronome_goal.tv_sec += usec/1000000;
		metronome_goal.tv_usec = usec%1000000;
	}
}

// Calculates the average milliseconds per beat
// Status: Done
int metronome_get_tap_mspb()
{
	int loop;
	int diff;
	int sum = 0;
	
	for( loop=0; loop<METRONOME_TAP_SIZE; loop++ )
	{
		diff = metronome_tap_diffs[loop];
		
		if( diff == 0 )
			break;
			
		sum += diff;
	}

	if( loop==0 )
		return 0;
		
	return (sum / loop);

}

// Calculates the difference between taps in tap mode
// Status: Done
void metronome_tap()
{
	struct timeval now;

	//get the current time
	gettimeofday( &now, NULL );
		
	// make sure this isn't the first tap so a difference can be calculated
	if( metronome_tap_time.tv_sec!=0 || metronome_tap_time.tv_usec!=0 )
	{
		int sec;
		int usec;
		int diff;
		int mspb;
		int bpm;
							
		//get difference between previous tap and now
		sec = (now.tv_sec - metronome_tap_time.tv_sec) * 1000;
		usec = (now.tv_usec - metronome_tap_time.tv_usec) / 1000;
		diff = sec + usec;
				
		//add difss to diff array
		metronome_tap_diffs[metronome_tap_count] = diff;
				
		//set the tap count/position in array
		if( metronome_tap_count == METRONOME_TAP_SIZE-1 )
			metronome_tap_count = 0;
		else
			metronome_tap_count++;
					
		//get the average milliseconds per tap beat & bpm
		mspb = metronome_get_tap_mspb();
		bpm = 60000/mspb;
				
		//limit the bpm
		if( bpm > METRONOME_MAX )
			bpm = METRONOME_MAX;
		else if( bpm < METRONOME_MIN )
			bpm = METRONOME_MIN;
					
		//draw the bpm
		metronome_draw_number( bpm );
	}
			
	//set the metronome tap time to now
	metronome_tap_time = now;							
}		


//****************************************************************************
// Button Presses
//****************************************************************************

// Wheel Right
// Status: Done
void metronome_button_right()
{
	switch( metronome_mode )
	{
    	case METRONOME_MODE_NORMAL:
    	{
    		switch( metronome_submode )
    		{
    			case METRONOME_SUBMODE_NORMAL:
    			   	if( metronome_bpm < METRONOME_MAX )	
		    		{
    					//calculate bpm and mspb and redraw screen
   						metronome_bpm++;
   						metronome_mspb = 60000/metronome_bpm;
						metronome_draw_bpm( metronome_bpm );        		
    				}
    				break;
    				
    			case METRONOME_SUBMODE_VOLUME:
    				metronome_volume += 3;
    				if( metronome_volume > 100 )
    					metronome_volume = 100;
    				metronome_draw_submode();	//draw the volume bar
      				break;

    			case METRONOME_SUBMODE_SCALE:
					//increase the scale
    				if( metronome_scale2b < 4 )
    					metronome_scale2b++;
    				//draw the scale
    				metronome_draw_submode();
    				break;
    		}		
    	}
    }
}

// Wheel Left
// Status: Done
void metronome_button_left()
{
	switch( metronome_mode )
	{
    	case METRONOME_MODE_NORMAL:
    	{
    		switch( metronome_submode )
    		{
    			case METRONOME_SUBMODE_NORMAL:
		    		if( metronome_bpm > METRONOME_MIN )
    				{
    					//decrease bpm and mspb and redraw screen
    					metronome_bpm--;
    					metronome_mspb = 60000/metronome_bpm;
					    metronome_draw_bpm( metronome_bpm ); 	       		
    				}
    				break;
    				
    			case METRONOME_SUBMODE_VOLUME:
					metronome_volume -= 3;
					if( metronome_volume < 0 )
						metronome_volume = 0;
    				metronome_draw_submode();	//draw the volume bar
    				break;

    			case METRONOME_SUBMODE_SCALE:
					//decrease the scale
    				if( metronome_scale2b > 1 )
    					metronome_scale2b--;
    				//draw the scale
    				metronome_draw_submode();    			
    				break;    				
    		}
    	}
    }
}

// Play Button
// Status: Check Tap Mode
void metronome_button_play()
{
	switch( metronome_mode )
	{
		case METRONOME_MODE_NORMAL:
			//switch the pause state
		    metronome_paused = !metronome_paused;
       		
		    //reset the goal to zero (gives an imediate tick when restarted)
		    if( metronome_paused )
		    {
				metronome_goal.tv_sec = 0;
				metronome_goal.tv_usec = 0;		
			
				metronome_tick_count = 0;
			}
				
			//draw the extra text (displays pause word if paused)
		    metronome_draw_extra( metronome_bpm );
		    break;
		    
		case METRONOME_MODE_TAP:
		{
			//this might be able to be put into the mode_change function
			int tap_mspb = metronome_get_tap_mspb();
			if( tap_mspb!=0 )
			{
				metronome_mspb = tap_mspb;
				metronome_bpm = 60000/metronome_mspb;
				if( metronome_bpm > METRONOME_MAX )
					metronome_bpm = METRONOME_MAX;
				else if( metronome_bpm < METRONOME_MIN )
					metronome_bpm = METRONOME_MIN;
				metronome_mode = METRONOME_MODE_NORMAL;
				metronome_mode_changed();
			}
			break;
		}
	}
}

// Action Button
// Status: Check
void metronome_button_action()
{
	switch( metronome_mode )
	{
		case METRONOME_MODE_NORMAL:
		{
			//increment the submode
			if( metronome_submode==METRONOME_SUBMODE_LAST )
				metronome_submode = 0;
			else
				metronome_submode++;
		
			//draw the submode
			metronome_draw_submode();
			
			break;
		}
				
		case METRONOME_MODE_TAP:
		{
			//make a sound
			metronome_tick(0);
			
			//calculate the tap differences
			metronome_tap();
			
			break;
		}
	}
}


// Forward Button
// Status: Done
void metronome_button_forward()
{
	switch( metronome_mode )
	{
		case METRONOME_MODE_NORMAL:
			metronome_mode = METRONOME_MODE_TAP;
			break;
		case METRONOME_MODE_TAP:
			metronome_mode = METRONOME_MODE_NORMAL;
			break;	
	}		
	
	metronome_mode_changed();
}

// Rewind Button
// Status: Done
void metronome_button_rewind()
{
	metronome_piezo = !metronome_piezo;
}

// Menu Button
// Status: Done
void metronome_button_menu()
{
	// destruct everything
	metronome_destruct();
	
	//quit metronome, stop everything
    pz_close_window( metronome_wid );
    GrDestroyTimer( metronome_timer_id );
	GrDestroyGC( metronome_gc );
}



//****************************************************************************
// EVENT HANDLERS
//****************************************************************************

// Nornal Mode Event Handler
// Status: Done
static int metronome_normal_handle( GR_EVENT *event )
{
	int ret = 0;
	
    switch (event->type)
    {
    	case GR_EVENT_TYPE_TIMER:
   			if( !metronome_paused && metronome_mode!=METRONOME_MODE_TAP )
			    metronome_check_ticks();
    	    break;

    	case GR_EVENT_TYPE_KEY_DOWN:
	        switch (event->keystroke.ch)
	        {
		        case IPOD_WHEEL_CLOCKWISE:		metronome_button_right();	break;
       			case IPOD_WHEEL_ANTICLOCKWISE:	metronome_button_left();	break;
       			case IPOD_BUTTON_PLAY:			metronome_button_play();	break;            
       			case IPOD_BUTTON_ACTION:    	metronome_button_action();  break;
       			case IPOD_BUTTON_MENU:			metronome_button_menu();	break;
				case IPOD_BUTTON_FORWARD:		metronome_button_forward();	break;
       			case IPOD_BUTTON_REWIND:		metronome_button_rewind();	break;
       			case IPOD_SWITCH_HOLD:       			
       			default:
       			    ret |= KEY_UNUSED;
       			    break;
       		}
	        break;
        
    	case GR_EVENT_TYPE_KEY_UP:
    	    switch (event->keystroke.ch)
    	    {
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
static int metronome_handle_event( GR_EVENT *event )
{
    int ret = 0;

	ret = metronome_normal_handle( event );
	
	return ret;
	
}



//***************************************************************************
// PROGRAM ENTERS HERE
//***************************************************************************

// Creates window
// Status: Done
void new_metronome_window (void)
{
	// Get a copy of the root graphics context
	metronome_gc = pz_get_gc( 1 );   
	
	// Set foreground color
	GrSetGCUseBackground( metronome_gc, GR_FALSE );
	GrSetGCForeground( metronome_gc, BLACK);

	// Open the window
	metronome_wid = pz_new_window (0, HEADER_TOPLINE + 1,
									screen_info.cols,  
									screen_info.rows - (HEADER_TOPLINE + 1),
									metronome_do_draw,
									metronome_handle_event);

	// Select the types of events you need for your window:
	GrSelectEvents( metronome_wid, GR_EVENT_MASK_EXPOSURE | 
					GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP
					| GR_EVENT_MASK_TIMER );

	// Map the window to the screen:
	GrMapWindow( metronome_wid );

	// Create the timer used for animating your application:
	metronome_timer_id = GrCreateTimer( metronome_wid, 1 ); // Timer use for key input
	
	metronome_construct();
}



//******************************************************
// BIG PRETTY NUMBERS
//******************************************************
void metronome_zero( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 163, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 17, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 36, grey );
	
}


void metronome_one( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 92, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 5, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 1, grey );
}

void metronome_two( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 140, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 18, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 23, grey );
	
}

void metronome_three( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 133, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 22, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 36, grey );
	
}

void metronome_four( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 159, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 20, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 8, grey );
	
}

void metronome_five( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 138, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 25, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 33, grey );
	
}

void metronome_six( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 126, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 22, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 19, grey );
	
}

void metronome_seven( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 105, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 17, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 18, grey );
	
}

void metronome_eight( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 178, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 25, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 32, grey );
	
}

void metronome_nine( int x, int y )
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
	
	GrSetGCForeground( metronome_gc, BLACK );
	GrPoints( metronome_wid, metronome_gc, 128, black );

	GrSetGCForeground( metronome_gc, GRAY );
	GrPoints( metronome_wid, metronome_gc, 17, darkgrey );
		
	GrSetGCForeground( metronome_gc, LTGRAY );
	GrPoints( metronome_wid, metronome_gc, 24, grey );
}

void metronome_dash( int x, int y )
{
	GrSetGCForeground( metronome_gc, BLACK );
	GrFillRect( metronome_wid, metronome_gc, 2+x, 10+y, 13, 4 );

}


void metronome_notes( int y )
{
	int width, pos, i, x;
	
	GrSetGCForeground( metronome_gc, BLACK );
	
	//get width and starting position
	width = 8 * metronome_scale2b - 3;
	pos = ((screen_info.cols - width ) / 2) + 3;	


	//draw top line
	if( metronome_scale2b > 1 )
		GrLine( metronome_wid, metronome_gc, pos+2, y, pos+width-3, y );	

	x = ( ( screen_info.cols ) / 2 )+ 2;	
			
	//draw extra lines or triplet number
	switch( metronome_scale2b )
	{
		case 3:
		{
			GR_POINT three[] = {
				{-2+x,-7+y}, {-1+x,-8+y}, {0+x,-8+y}, {1+x,-7+y}, {1+x,-6+y}, {0+x,-5+y}, {1+x,-4+y}, {1+x,-3+y}, {0+x,-2+y}, {-1+x,-2+y}, {-2+x,-3+y}
		 	};
	 		GrPoints( metronome_wid, metronome_gc, 11, three );
			break;
		}
		case 4:
			GrLine( metronome_wid, metronome_gc, pos+2, y-1, pos+width-3, y-1 );	
			GrLine( metronome_wid, metronome_gc, pos+2, y+2, pos+width-3, y+2 );				
			break;
	}

	//draw each note	
	for( i=0; i<metronome_scale2b; i++ )
	{
		GrFillEllipse( metronome_wid, metronome_gc, pos, y+10, 2, 2 );
		GrLine( metronome_wid, metronome_gc, pos+2, y+10, pos+2, y );

		pos += 8;		
	}
	
}

