/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
 * Scroll Pad concept by veteran, implemented by jonrelay
 * Dial Type concept by Peter Burk, implemented by jonrelay
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
#include "../textinput.h"

const char text_charlist[] = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ=+?!@#$%^&*()[]{}_-<>~`\"\'1234567890\\/|:;.,M";
const char text_charlist_n[] = ".0123456789-E+-*/^M";
static int text_charlist_pos = 0;
static int text_charlist_cursormode = 0;

void text_dial_draw_status(GR_GC_ID gc, int x, int y)
{
	char s[2];
	s[0] = (text_get_numeric_mode() != 0)?text_charlist_n[text_charlist_pos]:text_charlist[text_charlist_pos];
	s[1] = 0;
	GrText(text_get_bufwid(), gc, x, y, s, -1, GR_TFASCII);
}

void text_dial_draw_status_full(GR_GC_ID gc)
{
	int sc, n, i, j;
	int m = (text_get_numeric_mode() != 0)?19:96;
	char s[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
	n = (text_get_width()-4) / 10;
	sc = text_charlist_pos - n/2;
	if (sc < 0) { sc = 0; }
	if (sc > (m-n) ) { sc = (m-n); }
	for (i = sc, j = 0; ((i<m) && (j<n)); i++, j++)
	{
		s[0] = (text_get_numeric_mode() != 0)?text_charlist_n[i]:text_charlist[i];
		s[1] = 0;
		GrText(text_get_bufwid(), gc, j*10+2, text_get_height()-4, s, -1, GR_TFASCII);
		if (i == text_charlist_pos) {
			GrRect(text_get_bufwid(), gc, j*10+1, text_get_height()-15, 10, 14);
		}
	}
}

char text_dial_get_char(void)
{
	return (text_get_numeric_mode() != 0)?text_charlist_n[text_charlist_pos]:text_charlist[text_charlist_pos];
}

void text_dial_next_char(void)
{
	text_charlist_pos++;
	if (text_charlist_pos >= ((text_get_numeric_mode() != 0)?19:96)) { text_charlist_pos = 0; }
}

void text_dial_prev_char(void)
{
	text_charlist_pos--;
	if (text_charlist_pos < 0) { text_charlist_pos = ((text_get_numeric_mode() != 0)?18:95); }
}

void text_dial_reset(void)
{
	text_charlist_pos = 0;
	text_charlist_cursormode = 0;
}

void text_dial_event(char ch)
{
	switch (ch) {
	case 'l':
		if (text_charlist_cursormode) {
			text_output_char(28);
		} else {
			text_dial_prev_char();
			text_draw();
		}
		break;
	case 'r':
		if (text_charlist_cursormode) {
			text_output_char(29);
		} else {
			text_dial_next_char();
			text_draw();
		}
		break;
	case '\r':
	case '\n':
		if (text_charlist_cursormode) {
			text_charlist_cursormode = 0;
			text_draw();
		} else if (text_charlist_pos == ((text_get_numeric_mode() != 0)?18:95)) {
			text_charlist_cursormode = 1;
			text_draw();
		} else {
			text_output_char(text_dial_get_char());
			if (text_get_input_method() == TI_DIALTYPE) { text_dial_reset(); text_draw(); }
		}
		break;
	}
}
