/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 * Four-Button Keyboard concept by Jonathan Bettencourt (jonrelay)
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
#include <unistd.h>
#include "../pz.h"
#include "../appearance.h"
#include "../textinput.h"

const int text_osk_key_x[] = {
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, /* number row */
	0, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, /* qwerty row */
	0, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, /* home row */
	0, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, /* zxcv row */
	0, 4, 22, 26, 28 /* space bar row */
};
const int text_osk_key_y[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* number row */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* qwerty row */
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* home row */
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, /* zxcv row */
	8, 8, 8, 8, 8 /* space bar row */
};
const int text_osk_key_width[] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* number row */
	3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, /* qwerty row */
	4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, /* home row */
	5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, /* zxcv row */
	4, 18, 4, 2, 2 /* space bar row */
};
const int text_osk_key_normal[] = {
	'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8, 127,
	9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
	2, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 10,
	1, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 1,
	3, ' ', 3, 28, 29
};
const int text_osk_key_shift[] = {
	'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8, 127,
	9, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
	1, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', 10,
	0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	4, ' ', 4, 28, 29
};
const int text_osk_key_caps[] = {
	'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8, 127,
	9, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 10,
	1, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 1,
	4, ' ', 4, 28, 29
};
const int text_osk_key_option[] = {
	7, '¡', 8482, '£', '¢', '¦', '§', '¶', 8226, 'ª', 'º', 8211, 8800, 8, 127,
	9, 339, 8721, 7, '®', 8224, '¥', 7, 7, 'ø', 'þ', 8220, 8216, '«',
	4, 'å', 'ß', 'ð', 402, '©', 295, 8710, 8734, '¬', 8230, 'æ', 10,
	4, 937, 8776, 'ç', 8730, 8747, 7, 'µ', 8804, 8805, '÷', 4,
	0, ' ', 0, 28, 29
};
const int text_osk_key_opt_shift[] = {
	'`', '¹', '²', '³', '¼', '½', '¾', 8225, '°', '·', 'º', 8212, '±', 8, 127,
	9, 338, 8719, '´', 8240, 8225, 8364, '¨', 710, 'Ø', 'Þ', 8221, 8217, '»',
	3, 'Å', 658, 'Ð', 64257, 8480, 294, 8471, 63743, 64258, 8253, 'Æ', 10,
	3, '¸', '×', 'Ç', 9674, 305, 732, 331, '¯', 728, '¿', 3,
	1, ' ', 1, 28, 29
};
const int text_osk_dead_keys[] = {
	'`', 'à', 'b', 'c', 'd', 'è', 'f', 'g', 'h', 'ì', 'j', 'k', 'l', 'm', 505, 'ò', 'p', 'q', 'r', 's', 't', 'ù', 'v', 'w', 'x', 'y', 'z',
	     'À', 'B', 'C', 'D', 'È', 'F', 'G', 'H', 'Ì', 'J', 'K', 'L', 'M', 504, 'Ò', 'P', 'Q', 'R', 'S', 'T', 'Ù', 'V', 'W', 'X', 'Y', 'Z',
	'´', 'á', 'b', 263, 'ð', 'é', 'f', 501, 'h', 'í', 'j', 'k', 322, 'm', 324, 'ó', 'p', 'q', 341, 347, 'þ', 'ú', 'v', 'w', 'x', 'ý', 378,
	     'Á', 'B', 262, 'Ð', 'É', 'F', 500, 'H', 'Í', 'J', 'K', 321, 'M', 323, 'Ó', 'P', 'Q', 340, 346, 'Þ', 'Ú', 'V', 'W', 'X', 'Ý', 377,
	710, 'â', 'b', 265, 'd', 'ê', 'f', 285, 293, 'î', 309, 'k', 'l', 'm', 'n', 'ô', 'p', 'q', 'r', 353, 't', 'û', 'v', 373, 'x', 375, 382,
	     'Â', 'B', 264, 'D', 'Ê', 'F', 284, 292, 'Î', 308, 'K', 'L', 'M', 'N', 'Ô', 'P', 'Q', 'R', 352, 'T', 'Û', 'V', 372, 'X', 374, 381,
	732, 'ã', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 297, 'j', 'k', 'l', 'm', 'ñ', 'õ', 'p', 'q', 'r', 's', 't', 361, 'v', 'w', 'x', 'y', 'z',
	     'Ã', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 296, 'J', 'K', 'L', 'M', 'Ñ', 'Õ', 'P', 'Q', 'R', 'S', 'T', 360, 'V', 'W', 'X', 'Y', 'Z',
	'¨', 'ä', 'b', 'c', 'd', 'ë', 'f', 'g', 'h', 'ï', 'j', 'k', 'l', 'm', 'n', 'ö', 'p', 'q', 'r', 's', 't', 'ü', 'v', 'w', 'x', 'ÿ', 'z',
	     'Ä', 'B', 'C', 'D', 'Ë', 'F', 'G', 'H', 'Ï', 'J', 'K', 'L', 'M', 'N', 'Ö', 'P', 'Q', 'R', 'S', 'T', 'Ü', 'V', 'W', 'X', 376, 'Z',
};
const char * text_osk_special_key_names[] = {
	"norm", "shft", "clk", "opt", "opsh", "5", "6", "dead", "bs", "tab", "nl", "vtab", "feed", "rtn", "14", "15",
	"16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "esc", "<-", "->", "/\\", "\\/", "space"
};
const int text_osk_cell_count = 59;
const int text_osk_key_x_n[] = {
	0, 2, 4, 6,  0, 2, 4, 6,  0, 2, 4, 6,  0, 2, 4, 6,  0, 4, 6
};
const int text_osk_key_y_n[] = {
	0, 0, 0, 0,  2, 2, 2, 2,  4, 4, 4, 4,  6, 6, 6, 6,  8, 8, 8
};
const int text_osk_key_width_n[] = {
	2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  4, 2, 2
};
const int text_osk_key_numeric[] = {
	8, 'E', '^', '/',  '7', '8', '9', '*',  '4', '5', '6', '-',  '1', '2', '3', '+',  '0', '.', 10
};
const int text_osk_cell_count_n = 19;

