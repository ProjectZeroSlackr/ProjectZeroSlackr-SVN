/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
 * WheelBoard concept by Stefan Lange-Hegermann (BlackMac)
 * Four-Button Keyboard concept by Jonathan Bettencourt (jonrelay)
 * Dial Type and Cursive concepts by Peter Burk, implemented by jonrelay
 * Morse Code concept by mattlivesey, implemented by fre_ber
 * Telephone Keypad concepts by jackinloadup
 * Four-Button Telephone Keypad concept by boza111, implemented by jonrelay
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

#include <string.h>
#include <stdlib.h>
#include "pz.h"
#include "ipod.h"
#include "appearance.h"
#include "textinput.h"
#include "clickwheel.h"

static GR_WINDOW_ID text_wid;
static GR_WINDOW_ID text_bufwid;
static GR_TIMER_ID text_timer;
static GR_GC_ID text_gc;
static int text_timer_created;
static int text_width;
static int text_height;
static char * text_input_buffer = NULL;
static int text_input_buffer_size = 0;
static int text_position;
static int text_iposition;
static int text_input_method;
static int text_numeric_mode;
int text_ipod_generation;
void (*text_callback)(void) = NULL;
void (*text_draw_hook)(void) = NULL;
void (*text_output_char_hook)(int) = NULL;
int (*text_handle_event_hook)(GR_EVENT *) = NULL;
void (*text_exit_hook)(void) = NULL;

/* cursive.c */
extern void text_cursive_reset(void);
extern void text_cursive_print_status(GR_GC_ID gc);
extern void text_cursive_accept_keystroke(char c);
extern void text_cursive_push(void);

/* dial.c */
extern void text_dial_draw_status_full(GR_GC_ID gc);
extern void text_dial_event(char ch);
extern void text_dial_reset(void);

/* fourbtnkbd.c */
extern void text_fbkSetMode(int m);
extern void text_fbkPushButton(int b);
extern void text_fbk_print_mode_status(GR_GC_ID gc);
extern void text_fbk_print_mode_status_4g(GR_GC_ID gc);

/* keypad.c */
extern void text_keypad_print_mode_status(GR_GC_ID gc);
extern void text_keypad_print_mode_status_4g(GR_GC_ID gc);
extern void text_keypad_output(void);
extern void text_keypad_reset(void);
extern int text_keypad_fbevent(GR_EVENT *event);
extern int text_keypad_4gevent(GR_EVENT *event);

/* morse.c */
extern void text_morse_init(void);
extern void text_morse_free(void);
extern void text_morse_handle_gr_event(GR_EVENT *event);

/* osk.c */
extern void text_osk_init(void);
extern void text_osk_free(void);
extern void text_osk_draw_state(GR_WINDOW_ID wid, GR_GC_ID gc);
extern void text_osk_handle_event(char ch);

/* thumbscript.c */
extern void text_keypad_thumbscript_reset(void);
extern void text_keypad_thumbscript_draw(GR_GC_ID gc);
extern int text_keypad_thumbscript_event(GR_EVENT *event);

/* wheelboard.c */
extern void text_wlb_draw_Menu(GR_GC_ID gc);
extern int text_wlb_handle_event(GR_EVENT *event);
extern void text_wlb_reset(void);


GR_WINDOW_ID text_get_wid(void)
{
	return text_wid;
}

GR_WINDOW_ID text_get_bufwid(void)
{
	return text_bufwid;
}

GR_TIMER_ID text_get_timer(void)
{
	return text_timer;
}

GR_GC_ID text_get_gc(void)
{
	return text_gc;
}

int text_get_timer_created(void)
{
	return text_timer_created;
}

int text_get_width(void)
{
	return text_width;
}

int text_get_height(void)
{
	return text_height;
}

char * text_get_buffer(void)
{
	return text_input_buffer;
}

