/*
 * Copyright (C) 2005 Stefan Lange-Hegermann (BlackMac)
 * Modifications by Jonathan Bettencourt (jonrelay)
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
	This wheelboard.c is *not* the same as BlackMac's wheelboard.c!
*/

#include <string.h>
#include "../pz.h"
#include "../appearance.h"
#include "../textinput.h"

const char text_wlb_lowercase[]=  "abcdefghijklmnopqrstuvwxyz";
const char text_wlb_uppercase[]=  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char text_wlb_numeric1[]=   "0123456789.,/!@#$%^&*()[]?";
const char text_wlb_numeric2[]=   "~`+-={}<>|:;\\\'\"_£¢¥µ÷©®¡¿§";
const char text_wlb_accentlc[]=   "àáâäåãèéêëìíîïòóôöøõùúûüñÿ";
const char text_wlb_accentuc[]=   "ÀÁÂÄÅÃÈÉÊËÌÍÎÏÒÓÔÖØÕÙÚÛÜÑß";
const char text_wlb_numericmode[]="0123456789.-E01234567+-*/^";
const char text_wlb_cursormode[] ="Move         Cursor       ";
static int text_wlb_cset = 0;
static char text_wlb_charset[27];
static int text_wlb_startpos=0;
static int text_wlb_limit=13;
static int text_wlb_position=3;

void text_wlb_reset(void)
{
	text_wlb_cset = 0;
	if (text_get_numeric_mode() != 0) {
		strcpy(text_wlb_charset, text_wlb_numericmode);
	} else {
		strcpy(text_wlb_charset, text_wlb_lowercase);
	}
	text_wlb_startpos=0;
	text_wlb_limit=13;
	text_wlb_position=3;
}

void text_wlb_switch_cset(void)
{
	text_wlb_limit=13;
	text_wlb_position=3;
	text_wlb_startpos=0;
	if (text_get_numeric_mode() != 0) {
		if (text_wlb_cset) {
			strcpy(text_wlb_charset, text_wlb_numericmode);
			text_wlb_cset = 0;
		} else {
			strcpy(text_wlb_charset, text_wlb_cursormode);
			text_wlb_cset = -1;
		}
	} else {
		text_wlb_cset++;
		if (text_wlb_cset >= 6) { text_wlb_cset = -1; }
		switch (text_wlb_cset) {
		case 0:
			strcpy(text_wlb_charset, text_wlb_lowercase);
			break;
		case 1:
			strcpy(text_wlb_charset, text_wlb_uppercase);
			break;
		case 2:
			strcpy(text_wlb_charset, text_wlb_numeric1);
			break;
		case 3:
			strcpy(text_wlb_charset, text_wlb_numeric2);
			break;
		case 4:
			strcpy(text_wlb_charset, text_wlb_accentlc);
			break;
		case 5:
			strcpy(text_wlb_charset, text_wlb_accentuc);
			break;
		case -1:
			strcpy(text_wlb_charset, text_wlb_cursormode);
			break;
		}
	}
	text_draw();
}

void text_wlb_advance_level(int i)
{
	int add;
	
	text_wlb_position=3;
	
	if (text_wlb_limit%2!=0) 
		add=1;
	else
		add=0;
		
	if (text_wlb_limit>1) {
		if (i==1) {
			text_wlb_startpos=text_wlb_startpos+(text_wlb_limit)-add;
		}
		text_wlb_limit=(text_wlb_limit)/2+add;
	} else {
		if (i==1) {
			text_output_char(text_wlb_charset[text_wlb_startpos+1]);
		} else {
			text_output_char(text_wlb_charset[text_wlb_startpos]);
		}
		text_wlb_limit=13;
		text_wlb_startpos=0;
	}
	
}

void text_wlb_set_blackonwhite(GR_GC_ID context, int b)
{
	if (b==0) {
		GrSetGCForeground(context, appearance_get_color(CS_SELFG));
    	GrSetGCBackground(context, appearance_get_color(CS_SELBG));
    } else {
    	GrSetGCForeground(context, appearance_get_color(CS_FG));
    	GrSetGCBackground(context, appearance_get_color(CS_BG));
    }
}

void text_wlb_draw_Menu(GR_GC_ID gc)
{
    /* GrClearWindow (text_get_bufwid(), GR_FALSE); */
    GrSetGCForeground(gc, appearance_get_color(CS_FG));
    GrSetGCBackground(gc, appearance_get_color(CS_BG));
    
    GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
    if (text_wlb_position<1) {
    	text_wlb_set_blackonwhite(gc,1);
    	GrFillRect(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width()/2, 20);
    	text_wlb_set_blackonwhite(gc,0);
    	
    } else {
    	text_wlb_set_blackonwhite(gc,1);
    }
    
    GrText(text_get_bufwid(),
           gc,
           1, text_get_height()-4,  /* Location */
           &text_wlb_charset[text_wlb_startpos],
           text_wlb_limit,
           GR_TFASCII);
           
	if (text_wlb_position>5) {
    	text_wlb_set_blackonwhite(gc,1);
    	GrFillRect(text_get_bufwid(), gc, text_get_width()/2, text_get_height()-16, text_get_width()/2, 20);
    	text_wlb_set_blackonwhite(gc,0);
    } else {
    	text_wlb_set_blackonwhite(gc,1);
    }
	GrText(text_get_bufwid(),
           gc,
           text_get_width()/2, text_get_height()-4,  /* Location */
           &text_wlb_charset[text_wlb_limit+text_wlb_startpos],
           text_wlb_limit,
           GR_TFASCII);
    
    /* Position */
    GrSetGCForeground(gc, LTGRAY);
    GrLine(text_get_bufwid(), gc, text_get_width()/2, text_get_height()-16, text_wlb_position*27, text_get_height()-16);
	
}

int text_wlb_handle_event (GR_EVENT *event)
{
    switch (event->type)
    {
    case GR_EVENT_TYPE_TIMER:
        break;

    case GR_EVENT_TYPE_KEY_DOWN:
        switch (event->keystroke.ch)
        {
        case '\r': /* Wheel button */
        case '\n':
        	text_wlb_switch_cset();
            break;
        case 'd': /* Play/pause button */
        	text_output_char(10);
            break;
        case 'w': /* Rewind button */
        	text_output_char(8);
			text_draw();
            break;
        case 'f': /* Fast forward button */
        	text_output_char(32);
			text_wlb_limit=13;
			text_wlb_position=3;
			text_wlb_startpos=0;
			text_draw();
            break;
        case 'l': /* Wheel left */
        	if (text_wlb_cset<0) {
        		text_output_char(28);
        	} else {
            	if ((text_wlb_position--)==0) {
            		text_wlb_advance_level(0);
            	}
            	text_draw();
            }
            break;
        case 'r': /* Wheel right */
        	if (text_wlb_cset<0) {
        		text_output_char(29);
        	} else {
            	if ((text_wlb_position++)==6) {
        			text_wlb_advance_level(1);
            	}
            	text_draw();
            }
            break;
        case 'm': /* Menu button */
        	text_exit();
            break;
        default:
            break;
        }
        break;   /* key down */
    }
    return 1;
}

