/*
 * Copyright (C) 2004 Zsombor Kovacs (zsk009)
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
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "pz.h"
#include "piezo.h"

#define FAHRENHEIT 0
#define CELSIUS    1
#define WRITE 1
#define LEAVE 0
#define YES 1

static GR_WINDOW_ID conv_wid;  // window object
static GR_GC_ID conv_gc;       // graphics context object
static GR_SCREEN_INFO screen_info;
float result=0;
int answer=0;
int redraw=0;
int number=50, result_type;    // 50 = initial value for the scrolling number

static void conv_do_draw()
{
  GR_SIZE width, height, base, text_width;
  char str[50];
  
  sprintf(str, "%d", number);
  GrGetGCTextSize(conv_gc, str, -1, GR_TFASCII, &width, &height, &base);
  text_width = (screen_info.cols/2)-(width/2);

      // clean the number we need to write over
  GrSetGCForeground(conv_gc, WHITE);
  GrFillRect(conv_wid, conv_gc, text_width-5, 41, 25, 18); // the number
  GrSetGCForeground(conv_gc, BLACK);

  // draw the number that the user is choosing
  GrText(conv_wid, conv_gc, text_width, 55, str, -1, GR_TFASCII);
  
  // Ok, this function gets called when podzilla needs to draw the screen
  if (answer==WRITE) {
    // this result variable is not declared yet, it will tell us if the user has pressed the center button and we now need to draw the result of the conversion

    // clean the results to write over them
  GrSetGCForeground(conv_gc, WHITE);
  GrFillRect(conv_wid, conv_gc, 0, 15, screen_info.cols, 20); // the results text
  GrSetGCForeground(conv_gc, BLACK);

    if (result_type == FAHRENHEIT)
      sprintf(str, "Fahrenheit: %.0f", result);
    else if (result_type == CELSIUS)
      sprintf(str, "Celsius: %.2f", result);

    // we are giving this function our string, and it will tell us the size of the text in pixels
    GrGetGCTextSize(conv_gc, str, -1, GR_TFASCII, &width, &height, &base);
    
    // the second and third arguments are LEFT and TOP
    // so, for left we have the middle of the screen minus the size of the text
    // and for top, 30 pixels... should do it
    GrText(conv_wid, conv_gc, (screen_info.cols/2)-(width/2), 30, str, -1, GR_TFASCII);
	answer=LEAVE;
  }
}

static int conv_do_keystroke(GR_EVENT * event)
{
	switch (event->type) {
  	case GR_EVENT_TYPE_KEY_DOWN: // if the key is down
      switch (event->keystroke.ch) {
		case 'd':
			// Play button
			// reset the textbox
			GrSetGCForeground(conv_gc, WHITE);
			GrFillRect(conv_wid, conv_gc, 0, 15, screen_info.cols, 20); // the results text
			GrSetGCForeground(conv_gc, BLACK);
			number = 50;
			beep();

			break;

        case 'm':
          // menu button
		  number = 50;
		  beep();
          pz_close_window (conv_wid);
          GrDestroyGC(conv_gc);

          break;
        case 'l':
          // scroll left
          number = number - 1; // same thing as number--
		  beep();

          break;
        case 'r':
          // scroll right
          number = number + 1; // same as number++
		  beep();

          break;

        case 'w':
          // rewind
          result = ((9* number)/5 + 32);
          result_type = FAHRENHEIT;
		  answer=WRITE;
		  beep();
          
          break;
        case 'f':
          // fast forward
          result = ((float) 5/9) * (number - 32);
          result_type = CELSIUS;
		  answer=WRITE;
		  beep();
          
          break;
      }
  }

  conv_do_draw();
	
	return (0);
}


// this function gets called by the actual podzilla application when you activate it in the menu
void new_conv_window()
{
  // get the graphics context
	conv_gc = pz_get_gc(1);

  pz_draw_header ("Temp Converter");

  // get the screen info
  GrGetScreenInfo(&screen_info);
	
	// pretty straight-forward
	GrSetGCUseBackground(conv_gc, GR_TRUE);
	GrSetGCBackground(conv_gc, WHITE);
	GrSetGCForeground(conv_gc, BLACK);

  // create a new window. The 4 first arguments are the size of the window (left, top, right, bottom)
  // conv_do_draw and conv_do_keystroke are our functions. We are basicly telling podzilla to call them when it needs to draw the screen or when you pressed a button
	conv_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), conv_do_draw, conv_do_keystroke);

  // these are the kinds of events that our application will accept KEY_UP and KEY_DOWN basicly means that podzilla will call conv_do_keystroke when you push a key down, and also when you release it (key_up)... the other one I dont know what it does but if it's not there it screws up the drawing
	GrSelectEvents(conv_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_UP|GR_EVENT_MASK_KEY_DOWN);

  // actually "activate" the new window
	GrMapWindow(conv_wid);

    //draw fahrenheit and celsius lables
  GrText(conv_wid, conv_gc, 1, (screen_info.rows-25), "Fahrenheit", -1, GR_TFASCII);
  GrText(conv_wid, conv_gc, (screen_info.cols-35), (screen_info.rows-25), "Celsius", -1, GR_TFASCII);

  // draw a rectangle for the input box
  GrRect(conv_wid, conv_gc, ((screen_info.cols/2)-40), 40, 80, 20);

 }