void text_set_buffer(char * s)
{
	text_free_buffer();
	text_input_buffer_size = (strlen(s) + TEXT_BUFFER_INC);
	if ( (text_input_buffer = (char *)malloc(text_input_buffer_size)) == NULL ) {
		pz_error(_("Not enough memory to create text input buffer."));
		text_input_buffer_size = 0;
		return;
	}
	strcpy(text_input_buffer, s);
	text_position = 0;
	text_iposition = 0;
}

void text_clear_buffer(void)
{
	text_free_buffer();
	if ( (text_input_buffer = (char *)malloc(TEXT_BUFFER_INC)) == NULL ) {
		pz_error(_("Not enough memory to create text input buffer."));
		return;
	} else {
		text_input_buffer_size = TEXT_BUFFER_INC;
	}
	text_input_buffer[0] = 0;
	text_position = 0;
	text_iposition = 0;
}

void text_free_buffer(void)
{
	/* hopefully we can call this as many times as we want and not get any double free bugs */
	if (text_input_buffer != NULL) {
		free(text_input_buffer);
		text_input_buffer = NULL;
		text_input_buffer_size = 0;
	}
}

int text_get_length(void)
{
	return text_position;
}

int text_get_iposition(void)
{
	return text_iposition;
}

void text_set_length(int i)
{
	text_position = i;
}

void text_set_iposition(int i)
{
	text_iposition = i;
}

int text_get_numeric_mode(void)
{
	return text_numeric_mode;
}

int text_get_input_method(void)
{
	return ipod_get_setting(TEXT_INPUT);
}

int text_get_ipod_generation(void)
{
	return text_ipod_generation;
}

void text_reset_timer(int t)
{
	if (text_timer_created != 0) {
		GrDestroyTimer(text_timer);
		text_timer = GrCreateTimer(text_wid, t);
	}
}

void text_exit(void)
{
	if (text_exit_hook != NULL) {
		(*text_exit_hook)();
	} else {
		text_exit_continue();
	}
}

void text_exit_continue(void)
{
	/* do anything the current input method needs to do on exit */
	switch (text_input_method) {
	case TI_ONSCREEN:
		text_osk_free();
		break;
	case TI_MORSE:
		text_morse_free();
		break;
	case TI_KEYPAD:
	case TI_THUMBSCRIPT:
		clickwheel_destroy_timer();
		break;
	}
	/* move the cursor back because message.c assumes it is in the center */
	GrGetScreenInfo(&screen_info);
	GrMoveCursor(screen_info.cols/2, screen_info.rows/2);
	/* destroy a timer if we created one */
	if (text_timer_created != 0) {
		GrDestroyTimer(text_timer);
		text_timer_created = 0;
	}
	/* close the text window */
	GrDestroyGC(text_gc);
	pz_close_window(text_wid);
	GrDestroyWindow(text_bufwid);
	if (text_callback != NULL) {
		(*text_callback)();
	}
}

void text_init(void)
{
	switch (text_input_method) {
	case TI_SCROLL:
		text_dial_reset();
		break;
	case TI_ONSCREEN:
		text_osk_init();
		break;
	case TI_MORSE:
		text_morse_init();
		break;
	case TI_CURSIVE:
		text_cursive_reset();
		break;
	case TI_WHEELBOARD:
		text_wlb_reset();
		break;
	case TI_FBK:
		text_fbkSetMode(-1);
		break;
	case TI_DIALTYPE:
		text_dial_reset();
		break;
	case TI_KEYPAD:
		clickwheel_create_timer(text_wid);
		text_keypad_reset();
		break;
	case TI_THUMBSCRIPT:
		clickwheel_create_timer(text_wid);
		text_keypad_thumbscript_reset();
		break;
	case TI_FBKEYPAD:
		text_keypad_reset();
		break;
	case TI_DASHER:
		break;
	case TI_SPEECH:
		break;
	}
}

