/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
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
#include "../pz.h"
#include "../ipod.h"
#include "../textinput.h"
#include "../clickwheel.h"

const char * text_keypad_buttons[] = {
	" \x08.,10!?\x0A:;\"\'\xA1\xBF", "abcABC2@/\\_|\xA6", "defDEF3#\xBA\xAA\xB9\xB2\xB3",
	"ghiGHI4$\xA2\xA3\xA4\xA5", "jklJKL5%\xBC\xBD\xBE", "mnoMNO6^~`\xB4\xA8\xB8",
	"pqrsPQRS7&\xA7\xB6\xB0\xA9\xAE\xB5", "tuvTUV8*+-=<>", "wxyzWXYZ9()[]{}"
};
const char * text_keypad_buttons_short[] = { "10.,", "2abc", "3def", "4ghi", "5jkl", "6mno", "7pqrs", "8tuv", "9wxyz" };
const char * text_keypad_buttons_n[] = { "10.-\x08""E+-*/^", "2", "3", "4", "5", "6", "7", "8", "9" };
const char * text_keypad_buttons_short_n[] = { "10.-", "2", "3", "4", "5", "6", "7", "8", "9" };
static int text_keypad_row = 0;
static int text_keypad_curr_btn = 0;
static int text_keypad_curr_char = 0;
// KERIPO MOD
//static int text_keypad_ptext = 0; /* predictive text mode */
//static GR_TIMER_ID text_keypad_ptext_tid; /* predictive text on/off timer */

/* ptext.c */
// KERIPO MOD
/*
extern int text_ptext_inited(void);
extern int text_ptext_init(void);
extern void text_ptext_free(void);
extern int text_ptext_predict(char * buf, int pos, int method);
*/

/* NON-IMPLEMENTATION-SPECIFIC KEYPAD FUNCTIONS */

void text_keypad_print_status(GR_GC_ID gc, char * s, int x, int y)
{
	GrText(text_get_bufwid(), gc, x, y, s, -1, GR_TFASCII);
}

