/*
 * Timer - a simple timer app
 * Copyright (C) 2005 Scott Lawrence
 *
 */

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/* 2005-05-08 - initial version:  (Taylor Helsper)
 *		- basic version done.
 *		- Text for timer
 *		- Center button pauses/starts timer
 *      - Play/Pause Button sets timer abck to zero
 * 2005-05-22 - Version 1.1 Complete
 *      - Made text larger and centered
 *      - Now accurate to tenths of seconds
 *      - Pausable on tenths of seconds
 */

/* Thanks to Scott Lawrence for writing the clock program which I modified and 
 * made into a timer.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "pz.h"
#include "mlist.h"
#include "vectorfont.h"

static GR_TIMER_ID 	StopwatchClock_timer = 0;
static GR_GC_ID		StopwatchClock_gc = 0;
static GR_WINDOW_ID	StopwatchClock_bufwid = 0;
static GR_WINDOW_ID	StopwatchClock_wid = 0;
static GR_SCREEN_INFO	StopwatchClock_screen_info;
static int 		StopwatchClock_height = 0;

int stopwatch_seconds = 0;
int stopwatch_minutes = 0;
int stopwatch_intpaused = 1;
int stopwatch_tensseconds = 0;

#define STOPWATCH_DISPLAY 	(0)
#define STOPWATCH_HOURS	(1)
#define STOPWATCH_MINUTES	(2)
#define STOPWATCH_SECONDS	(3)
#define STOPWATCH_MAX	(3)


static void StopwatchClock_do_draw( void )
{
	char s[30];
	
    /* start clear */
    GrSetGCForeground( StopwatchClock_gc, WHITE );
    GrFillRect( StopwatchClock_bufwid, StopwatchClock_gc, 0, 0,
                StopwatchClock_screen_info.cols, StopwatchClock_height );

    /* copy the buffer into place */
    GrCopyArea( StopwatchClock_wid, StopwatchClock_gc, 0, 0,
                StopwatchClock_screen_info.cols,
                (screen_info.rows - (HEADER_TOPLINE + 1)),
                StopwatchClock_bufwid, 0, 0, MWROP_SRCCOPY);

	GrSetGCForeground( StopwatchClock_gc, BLACK );

	// Always shows 0s if digits are below 10 - keeps clock in middle of screen
	if ((stopwatch_seconds < 10) & (stopwatch_minutes < 10)) {
			sprintf( s, "0%i:0%i", stopwatch_minutes, stopwatch_seconds );
	}
	else if ((stopwatch_seconds < 10) & (stopwatch_minutes >= 10)) {
			sprintf( s, "%i:0%i", stopwatch_minutes, stopwatch_seconds );
	}
	else if ((stopwatch_seconds >= 10) & (stopwatch_minutes < 10)) {
			sprintf( s, "0%i:%i", stopwatch_minutes, stopwatch_seconds );
	}
	else {
			sprintf( s, "%i:%i", stopwatch_minutes, stopwatch_seconds );
	}

	// Draws string with vectorfont                                       x   y
	vector_render_string( StopwatchClock_wid, StopwatchClock_gc, s, 1, 3, 23, 32);

	// Increments counters
	if (stopwatch_intpaused==0) {
			stopwatch_tensseconds++;
	}

	// Change values when limit is reached
	if (stopwatch_tensseconds==10) {
			stopwatch_seconds++;
			stopwatch_tensseconds = 0;
	}

	if (stopwatch_seconds==60) {
			stopwatch_seconds = 0;
			stopwatch_minutes++;
	}

}

/* shut them down.. shut them all down! */
static void StopwatchClock_exit( void ) 
{
	GrDestroyTimer( StopwatchClock_timer );
	GrDestroyGC( StopwatchClock_gc );
	pz_close_window( StopwatchClock_wid );
	GrDestroyWindow( StopwatchClock_bufwid );
}


/* event handler */
static int StopwatchClock_handle_event (GR_EVENT *event)
{
	switch( event->type )
	{
	    case GR_EVENT_TYPE_TIMER:
		if (stopwatch_intpaused==1) {
				pz_draw_header("Paused");
		} else {
				pz_draw_header("Timer");
		}

		StopwatchClock_do_draw();
		break;

	    case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch)
		{
		case '\r': // Wheel button
			if ( stopwatch_intpaused==0) {
					pz_draw_header("Paused");
					stopwatch_intpaused=1;
			} else {
					pz_draw_header("Timer");
					stopwatch_intpaused=0;
			}
		    break;
		case 'd': // Play/pause button
			stopwatch_seconds = 0;
			stopwatch_minutes = 0;
			stopwatch_tensseconds = 0;
		    break;
		case 'w': // Rewind button
		    break;
		case 'f': // Fast forward button
		    break;
		case 'l': // Wheel left
		    break;
		case 'r': // Wheel right
		    break;
		case 'q': // (quit)
		case 'm': // Menu button
		    StopwatchClock_exit();
		    break;
		default:
		    break;
		} // keystroke
		break;   // key down

	} // event type

	return 1;
}


/* the main entry point */
void new_stopwatch_window(void)
{
	/* Init randomizer */
	srand(time(NULL));

	GrGetScreenInfo(&StopwatchClock_screen_info);

	StopwatchClock_gc = GrNewGC();
        GrSetGCUseBackground(StopwatchClock_gc, GR_FALSE);
        GrSetGCForeground(StopwatchClock_gc, BLACK);

	StopwatchClock_height = (screen_info.rows - (HEADER_TOPLINE + 1));

	StopwatchClock_wid = pz_new_window( 0, HEADER_TOPLINE + 1,
		    screen_info.cols, StopwatchClock_height,
		    StopwatchClock_do_draw, StopwatchClock_handle_event );

	StopwatchClock_bufwid = GrNewPixmap( screen_info.cols, StopwatchClock_height, NULL );

        GrSelectEvents( StopwatchClock_wid, GR_EVENT_MASK_TIMER
					| GR_EVENT_MASK_EXPOSURE
					| GR_EVENT_MASK_KEY_UP
					| GR_EVENT_MASK_KEY_DOWN );

	StopwatchClock_timer = GrCreateTimer( StopwatchClock_wid, 100 ); /* 1/10 sec. */

	GrMapWindow( StopwatchClock_wid );
}