void text_draw_status(GR_GC_ID gc)
{
    GrSetGCForeground(gc, appearance_get_color(CS_FG));
    GrSetGCBackground(gc, appearance_get_color(CS_BG));
	switch (text_input_method) {
	case TI_SCROLL:
		text_dial_draw_status_full(gc);
		break;
	case TI_ONSCREEN:
		text_osk_draw_state(text_bufwid, gc);
		break;
	case TI_MORSE:
		break;
	case TI_CURSIVE:
		text_cursive_print_status(gc);
		break;
	case TI_WHEELBOARD:
		text_wlb_draw_Menu(gc);
		break;
	case TI_FBK:
		if (text_ipod_generation == 3) {
			text_fbk_print_mode_status(gc);
		} else {
			text_fbk_print_mode_status_4g(gc);
		}
		break;
	case TI_DIALTYPE:
		text_dial_draw_status_full(gc);
		break;
	case TI_KEYPAD:
		text_keypad_print_mode_status_4g(gc);
		break;
	case TI_THUMBSCRIPT:
		text_keypad_thumbscript_draw(gc);
		break;
	case TI_FBKEYPAD:
		text_keypad_print_mode_status(gc);
		break;
	case TI_DASHER:
		break;
	case TI_SPEECH:
		break;
	}
}

void text_draw(void)
{
	if (text_draw_hook != NULL) {
		(*text_draw_hook)();
	} else {
		text_draw_continue();
	}
}

void text_draw_continue(void)
{
	int tw, th, tb;
	/* draw the box */
    GrClearWindow (text_bufwid, GR_FALSE);
    GrSetGCBackground(text_gc, appearance_get_color(CS_BG));
    GrSetGCForeground(text_gc, appearance_get_color(CS_BG));
    GrFillRect(text_bufwid, text_gc, 0, 0, text_width, text_height);
    GrSetGCForeground(text_gc, appearance_get_color(CS_FG));
    GrRect(text_bufwid, text_gc, 0, 0, text_width, text_height);
    /* draw the text */
    GrText(text_bufwid, text_gc, 5, 15, text_input_buffer, strlen(text_input_buffer), GR_TFUTF8);
    /* draw the cursor */
    GrSetGCForeground(text_gc, GRAY);
    if ((text_input_buffer[0] != 0) && (text_iposition > 0)) {
    	GrGetGCTextSize(text_gc, text_input_buffer, text_iposition, GR_TFUTF8, &tw, &th, &tb);
    } else {
    	GrGetGCTextSize(text_gc, " ", 1, GR_TFUTF8, &tw, &th, &tb);
    	tw=0;
    }
    GrLine(text_bufwid, text_gc, tw+5, 15-tb, tw+5, 15-tb+th);
    /* draw stuff specific to the current input method */
	text_draw_status(text_gc);
	/* copy the buffer to the actual window */
	GrCopyArea(text_wid, text_gc, 0, 0, text_width+1, text_height+1, text_bufwid, 0, 0, MWROP_SRCCOPY);
}

void text_draw_password(void)
{
	int tw, th, tb;
	char * b;
	int i;
	/* draw the box */
    GrClearWindow (text_bufwid, GR_FALSE);
    GrSetGCBackground(text_gc, appearance_get_color(CS_BG));
    GrSetGCForeground(text_gc, appearance_get_color(CS_BG));
    GrFillRect(text_bufwid, text_gc, 0, 0, text_width, text_height);
    GrSetGCForeground(text_gc, appearance_get_color(CS_FG));
    GrRect(text_bufwid, text_gc, 0, 0, text_width, text_height);
    /* draw the text */
    if ((  b = (char *)malloc((text_position+1) * sizeof(char))  ) != NULL) {
    	for (i=0; i<text_position; i++) { b[i] = '*'; }
    	b[text_position] = 0;
	    GrText(text_bufwid, text_gc, 5, 15, b, text_position, GR_TFASCII);
	    free(b);
    }
    /* draw the cursor */
    GrSetGCForeground(text_gc, GRAY);
    GrGetGCTextSize(text_gc, "*", 1, GR_TFASCII, &tw, &th, &tb);
    tw *= text_iposition;
    GrLine(text_bufwid, text_gc, tw+5, 15-tb, tw+5, 15-tb+th);
    /* draw stuff specific to the current input method */
	text_draw_status(text_gc);
	/* copy the buffer to the actual window */
	GrCopyArea(text_wid, text_gc, 0, 0, text_width+1, text_height+1, text_bufwid, 0, 0, MWROP_SRCCOPY);
}

