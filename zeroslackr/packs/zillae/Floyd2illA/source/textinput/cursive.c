/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
 * Cursive concept by Peter Burk, implemented by jonrelay
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

const char * text_cursive_records[] = {
	"l", "-", "_",
	"ll", "l", "L",
	"lll", "c", "C",
	"llll", "o", "O",
	"lllll", "0", ")",
	"llllo", "q", "Q",
	"llllr", "a", "A",
	"lllo", "g", "G",
	"llloo", "\\", "|",
	"lllr", "u", "U",
	"lllro", "b", "B",
	"llo", "6", "^",
	"lloo", "(", "[",
	"llooo", "[", "{",
	"llor", "4", "$",
	"llorr", "9", "(",
	"lo", "h", "H",
	"lol", "w", "W",
	"lolo", "=", "+",
	"lorr", "5", "%",
	"lor", "s", "S",
	"loro", "8", "*",
	"lro", "k", "K",
	"lrrr", "n", "N",
	"o", "x", "X",
	"ol", "v", "V",
	"oll", "\x1C", "\x1C",
	"olll", "e", "E",
	"oo", ".", ">",
	"ooo", ":", "\"",
	"ooor", ";", "\'",
	"oor", ",", "<",
	"or", "y", "Y",
	"orr", "\x1D", "\x1D",
	"orrr", "\x0A", "\x0A",
	"r", "1", "!",
	"ro", "t", "T",
	"rol", "z", "Z",
	"ror", "i", "I",
	"rorr", "3", "#",
	"rr", "r", "R",
	"rrl", "~", "`",
	"rro", "f", "F",
	"rrol", "2", "@",
	"rroo", ")", "]",
	"rrooo", "]", "}",
	"rror", "7", "&",
	"rrr", "j", "J",
	"rrrlo", "?", "/",
	"rrro", "p", "P",
	"rrroo", "/", "?",
	"rrrr", "d", "D",
	"rrrro", "m", "M"
};
const int text_cursive_record_count = 51;

static char text_cursive_buffer[6];
static int text_cursive_buffer_pos = 0;
static int text_cursive_shift = 0;
static int text_cursive_wheel_l_count = 0;
static int text_cursive_wheel_r_count = 0;
static int text_cursive_wheel_debounce = 1;

void text_cursive_reset(void)
{
	text_cursive_buffer[0] = 0;
	text_cursive_buffer_pos = 0;
	text_cursive_shift = 0;
	text_cursive_wheel_l_count = 0;
	text_cursive_wheel_r_count = 0;
	text_cursive_wheel_debounce = ipod_get_setting(WHEEL_DEBOUNCE);
}

char text_cursive_get_char(void)
{
	int i;
	for (i=0; i<text_cursive_record_count; i++) {
		if (strcmp(text_cursive_buffer, text_cursive_records[i*3]) == 0) {
			if (text_cursive_shift != 0) {
				return text_cursive_records[i*3+2][0];
			} else {
				return text_cursive_records[i*3+1][0];
			}
		}
	}
	return 0;
}

void text_cursive_print_status(GR_GC_ID gc)
{
	char s[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
	GrText(text_get_bufwid(), gc, 20, text_get_height()-4, text_cursive_buffer, -1, GR_TFASCII);
	s[0] = text_cursive_get_char();
	s[1] = 0;
	if (s[0]>32) {
		GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, s, -1, GR_TFASCII);
	} else {
		switch(s[0]) {
		case 0:
			break;
		case 32:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "space", -1, GR_TFASCII);
			break;
		case 28:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "left", -1, GR_TFASCII);
			break;
		case 29:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "right", -1, GR_TFASCII);
			break;
		case 8:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "bksp", -1, GR_TFASCII);
			break;
		case 127:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "del", -1, GR_TFASCII);
			break;
		case 10:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "nwln", -1, GR_TFASCII);
			break;
		default:
			GrText(text_get_bufwid(), gc, text_get_width()-30, text_get_height()-4, "??", -1, GR_TFASCII);
			break;
		}
	}
	if (text_cursive_shift != 0) {
		GrText(text_get_bufwid(), gc, text_get_width()/2, text_get_height()-4, "2nd", -1, GR_TFASCII);
	}
}

void text_cursive_accept_keystroke(char c)
{
	int db;
	db = text_cursive_wheel_debounce;
	switch (c) {
	case 'l':
		text_cursive_wheel_l_count++;
		text_cursive_wheel_r_count = 0;
		if (((text_cursive_wheel_l_count*db) >= 24) && (text_cursive_buffer_pos < 5)) {
			text_cursive_buffer[text_cursive_buffer_pos] = 'l';
			text_cursive_buffer_pos++;
			text_cursive_buffer[text_cursive_buffer_pos] = 0;
			text_cursive_wheel_l_count = 0;
		}
		break;
	case 'r':
		text_cursive_wheel_l_count = 0;
		text_cursive_wheel_r_count++;
		if (((text_cursive_wheel_r_count*db) >= 24) && (text_cursive_buffer_pos < 5)) {
			text_cursive_buffer[text_cursive_buffer_pos] = 'r';
			text_cursive_buffer_pos++;
			text_cursive_buffer[text_cursive_buffer_pos] = 0;
			text_cursive_wheel_r_count = 0;
		}
		break;
	case '\r':
	case '\n':
		text_cursive_wheel_l_count = 0;
		text_cursive_wheel_r_count = 0;
		if (text_cursive_buffer_pos < 5) {
			text_cursive_buffer[text_cursive_buffer_pos] = 'o';
			text_cursive_buffer_pos++;
			text_cursive_buffer[text_cursive_buffer_pos] = 0;
		}
		break;
	case 'd':
		if (text_cursive_shift != 0) {
			text_cursive_shift = 0;
		} else {
			text_cursive_shift = 1;
		}
		break;
	case 'm':
		text_exit();
		break;
	case 'w':
		text_output_char(8);
		break;
	case 'f':
		text_output_char(32);
		break;
	}
}

void text_cursive_push(void)
{
	char c;
	c = text_cursive_get_char();
	if (c != 0) { text_output_char(c); }
	text_cursive_buffer[0] = 0;
	text_cursive_buffer_pos = 0;
	text_cursive_wheel_l_count = 0;
	text_cursive_wheel_r_count = 0;
}


