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

#include <string.h>
#include <unistd.h>
#include "../pz.h"
#include "../textinput.h"
#include "../clickwheel.h"

const int text_keypad_thumbscript_normal[] = {
	2,    'v',  'w',  'p',  '1',  'q',  'x',  'y',  'u',
	'`',  1,    'k',  'j',  '2',  'l',  'd',  'i',  'b',
	'\"', '\'', 8,    42,   '3',  'g',  'e',  'f',  'c',
	'>',  '=',  3,    '\t', '4',  'o',  'z',  'n',  'h',
	'!',  '@',  '#',  '$',  '5',  '^',  '&',  '*',  '%',
	27,   '+',  '<',  '0',  '6',  '\n', '\r', 't',  's',
	')',  '?',  '/',  ';',  '7',  5,    ',',  'm',  'a',
	'}',  '|',  '{',  ']',  '8',  '[',  '~',  ' ',  'r',
	'\\', 4,    '(',  6,    '9',  ':',  '_',  '-',  '.',
	0
};

const int text_keypad_thumbscript_shift[] = {
	2,    'V',  'W',  'P',  128,  'Q',  'X',  'Y',  'U',
	'`',  1,    'K',  'J',  129,  'L',  'D',  'I',  'B',
	'\"', '\'', 8,    42,   130,  'G',  'E',  'F',  'C',
	'>',  '=',  3,    9,    131,  'O',  'Z',  'N',  'H',
	'!',  '@',  '#',  '$',  132,  '^',  '&',  '*',  '%',
	27,   '+',  '<',  '0',  133,  10,   13,   'T',  'S',
	')',  '?',  '/',  ';',  134,  5,    ',',  'M',  'A',
	'}',  '|',  '{',  ']',  135,  '[',  '~',  ' ',  'R',
	'\\', 4,    '(',  6,    136,  ':',  '_',  '-',  '.',
	0
};

const int text_keypad_thumbscript_mod[] = {
	2,    'V',  'W',  182,  185,  186,  215,  165,  181,
	8216, 1,    'K',  'J',  178,  163,  208,  305,  223,
	180,  8217, 8,    42,   179,  'G',  8364, 402,  169,
	187,  247,  3,    9,    188,  176,  'Z',  'N',  'H',
	161,  '@',  164,  162,  189,  '^',  '&',  8226, 8240,
	27,   177,  171,  216,  190,  10,   13,   8482, 167,
	')',  191,  8221, ';',  '7',  5,    184,  'M',  170,
	'}',  166,  '{',  ']',  '8',  '[',  172,  160,  174,
	8220, 4,    '(',  6,    '9',  168,  175,  173,  183,
	0
};

const int text_keypad_thumbscript_accents[] = {
	/* ` grave */
	192,'B','C','D',200,'F','G','H',204,'J','K','L','M','N',210,'P','Q','R','S','T',217,'V','W','X','Y','Z',
	224,'b','c','d',232,'f','g','h',236,'j','k','l','m','n',242,'p','q','r','s','t',249,'v','w','x','y','z',
	/* ^ circumflex */
	194,'B',264,'D',202,'F',284,292,206,308,'K','L','M','N',212,'P','Q','R',348,'T',219,'V',372,'X',374,'Z',
	226,'b',265,'d',234,'f',285,293,238,309,'k','l','m','n',244,'p','q','r',349,'t',251,'v',373,'x',375,'z',
	/* « acute */
	193,'B',262,208,201,'F','G','H',205,'J','K',313,'M',323,211,'P','Q',340,346,222,218,'V','W','X',221,377,
	225,'b',263,240,233,'f','g','h',237,'j','k',314,'m',324,243,'p','q',341,347,254,250,'v','w','x',253,378,
	/* ~ tilde */
	195,'B','C','D','E','F','G','H',296,'J',0xF700,'L','M',209,213,'P','Q','R','S','T',360,'V','W','X','Y','Z',
	227,'b','c','d','e','f','g','h',297,'j',0xF701,'l','m',241,245,'p','q','r','s','t',361,'v','w','x','y','z',
	/* ¬ umlaut */
	196,'B','C','D',203,'F','G','H',207,'J','K','L','M','N',214,'P','Q','R','S','T',220,'V','W','X',376,'Z',
	228,'b','c','d',235,'f','g','h',239,'j','k','l','m','n',246,'p','q','r','s','t',252,'v','w','x',255,'z',
	/* ¡ ring */
	197,'B','C','D','E','F','G',294,'I','J','K',321,'M',330,216,'P','Q','R','S',358,366,'V','W','X','Y','Z',
	229,'b','c','d','e','f','g',295,'i','j','k',322,'m',331,248,'p','q','r','s',359,367,'v','w','x','y','z',
	/* & ligature */
	198,'B','C','D','E', 'F' ,'G','H',64259,'J','K',64260,'M','N',338,'P','Q','R','S','T','U','V','W','X','Y','Z',
	230,'b','c','d','e',64256,'g','h',64257,'j','k',64258,'m','n',339,'p','q','r','s','t','u','v','w','x','y','z',
	/* ü cedilla */
	'A','B',199,'D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R',350,354,'U','V','W','X','Y','Z',
	'a','b',231,'d','e','f','g','h','i','j','k','l','m','n','o','p','q','r',351,355,'u','v','w','x','y','z',
	/* ø macron */
	256,'B','C','D',274,'F','G','H',298,'J','K','L','M','N',332,'P','Q','R','S','T',362,'V','W','X','Y','Z',
	257,'b','c','d',275,'f','g','h',299,'j','k','l','m','n',333,'p','q','r','s','t',363,'v','w','x','y','z',
	/* end */
	0
};