static int text_osk_modifier_state = 0;
static int text_osk_dead_key_state = 0;
static int text_osk_current_key = 0;

static GR_GC_ID text_osk_gc;

/* fonts.c */
extern void set_font(char *file, int size, GR_GC_ID gc, int local);
extern void destroy_font(int local);


char * text_osk_utf8encode(int c)
{
	char ch[4];
	ch[0]=0;
	ch[1]=0;
	ch[2]=0;
	ch[3]=0;
	if (c < 0) {
		ch[0]='?';
	} else if (c < 0x80) {
		ch[0]=c;
	} else if (c < 0x0800) {
		ch[0]=((c >> 6        ) | 0xC0);
		ch[1]=((c       & 0x3F) | 0x80);
	} else if (c < 0x10000) {
		ch[0]=((c >> 12       ) | 0xE0);
		ch[1]=((c >> 6  & 0x3F) | 0x80);
		ch[2]=((c       & 0x3F) | 0x80);
	} else if (c < 0x110000) {
		ch[0]=((c >> 18       ) | 0xF0);
		ch[1]=((c >> 12 & 0x3F) | 0x80);
		ch[2]=((c >> 6  & 0x3F) | 0x80);
		ch[3]=((c       & 0x3F) | 0x80);
	} else {
		ch[0]='?';
	}
	return strdup(ch);
}

