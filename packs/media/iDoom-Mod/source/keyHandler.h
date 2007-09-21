/*
 *  gamelib - keyHandler.h - Keyhandling.
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

#ifndef __KEY_HANDLER__
#define __KEY_HANDLER__

#include <sys/time.h>
#include "d_event.h"

#define KH_KEY_MENU    50
#define KH_KEY_REWIND  17
#define KH_KEY_PLAY    32
#define KH_KEY_FORWARD 33

#define KH_KEY_ACTION  28
#define KH_KEY_HOLD    35

#define KH_WHEEL_L     38
#define KH_WHEEL_R     19

#define KH_RELEASED 0x80
#define KH_PRESSED     0

#define  KEYCODE(a) (a & 0x7f)  // Use to get keycode of scancode.
#define KEYSTATE(a) (a & 0x80)  // Check if PRESSED or RELEASED.

int  startKeyHandler(void);
int           getKey(void);
void closeKeyHandler(void);
int enhancedInput(struct timeval diff);
void parseKeyMapFile(char * fileName);
boolean getEventFromButton(int button, event_t * event);

#endif // __KEY_HANDLER__
