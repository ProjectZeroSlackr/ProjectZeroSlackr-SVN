/*
 * Copyright (C) 2005 fre_ber and jonrelay
 *
 * Morse Code concept by mattlivesey, implemented by fre_ber,
 * 		adopted to text input system by jonrelay
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

/*
	/!\ Careful!
	This morse.c is *not* the same as fre_ber's morse.c!
*/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
/* #include <pthread.h> */
#include "../pz.h"
#include "../textinput.h"

#ifndef __MORSE_H__
#define __MORSE_H__

#define NUM_MORSE_SYMBOLS 79
#define MORSE_MAX_SYMBOL_LENGTH 10
#define MORSE_BEEP_SEPARATION 1
#define MORSE_IDLE    0
#define MORSE_SILENCE 1
#define MORSE_SIGNAL  2

typedef struct text_MorseSettings_
{
    int symbolSeparation;  // Number of millisecons that separates symbols
    int wordSeparation;    // Number of milliseconds that separates words
    int longSeparation;    // Number of milliseconds that separates short
                           // from long
} text_MorseSettings;

typedef struct text_Symbol_
{
    char character;
    char *symbol;
} text_Symbol;

#endif

text_MorseSettings text_morse_settings = 
{
    /*2000,      // symbolSeparation
    5000,      // wordSeparation
    500        // longSeparation*/
    /*400,      // symbolSeparation
    1000,      // wordSeparation
    100        // longSeparation*/
    800,      // symbolSeparation
    2000,      // wordSeparation
    200        // longSeparation
};

text_Symbol text_morseTable[NUM_MORSE_SYMBOLS] =
{
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.."},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."},
    {'0', "-----"},
    {'1', ".----"},
    {'2', "..---"},
    {'3', "...--"},
    {'4', "....-"},
    {'5', "....."},
    {'6', "-...."},
    {'7', "--..."},
    {'8', "---.."},
    {'9', "----."},
    {'.', ".-.-.-"},
    {',', "--..--"},
    {'?', "..--.."},
    {'\'', ".----."},
    {'/', "-..-."},
    {'\\', "-..-.."}, /* not standard morse code */
    {'|', "-..-.-"}, /* not standard morse code */
    {'(', "-.--.-"},
    {')', "-.--.."}, /* not standard morse code */
    {'[', "----.-"}, /* not standard morse code */
    {']', "----.."}, /* not standard morse code */
    {'{', "------"}, /* not standard morse code */
    {'}', "-----."}, /* not standard morse code */
    {'<', "--...-"}, /* not standard morse code */
    {'>', "--...."}, /* not standard morse code */
    {':', "---..."},
    {';', "...---"}, /* not standard morse code */
    {'=', "-...-"},
    {'-', "-....-"},
    {'+', ".-.-."},
    {'_', ".-----"}, /* not standard morse code */
    {'\"', ".-..-."},
    {'@', ".--.-."}, /* AC */
    {'#', "-.---"}, /* NO not standard morse code */
    {'$', "..-..."}, /* US not standard morse code */
    {'&', ".-.-.."}, /* RD not standard morse code */
    {'*', ".-...-"}, /* AST not standard morse code */
    {'%', ".--..."}, /* PI not standard morse code */
    {'~', "-.-.-."}, /* not standard morse code */
    {'^', "---.--"}, /* not standard morse code */
    {'`', "--.---"}, /* not standard morse code */
    {'\xC4', ".-.-"},
    {'\xC5', ".--.-"},
    {'\xC7', "----"},
    {'\xC9', "..-.."},
    {'\xD1', "-.-.-"}, /* not standard morse code */
    {'\xD6', "---."},
    {'\xDC', "..--"},
    {'!', "..--."},
    {'\x1C', ".-..."}, /* not standard morse code */
    {'\x1D', ".-..-"}, /* not standard morse code */
    {'\x0A', ".-.--"}, /* not standard morse code */
    {'\x08', "......"} /* not standard morse code */
};

static GR_TIMER_ID text_symbolTimer_id;
static GR_TIMER_ID text_wordTimer_id;
static GR_TIMER_ID text_longTimer_id;

static int  text_state;
static int  text_longSignal;
static char text_symbol[MORSE_MAX_SYMBOL_LENGTH];
static int  text_symbolLen = 0;
static char text_mlastchar = '\0';
static int  text_mlowercase = 1;