void text_osk_init(void)
{
	text_osk_modifier_state = 0;
	text_osk_dead_key_state = 0;
	text_osk_current_key = (text_get_numeric_mode() != 0)?0:55;
	text_osk_gc = pz_get_gc(1);
// KERIPO MOD
	//if (access("/usr/share/fonts/SeaChelUnicode.fnt", R_OK) == 0) {
	if (access("/usr/share/fonts/Unicode.fnt", R_OK) == 0) {
		text_draw_message(_("Loading keyboard font..."));
		//set_font("/usr/share/fonts/SeaChelUnicode.fnt", 9, text_osk_gc, 1);
		set_font("/usr/share/fonts/Unicode.fnt", 9, text_osk_gc, 1);
	//} else if (access("/usr/share/fonts/SeaChel.fnt", R_OK) == 0) {
	} else if (access("/usr/share/fonts/EspySans-13.fnt", R_OK) == 0) {
		text_draw_message(_("Loading keyboard font..."));
		//set_font("/usr/share/fonts/SeaChel.fnt", 9, text_osk_gc, 1);
		set_font("/usr/share/fonts/EspySans-13.fnt", 9, text_osk_gc, 1);
	}
}

void text_osk_free(void)
{
// KERIPO MOD
	//if ( (access("/usr/share/fonts/SeaChel.fnt", R_OK) == 0) || (access("/usr/share/fonts/SeaChelUnicode.fnt", R_OK) == 0) ) {
	if ( (access("/usr/share/fonts/Unicode.fnt", R_OK) == 0) || (access("/usr/share/fonts/EspySans-13.fnt", R_OK) == 0) ) {
		destroy_font(1);
	}
	GrDestroyGC(text_osk_gc);
}

int text_osk_get_key(int k)
{
	if (text_get_numeric_mode() != 0) {
		return text_osk_key_numeric[k];
	} else {
		switch (text_osk_modifier_state) {
		case 0:
			return text_osk_key_normal[k];
		case 1:
			return text_osk_key_shift[k];
		case 2:
			return text_osk_key_caps[k];
		case 3:
			return text_osk_key_option[k];
		case 4:
			return text_osk_key_opt_shift[k];
		}
	}
	return 0;
}

void text_osk_draw_keyboard(GR_WINDOW_ID wid, GR_GC_ID gc, int x, int y, int gridsize, int sel)
{
	int i;
	int c;
	int kx, ky;
	char * keylabel;
	GR_SIZE width, height, base;
	GrSetGCBackground(gc, appearance_get_color(CS_BG));
	if (text_get_numeric_mode() != 0) {
	
	for (i=0; i<text_osk_cell_count_n; i++) {
		GrSetGCForeground(gc, appearance_get_color(CS_FG));
		if (i == sel) {
			GrSetGCForeground(gc, appearance_get_color(CS_SELBG));
			GrFillRect(wid, gc, x+gridsize*text_osk_key_x_n[i], y+gridsize*text_osk_key_y_n[i], gridsize*text_osk_key_width_n[i]+1, gridsize*2+1);
			GrSetGCForeground(gc, appearance_get_color(CS_SELFG));
		} else {
			GrRect(wid, gc, x+gridsize*text_osk_key_x_n[i], y+gridsize*text_osk_key_y_n[i], gridsize*text_osk_key_width_n[i]+1, gridsize*2+1);
			GrSetGCForeground(gc, appearance_get_color(CS_FG));
		}
		kx = x+gridsize*text_osk_key_x_n[i]+gridsize*text_osk_key_width_n[i]/2;
		ky = y+gridsize*text_osk_key_y_n[i]+gridsize;
		c = text_osk_get_key(i);
		if (c == 127) {
			keylabel = "dl";
		} else if (c < 34) {
			keylabel = (char *)text_osk_special_key_names[c];
		} else {
			keylabel = text_osk_utf8encode(c);
		}
		GrGetGCTextSize(gc, keylabel, strlen(keylabel), GR_TFUTF8, &width, &height, &base);
		GrText(wid, gc, kx-width/2, ky-height/2+base, keylabel, strlen(keylabel), GR_TFUTF8);
	}
	
	} else {
	
	for (i=0; i<text_osk_cell_count; i++) {
		GrSetGCForeground(gc, appearance_get_color(CS_FG));
		if (i == sel) {
			GrSetGCForeground(gc, appearance_get_color(CS_SELBG));
			GrFillRect(wid, gc, x+gridsize*text_osk_key_x[i], y+gridsize*text_osk_key_y[i], gridsize*text_osk_key_width[i]+1, gridsize*2+1);
			GrSetGCForeground(gc, appearance_get_color(CS_SELFG));
		} else {
			GrRect(wid, gc, x+gridsize*text_osk_key_x[i], y+gridsize*text_osk_key_y[i], gridsize*text_osk_key_width[i]+1, gridsize*2+1);
			GrSetGCForeground(gc, appearance_get_color(CS_FG));
		}
		kx = x+gridsize*text_osk_key_x[i]+gridsize*text_osk_key_width[i]/2;
		ky = y+gridsize*text_osk_key_y[i]+gridsize;
		c = text_osk_get_key(i);
		if (c == 127) {
			keylabel = "dl";
		} else if (c == 7) {
			keylabel = text_osk_utf8encode(text_osk_key_opt_shift[i]);
		} else if (c < 34) {
			keylabel = (char *)text_osk_special_key_names[c];
		} else {
			keylabel = text_osk_utf8encode(c);
		}
		GrGetGCTextSize(gc, keylabel, strlen(keylabel), GR_TFUTF8, &width, &height, &base);
		GrText(wid, gc, kx-width/2, ky-height/2+base, keylabel, strlen(keylabel), GR_TFUTF8);
	}
	
	}
}