/* text_output_byte is used to put individual raw bytes into the buffer */

void text_output_byte(char ch)
{
	int i;
	if ((text_position + 4) > text_input_buffer_size) {
		char * tmp;
		if ( (tmp = (char *)realloc(text_input_buffer, text_input_buffer_size + TEXT_BUFFER_INC)) == NULL ) {
			pz_error(_("Not enough memory to expand text input buffer."));
			return;
		} else {
			text_input_buffer = tmp;
			text_input_buffer_size += TEXT_BUFFER_INC;
		}
	}
	i = text_position;
	while (i >= text_iposition) {
		text_input_buffer[i+1] = text_input_buffer[i];
		i--;
	}
	text_input_buffer[text_iposition]=ch;
	text_position++;
	text_iposition++;
	text_input_buffer[text_position]=0;
}

void text_remove_byte(int p)
{
	char c = 32;
	int i = 0;
	while (c != 0) {
		c = text_input_buffer[p + i + 1];
		text_input_buffer[p + i] = c;
		i++;
	}
	text_position--;
	if (text_position < 0) { text_position = 0; }
	text_input_buffer[text_position]=0;
}

/* text_output_char is used to put characters into the buffer, which might
be different from bytes (i.e. for UTF-8 support); it uses int so that
Unicode characters may be used; also, several control characters are
handled (delete, backspace, newline, left and right arrow keys) */

void text_output_char(int ch)
{
	if (text_output_char_hook != NULL) {
		(*text_output_char_hook)(ch);
	} else {
		text_output_char_continue(ch);
	}
}

void text_output_char_continue(int ch)
{
	if (ch == 8) {
		/* backspace */
		if (text_iposition > 0) {
			while ( (text_iposition > 0) && ((text_input_buffer[text_iposition-1] & 0xC0) == 0x80) ) {
				text_iposition--;
				text_remove_byte(text_iposition);
			}
			text_iposition--;
			text_remove_byte(text_iposition);
			text_draw();
		}
	} else if (ch == 127) {
		/* forward delete */
		if (text_iposition < text_position) {
			unsigned char c = text_input_buffer[text_iposition];
			int i = 0;
			if (c < 128) {
				text_remove_byte(text_iposition);
			} else {
				while (c >= 128) { i++; c = ((c * 2) % 256); }
				while (i > 0) {
					text_remove_byte(text_iposition);
					i--;
				}
			}
			text_draw();
		}
	} else if ((ch == 10) || (ch == 13)) {
		/* enter */
		text_exit();
	} else if (ch == 27) {
		/* escape */
		text_exit();
	} else if (ch == 28) {
		/* move left - 28 is left arrow key in Mac OS */
		if (text_iposition > 0) {
			while ( (text_iposition > 0) && ((text_input_buffer[text_iposition-1] & 0xC0) == 0x80) ) {
				text_iposition--;
			}
			text_iposition--;
			if (text_iposition < 0) { text_iposition = 0; }
			text_draw();
		}
	} else if (ch == 29) {
		/* move right - 29 is right arrow key in Mac OS */
		if (text_iposition < text_position) {
			unsigned char c = text_input_buffer[text_iposition];
			int i = 0;
			if (c < 128) {
				text_iposition++;
			} else {
				while (c >= 128) { i++; c = ((c * 2) % 256); }
				text_iposition += i;
			}
			if (text_iposition > text_position) { text_iposition = text_position; }
			text_draw();
		}
	} else {
		/* any other characters we add to the buffer */
		if (text_numeric_mode != 0) {
			if ((ch >= '0') && (ch <= '9')) { text_output_byte(ch); }
			else if ((ch == '+') || (ch == '*') || (ch == '/') || (ch == '^')) { text_output_byte(ch); }
			else if ((ch == '-') || (ch == '=')) { text_output_byte('-'); }
			else if ((ch >= ' ') && (ch <= '?')) { text_output_byte('.'); }
			else if ((ch == 'E') || (ch == '_')) { text_output_byte('e'); }
			else { text_output_byte( '0' + ((ch-7)%10) ); }
		} else {
			/* here we encode the character as UTF-8 */
			if (ch < 0) {
				text_output_byte('?');
			} else if (ch < 0x80) {
				text_output_byte(ch);
			} else if (ch < 0x0800) {
				text_output_byte((ch >> 6        ) | 0xC0);
				text_output_byte((ch       & 0x3F) | 0x80);
			} else if (ch < 0x10000) {
				text_output_byte((ch >> 12       ) | 0xE0);
				text_output_byte((ch >> 6  & 0x3F) | 0x80);
				text_output_byte((ch       & 0x3F) | 0x80);
			} else if (ch < 0x110000) {
				text_output_byte((ch >> 18       ) | 0xF0);
				text_output_byte((ch >> 12 & 0x3F) | 0x80);
				text_output_byte((ch >> 6  & 0x3F) | 0x80);
				text_output_byte((ch       & 0x3F) | 0x80);
			} else {
				text_output_byte('?');
			}
		}
		text_draw();
	}
}