/* static int          text_beep_enable = GR_TRUE;
static int          text_beep_now    = GR_FALSE;
static int          text_beep_fd     = -1;
static int          text_beep_end    = GR_FALSE;
static pthread_t    text_beep_thread;
pthread_mutex_t     text_beep_now_mutex;
pthread_mutex_t     text_beep_end_mutex;
pthread_attr_t      text_beep_tAttr; */

/* Beeping Functions */

/* static void *text_beep_play (void *data)
{
    static char buf = 0x55;
    int end = GR_FALSE;
    while (!end)
    {
	pthread_mutex_lock (&text_beep_now_mutex);
	if (text_beep_fd != -1 && text_beep_now)
	{
	    write (text_beep_fd, &buf, 1);
	}
	pthread_mutex_unlock (&text_beep_now_mutex);
	usleep (100);

	pthread_mutex_lock (&text_beep_end_mutex);
	end = text_beep_end;
	pthread_mutex_unlock (&text_beep_end_mutex);
    }
    pthread_exit (NULL);
}

void text_beep_init()
{
#ifdef IPOD
    text_beep_fd = open("/dev/ttyS1", O_WRONLY);
    if (text_beep_fd == -1)
    {
	text_beep_fd = open("/dev/tts/1", O_WRONLY);
    }
    text_beep_end = GR_FALSE;
    text_beep_now = GR_FALSE;
    pthread_mutex_init          (&text_beep_now_mutex, NULL);
    pthread_mutex_init          (&text_beep_end_mutex, NULL);
    pthread_attr_init           (&text_beep_tAttr);
    pthread_attr_setdetachstate (&text_beep_tAttr, PTHREAD_CREATE_JOINABLE);
    pthread_create              (&text_beep_thread, &text_beep_tAttr, text_beep_play, NULL);
#endif
}

void text_beep_free()
{
#ifdef IPOD
    pthread_mutex_lock    (&text_beep_end_mutex);
    text_beep_end = GR_TRUE;
    pthread_mutex_unlock  (&text_beep_end_mutex);
    pthread_join          (text_beep_thread, NULL);
    pthread_attr_destroy  (&text_beep_tAttr);
    pthread_mutex_destroy (&text_beep_now_mutex);
    pthread_mutex_destroy (&text_beep_end_mutex);
    if (text_beep_fd != -1)
    {
	close (text_beep_fd);
	text_beep_fd = -1;
    }
#endif
} */

/* Morse Decoding Functions */

char text_morse_decode (char *symbol)
{
    unsigned char c = '\0';
    int i;
    while (i < NUM_MORSE_SYMBOLS && c == '\0')
    {
	if (strcmp (text_symbol, text_morseTable[i].symbol) == 0)
	{
	    c = text_morseTable[i].character;
	}
	else
	{
	    i++;
	}
    }
    if ((c != 0) && (text_mlowercase != 0) && ( ((c >= 'A') && (c <= 'Z')) || ((c >= 192) && (c < 224)) )) {
    	c += 32;
    }
    return c;
}

void text_morse_key_down()
{
    text_longSignal = GR_FALSE;
    text_longTimer_id = GrCreateTimer(text_get_wid(), text_morse_settings.longSeparation);
    text_state = MORSE_SIGNAL;

    /* if (text_beep_enable)
    {
		pthread_mutex_lock (&text_beep_now_mutex);
		text_beep_now = GR_TRUE;
		pthread_mutex_unlock (&text_beep_now_mutex);
    } */
}

void text_morse_key_up()
{
    GrDestroyTimer(text_longTimer_id);
    if (text_symbolLen < MORSE_MAX_SYMBOL_LENGTH)
    {
		char c = text_longSignal ? '-' : '.';
		text_symbol[text_symbolLen++] = c;
    }
    /* if (text_beep_enable)
    {
		pthread_mutex_lock (&text_beep_now_mutex);
		text_beep_now = GR_FALSE;
		pthread_mutex_unlock (&text_beep_now_mutex);
    } */

    GrDestroyTimer(text_symbolTimer_id);
    GrDestroyTimer(text_wordTimer_id);
    text_symbolTimer_id = GrCreateTimer(text_get_wid(), text_morse_settings.symbolSeparation);
    text_wordTimer_id = GrCreateTimer(text_get_wid(), text_morse_settings.wordSeparation);
    text_state = MORSE_SILENCE;
}