void text_osk_push_key(int k)
{
	int c;
	int i;
	c = text_osk_get_key(k);
	if (c < 7) {
		text_osk_modifier_state = c;
		text_draw();
	} else if (c == 7) {
		text_osk_dead_key_state = text_osk_key_opt_shift[k];
	} else {
		if (text_osk_dead_key_state != 0) {
			for (i=0; i<5; i++) {
				if (text_osk_dead_keys[i*53] == text_osk_dead_key_state) {
					if (c == 32) {
						text_output_char(text_osk_dead_key_state);
					} else if ((c >= 'A') && (c <= 'Z')) {
						text_output_char(text_osk_dead_keys[i*53 + 27 + (c-'A')]);
					} else if ((c >= 'a') && (c <= 'z')) {
						text_output_char(text_osk_dead_keys[i*53 + 1 + (c-'a')]);
					} else {
						text_output_char(text_osk_dead_key_state);
						text_output_char(c);
					}
				}
			}
		} else {
			text_output_char(c);
		}
		text_osk_dead_key_state = 0;
	}
}

void text_osk_draw_state(GR_WINDOW_ID wid, GR_GC_ID gc)
{
	int x, y;
	if (text_get_numeric_mode() != 0) {
		x = text_get_width()/2 - 17;
		y = text_get_height() - 43;
	} else {
		x = text_get_width()/2 - 61;
		y = text_get_height() - 43;
	}
	GrSetGCForeground(gc, GRAY);
	GrLine(wid, gc, 0, y-2, text_get_width(), y-2);
	text_osk_draw_keyboard(wid, text_osk_gc, x, y, 4, text_osk_current_key);
	GrSetGCForeground(gc, BLACK);
}

void text_osk_handle_event(char ch)
{
	switch (ch) {
	case 'l':
		text_osk_current_key--;
		if (text_osk_current_key < 0) { text_osk_current_key = ((text_get_numeric_mode() != 0)?(text_osk_cell_count_n-1):(text_osk_cell_count-1)); }
		text_draw();
		break;
	case 'r':
		text_osk_current_key++;
		if (text_osk_current_key >= ((text_get_numeric_mode() != 0)?text_osk_cell_count_n:text_osk_cell_count)) { text_osk_current_key = 0; }
		text_draw();
		break;
	case '\n':
	case '\r':
		text_osk_push_key(text_osk_current_key);
		break;
	case 'w':
		text_output_char(8);
		break;
	case 'f':
		text_output_char(32);
		break;
	case 'm':
		text_exit();
		break;
	case 'd':
		text_output_char(10);
		break;
	}
}