void text_keypad_print_mode_status(GR_GC_ID gc)
{
	int i;
	char s[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
	if (text_get_numeric_mode() != 0) {
		for (i=0; i<3; i++) {
			text_keypad_print_status(gc, (char *)text_keypad_buttons_short_n[text_keypad_row*3 + i], (text_get_width()/4)*i+1, text_get_height()-4);
		}
	} else {
		for (i=0; i<3; i++) {
			text_keypad_print_status(gc, (char *)text_keypad_buttons_short[text_keypad_row*3 + i], (text_get_width()/4)*i+1, text_get_height()-4);
		}
// KERIPO MOD
/*
		if (text_keypad_ptext) {
			text_keypad_print_status(gc, "P", text_get_width()-10, text_get_height()-4);
		}
*/
	}
	if (text_keypad_curr_char != 0) {
		if (text_get_numeric_mode() != 0) {
			s[0] = text_keypad_buttons_n[text_keypad_curr_btn][text_keypad_curr_char - 1];
		} else {
			s[0] = text_keypad_buttons[text_keypad_curr_btn][text_keypad_curr_char - 1];
		}
		s[1]=0;
		switch (s[0]) {
		case  10: text_keypad_print_status(gc, "nwln",  (text_get_width()/4)*3+1, text_get_height()-4); break;
		case  32: text_keypad_print_status(gc, "sp",    (text_get_width()/4)*3+1, text_get_height()-4); break;
		case   8: text_keypad_print_status(gc, "bs",    (text_get_width()/4)*3+1, text_get_height()-4); break;
		case 127: text_keypad_print_status(gc, "del",   (text_get_width()/4)*3+1, text_get_height()-4); break;
		case   9: text_keypad_print_status(gc, "tab",   (text_get_width()/4)*3+1, text_get_height()-4); break;
		case  28: text_keypad_print_status(gc, "left",  (text_get_width()/4)*3+1, text_get_height()-4); break;
		case  29: text_keypad_print_status(gc, "right", (text_get_width()/4)*3+1, text_get_height()-4); break;
		default:
			text_keypad_print_status(gc, s, (text_get_width()/4)*3+1, text_get_height()-4);
		}
	}
}

void text_keypad_print_mode_status_4g(GR_GC_ID gc)
{
	char s[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
// KERIPO MOD
/*
	if (text_keypad_ptext) {
		text_keypad_print_status(gc, "P", text_get_width()-10, text_get_height()-4);
	}
*/
	if (text_keypad_curr_char != 0) {
		if (text_get_numeric_mode() != 0) {
			s[0] = text_keypad_buttons_n[text_keypad_curr_btn][text_keypad_curr_char - 1];
		} else {
			s[0] = text_keypad_buttons[text_keypad_curr_btn][text_keypad_curr_char - 1];
		}
		s[1]=0;
		switch (s[0]) {
		case  10: text_keypad_print_status(gc, "nwln",  text_get_width()/2, text_get_height()-4); break;
		case  32: text_keypad_print_status(gc, "sp",    text_get_width()/2, text_get_height()-4); break;
		case   8: text_keypad_print_status(gc, "bs",    text_get_width()/2, text_get_height()-4); break;
		case 127: text_keypad_print_status(gc, "del",   text_get_width()/2, text_get_height()-4); break;
		case   9: text_keypad_print_status(gc, "tab",   text_get_width()/2, text_get_height()-4); break;
		case  28: text_keypad_print_status(gc, "left",  text_get_width()/2, text_get_height()-4); break;
		case  29: text_keypad_print_status(gc, "right", text_get_width()/2, text_get_height()-4); break;
		default:
			text_keypad_print_status(gc, s, text_get_width()/2, text_get_height()-4);
		}
	}
}

void text_keypad_output(void)
{
	//int l;
	if (text_keypad_curr_char != 0)
	{
		if (text_get_numeric_mode() != 0) {
			text_output_char(text_keypad_buttons_n[text_keypad_curr_btn][text_keypad_curr_char - 1]);
		// KERIPO MOD
		/*
		} else if (text_keypad_ptext) {
			if (text_keypad_curr_btn == 0) {
				text_output_char(text_keypad_buttons[text_keypad_curr_btn][text_keypad_curr_char - 1]);
			} else {
				text_output_char(text_keypad_buttons_n[text_keypad_curr_btn][text_keypad_curr_char - 1]);
			}
			if ((  l = text_ptext_predict(text_get_buffer(), text_get_iposition(), 1)  )) {
				text_set_iposition(text_get_iposition() - l);
				text_set_length(text_get_length() - l);
			}
		*/
		} else {
			text_output_char(text_keypad_buttons[text_keypad_curr_btn][text_keypad_curr_char - 1]);
		}
	}
	text_keypad_curr_char = 0;
}

void text_keypad_switch_row(void)
{
	text_keypad_output();
	text_keypad_row++;
	if (text_keypad_row >= 3) { text_keypad_row = 0; }
}

void text_keypad_reset(void)
{
	text_keypad_row = 0;
	text_keypad_curr_btn = 0;
	text_keypad_curr_char = 0;
}

void text_keypad_push(int n)
{
	/* this takes 0-8, NOT 1-9 */
	// KERIPO MOD
	//if ((n != 0) && (text_keypad_ptext || (text_get_numeric_mode() != 0))) {
	if ((n != 0) && ((text_get_numeric_mode() != 0))) {
		text_keypad_output();
		text_keypad_curr_btn = n;
		text_keypad_curr_char = 1;
		text_keypad_output();
	} else if (n == text_keypad_curr_btn) {
		text_keypad_curr_char++;
		if (text_keypad_curr_char > strlen((text_get_numeric_mode() != 0)?text_keypad_buttons_n[text_keypad_curr_btn]:text_keypad_buttons[text_keypad_curr_btn])) { text_keypad_curr_char = 1; }
	} else {
		text_keypad_output();
		text_keypad_curr_btn = n;
		text_keypad_curr_char = 1;
	}
}

/*
void text_keypad_ptext_toggle(void)
{
	text_keypad_ptext = (!text_keypad_ptext);
	if (text_keypad_ptext) {
		if (!text_ptext_inited()) {
			text_draw_message(_("Loading ptext..."));
			text_ptext_init();
			text_draw();
		}
	} else {
		text_ptext_free();
	}
}
*/


/* IMPLEMENTATION-SPECIFIC */

int text_keypad_button_map(char c)
{
	/*
		0 - switch rows
		1-3 - keypad buttons
		4 - move left (was backspace)
		5 - move right (was space)
		6 - exit (was newline)
	*/
	if (text_get_ipod_generation()==3) {
		switch (c) {
		case 'w':
			return 1;
			break;
		case 'm':
			return 2;
			break;
		case 'd':
			return 3;
			break;
		case 'f':
			return 0;
			break;
		case 'l':
			return 4;
			break;
		case 'r':
			return 5;
			break;
		case '\r':
		case '\n':
			return 6;
			break;
		}
	} else {
		switch (c) {
		case 'w':
			return 1;
			break;
		case '\r':
		case '\n':
			return 2;
			break;
		case 'f':
			return 3;
			break;
		case 'd':
			return 0;
			break;
		case 'l':
			return 4;
			break;
		case 'r':
			return 5;
			break;
		case 'm':
			return 6;
			break;
		}
	}
	return 7;
}

void text_keypad_fbpush(int n)
{
	if (n == 0) {
		text_keypad_switch_row();
		text_keypad_curr_char = 0;
	} else if (n == 4) {
		/* text_output_char(8); */
		text_output_char(28);
	} else if (n == 5) {
		/* text_output_char(32); */
		text_output_char(29);
	} else if (n == 6) {
		text_exit();
	} else if (n != 7) {
		text_keypad_push(text_keypad_row*3 + n - 1);
	}
}

void text_keypad_4gpush(int n)
{
	/* n is our position on the click wheel as given by clickwheel_octrant */
	switch (n) {
	case 0: text_keypad_push(1); break;
	case 1: text_keypad_push(2); break;
	case 2: text_keypad_push(5); break;
	case 3: text_keypad_push(8); break;
	case 4: text_keypad_push(7); break;
	case 5: text_keypad_push(6); break;
	case 6: text_keypad_push(3); break;
	case 7: text_keypad_push(0); break;
	}
	/* the center button would do text_keypad_push(4); */
}

int text_keypad_fbevent(GR_EVENT *event)
{
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
		// KERIPO MOD
		/*
		if (((GR_EVENT_TIMER *)event)->tid == text_keypad_ptext_tid) {
			GrDestroyTimer(text_keypad_ptext_tid);
			text_keypad_ptext_tid = 0;
			text_keypad_ptext_toggle();
			text_draw();
		} else {
		*/
			text_keypad_output();
			text_draw();
		//}
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		// KERIPO MOD
		/*
		if ( ((event->keystroke.ch) == '\n') || ((event->keystroke.ch) == '\r') ) {
			text_keypad_ptext_tid = GrCreateTimer(text_get_wid(), 500);
		} else {
		*/
			text_keypad_fbpush(text_keypad_button_map(event->keystroke.ch));
			text_reset_timer(1000);
			text_draw();
		//}
		break;
	case GR_EVENT_TYPE_KEY_UP:
		// KERIPO MOD
		/*
		if ( ((event->keystroke.ch) == '\n') || ((event->keystroke.ch) == '\r') ) {
			if (text_keypad_ptext_tid) {
				GrDestroyTimer(text_keypad_ptext_tid);
				text_keypad_ptext_tid = 0;
				
				text_keypad_fbpush(text_keypad_button_map(event->keystroke.ch));
				text_reset_timer(1000);
				text_draw();
			}
		}
		*/
		break;
	}
	return 1;
}

int text_keypad_4gevent(GR_EVENT *event)
{
	int i;
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
		if (is_clickwheel_timer(event)) {
			i = clickwheel_octrant();
			if (clickwheel_change(i) && (i>=0)) {
				text_keypad_4gpush(i);
				text_reset_timer(1000);
				text_draw();
			}
		// KERIPO MOD
		/*
		} else if (((GR_EVENT_TIMER *)event)->tid == text_keypad_ptext_tid) {
			GrDestroyTimer(text_keypad_ptext_tid);
			text_keypad_ptext_tid = 0;
			text_keypad_ptext_toggle();
			text_draw();
		*/
		} else {
			text_keypad_output();
			text_draw();
		}
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		case '\n':
		case '\r':
			// KERIPO MOD
			//text_keypad_ptext_tid = GrCreateTimer(text_get_wid(), 500);
			break;
		case 'l':
		case 'r':
			break;
		case 'w':
			text_output_char(28);
			break;
		case 'f':
			text_output_char(29);
			break;
		case 'd':
			text_output_char(10);
			break;
		case 'm':
			text_exit();
			break;
		}
		break;
	case GR_EVENT_TYPE_KEY_UP:
		// KERIPO MOD
		/*
		if ( ((event->keystroke.ch) == '\n') || ((event->keystroke.ch) == '\r') ) {
			if (text_keypad_ptext_tid) {
				GrDestroyTimer(text_keypad_ptext_tid);
				text_keypad_ptext_tid = 0;
				
				text_keypad_push(4);
				text_reset_timer(1000);
				text_draw();
			}
		}
		*/
		break;
	}
	return 1;
}