char text_wordSeparation()
{
    return ' ';
}

char text_symbolSeparation()
{
    char ch = '\0';
    int i;
    if (text_symbolLen != 0)
    {
		ch = text_morse_decode (text_symbol);
		for (i = 0; i < MORSE_MAX_SYMBOL_LENGTH; i++)
		{
		    text_symbol[i] = '\0';
		}
		text_symbolLen = 0;
    }
    return ch;
}

void text_longSeparation()
{
    text_longSignal = GR_TRUE;
}

void text_morse_play()
{
    switch (text_state)
    {
    case MORSE_IDLE:
	text_symbolTimer_id = GrCreateTimer(text_get_wid(), text_morse_settings.symbolSeparation);
	text_wordTimer_id = GrCreateTimer(text_get_wid(), text_morse_settings.wordSeparation);
	text_state = MORSE_SILENCE;
	break;

    case MORSE_SIGNAL:
	GrDestroyTimer(text_longTimer_id);
    case MORSE_SILENCE:
	GrDestroyTimer(text_symbolTimer_id);
	GrDestroyTimer(text_wordTimer_id);
	text_state = MORSE_IDLE;
    }
}

/* Morse Device Events */

char text_morse_handle_event (GR_EVENT *event)
{
    char ch = '\0';
    GR_EVENT_TIMER *timerEvent = (GR_EVENT_TIMER *)event;

    switch (text_state)
    {
    case MORSE_IDLE:
	break;

    case MORSE_SILENCE:
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
	    if (timerEvent->tid == text_symbolTimer_id)
	    {
		ch = text_symbolSeparation();
	    }
	    else if (timerEvent->tid == text_wordTimer_id)
	    {
		ch = text_wordSeparation();
	    }
	    break;

	case GR_EVENT_TYPE_KEY_DOWN:
	    switch (event->keystroke.ch)
	    {
	    case '\r': // Wheel button
			text_morse_key_down();
			break;
	    }
	}
	break;

    case MORSE_SIGNAL:
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
	    if (timerEvent->tid == text_longTimer_id)
	    {
			text_longSeparation();
	    }
	    break;

	case GR_EVENT_TYPE_KEY_UP:
	    switch (event->keystroke.ch)
	    {
	    case '\r': // Wheel button
			text_morse_key_up();
			break;
	    }
	}
	break;
    }

    return ch;
}

void text_morse_enable()
{
    text_morse_play();
}

void text_morse_init(void)
{
    int i;

    for (i = 0; i < MORSE_MAX_SYMBOL_LENGTH; i++) {
		text_symbol[i] = '\0';
    }

	/* if (text_beep_enable) {
		text_beep_init();
	} */
    
    text_state = MORSE_IDLE;
    text_morse_play();
}

void text_morse_free()
{
	/* if (text_beep_enable) {
		text_beep_free();
	} */

    if (text_state != MORSE_IDLE) {
		text_morse_play(); // Go to IDLE in order to destroy all timers
    }
}

/* Morse Text Input Module */

void text_morse_handle_gr_event(GR_EVENT *event)
{
	char ch;
	ch = text_morse_handle_event(event);
	if (ch != '\0') {
		if ( (ch != ' ') || (ch != text_mlastchar) ) {
			text_output_char(ch);
			text_mlastchar = ch;
		}
	}
	if ((event->type) == GR_EVENT_TYPE_KEY_DOWN) {
		switch (event->keystroke.ch) {
			case 'd':
				/* text_morse_enable(); */
				/*
					The proper distinction between newline and exit
					is more important than pausing morse input.
					It's not like anybody uses morse code text input
					anyway.
				*/
				text_output_char(10);
				text_mlastchar = 10;
				break;
			case 'm':
				text_exit();
				break;
			case 'w':
				text_output_char(8);
				text_mlastchar = 8;
				break;
			case 'f':
				text_output_char(32);
				text_mlastchar = 32;
				break;
			case 'l':
				text_mlowercase = 0;
				break;
			case 'r':
				text_mlowercase = 1;
				break;
			break;
		}
	}
}