int text_handle_event(GR_EVENT *event)
{
	if (text_handle_event_hook != NULL) {
		return (*text_handle_event_hook)(event);
	} else {
		return text_handle_event_continue(event);
	}
}

int text_handle_event_continue(GR_EVENT *event)
{
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
		switch (text_input_method) {
		case TI_MORSE:
			text_morse_handle_gr_event(event);
			break;
		case TI_CURSIVE:
			text_cursive_push();
			text_draw();
			break;
		case TI_FBK:
			if(((GR_EVENT_TIMER *)event)->tid == text_timer) {
				GrDestroyTimer(text_timer);
				text_timer = 0;
				text_exit();
			}
			break;
		case TI_DIALTYPE:
			/* if (text_dial_get_char() != ' ') {
				text_output_char(text_dial_get_char());
				text_dial_reset();
				text_draw();
			} */
			break;
		case TI_KEYPAD:
			return text_keypad_4gevent(event);
			break;
		case TI_THUMBSCRIPT:
			return text_keypad_thumbscript_event(event);
			break;
		case TI_FBKEYPAD:
			return text_keypad_fbevent(event);
			break;
		case TI_DASHER:
			break;
		}
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		if (event->keystroke.ch == 27) { text_exit(); break; }
		switch (text_input_method) {
		case TI_SERIAL:
			text_output_char(event->keystroke.ch);
			break;
		case TI_SCROLL:
		case TI_DIALTYPE:
			switch (event->keystroke.ch) {
			case 'w':
				text_output_char(8);
				if (text_input_method == TI_DIALTYPE) { text_dial_reset(); text_draw(); }
				break;
			case 'm':
				text_exit();
				break;
			case 'd':
				text_output_char(10);
				break;
			case 'f':
				text_output_char(32);
				if (text_input_method == TI_DIALTYPE) { text_dial_reset(); text_draw(); }
				break;
			case 'l':
			case 'r':
			case '\r':
			case '\n':
				text_dial_event(event->keystroke.ch);
				break;
			}
			break;
		case TI_ONSCREEN:
			text_osk_handle_event(event->keystroke.ch);
			break;
		case TI_MORSE:
			text_morse_handle_gr_event(event);
			break;
		case TI_CURSIVE:
			if ((event->keystroke.ch) == 'm') {
				text_exit();
			} else {
				text_cursive_accept_keystroke(event->keystroke.ch);
				text_reset_timer(1000);
				text_draw();
			}
			break;
		case TI_WHEELBOARD:
			text_wlb_handle_event(event);
			break;
		case TI_FBK:
			switch (event->keystroke.ch) {
			case 'w':
				text_fbkPushButton(0);
				text_draw();
				break;
			case 'm':
				text_fbkPushButton(1);
				text_draw();
				break;
			case 'd':
				text_fbkPushButton(2);
				text_draw();
				break;
			case 'f':
				text_fbkPushButton(3);
				text_draw();
				break;
			case 'l':
				text_output_char(28);
				break;
			case 'r':
				text_output_char(29);
				break;
			case '\r':
			case '\n':
				/* text_exit(); */
				text_timer = GrCreateTimer(text_wid, 500);
				break;
			}
			break;
		case TI_KEYPAD:
			return text_keypad_4gevent(event);
			break;
		case TI_THUMBSCRIPT:
			return text_keypad_thumbscript_event(event);
			break;
		case TI_FBKEYPAD:
			return text_keypad_fbevent(event);
			break;
		case TI_DASHER:
			break;
		case TI_SPEECH:
			break;
		}
		break;
	case GR_EVENT_TYPE_KEY_UP:
		switch (text_input_method) {
		case TI_MORSE:
			text_morse_handle_gr_event(event);
			break;
		case TI_FBK:
			if (text_timer) {
				GrDestroyTimer(text_timer);
				text_timer = 0;
				text_fbkSetMode(-1);
				text_draw();
			}
			break;
		case TI_KEYPAD:
			return text_keypad_4gevent(event);
			break;
		case TI_FBKEYPAD:
			return text_keypad_fbevent(event);
			break;
		}
		break;
	}
	return 1;
}

