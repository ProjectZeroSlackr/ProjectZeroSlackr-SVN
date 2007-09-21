/*
 *  gamelib - keyHandler.c - Keyhandling.
 *
 *  This file contains functions to handle key events from the iPod.
 *
 *  Copyright 2005 Benjamin Eriksson & Mattias Pierre.
 * 
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <linux/kd.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h> // For O_NONBLOCK
#include "keyHandler.h"
#include "d_event.h"
#include "doomdef.h"
#include "d_event.h"

#define USE_WHEEL  255
#define WHEEL_SENS 254

static int     consoleFd = -1;
static struct  termios old;
static char    keymap[60];
static boolean useWheelTurn     = false;
static char    wheelSensitivity = 150;

void finishWithMessage(char * msg)
{
	fprintf(stderr, msg);
	closeKeyHandler();
	exit(-1);
}

int startKeyHandler(void)
{
	struct termios new;
		
	if ((consoleFd = open("/dev/console", O_NONBLOCK)) < 0)
		fprintf(stderr, "Could not open /dev/console");
	
	if (tcgetattr(consoleFd, &old) < 0)
		fprintf(stderr, "Could not save old termios");
	
	new = old;
	
	new.c_lflag    &= ~(ICANON | ECHO  | ISIG);
	new.c_iflag    &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON | BRKINT);
	new.c_cc[VMIN]  = 0;
	new.c_cc[VTIME] = 0;
	
	if (tcsetattr(consoleFd, TCSAFLUSH, &new) < 0)
		finishWithMessage( "Could not set new termios");

	if (ioctl(consoleFd, KDSKBMODE, K_MEDIUMRAW) < 0)
		finishWithMessage( "Could not set keyboardmode");

	return 0;
}

void closeKeyHandler(void)
{	
	if (tcsetattr(consoleFd, TCSAFLUSH, &old) < 0)
	fprintf(stderr, "Could not reset old termios");
	
	if (consoleFd > 2)
		if (close(consoleFd) < 0)
			fprintf(stderr, "Could not close console");
}

int i_getKey(void)
{	
	int c = 0;
	
	if (read(consoleFd, &c, 1) != 1)
		c = -1;
	
	return c;
}

int getKeys(int buffsize, char* buff)
{
	return read(consoleFd, buff, buffsize);
}

int tv_cmp(struct timeval *a, struct timeval *b) {
  if (a->tv_sec > b->tv_sec)
    return 1;
  if (a->tv_sec < b->tv_sec)
    return -1;
  if (a->tv_usec > b->tv_usec)
    return 1;
  if (a->tv_usec < b->tv_usec)
    return -1;
  return 0;
}

void tv_add(struct timeval *a, struct timeval *b) {
  a->tv_usec += b->tv_usec;
  a->tv_sec  += b->tv_sec + (a->tv_usec / 1000000);
  a->tv_usec %= 1000000;
}

// is t1 greater than t2?
int isGreaterThanTimeWithDiff( struct timeval * t1,  struct timeval * t2,
                               struct timeval * diff )
{
	if ((t1->tv_sec - t2->tv_sec) > diff->tv_sec)
		return 1;
	
	if ((t1->tv_sec  - t2->tv_sec) == diff->tv_sec && 
	    (t1->tv_usec - t2->tv_usec) > diff->tv_usec)
		return 1;
	
	return 0;
}

int enhancedInput(struct timeval diff)
{
	static int  buff = 0;
	static int  wheelDirection = 0;
	static struct timeval lastTime = {0, 0};
	
	// Get key event.
	int c = i_getKey();
	
	if (buff > 0)
	{
		int ret  = buff;
		    buff = c;
		    c    = ret;
	}
	
	if (wheelDirection != 0)
	{
		struct timeval now;
		struct timeval tmp = lastTime;
		gettimeofday(&now, 0);
		tv_add(&tmp, &diff);
		
		if (tv_cmp(&now, &tmp) == 1)
		{
			int dir = wheelDirection;
			buff = c;
			wheelDirection = 0;
			gettimeofday(&lastTime, 0); // NOTE: This should not be needed.
			return dir | KH_RELEASED;
		}
	}
	
	if(KEYCODE(c) == KH_WHEEL_L || KEYCODE(c) == KH_WHEEL_R)
	{
		gettimeofday(&lastTime, 0);
		
		if(wheelDirection != KEYCODE(c) && KEYSTATE(c) == KH_PRESSED)
		{
			int dir = wheelDirection;
			wheelDirection = KEYCODE(c);
			
			if (dir != 0)
			{
				buff = c;
				return dir | KH_RELEASED;
			}
		}
		else
			c = 0;
	}
	
	return c;
}

boolean getEventFromButton(int button, event_t * event)
{
	event->data1 = keymap[KEYCODE(button)];
	
	if (event->data1 < 1)
		return false;
	
	if(KEYSTATE(button) == KH_PRESSED)
		event->type = ev_keydown;
	else
		event->type = ev_keyup;
	
	return true;
}

char getKeyEquiv(char * string, boolean real)
{
	if (string == NULL)
		return 0;
	
	if (real)
	{
		// Real keys
		if (strncmp(string, "RIGHTARROW",10) == 0) return KEY_RIGHTARROW;
		if (strncmp(string, "LEFTARROW" , 9) == 0) return KEY_LEFTARROW;
		if (strncmp(string, "UPARROW"   , 7) == 0) return KEY_UPARROW;
		if (strncmp(string, "DOWNARROW" , 9) == 0) return KEY_DOWNARROW;
		if (strncmp(string, "ESCAPE"    , 6) == 0) return KEY_ESCAPE;
		if (strncmp(string, "ENTER"     , 5) == 0) return KEY_ENTER;
		if (strncmp(string, "TAB"       , 3) == 0) return KEY_TAB;
		if (strncmp(string, "F1"        , 2) == 0) return KEY_F1;
		if (strncmp(string, "F2"        , 2) == 0) return KEY_F2;
		if (strncmp(string, "F3"        , 2) == 0) return KEY_F3;
		if (strncmp(string, "F4"        , 2) == 0) return KEY_F4;
		if (strncmp(string, "F5"        , 2) == 0) return KEY_F5;
		if (strncmp(string, "F6"        , 2) == 0) return KEY_F6;
		if (strncmp(string, "F7"        , 2) == 0) return KEY_F7;
		if (strncmp(string, "F8"        , 2) == 0) return KEY_F8;
		if (strncmp(string, "F9"        , 2) == 0) return KEY_F9;
		if (strncmp(string, "F10"       , 3) == 0) return KEY_F10;
		if (strncmp(string, "F11"       , 3) == 0) return KEY_F11;
		if (strncmp(string, "F12"       , 3) == 0) return KEY_F12;
		if (strncmp(string, "BACKSPACE" , 9) == 0) return KEY_BACKSPACE;
		if (strncmp(string, "PAUSE"     , 5) == 0) return KEY_PAUSE;
		if (strncmp(string, "EQUALS"    , 6) == 0) return KEY_EQUALS;
		if (strncmp(string, "MINUS"     , 5) == 0) return KEY_MINUS;
		if (strncmp(string, "RSHIFT"    , 6) == 0) return KEY_RSHIFT;
		if (strncmp(string, "RCTRL"     , 5) == 0) return KEY_RCTRL;
		if (strncmp(string, "RALT"      , 4) == 0) return KEY_RALT;
		if (strncmp(string, "LALT"      , 4) == 0) return KEY_LALT;
		if (strncmp(string, "SPACE"     , 5) == 0) return ' ';
		
		// Bool values
		if (strncmp(string, "YES"       , 3) == 0) return 1;
		if (strncmp(string, "NO"        , 2) == 0) return 0;
		
		// Regular characters
		return string[0];
	}
	else
	{
		// iPod Keys
		if (strncmp(string, "MENU"   , 4) == 0) return KH_KEY_MENU;
		if (strncmp(string, "REWIND" , 6) == 0) return KH_KEY_REWIND;  
		if (strncmp(string, "PLAY"   , 4) == 0) return KH_KEY_PLAY;    
		if (strncmp(string, "FORWARD", 7) == 0) return KH_KEY_FORWARD; 
		if (strncmp(string, "ACTION" , 6) == 0) return KH_KEY_ACTION;  
		if (strncmp(string, "HOLD"   , 4) == 0) return KH_KEY_HOLD;    
		if (strncmp(string, "WHEEL_L", 7) == 0) return KH_WHEEL_L; 
		if (strncmp(string, "WHEEL_R", 7) == 0) return KH_WHEEL_R;
	}
	
	// Options (expand this or move to another file or whatever).
	if (strncmp(string, "UseWheelTurning",  15) == 0) return USE_WHEEL;
	if (strncmp(string, "WheelSensitivity", 16) == 0) return WHEEL_SENS;
	
	return 0;
}

void parseKeyMapFile(char * fileName)
{
	char   buffer[128];
	char * delim = " \t";
	char * commentPos;
	int iPodKey, realKey;
	FILE * keyFile;
	
	// Set the default values.
	keymap[KH_KEY_MENU   ] = KEY_UPARROW;
	keymap[KH_KEY_REWIND ] = KEY_LEFTARROW;
	keymap[KH_KEY_PLAY   ] = KEY_RCTRL;
	keymap[KH_KEY_FORWARD] = KEY_RIGHTARROW;
	keymap[KH_KEY_ACTION ] = ' ';
	keymap[KH_KEY_HOLD   ] = KEY_ESCAPE;
	keymap[KH_WHEEL_L    ] = 'y';
	keymap[KH_WHEEL_R    ] = 'r';
	
	if ((keyFile = fopen(fileName, "r")) == 0)
		return; // There is no actual error in not having a key-def file.
	
	while (fgets(buffer, 128, keyFile))
	{
		if ((commentPos = strchr(buffer, '#')) != NULL)
			*commentPos = '\0';
		
		char * tok1 = strtok(buffer, delim);
		char * tok2 = strtok(NULL,   delim);
		
		iPodKey = getKeyEquiv(tok1, false);
		realKey = getKeyEquiv(tok2, true );
		
		if (iPodKey == 0 || realKey == 0)
		    continue;
		
		if (iPodKey == WHEEL_SENS)
		{
			int value;
			sscanf(tok2, "%d", &value);
			wheelSensitivity = value;
		}
		
		if (iPodKey == USE_WHEEL)
			useWheelTurn = realKey;
		else
			keymap[iPodKey] = realKey;
	}
	
	fclose(keyFile);
}

int getKey()
{
	int button;
	
	if (useWheelTurn)
	{
		static struct timeval diff;
		diff.tv_sec  = 0;
		diff.tv_usec = wheelSensitivity * 1000;
		button       = enhancedInput(diff);
	}
	else
		button = i_getKey();
	
	return button;
}