static int text_keypad_thumbscript_mode = 0;
static int text_keypad_thumbscript_last = 0;

void text_keypad_thumbscript_reset(void)
{
	text_keypad_thumbscript_mode = 0;
	text_keypad_thumbscript_last = 0;
}

void text_keypad_thumbscript_draw(GR_GC_ID gc)
{
	char s[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
	switch (text_keypad_thumbscript_mode) {
	case 1:
		GrText(text_get_bufwid(), gc, text_get_width()/2, text_get_height()-4, "shift", -1, GR_TFASCII);
		break;
	case 3:
	case 2: case 4: case 5: case 6:
		GrText(text_get_bufwid(), gc, text_get_width()/2, text_get_height()-4, "mod", -1, GR_TFASCII);
		break;
	}
	if (text_keypad_thumbscript_last != 0) {
		s[0] = (text_keypad_thumbscript_last + '1');
		s[1] = 0;
		GrText(text_get_bufwid(), gc, (text_get_width()*3)/4, text_get_height()-4, s, -1, GR_TFASCII);
	}
}

void text_keypad_thumbscript_push(int n)
{
	int fc, lc, ch;
	if (text_keypad_thumbscript_last == 0) {
		text_keypad_thumbscript_last = n;
	} else {
		fc = text_keypad_thumbscript_last;
		lc = n;
		text_keypad_thumbscript_last = 0;
		
		switch (text_keypad_thumbscript_mode) {
		case 0: /* normal */
			ch = text_keypad_thumbscript_normal[fc*9 + lc];
			break;
		case 1: /* shift */
			ch = text_keypad_thumbscript_shift[fc*9 + lc];
			break;
		case 3: /* mod */
		case 2: /* cmd */
		case 4: /* alt */
		case 5: /* meta/ctrl */
		case 6: /* fn */
			ch = text_keypad_thumbscript_mod[fc*9 + lc];
			break;
		}
		if (ch < 7) {
			text_keypad_thumbscript_mode = ch;
		} else {
			text_keypad_thumbscript_mode = 0;
			if ((ch >= 128) && (ch <= 136)) {
				fc = text_get_buffer()[text_get_iposition()-1];
				if ((fc >= 'A') && (fc <= 'Z')) {
					text_output_char(8);
					text_output_char(text_keypad_thumbscript_accents[(ch-128)*52 + 0 + (fc-'A')]);
				} else if ((fc >= 'a') && (fc <= 'z')) {
					text_output_char(8);
					text_output_char(text_keypad_thumbscript_accents[(ch-128)*52 + 26 + (fc-'a')]);
				}
			} else {
				text_output_char(ch);
			}
		}
	}
}

void text_keypad_thumbscript_4gpush(int n)
{
	/* n is our position on the click wheel as given by clickwheel_octrant */
	switch (n) {
	case 0: text_keypad_thumbscript_push(1); break;
	case 1: text_keypad_thumbscript_push(2); break;
	case 2: text_keypad_thumbscript_push(5); break;
	case 3: text_keypad_thumbscript_push(8); break;
	case 4: text_keypad_thumbscript_push(7); break;
	case 5: text_keypad_thumbscript_push(6); break;
	case 6: text_keypad_thumbscript_push(3); break;
	case 7: text_keypad_thumbscript_push(0); break;
	}
	/* the center button would do text_keypad_thumbscript_push(4); */
}

int text_keypad_thumbscript_event(GR_EVENT *event)
{
	int i;
	switch (event->type)
	{
	case GR_EVENT_TYPE_TIMER:
		if (is_clickwheel_timer(event)) {
			i = clickwheel_octrant();
			if (clickwheel_change(i) && (i>=0)) {
				text_keypad_thumbscript_4gpush(i);
				text_draw();
			}
		}
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		case '\n':
		case '\r':
			text_keypad_thumbscript_push(4);
			text_draw();
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
	}
	return 1;
}