void text_draw_message(char * s)
{
	int tw, th, tb, tx, tby, ty;
	GrGetGCTextSize(text_gc, s, strlen(s), GR_TFASCII, &tw, &th, &tb);
	tx = (text_width - tw)/2;
	tby = (text_height - th)/2;
	ty = (text_height - th)/2 + tb;
    GrSetGCBackground(text_gc, appearance_get_color(CS_BG));
    GrSetGCForeground(text_gc, appearance_get_color(CS_BG));
    GrFillRect(text_wid, text_gc, tx-5, tby-5, tw+10, th+10);
    GrSetGCForeground(text_gc, appearance_get_color(CS_FG));
    GrRect(text_wid, text_gc, tx-5, tby-5, tw+10, th+10);
	GrText(text_wid, text_gc, tx, ty, s, strlen(s), GR_TFASCII);
}

int text_optimum_height(void)
{
	switch (text_input_method) {
	case TI_OFF:
		return 1;
		break;
	case TI_SERIAL:
	case TI_MORSE:
	case TI_SPEECH:
		return 22;
		break;
	case TI_ONSCREEN:
		return 68;
		break;
	case TI_FBK:
		if (text_ipod_generation == 3) {
			return 40;
		} else {
			return 52;
		}
	case TI_DASHER:
		return 100;
		break;
	default:
		return 40;
		break;
	}
}

int text_optimum_width(void)
{
	GrGetScreenInfo(&screen_info);
	return screen_info.cols - 20;
}

void text_rehook(void (*callback)(void), void (*tdraw)(void), void (*tout)(int), int (*thandle)(GR_EVENT *), void (*texit)(void))
{
	text_callback = callback;
	text_draw_hook = tdraw;
	text_output_char_hook = tout;
	text_handle_event_hook = thandle;
	text_exit_hook = texit;
}

/*
	New text box (e)x(tended) version - four additional parameters
	specify custom functions for text_draw, text_output_char,
	text_handle_event, and text_exit. These can be used for (hopefully)
	any instance in which you might need text input, meaning that
	(hopefully) I'll never have to touch textinput.c again (hopefully).
	PodWrite uses these for multiple line support and other stuff.
*/

