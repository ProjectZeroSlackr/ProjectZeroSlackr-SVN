/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
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

#ifndef _TEXTINPUT_H_
#define _TEXTINPUT_H_

#define TEXT_INPUT 50

#define TI_OFF 0			/* implemented */
#define TI_SERIAL 1			/* implemented */
#define TI_SCROLL 2			/* implemented */
#define TI_ONSCREEN 3		/* implemented */
#define TI_MORSE 4			/* implemented */
#define TI_CURSIVE 5		/* implemented */
#define TI_WHEELBOARD 6		/* implemented */
#define TI_FBK 7			/* implemented */
#define TI_DIALTYPE 8		/* implemented */
#define TI_KEYPAD 9			/* implemented */
#define TI_THUMBSCRIPT 10	/* implemented */
#define TI_FBKEYPAD 11		/* implemented */
#define TI_DASHER 12		/* WILL NOT BE IMPLEMENTED AT THIS TIME */
#define TI_SPEECH 13		/* WILL NOT BE IMPLEMENTED AT ALL, PROBABLY */

#define TEXT_MAX_LENGTH 4096 /* no longer used, kept for compatibility */
#define TEXT_BUFFER_INC 1024 /* how much bigger does the buffer grow when it is full */

/* calls to retrieve globals */
GR_WINDOW_ID text_get_wid(void);
GR_WINDOW_ID text_get_bufwid(void);
GR_TIMER_ID text_get_timer(void);
GR_GC_ID text_get_gc(void);
int text_get_timer_created(void);
int text_get_width(void);
int text_get_height(void);
char * text_get_buffer(void);
void text_set_buffer(char * s);
void text_clear_buffer(void);
void text_free_buffer(void);
int text_get_length(void);
int text_get_iposition(void);
void text_set_length(int i);
void text_set_iposition(int i);
void text_reset_timer(int t);
int text_get_input_method(void);
int text_get_ipod_generation(void);
int text_get_numeric_mode(void);

int text_optimum_height(void);
int text_optimum_width(void);

/* calls to create a new text window */
void new_text_box_np(void);
void new_text_box_nppw(void);
void new_text_box(int x, int y, int w, int h, char * defaulttext, void (*callback)(void));
void new_text_box_password(int x, int y, int w, int h, char * defaulttext, void (*callback)(void));
void new_text_box_numeric(int x, int y, int w, int h, char * defaulttext, void (*callback)(void));
void new_text_box_x(int x, int y, int w, int h, char * defaulttext, void (*callback)(void),
	void (*tdraw)(void), void (*tout)(int), int (*thandle)(GR_EVENT *), void (*texit)(void) );

/* functions for controlling text window */
void text_init(void);
void text_output_byte(char ch);
void text_remove_byte(int p);
void text_output_char(int ch);
void text_output_char_continue(int ch);
void text_exit(void);
void text_exit_continue(void);

/* event handling in the text window */
void text_draw(void);
void text_draw_continue(void);
void text_draw_password(void);
void text_draw_status(GR_GC_ID gc);
int text_handle_event(GR_EVENT *event);
int text_handle_event_continue(GR_EVENT *event);

void text_draw_message(char * s);

void text_rehook(void (*callback)(void), void (*tdraw)(void), void (*tout)(int),
	int (*thandle)(GR_EVENT *), void (*texit)(void));


#endif