void new_text_box_x(int xx, int yy, int w, int h, char * defaulttext, void (*callback)(void),
	void (*tdraw)(void), void (*tout)(int), int (*thandle)(GR_EVENT *), void (*texit)(void) )
{
	int x, y;
	text_input_method = text_get_input_method();
	text_ipod_generation = (ipod_get_hw_version() / 10000);
	if (text_input_method == TI_OFF) {
		text_callback = NULL;
		text_draw_hook = text_draw_continue;
		text_output_char_hook = text_output_char_continue;
		text_handle_event_hook = text_handle_event_continue;
		text_exit_hook = text_exit_continue;
		new_message_window(_("Text input is currently turned off. Select an item in Settings > Text Input to enable it."));
	} else {
		x = xx?xx:10;
		text_width = w?w:(screen_info.cols - 2*x);
		text_height = h?h:text_optimum_height();
		y = yy?yy:( screen_info.rows/2 - text_height/2 );
		
		text_numeric_mode = (text_numeric_mode == 0x7E55AB55)?1:0;
		text_set_buffer(defaulttext);
		if (text_input_buffer == NULL) { return; }
		text_position = strlen(text_input_buffer);
		text_iposition = text_position;
		
		text_callback = callback;
		text_draw_hook = tdraw;
		text_output_char_hook = tout;
		text_handle_event_hook = thandle;
		text_exit_hook = texit;
		text_gc = pz_get_gc(1); /* use to use GrNewGC, but only pz_get_gc gets the font right */
	    /* Open the window: */
	    text_wid = pz_new_window (x, y, text_width, text_height, text_draw, text_handle_event);
	    text_bufwid = GrNewPixmap(text_width+1, text_height+1, NULL);
		
	    /* Select the types of events you need for your window: */
	    GrSelectEvents (text_wid, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP | GR_EVENT_MASK_TIMER);
	    /*
	    	We used to check which text input method was selected
	    	and either select the timer event or not based on whether
	    	or not the input method needed it, but I decided to
	    	just select the timer event anyway because
	    	a) the majority of input methods used timers (8 out of 13)
	    	b) having the timer event go off when it isn't used by
	    	   the text input method is harmless
	    	c) applications that use the text input hooks might
	    	   want to use timers, and they'd never get a timer
	    	   event if the input method didn't use it
	    */
		
	    /* Display the window: */
	    GrMapWindow(text_wid);
	    text_init();
		text_draw();
		
		/* Set the mouse location to ensure our window has the focus: */
		GrMoveCursor(x + text_width/2, y + text_height/2);
		/* GrSetFocus(text_wid); */
		
		/* Register for a timer if we need it: */
		text_timer_created = 0;
		if (
			/* (text_input_method == TI_MORSE) || */ /* morse uses its own timers */
			(text_input_method == TI_CURSIVE) ||
			(text_input_method == TI_DIALTYPE) ||
			(text_input_method == TI_KEYPAD) ||
			(text_input_method == TI_FBKEYPAD)
			) {
			text_timer = GrCreateTimer(text_wid, 1000);
			text_timer_created = 1;
		}
		if (
			(text_input_method == TI_DASHER)
			) {
			text_timer = GrCreateTimer(text_wid, 10);
			text_timer_created = 1;
		}
	}
}

/*
	Regular new text box function - accepts x, y, width, height,
	default text, and callback function for when text box closes.
	Used for basic instances when only one line of text input is
	necessary.
*/

void new_text_box(int x, int y, int w, int h, char * defaulttext, void (*callback)(void) )
{
	new_text_box_x(x, y, w, h, defaulttext, callback,
		text_draw_continue, text_output_char_continue, text_handle_event_continue, text_exit_continue);
}

void new_text_box_password(int x, int y, int w, int h, char * defaulttext, void (*callback)(void) )
{
	new_text_box_x(x, y, w, h, defaulttext, callback,
		text_draw_password, text_output_char_continue, text_handle_event_continue, text_exit_continue);
}

void new_text_box_numeric(int x, int y, int w, int h, char * defaulttext, void (*callback)(void) )
{
	text_numeric_mode = 0x7E55AB55;
	new_text_box_x(x, y, w, h, defaulttext, callback,
		text_draw_continue, text_output_char_continue, text_handle_event_continue, text_exit_continue);
}

/*
	New text box n(o) p(arameters) - just displays a text box for
	n(o) p(urpose) whatsoever - used for Text Input Demo menu item
	because menu items can only take void f(void) functions.
*/

void new_text_box_np(void)
{
	new_text_box(0, 0, 0, 0, "", NULL);
}

void new_text_box_nppw(void)
{
	new_text_box_password(0, 0, 0, 0, "", NULL);
}
