/*
 * This is basically an all-in-one module containing textinput,
 * tiwidgets, tixtensions, tidial, and podwrite modules.
 * This was made for ZeroLauncher but can act as an ordinary module.
 * All source is from SVN July 4, 2007 thanks to Rufus.
 * This is basically a full copy-and-paste job with a few modifications.
 * (removal of some stuff, merged initiation and anesthetic tweaks)
 * There are still some problems due to the original coding however;
 * file manipulation seems broken (and is thus removed), the "Run..."
 * function is useless (and is thus removed), predict text doesn't seem
 * to work (and is thus removed), and the Podwrite menu is buggy for nanos.
 * Also, to save space, all he text input demos have been removed.
 *
 * Note that because this is one module, it will not fulfill any
 * dependencies on the original modules. The trade-off, however, is
 * that loading it is much faster ; )
 *
 * ~Keripo
 *
 * Last updated: August 22, 2007
 */

/*
 * All the original copywrite/licensing stuff - jonrelay really is quite the coding girl
 *
 * Copyright (C) 2005-2006 Jonathan Bettencourt (jonrelay) [textinput, tiwidgets, tixtension, tidial, podwrite]
 * For the tidial module:
 * Scroll Pad concept by veteran.
 * Dial Type concept by Peter Burk.
 * Fixed Layout and Scroll with Prediction concepts by mp.
 * All implemented by jonrelay.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/////////////////////////////////
// textinput module
/////////////////////////////////

/*
 *  /!\ Warning! This is NOT the same as legacy textinput.c!
 */

#include "pz.h"
#include <string.h>

/* defines for text input */

#define TI_MAX_TIMS 64
#define TI_SETTING_SEL_TIM 1
#define TI_DEFAULT_TIM 2

/* variables for text input */

static PzModule * ti_module;
static PzConfig * ti_conf;
static int ti_selected_tim;
static int ti_saved_tim;
static TWidget * (* ti_tim_creators[TI_MAX_TIMS])();
static TWidget * (* ti_tim_ncreators[TI_MAX_TIMS])();

/* API calls for TIM widgets */

TApItem * ti_ap_getx(int i)
{
	TApItem * tai;
	switch (i) {
	case 0:
		tai = ttk_ap_get("input.bg");
		if (!tai) { tai = ttk_ap_getx("window.bg"); }
		return tai;
		break;
	case 1:
		tai = ttk_ap_get("input.fg");
		if (!tai) { tai = ttk_ap_getx("window.fg"); }
		return tai;
		break;
	case 2:
		tai = ttk_ap_get("input.selbg");
		if (!tai) { tai = ttk_ap_getx("menu.selbg"); }
		return tai;
		break;
	case 3:
		tai = ttk_ap_get("input.selfg");
		if (!tai) { tai = ttk_ap_getx("menu.selfg"); }
		return tai;
		break;
	case 4:
		tai = ttk_ap_get("input.border");
		if (!tai) { tai = ttk_ap_getx("window.border"); }
		return tai;
		break;
	case 5:
		tai = ttk_ap_get("input.cursor");
		if (!tai) { tai = ttk_ap_getx("window.border"); }
		return tai;
		break;
	}
	return 0;
}

ttk_color ti_ap_get(int i)
{
	TApItem * tai = ti_ap_getx(i);
	if (!tai) {
		switch (i) {
		case 0:
		case 3:
			return ttk_makecol(WHITE);
			break;
		case 4:
		case 5:
			return ttk_makecol(GREY);
			break;
		case 2:
			return ttk_makecol(DKGREY);
			break;
		default:
			return ttk_makecol(BLACK);
			break;
		}
	} else {
		return tai->color;
	}
}

TWidget * ti_create_tim_widget(int ht, int wd)
{
	int sh, sw;
	TWidget * wid;
	ttk_get_screensize(&sw, &sh, 0);
	wid = ttk_new_widget( ((wd>0)?(sw-wd):(-wd)), sh-((ht<0)?(ttk_text_height(ttk_menufont)*(-ht)+1):ht) );
	wid->h = ((ht<0)?(ttk_text_height(ttk_menufont)*(-ht)+1):ht);
	wid->w = ((wd>0)?(wd):(sw+wd));
	wid->focusable = 1;
	return wid;
}

/* the built-in Serial TIM */

// KERIPO MOD
// removal of the "Serial" input method

/*
#ifdef IPOD
int ti_serial_abort(TWidget * this, int ch) 
{
	if (ch == TTK_BUTTON_ACTION) {
		pz_warning (_("I'd suggest picking another text input method before you try that again."));
		ttk_input_end();
		return TTK_EV_CLICK;
	}
	return TTK_EV_UNUSED;
}
#endif

int ti_serial_down(TWidget * this, int ch)
{
	if (ch == 27) {
		ttk_input_end();
		return TTK_EV_CLICK;
	} else if (ch == 10 || ch == 13) {
		ttk_input_char(TTK_INPUT_ENTER);
		return TTK_EV_CLICK;
	} else if (ch < 256) {
		ttk_input_char(ch);
		return TTK_EV_CLICK;
	}
	return TTK_EV_UNUSED;
}

TWidget * ti_serial_create()
{
	TWidget * wid = ti_create_tim_widget(0, 0);
	wid->rawkeys = 1;
	wid->keyrepeat = 1;
	wid->down = ti_serial_down;
#ifdef IPOD
	wid->held = ti_serial_abort;
	wid->holdtime = 3000;
#endif
	return wid;
}
*/

/*
 * Known TIM ID numbers:
 *  0 - Serial (was Off)
 *  1 - Reserved (was Serial)
 *  2 - Scroll Through
 *  3 - On-Screen Keyboard
 *  4 - Morse Code
 *  5 - Cursive
 *  6 - Wheelboard
 *  7 - Four-Button Keyboard
 *  8 - Scroll Through with Return
 *  9 - Telephone Keypad
 * 10 - Thumbscript
 * 11 - Four-Button Keypad
 * 12 - Dasher
 * 13 - Speech Recognition
 * 14 - FreqMod's TIM (http://freqmod.dyndns.org/upload/ipnno.png)
 * 15 - Reserved
 * 16 - Unicode Hex Input
 * 17 - Kana Palette
 * 18 - Multilingual Wheelboard
 * 19 - Reserved
 * 20 - Scroll Through with Fixed Layout
 * 21 - Scroll Through with Prediction
 * 22 - Reserved
 * 23 - Reserved
 *
 * All other ID numbers (24-63) are free for other devs to use.
 */

/* API calls for TIM modules */

int ti_supports_method(int id)
{
	return (ti_tim_creators[id] && ti_tim_ncreators[id]);
}

int ti_select(int id)
{
	if (!ti_supports_method(id)) return 0;
	pz_register_input_method(ti_tim_creators[id]);
	pz_register_input_method_n(ti_tim_ncreators[id]);
	ti_saved_tim = id;
	ti_selected_tim = id;
	pz_set_int_setting(ti_conf, TI_SETTING_SEL_TIM, ti_selected_tim);
	pz_save_config(ti_conf);
	return 0;
}

int ti_current_method()
{
	return ti_selected_tim;
}

int ti_use_method(int id)
{
	if (!ti_supports_method(id)) return ti_selected_tim;
	pz_register_input_method(ti_tim_creators[id]);
	pz_register_input_method_n(ti_tim_ncreators[id]);
	ti_saved_tim = ti_selected_tim;
	ti_selected_tim = id;
	return ti_saved_tim;
}

int ti_unuse_method()
{
	int id = ti_saved_tim;
	pz_register_input_method(ti_tim_creators[id]);
	pz_register_input_method_n(ti_tim_ncreators[id]);
	ti_selected_tim = id;
	return 0;
}

TWindow * ti_ttkselect(ttk_menu_item * item)
{
	ti_select((long)item->data);
	return TTK_MENU_UPONE;
}

int ti_register(TWidget * (* create)(), TWidget * (* ncreate)(), char * name, int id)
{
	char menupath[128] = "/Settings/Text Input/";
	ti_tim_creators[id] = create;
	ti_tim_ncreators[id] = ncreate;
	strcat(menupath, name);
	pz_menu_add_ttkh(menupath, ti_ttkselect, (void *)(long)id);
	if (ti_selected_tim == id) {
		ti_select(id);
	}
	return 0;
}

/* main text input module initialization */

/*
void ti_init()
{
	int i;
	ti_module = pz_register_module("textinput", 0);
	ti_conf = pz_load_config(pz_module_get_cfgpath(ti_module, "textinput.conf"));
	for (i=0; i<64; i++) {
		ti_tim_creators[i]=0;
		ti_tim_ncreators[i]=0;
	}
	if (pz_get_setting(ti_conf, TI_SETTING_SEL_TIM)) {
		ti_selected_tim = ti_saved_tim = pz_get_int_setting(ti_conf, TI_SETTING_SEL_TIM);
	} else {
		ti_selected_tim = ti_saved_tim = TI_DEFAULT_TIM;
	}
#ifdef IPOD
	if (!ti_selected_tim) {
		pz_warning (_("Serial text input is selected. Make sure a keyboard is attached or select a different input method."));
	}
#endif
	ti_register(ti_serial_create, ti_serial_create, N_("Serial"), 0);
}

PZ_MOD_INIT(ti_init)
*/

/////////////////////////////////
// tiwidgets module
/////////////////////////////////

#include "pz.h"
#include <ctype.h>
#include <string.h>

/* dependent on textinput module */
extern ttk_color ti_ap_get(int);
extern TApItem * ti_ap_getx(int);

//static PzModule * module;

/* Text Input Buffer Management */

typedef struct TiBuffer {
	char * text;
	int asize; /* allocated size */
	int usize; /* used size */
	int cpos; /* cursor position */
	int idata[4];
	void * data;
	int (*callback)(TWidget *, char *); /* callback function */
} TiBuffer;

TiBuffer * ti_create_buffer(int size)
{
	TiBuffer * buf = (TiBuffer *)malloc(sizeof(TiBuffer));
	int s = (size>1024)?size:1024;
	if (buf) {
		if ( (buf->text = (char *)malloc(s)) ) {
			buf->asize = s;
			buf->text[0] = 0;
		} else {
			buf->asize = 0;
		}
		buf->usize = 0;
		buf->cpos = 0;
		buf->idata[0] = 0;
		buf->idata[1] = 0;
		buf->idata[2] = 0;
		buf->idata[3] = 0;
		buf->data = 0;
		buf->callback = 0;
	}
	return buf;
}

void ti_destroy_buffer(TiBuffer * buf)
{
	if (buf->text) free(buf->text);
	free(buf);
}

int ti_expand_buffer(TiBuffer * buf, int exp)
{
	char * t;
	if ((buf->usize + exp) >= buf->asize) {
		if ( (t = (char *)realloc(buf->text, buf->asize + 1024)) ) {
			buf->text = t;
			buf->asize += 1024;
			buf->usize += exp;
			return 1;
		}
		return 0; /* realloc failed */
	} else {
		buf->usize += exp;
		return 1;
	}
}

int ti_set_buffer(TiBuffer * buf, char * s)
{
	char * t;
	int l = strlen(s);
	if ( (t = (char *)malloc(l+1024)) ) {
		if (buf->text) free(buf->text);
		buf->text = t;
		buf->asize = l+1024;
		buf->usize = l;
		buf->cpos = l;
		strcpy(buf->text, s);
		return 1;
	}
	return 0;
}

TiBuffer * ti_create_buffer_with(char * s)
{
	TiBuffer * b = ti_create_buffer(0);
	if (b) {
		ti_set_buffer(b, s);
	}
	return b;
}

char * ti_get_buffer(TiBuffer * buf)
{
	return buf->text;
}

void ti_buffer_insert_byte(TiBuffer * buf, int pos, char ch)
{
	int i;
	if (pos < 0) return;
	if (ti_expand_buffer(buf, 1)) {
		for (i=(buf->usize); i>pos; i--) {
			buf->text[i] = buf->text[i-1];
		}
		buf->text[pos] = ch;
	}
}

void ti_buffer_remove_byte(TiBuffer * buf, int pos)
{
	int i;
	if ((pos < 0) || (pos >= (buf->usize))) return;
	for (i=pos; i<(buf->usize); i++) {
		buf->text[i] = buf->text[i+1];
	}
	buf->usize--;
}

int ti_utf8_encode(int ch, char c[4])
{
	if (ch < 0) {
		c[0]=('?');
		return 0;
	} else if (ch < 0x80) {
		c[0]=(ch);
		return 1;
	} else if (ch < 0x0800) {
		c[0]=((ch >> 6        ) | 0xC0);
		c[1]=((ch       & 0x3F) | 0x80);
		return 2;
	} else if (ch < 0x10000) {
		c[0]=((ch >> 12       ) | 0xE0);
		c[1]=((ch >> 6  & 0x3F) | 0x80);
		c[2]=((ch       & 0x3F) | 0x80);
		return 3;
	} else if (ch < 0x110000) {
		c[0]=((ch >> 18       ) | 0xF0);
		c[1]=((ch >> 12 & 0x3F) | 0x80);
		c[2]=((ch >> 6  & 0x3F) | 0x80);
		c[3]=((ch       & 0x3F) | 0x80);
		return 4;
	} else {
		c[0]=('?');
		return 0;
	}
}

void ti_buffer_insert_char(TiBuffer * buf, int pos, int ch)
{
	int i;
	char c[4];
	int l = ti_utf8_encode(ch, c);
	if (pos<0 || l<=0) return;
	if (ti_expand_buffer(buf, l)) {
		for (i=(buf->usize)-l; i>=pos; i--) {
			buf->text[i+l] = buf->text[i];
		}
		for (i=0; i<l; i++) {
			buf->text[pos+i] = c[i];
		}
	}
}

void ti_buffer_remove_char(TiBuffer * buf, int pos)
{
	ti_buffer_remove_byte(buf, pos);
	while ((buf->text[pos] & 0xC0) == 0x80) {
		ti_buffer_remove_byte(buf, pos);
	}
}

void ti_buffer_cmove(TiBuffer * buf, int m)
{
	if (m<0) {
		/* move left */
		while (m<0) {
			do {
				buf->cpos--;
				if (buf->cpos < 0) {
					buf->cpos = 0;
					return;
				}
			} while ((buf->text[buf->cpos] & 0xC0) == 0x80);
			m++;
		}
	} else {
		/* move right */
		while (m>0) {
			do {
				buf->cpos++;
				if (buf->cpos > buf->usize) {
					buf->cpos = buf->usize;
					return;
				}
			} while ((buf->text[buf->cpos] & 0xC0) == 0x80);
			m--;
		}
	}
}

void ti_buffer_cset(TiBuffer * buf, int c)
{
	if (c < 0) {
		buf->cpos = 0;
	} else if (c > buf->usize) {
		buf->cpos = buf->usize;
	} else {
		buf->cpos = c;
	}
}

int ti_buffer_cget(TiBuffer * buf)
{
	return buf->cpos;
}

void ti_buffer_input(TiBuffer * buf, int ch)
{
	switch (ch) {
	/* case 0: */
	case TTK_INPUT_END:
		break;
	case 28:
	case TTK_INPUT_LEFT:
		ti_buffer_cmove(buf,-1);
		break;
	case 29:
	case TTK_INPUT_RIGHT:
		ti_buffer_cmove(buf,1);
		break;
	/* case 8: */
	case TTK_INPUT_BKSP:
		if (!(buf->cpos)) break;
		ti_buffer_cmove(buf,-1);
	case 127:
		ti_buffer_remove_char(buf, buf->cpos);
		break;
	case 10:
	/* case 13: */
	case TTK_INPUT_ENTER:
		ch = 10;
	default:
		ti_buffer_insert_char(buf, buf->cpos, ch);
		ti_buffer_cmove(buf,1);
		break;
	}
}

/* Multiline Text */

void ti_multiline_text(ttk_surface srf, ttk_font fnt, int x, int y, int w, int h, ttk_color col, const char *t, int cursorpos, int scroll, int * lc, int * sl, int * cb)
{
	char * sot; /* start of text */
	char * curtextptr;
	char curline[1024];
	int currentLine = 0;
	int screenlines = 0;
	int coob = 0; /* Cursor Out Of Bounds, or iPodLinux dev; you decide */
	int width, height;
	TApItem * curcol = ti_ap_getx(5);
	height = ttk_text_height(fnt);
	screenlines = h / height;

	sot = (char *)t;
	curtextptr = (char *)t;
	if ((cursorpos >= 0) && (*curtextptr == '\0')) {
		/* text is empty, draw the cursor at initial position */
		ttk_ap_vline(srf, curcol, x, y, y+height);
	}
	while (*curtextptr != '\0') {
		char * sol; /* start of line */
		char ctp;
		sol = curtextptr;
		curline[0] = 0;
		/* find the line break */
		while (1) {
			/* bring in the next character */
			do {
				ctp=curline[curtextptr-sol]=(*curtextptr);
				curline[curtextptr-sol+1]=0;
				curtextptr++;
			} while ((*curtextptr & 0xC0) == 0x80);
			/* if that was a newline, that's where the break is */
			if ((ctp == '\n')||(ctp == '\r')) {
				curline[curtextptr-sol-1]=0;
				break;
			/* if that was a null, that's where the break is */
			} else if (ctp == '\0') {
				curtextptr--; /* need to decrease so that the next iteration sees that *curtextptr == 0 and exits */
				break;
			}
			/* determine if we've gotten a few characters too many */
			width = ttk_text_width(fnt, curline);
			if (width > w) {
				/* backtrack to the last word */
				char *optr;
				int fndWrd = 0;
				for (optr=curtextptr-1; optr>sol; optr--) {
					if (isspace(*optr)||(*optr=='\t')||(*optr=='-')) {
						curtextptr=optr+1;
						curline[curtextptr-sol]=0;
						fndWrd = 1;
						break;
					}
				}
				if (!fndWrd) {
					/* it's one long string of letters, chop off the last one */
					curtextptr--;
					while ((*curtextptr & 0xC0) == 0x80) {
						curtextptr--;
					}
					curline[curtextptr-sol]=0;
				}
				break;
			}
		}
		/* if the line is in the viewable area, draw it */
		if ( ((currentLine - scroll) >= 0) && ((currentLine - scroll) < screenlines) ) {
			ttk_text(srf, fnt, x, y+((currentLine-scroll)*height), col, curline);
			if ((cursorpos >= sol-sot) && (cursorpos < curtextptr-sot)) {
				/* cursor is on the line, but not at the very end */
				if (cursorpos == (sol-sot)) {
					width = 0;
				} else {
					curline[cursorpos-(sol-sot)] = 0;
					width = ttk_text_width(fnt, curline);
				}
				ttk_ap_vline(srf, curcol, x+width, y+((currentLine-scroll)*height), y+((currentLine-scroll+1)*height));
			} else if ((cursorpos == curtextptr-sot) && (*curtextptr == '\0')) {
				/* cursor is not only at the very end of the line, but the very end of the text */
				if ((*(curtextptr-1) == '\n')||(*(curtextptr-1) == '\r')) {
					/* last character of the text is a newline, so display the cursor on the next line */
					ttk_ap_vline(srf, curcol, x, y+((currentLine-scroll+1)*height), y+((currentLine-scroll+2)*height));
					if ((currentLine - scroll + 1) == screenlines) { coob = 1; }
				} else {
					/* display the cursor normally */
					curline[cursorpos-(sol-sot)] = 0;
					width = ttk_text_width(fnt, curline);
					ttk_ap_vline(srf, curcol, x+width, y+((currentLine-scroll)*height), y+((currentLine-scroll+1)*height));
				}
			}
			
			if ( ((currentLine - scroll) == 0) && (cursorpos < sol-sot) ) {
				coob = -1; /* cursor is before the viewable area */
			} else if ( ((currentLine - scroll + 1) == screenlines) && (cursorpos >= curtextptr-sot) ) {
				coob = 1; /* cursor is after the viewable area */
			}
		}
		currentLine++;
	}
	if ( (*curtextptr == '\0') && ((*(curtextptr-1) == '\n') || (*(curtextptr-1) == '\r')) && (t[0] != 0) ) {
		currentLine++; /* while loop doesn't count trailing newline as another line */
	}
	
	if (lc) *lc = currentLine;
	if (sl) *sl = screenlines;
	if (cb) *cb = coob;
}

/* Text Input Widget Event Handlers */

int ti_widget_start(TWidget * wid)
{
	/* Start text input if it hasn't been started already. */
	int ht;
	if (((TiBuffer *)wid->data)->idata[0]) {
		ht = pz_start_input_n_for (wid->win);
	} else {
		ht = pz_start_input_for (wid->win);
	}
	if (((TiBuffer *)wid->data)->idata[1]) {
		ttk_input_move_for(wid->win, wid->win->x+wid->x+1, wid->win->y+wid->y+wid->h-ht-1);
		((TiBuffer *)wid->data)->idata[2] = ht;
	} else {
		ttk_input_move_for(wid->win, wid->win->x+wid->x+1, wid->win->y+wid->y+wid->h);
		((TiBuffer *)wid->data)->idata[2] = 0;
	}
	wid->win->input->w = wid->w-2;
	return TTK_EV_CLICK;
}

int ti_widget_start1(TWidget * wid, int x)
{
	return ti_widget_start(wid);
}

int ti_widget_start2(TWidget * wid, int x, int y)
{
	return ti_widget_start(wid);
}

int ti_widget_input(TWidget * wid, int ch)
{
	if (ch == TTK_INPUT_END || ch == TTK_INPUT_ENTER) {
		if (((TiBuffer *)wid->data)->callback) {
			if (((TiBuffer *)wid->data)->callback(wid, ((TiBuffer *)wid->data)->text)) {
				ti_widget_start(wid);
			}
		}
	} else {
		ti_buffer_input(((TiBuffer *)wid->data), ch);
		wid->dirty = 1;
	}
	return 0;
}

int ti_widget_input_n(TWidget * wid, int ch)
{
	if (ch == TTK_INPUT_END || ch == TTK_INPUT_ENTER) {
		if (((TiBuffer *)wid->data)->callback) {
			if (((TiBuffer *)wid->data)->callback(wid, ((TiBuffer *)wid->data)->text)) {
				ti_widget_start(wid);
			}
		}
	} else {
		if (ch >= 32) {
			if ((ch >= '0') && (ch <= '9')) {}
			else if ((ch == '+') || (ch == '*') || (ch == '/') || (ch == '^')) {}
			else if ((ch == '-') || (ch == '=')) { ch='-'; }
			else if ((ch >= ' ') && (ch <= '?')) { ch='.'; }
			else if ((ch == 'E') || (ch == '_')) { ch='e'; }
			else { ch = ( '0' + ((ch-7)%10) ); }
		}
		ti_buffer_input(((TiBuffer *)wid->data), ch);
		wid->dirty = 1;
	}
	return 0;
}

int ti_widget_input_ml(TWidget * wid, int ch)
{
	if (ch == TTK_INPUT_END) {
		if (((TiBuffer *)wid->data)->callback) {
			if (((TiBuffer *)wid->data)->callback(wid, ((TiBuffer *)wid->data)->text)) {
				ti_widget_start(wid);
			}
		}
	} else {
		ti_buffer_input(((TiBuffer *)wid->data), ch);
		wid->dirty = 1;
	}
	return 0;
}

void ti_widget_destroy(TWidget * wid)
{
	ttk_input_end();
	ti_destroy_buffer(((TiBuffer *)wid->data));
}

void ti_widget_draw(TWidget * wid, ttk_surface srf)
{
	int sw, cp; /* string width, cursor position */
	int spw, cpw; /* string pixel width, cursor pixel width */
	int h, blitx; /* height, blit source x */
	char * tmp;
	ttk_surface srf2;
	
	/* get info */
	sw = ((TiBuffer *)wid->data)->usize;
	cp = ((TiBuffer *)wid->data)->cpos;
	tmp = (char *)malloc(sw+1);
	strcpy(tmp, ((TiBuffer *)wid->data)->text);
	spw = ttk_text_width(ttk_textfont, tmp);
	h = wid->h; /*ttk_text_height(ttk_textfont);*/
	
	/* create graphics buffer */
	srf2 = ttk_new_surface(spw+1, h, ttk_screen->bpp);
	
	/* background */
	ttk_ap_fillrect(srf2, ti_ap_getx(0), 0, 0, spw+1, h);
	
	/* text */
	ttk_text(srf2, ttk_textfont, 0, 5, ti_ap_get(1), tmp);
	
	/* cursor */
	tmp[cp]=0;
	cpw = ttk_text_width(ttk_textfont, tmp);
	ttk_ap_vline(srf2, ti_ap_getx(5), cpw, 5, h-5);
	
	/* figure out where to blit it */
	blitx = cpw - (wid->w - 10)/2;
	if ((blitx + wid->w - 10) > (spw+1)) {
		blitx = (spw+1) - wid->w + 10;
	}
	if (blitx < 0) blitx = 0;
	
	/* background and border */
	ttk_ap_fillrect(srf, ti_ap_getx(0), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	/* blit it */
	ttk_blit_image_ex(srf2, blitx, 0, wid->w-10, h, srf, wid->x+5, wid->y);
	/* border */
	ttk_ap_rect(srf, ti_ap_getx(4), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	
	/* done */
	ttk_free_surface(srf2);
	free(tmp);
}

void ti_widget_draw_p(TWidget * wid, ttk_surface srf)
{
	int sw, cp, i; /* string width, cursor position */
	int spw, cpw; /* string pixel width, cursor pixel width */
	int h, blitx; /* height, blit source x */
	char * tmp;
	ttk_surface srf2;
	
	/* get info */
	sw = ((TiBuffer *)wid->data)->usize;
	cp = ((TiBuffer *)wid->data)->cpos;
	tmp = (char *)malloc(sw+1);
	strcpy(tmp, ((TiBuffer *)wid->data)->text);
	for (i=0; i<sw; i++) tmp[i]='*';
	spw = ttk_text_width(ttk_textfont, tmp);
	h = wid->h; /*ttk_text_height(ttk_textfont);*/
	
	/* create graphics buffer */
	srf2 = ttk_new_surface(spw+1, h, ttk_screen->bpp);
	
	/* background */
	ttk_ap_fillrect(srf2, ti_ap_getx(0), 0, 0, spw+1, h);
	
	/* text */
	ttk_text(srf2, ttk_textfont, 0, 5, ti_ap_get(1), tmp);
	
	/* cursor */
	tmp[cp]=0;
	cpw = ttk_text_width(ttk_textfont, tmp);
	ttk_ap_vline(srf2, ti_ap_getx(5), cpw, 5, h-5);
	
	/* figure out where to blit it */
	blitx = cpw - (wid->w - 10)/2;
	if ((blitx + wid->w - 10) > (spw+1)) {
		blitx = (spw+1) - wid->w + 10;
	}
	if (blitx < 0) blitx = 0;
	
	/* background and border */
	ttk_ap_fillrect(srf, ti_ap_getx(0), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	/* blit it */
	ttk_blit_image_ex(srf2, blitx, 0, wid->w-10, h, srf, wid->x+5, wid->y);
	/* border */
	ttk_ap_rect(srf, ti_ap_getx(4), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	
	/* done */
	ttk_free_surface(srf2);
	free(tmp);
}

void ti_widget_draw_ml(TWidget * wid, ttk_surface srf)
{
	ttk_ap_fillrect(srf, ti_ap_getx(0), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	
	/* draw border */
	ttk_ap_rect(srf, ti_ap_getx(4), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	
	ti_multiline_text(srf, ttk_textfont, wid->x+5, wid->y+5, wid->w-10, wid->h-10-(((TiBuffer *)wid->data)->idata[2]),
		ti_ap_get(1), ((TiBuffer *)wid->data)->text, ((TiBuffer *)wid->data)->cpos, 0, 0, 0, 0);
}

//Text Input Widgets

//What does this code (particularly the headers) remind you of? ^.^

TWidget * ti_new_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *),
	void (*draw)(TWidget *, ttk_surface), int (*input)(TWidget *, int), int numeric)
{
	TWidget * wid = ttk_new_widget(x,y);
	wid->h = h;
	wid->w = w;
	if ( (wid->data = ti_create_buffer_with(dt)) ) {
		((TiBuffer *)wid->data)->callback = callback;
	}
	wid->focusable = 1;
	wid->button = ti_widget_start2;
	wid->scroll = ti_widget_start1;
	wid->input = input;
	wid->draw = draw;
	wid->destroy = ti_widget_destroy;
	((TiBuffer *)wid->data)->idata[0] = numeric;
	((TiBuffer *)wid->data)->idata[1] = absheight;
	return wid;
}

TWidget * ti_new_standard_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *))
{
	return ti_new_text_widget(x, y, w, h, absheight, dt, callback, ti_widget_draw, ti_widget_input, 0);
}

// Text Input Demo
// KERIPO MOD
// Remove all the demos - save space ; /

/*
TWidget * ti_new_numeric_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *))
{
	return ti_new_text_widget(x, y, w, h, absheight, dt, callback, ti_widget_draw, ti_widget_input_n, 1);
}



TWidget * ti_new_password_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *))
{
	return ti_new_text_widget(x, y, w, h, absheight, dt, callback, ti_widget_draw_p, ti_widget_input, 0);
}



TWidget * ti_new_multiline_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *))
{
	return ti_new_text_widget(x, y, w, h, absheight, dt, callback, ti_widget_draw_ml, ti_widget_input_ml, 0);
}


int ti_destructive_callback(TWidget * wid, char * txt)
{
	pz_close_window(wid->win);
	return 0;
}

PzWindow * new_standard_text_demo_window()
{
	PzWindow * ret;
	TWidget * wid;
	ret = pz_new_window(_("Text Input Demo"), PZ_WINDOW_NORMAL);
	wid = ti_new_standard_text_widget(10, 40, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", ti_destructive_callback);
	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

PzWindow * new_numeric_text_demo_window()
{
	PzWindow * ret;
	TWidget * wid;
	ret = pz_new_window(_("Text Input Demo"), PZ_WINDOW_NORMAL);
	wid = ti_new_numeric_text_widget(10, 40, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", ti_destructive_callback);
	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

PzWindow * new_password_text_demo_window()
{
	PzWindow * ret;
	TWidget * wid;
	ret = pz_new_window(_("Text Input Demo"), PZ_WINDOW_NORMAL);
	wid = ti_new_password_text_widget(10, 40, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", ti_destructive_callback);
	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

PzWindow * new_multiline_text_demo_window()
{
	PzWindow * ret;
	TWidget * wid;
	ret = pz_new_window(_("Text Input Demo"), PZ_WINDOW_NORMAL);
	wid = ti_new_multiline_text_widget(10, 10, ret->w-20, ret->h-20, 1, "", ti_destructive_callback);
	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}
*/

/*
void init_textinput_demos()
{
	module = pz_register_module ("tiwidgets", 0);
	pz_menu_add_action ("/Extras/Demos/Text Input Demo/Standard", new_standard_text_demo_window);
	pz_menu_add_action ("/Extras/Demos/Text Input Demo/Numeric", new_numeric_text_demo_window);
	pz_menu_add_action ("/Extras/Demos/Text Input Demo/Password", new_password_text_demo_window);
	pz_menu_add_action ("/Extras/Demos/Text Input Demo/Multiline", new_multiline_text_demo_window);
}

PZ_MOD_INIT(init_textinput_demos)
*/

/////////////////////////////////
// tixtensions module
/////////////////////////////////

#include "pz.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

//static PzModule * module;

// KERIPO MOD
// remove all copy/paste stuff

static char * tix_rename_oldname;
//static char * tix_file_clipboard = 0;
//static char * tix_file_clipboard_name = 0;
//static int    tix_file_clipboard_copy = 0;

static ttk_menu_item tix_rename_menuitem;
static ttk_menu_item tix_mkdir_menuitem;
//static ttk_menu_item tix_cut_menuitem;
//static ttk_menu_item tix_copy_menuitem;
//static ttk_menu_item tix_paste_menuitem;
//static ttk_menu_item tix_pastelink_menuitem;

extern TWidget * ti_new_standard_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *));
extern void ti_multiline_text(ttk_surface srf, ttk_font fnt, int x, int y, int w, int h, ttk_color col, const char *t, int cursorpos, int scroll, int * lc, int * sl, int * cb);
extern int ti_widget_start(TWidget * wid);

/* ===== COMMON ===== */

int tix_true(const char * fn)
{
	return 1;
}

// KERIPO MOD
// Remove entire copy/paste section - doesn't seem to work ; /
/*
//===== CUT COPY PASTE =====

int tix_paste_visible(ttk_menu_item * item)
{
	return !!tix_file_clipboard;
}

#define TIX_FILE_BUFFER_SIZE 10240

PzWindow * tix_paste_handler(ttk_menu_item * item)
{
	char fn[1024];
	FILE * fin;
	FILE * fout;
	char * buf;
	int buflen;
	if (tix_file_clipboard) {
		getcwd(fn, 1024);
		if (fn[strlen(fn)-1] != '/') strcat(fn, "/");
		strcat(fn, tix_file_clipboard_name);
		if (tix_file_clipboard_copy) {
			
			if (! (buf = (char *)malloc(TIX_FILE_BUFFER_SIZE)) ) {
				pz_error("malloc for file copy failed\n");
				return TTK_MENU_UPONE;
			}
			fin = fopen(tix_file_clipboard, "rb");
			fout = fopen(fn, "wb");
			while ((buflen = fread(buf, 1, TIX_FILE_BUFFER_SIZE, fin))) {
				fwrite(buf, 1, buflen, fout);
			}
			if (!feof(fin)) {
				pz_perror("fwrite call returned error in file copy");
			}
			if (fin) fclose(fin);
			if (fout) fclose(fout);
			free(buf);
			
		} else {
			if (rename(tix_file_clipboard, fn)) {
				pz_perror("rename call returned error");
			}
			free(tix_file_clipboard);
			free(tix_file_clipboard_name);
			tix_file_clipboard = 0;
			tix_file_clipboard_name = 0;
		}
	}
	return TTK_MENU_UPONE;
}

PzWindow * tix_pastelink_handler(ttk_menu_item * item)
{
	char fn[1024];
	if (tix_file_clipboard) {
		getcwd(fn, 1024);
		if (fn[strlen(fn)-1] != '/') strcat(fn, "/");
		strcat(fn, tix_file_clipboard_name);
		if (symlink(tix_file_clipboard, fn)) {
			pz_perror("symlink call returned error");
		}
	}
	return TTK_MENU_UPONE;
}

PzWindow * tix_copy_handler(ttk_menu_item * item)
{
	if (tix_file_clipboard) free(tix_file_clipboard);
	if (tix_file_clipboard_name) free(tix_file_clipboard_name);
	tix_file_clipboard = (char *)malloc(1024);
	getcwd(tix_file_clipboard, 1024);
	if (tix_file_clipboard[strlen(tix_file_clipboard)-1] != '/') strcat(tix_file_clipboard, "/");
	strcat(tix_file_clipboard, item->data);
	tix_file_clipboard_name = strdup(item->data);
	tix_file_clipboard_copy = 1;
	return TTK_MENU_UPONE;
}

PzWindow * tix_cut_handler(ttk_menu_item * item)
{
	if (tix_file_clipboard) free(tix_file_clipboard);
	if (tix_file_clipboard_name) free(tix_file_clipboard_name);
	tix_file_clipboard = (char *)malloc(1024);
	getcwd(tix_file_clipboard, 1024);
	if (tix_file_clipboard[strlen(tix_file_clipboard)-1] != '/') strcat(tix_file_clipboard, "/");
	strcat(tix_file_clipboard, item->data);
	tix_file_clipboard_name = strdup(item->data);
	tix_file_clipboard_copy = 0;
	return TTK_MENU_UPONE;
}
*/

/* ===== MAKE DIR ===== */

void tix_mkdir_draw(TWidget * wid, ttk_surface srf)
{
	ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), wid->x, wid->y, wid->w, wid->h);
	ttk_text(srf, ttk_menufont, wid->x, wid->y, ttk_ap_getx("window.fg")->color, _("Make directory named:"));
}

int tix_mkdir_callback(TWidget * wid, char * fn)
{
	pz_close_window(wid->win);
	if (fn[0]) {
		if (mkdir(fn, 0755)) {
			pz_perror("mkdir call returned error");
		}
	}
	return 0;
}

PzWindow * new_mkdir_window(ttk_menu_item * item)
{
	PzWindow * ret;
	TWidget * wid;
	TWidget * wid2;
	ret = pz_new_window(_("Make Directory"), PZ_WINDOW_NORMAL);
	wid = ti_new_standard_text_widget(10, 10+ttk_text_height(ttk_textfont)*2, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", tix_mkdir_callback);
	ttk_add_widget(ret, wid);
	wid2 = ttk_new_widget(10, 10);
	wid2->w = ret->w-20;
	wid2->h = ttk_text_height(ttk_menufont);
	wid2->draw = tix_mkdir_draw;
	ttk_add_widget(ret, wid2);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

/* ===== RENAME ===== */

void tix_rename_draw(TWidget * wid, ttk_surface srf)
{
	ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), wid->x, wid->y, wid->w, wid->h);
	ttk_text(srf, ttk_menufont, wid->x, wid->y, ttk_ap_getx("window.fg")->color, _("Rename file to:"));
}

int tix_rename_callback(TWidget * wid, char * fn)
{
	pz_close_window(wid->win);
	if (fn[0]) {
		if (rename(tix_rename_oldname, fn)) {
			pz_perror("rename call returned error");
		}
	}
	free(tix_rename_oldname);
	return 0;
}

PzWindow * new_rename_window(ttk_menu_item * item)
{
	PzWindow * ret;
	TWidget * wid;
	TWidget * wid2;
	tix_rename_oldname = strdup(item->data);
	ret = pz_new_window(_("Rename"), PZ_WINDOW_NORMAL);
	wid = ti_new_standard_text_widget(10, 10+ttk_text_height(ttk_textfont)*2, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, tix_rename_oldname, tix_rename_callback);
	ttk_add_widget(ret, wid);
	wid2 = ttk_new_widget(10, 10);
	wid2->w = ret->w-20;
	wid2->h = ttk_text_height(ttk_menufont);
	wid2->draw = tix_rename_draw;
	ttk_add_widget(ret, wid2);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

// KERIPO MOD
// "Run" function seems to be obsolete ; /

/*
//===== RUN =====

void tix_run_draw(TWidget * wid, ttk_surface srf)
{
	ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), wid->x, wid->y, wid->w, wid->h);
	if (ttk_screen->w < 160) {
		ti_multiline_text(srf, ttk_textfont, wid->x, wid->y, wid->w, wid->h, ttk_ap_getx("window.fg")->color,
			_("Type the name of a file to open."), -1, 0, 0, 0, 0);
	} else if (ttk_screen->w < 200) {
		ti_multiline_text(srf, ttk_textfont, wid->x, wid->y, wid->w, wid->h, ttk_ap_getx("window.fg")->color,
			_("Type the name of a program, folder, or document to open."), -1, 0, 0, 0, 0);
	} else {
		ti_multiline_text(srf, ttk_textfont, wid->x, wid->y, wid->w, wid->h, ttk_ap_getx("window.fg")->color,
			_("Type the name of a program, folder, or document, and podzilla will open it for you."), -1, 0, 0, 0, 0);
	}
}

int tix_run_callback(TWidget * wid, char * fn)
{
	TWindow * w;
	pz_close_window(wid->win);
	if (fn[0]) {
		w = pz_browser_open(fn);
		if (w) {
			ttk_show_window(w);
		}
	}
	return 0;
}

PzWindow * new_run_window()
{
	PzWindow * ret;
	TWidget * wid;
	TWidget * wid2;
	ret = pz_new_window(_("Run..."), PZ_WINDOW_NORMAL);
	wid = ti_new_standard_text_widget(10, 10+ttk_text_height(ttk_textfont)*((ttk_screen->w < 160 || ttk_screen->w >= 320)?2:3), ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", tix_run_callback);
	ttk_add_widget(ret, wid);
	wid2 = ttk_new_widget(10, 5);
	wid2->w = ret->w-20;
	wid2->h = ttk_text_height(ttk_menufont)*((ttk_screen->w < 160 || ttk_screen->w >= 320)?2:3);
	wid2->draw = tix_run_draw;
	ttk_add_widget(ret, wid2);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}
*/

/* ===== MODULE INIT ===== */

/*
void init_ti_extensions()
{
	module = pz_register_module ("tixtensions", 0);
	tix_file_clipboard = 0;
	tix_file_clipboard_name = 0;
	tix_file_clipboard_copy = 0;
	
	pz_menu_add_action ("/Run...", new_run_window);
	
	tix_rename_menuitem.name = N_("Rename");
	tix_rename_menuitem.makesub = new_rename_window;
	pz_browser_add_action (tix_true, &tix_rename_menuitem);
	
	tix_mkdir_menuitem.name = N_("Make Directory");
	tix_mkdir_menuitem.makesub = new_mkdir_window;
	pz_browser_add_action (tix_true, &tix_mkdir_menuitem);
	
	tix_cut_menuitem.name = N_("Cut");
	tix_cut_menuitem.makesub = tix_cut_handler;
	pz_browser_add_action (tix_true, &tix_cut_menuitem);
	
	tix_copy_menuitem.name = N_("Copy");
	tix_copy_menuitem.makesub = tix_copy_handler;
	pz_browser_add_action (tix_true, &tix_copy_menuitem);
	
	tix_paste_menuitem.name = N_("Paste");
	tix_paste_menuitem.makesub = tix_paste_handler;
	tix_paste_menuitem.visible = tix_paste_visible;
	pz_browser_add_action (tix_true, &tix_paste_menuitem);
	
	tix_pastelink_menuitem.name = N_("Paste Link");
	tix_pastelink_menuitem.makesub = tix_pastelink_handler;
	tix_pastelink_menuitem.visible = tix_paste_visible;
	pz_browser_add_action (tix_true, &tix_pastelink_menuitem);
}

PZ_MOD_INIT(init_ti_extensions)
*/

/////////////////////////////////
// tidial module
/////////////////////////////////

//Note: Predict text has been removed cause it seems pretty obsolete

#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* dependent on textinput module */
extern ttk_color ti_ap_get(int);
extern TApItem * ti_ap_getx(int);
extern int ti_register(TWidget *(* cr)(), TWidget *(* ncr)(), char *, int);
extern TWidget * ti_create_tim_widget(int ht, int wd);

//static PzModule * module;

const char ti_dial_charlist[] = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ=+?!@#$%^&*()[]{}_-<>~`\"\'1234567890\\/|:;.,M";
const char ti_dial_charlist_n[] = ".0123456789-E+-*/^M";
const char ti_dial_cmstring[] = "Move Cursor";
const int ti_dial_flchars[][32] = {
	{' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!','?',',','.','\''},
	{' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',161,191,';',':','\"'},
	{'.','0','1','2','3','4','5','6','7','8','9','.','-','E','0','1','2','3','4','5','6','7','8','9','.','-','E','+','-','*','/','^'},
	{' ','@','#','$','%','&','(',')','[',']','{','}','<','>','=','_','|','\\','~','`',180,168,184,163,165,162,171,187,215,247,177,176},
	{' ',224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,248,249,250,251,252,253,254,255},
	{' ',192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,216,217,218,219,220,221,222,376},
	{160,338,339,223,402,169,174,8482,170,186,185,178,179,188,189,190,167,182,181,166,164,175,172,173,183,8364,8211,8212,8216,8217,8220,8221}
};
const char ti_dial_eta_lc[] = "abcdefghijklmnopqrstuvwxyz"; //26
const char ti_dial_eta_uc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //26
const char ti_dial_number[] = "0123456789"; //10
const char ti_dial_stuff1[] = "=+?!@#$%^&*()[]{}_-<>~`"; //23
const char ti_dial_stuff2[] = "\"\'\\/|:;.,M"; //10
const char ti_dial_numberstarters[] = "+*=^#$%<>0123456789";
static int ti_dial_mode = 0;
/*
	 -2 - old numeric cursormode
	 -1 - old cursormode
	  0 - old charlist
	  1 - old numeric
	2-8 - fixed layout
	  9 - fixed layout cursormode
	 10 - prediction
	 11 - prediction cursormode
*/
static int ti_dial_charlist_pos = 0;
static int ti_dial_snapback = 0;
static int ti_dial_ppchar = 0;
static int ti_dial_pchar = 0;
//static char ti_dial_predict[64][64][5];


#define C2N(c) (((c)<32)?(0):(((c)<96)?((c)-32):(((c)<127)?((c)-64):(0))))

int ti_dial_max(void)
{
	switch (ti_dial_mode) {
	case -2:	case -1:
	case  9:	case 11:
		return 10;
		break;
	case 0:
		return 95;
		break;
	case 1:
		return 18;
		break;
	case 2:	case 3:	case 4:	case 5:
	case 6:	case 7:	case 8:
		return 39;
		break;
	//case 10:
	//	return 100;
	//	break;
	}
	return 0;
}

int ti_dial_cursormode(void)
{
	return (ti_dial_mode == -2 || ti_dial_mode == -1 || ti_dial_mode == 9 || ti_dial_mode == 11);
}

void ti_dial_togglecursor(void)
{
	switch (ti_dial_mode) {
	case -2:
		ti_dial_mode = 1;
		break;
	case -1:
		ti_dial_mode = 0;
		break;
	case 0:
		ti_dial_mode = -1;
		break;
	case 1:
		ti_dial_mode = -2;
		break;
	case 2:	case 3:	case 4:	case 5:
	case 6:	case 7:	case 8:
		ti_dial_mode = 9;
		break;
	case 9:
		ti_dial_mode = 2;
		break;
	//case 10:
	//	ti_dial_mode = 11;
	//	break;
	//case 11:
	//	ti_dial_mode = 10;
	//	break;
	}
}

int ti_dial_get_char(int w)
{
	if (w<0) {
		w = ti_dial_charlist_pos;
	}
	switch (ti_dial_mode) {
	case -2:	case -1:
	case  9:	case 11:
		return ti_dial_cmstring[w];
		break;
	case 0:
		return ti_dial_charlist[w];
		break;
	case 1:
		return ti_dial_charlist_n[w];
		break;
	case 2:	case 3:	case 4:	case 5:
	case 6:	case 7:	case 8:
		switch (w) {
		case 32: return 'a'; break;
		case 33: return 'A'; break;
		case 34: return '0'; break;
		case 35: return '@'; break;
		case 36: return 228; break;
		case 37: return 196; break;
		case 38: return 169; break;
		case 39: return 'M'; break;
		default:
			return ti_dial_flchars[ti_dial_mode-2][w];
			break;
		}
		break;
	// KERIPO MOD
	// No predict text - doesn't seem to work
	/*
	case 10:
		if (w == 0) {
			return ' ';
		} else if (w<6) {
			int c = ti_dial_predict[C2N(ti_dial_ppchar)][C2N(ti_dial_pchar)][w-1];
			if (!c) c = ti_dial_predict[10][C2N(ti_dial_pchar)][w-1];
			if (!c) c = ti_dial_predict[C2N(ti_dial_ppchar)][10][w-1];
			if (!c) c = ti_dial_predict[10][10][w-1];
			return c;
		} else {
			int c = ti_dial_predict[C2N(ti_dial_ppchar)][C2N(ti_dial_pchar)][0];
			if (!c) c = ti_dial_predict[10][C2N(ti_dial_pchar)][0];
			if (!c) c = ti_dial_predict[C2N(ti_dial_ppchar)][10][0];
			if (!c) c = ti_dial_predict[10][10][0];
			w -= 6;
			if (strchr(ti_dial_numberstarters,c) || isdigit(c)) {
				if (w<10) { return ti_dial_number[w]; } else { w -= 10; }
				if (w<23) { return ti_dial_stuff1[w]; } else { w -= 23; }
				if (w<26) { return ti_dial_eta_lc[w]; } else { w -= 26; }
				if (w<26) { return ti_dial_eta_uc[w]; } else { w -= 26; }
				if (w<10) { return ti_dial_stuff2[w]; } else { w -= 10; }
				return ' ';
			} else if (isupper(c)) {
				if (w<26) { return ti_dial_eta_uc[w]; } else { w -= 26; }
				if (w<26) { return ti_dial_eta_lc[w]; } else { w -= 26; }
				if (w<23) { return ti_dial_stuff1[w]; } else { w -= 23; }
				if (w<10) { return ti_dial_number[w]; } else { w -= 10; }
				if (w<10) { return ti_dial_stuff2[w]; } else { w -= 10; }
				return ' ';
			} else {
				if (w<26) { return ti_dial_eta_lc[w]; } else { w -= 26; }
				if (w<26) { return ti_dial_eta_uc[w]; } else { w -= 26; }
				if (w<23) { return ti_dial_stuff1[w]; } else { w -= 23; }
				if (w<10) { return ti_dial_number[w]; } else { w -= 10; }
				if (w<10) { return ti_dial_stuff2[w]; } else { w -= 10; }
				return ' ';
			}
		}
		break;
	*/
	}
	return 0;
}

void ti_dial_next_char(void)
{
	ti_dial_charlist_pos++;
	if (ti_dial_charlist_pos > ti_dial_max()) { ti_dial_charlist_pos = 0; }
}

void ti_dial_prev_char(void)
{
	ti_dial_charlist_pos--;
	if (ti_dial_charlist_pos < 0) { ti_dial_charlist_pos = ti_dial_max(); }
}

void ti_dial_reset(void)
{
	ti_dial_charlist_pos = 0;
}

int ti_dial_rotatechar(int a)
{
	ti_dial_ppchar = ti_dial_pchar;
	ti_dial_pchar = a;
	return a;
}

// KERIPO MOD
// No predict text - doesn't seem to work

/*
void ti_dial_predict_clear(void)
{
	int i;
	for (i=0; i<(64*64*5); i++) {
		((char *)ti_dial_predict)[i] = 0;
	}
}
*/

/*
void ti_dial_predict_load(const char * fn)
{
	FILE * fp;
	char buf[120];
	ti_dial_predict_clear();
	fp = fopen(fn, "r");
	if (fp) {
		while (fgets(buf, 120, fp)) {
			if (strlen(buf)>=7 && !(buf[0]=='#' && buf[1]=='#')) {
				strncpy(ti_dial_predict[C2N(buf[0])][C2N(buf[1])], &buf[2], 5);
			}
		}
		fclose(fp);
	}
}
*/

#define CW 10 /* width of the space for each char */

void ti_dial_draw(TWidget * wid, ttk_surface srf)
{
	int sc, n, i, j, ty;
	int m = ti_dial_max()+1;
	uc16 s[2] = {0,0};
	ttk_ap_fillrect(srf, ti_ap_getx(0), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	n = wid->w / CW;
	sc = ti_dial_charlist_pos - n/2;
	ty = 0;
	if (sc > (m-n) ) { sc = (m-n); }
	if (sc < 0) { sc = 0; }
	if (ti_dial_cursormode()) {
		ttk_text(srf, ttk_menufont, wid->x+(wid->w - ttk_text_width(ttk_menufont, ti_dial_cmstring))/2, wid->y+ty, ti_ap_get(1), ti_dial_cmstring);
	} else {
		for (i = sc, j = 0; ((i<m) && (j<n)); i++, j++)
		{
			s[0] = ti_dial_get_char(i);
			if (i == ti_dial_charlist_pos) {
				ttk_ap_fillrect(srf, ti_ap_getx(2), wid->x+j*CW, wid->y, wid->x+j*CW+CW, wid->y+wid->h);
				ttk_text_uc16(srf, ttk_menufont, wid->x+j*CW+(10-ttk_text_width_uc16(ttk_menufont, s))/2, wid->y+ty, ti_ap_get(3), s);
			} else {
				ttk_text_uc16(srf, ttk_menufont, wid->x+j*CW+(10-ttk_text_width_uc16(ttk_menufont, s))/2, wid->y+ty, ti_ap_get(1), s);
			}
		}
	}
}

int ti_dial_scroll(TWidget * wid, int dir)
{
	TTK_SCROLLMOD (dir, 4);
	
	if (dir<0) {
		if (ti_dial_cursormode()) {
			ttk_input_char(TTK_INPUT_LEFT);
		} else {
			ti_dial_prev_char();
			wid->dirty = 1;
		}
	} else {
		if (ti_dial_cursormode()) {
			ttk_input_char(TTK_INPUT_RIGHT);
		} else {
			ti_dial_next_char();
			wid->dirty = 1;
		}
	}
	return TTK_EV_CLICK;
}

int ti_dial_down(TWidget * wid, int btn)
{
	switch (btn) {
	case TTK_BUTTON_ACTION:
		if (ti_dial_cursormode() || ti_dial_charlist_pos == ti_dial_max()) {
			ti_dial_togglecursor();
			wid->dirty = 1;
		} else if (ti_dial_mode >= 2 && ti_dial_mode <= 8 && ti_dial_charlist_pos >= 32) {
			ti_dial_mode = (ti_dial_charlist_pos - 30);
			ti_dial_reset();
			wid->dirty = 1;
		} else {
			ttk_input_char(ti_dial_rotatechar(ti_dial_get_char(-1)));
			if (ti_dial_snapback) {
				ti_dial_reset();
				wid->dirty = 1;
			}
		}
		break;
	case TTK_BUTTON_PREVIOUS:
		ttk_input_char(TTK_INPUT_BKSP);
		break;
	case TTK_BUTTON_NEXT:
		ttk_input_char(ti_dial_rotatechar(32));
		break;
	case TTK_BUTTON_PLAY:
		ttk_input_char(ti_dial_rotatechar(TTK_INPUT_ENTER));
		break;
	case TTK_BUTTON_MENU:
		ttk_input_end();
		break;
	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

int ti_dial_stap(TWidget * wid, int p)
{
	int m;
	if (ti_dial_cursormode()) { return TTK_EV_UNUSED; }
	m = ti_dial_max()+1;
	ti_dial_charlist_pos = (p*m)/96;
	wid->dirty = 1;
	return TTK_EV_CLICK;
}

/*
	When calling ti_dial_create:
	The first parameter is the starting ti_dial_mode
	The second parameter is a boolean indicating if the thing should
		snap back to the beginning after input.
	The third parameter is a boolean indicating if staps should be
		caught and the position set to the position on the wheel.
*/

TWidget * ti_dial_create(int n, int s, int st)
{
	TWidget * wid = ti_create_tim_widget(-1, 0);
	wid->down = ti_dial_down;
	wid->scroll = ti_dial_scroll;
	wid->draw = ti_dial_draw;
	if (st) {
		wid->stap = ti_dial_stap;
	}
	ti_dial_mode = n;
	ti_dial_snapback = s;
	ti_dial_reset();
	ti_dial_ppchar = 0;
	ti_dial_pchar = 0;
	return wid;
}

TWidget * ti_dial_stcreate()
{
	return ti_dial_create(0, 0, 0);
}

TWidget * ti_dial_stncreate()
{
	return ti_dial_create(1, 0, 0);
}

TWidget * ti_dial_dtcreate()
{
	return ti_dial_create(0, 1, 0);
}

TWidget * ti_dial_dtncreate()
{
	return ti_dial_create(1, 1, 0);
}

TWidget * ti_dial_ftcreate()
{
	return ti_dial_create(2, 0, 1);
}

TWidget * ti_dial_ftncreate()
{
	return ti_dial_create(1, 0, 1);
}

// KERIPO MOD
// No predict text - doesn't seem to work
/*
TWidget * ti_dial_ptcreate()
{
	return ti_dial_create(10, 1, 0);
}


TWidget * ti_dial_ptncreate()
{
	return ti_dial_create(1, 1, 0);
}
*/

/*
void ti_dial_init()
{
	module = pz_register_module("tidial", 0);
	ti_register(ti_dial_stcreate, ti_dial_stncreate, N_("Scroll-Through"), 2);
	ti_register(ti_dial_dtcreate, ti_dial_dtncreate, N_("Scroll with Return"), 8);
	ti_register(ti_dial_ftcreate, ti_dial_ftncreate, N_("Scroll with Fixed Layout"), 20);
	ti_register(ti_dial_ptcreate, ti_dial_ptncreate, N_("Scroll with Prediction"), 21);
	ti_dial_predict_load(pz_module_get_datapath(module, "predict.txt"));
}

PZ_MOD_INIT(ti_dial_init)
*/
/////////////////////////////////
// podwrite module
/////////////////////////////////

#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* Dependency */
// KERIPO MOD
// Removing repeat definition
/*
typedef struct TiBuffer {
	char * text;
	int asize;
	int usize;
	int cpos;
	int idata[4];
	void * data;
	int (*callback)(TWidget *, char *);
} TiBuffer;
*/

/* PodWrite State */
static int podwrite_mode = 0;
static int podwrite_linecount = 0;
static int podwrite_screenlines = 0;
static int podwrite_scroll = 0;
static int podwrite_cursor_out_of_bounds = 0;
static char * podwrite_filename = 0;

static PzWindow * podwrite_win;
static TWidget  * podwrite_wid;
static TiBuffer * podwrite_buf;
static TWidget * podwrite_menu;
static ttk_menu_item podwrite_fbx;

/* Dependencies */
extern TWidget * ti_new_standard_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *));
extern TWidget * ti_new_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *),
	void (*draw)(TWidget *, ttk_surface), int (*input)(TWidget *, int), int numeric);
extern int ti_widget_input_ml(TWidget * wid, int ch);
extern void ti_multiline_text(ttk_surface srf, ttk_font fnt, int x, int y, int w, int h, ttk_color col, const char *t,
	int cursorpos, int scroll, int * lc, int * sl, int * cb);
extern int ti_widget_start(TWidget * wid);
extern int ti_set_buffer(TiBuffer * buf, char * s);
extern void ti_buffer_cmove(TiBuffer * buf, int m);
extern void ti_buffer_cset(TiBuffer * buf, int c);
extern ttk_color ti_ap_get(int i);
extern TApItem * ti_ap_getx(int i);

/* PodWrite Menu */

int podwrite_save_callback(TWidget * wid, char * fn)
{
	FILE * f;
	pz_close_window(wid->win);
	if (fn[0]) {
		f = fopen(fn, "wb");
		if (!f) {
			pz_error(_("Could not open file for saving."));
			return 0;
		}
		fwrite(podwrite_buf->text, 1, podwrite_buf->usize, f);
		fclose(f);
	}
	podwrite_mode = 0;
	pz_close_window(podwrite_menu->win);
	ti_widget_start(podwrite_wid);
	return 0;
}

PzWindow * podwrite_mh_save(ttk_menu_item * item)
{
	PzWindow * ret;
	TWidget * wid;
	ret = pz_new_window(_("Save to..."), PZ_WINDOW_NORMAL);
	// KERIPO MOD
	//wid = ti_new_standard_text_widget(10, 40, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, (podwrite_filename?podwrite_filename:"/Notes/"), podwrite_save_callback);
	wid = ti_new_standard_text_widget(10, 40, ret->w-20, 10+ttk_text_height(ttk_textfont), 0, (podwrite_filename?podwrite_filename:"/mnt/Notes/"), podwrite_save_callback);

	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	return ret;
}

TWindow * podwrite_mh_return(ttk_menu_item * item)
{
	podwrite_mode = 0;
	pz_close_window(podwrite_menu->win);
	ti_widget_start(podwrite_wid);
	return TTK_MENU_DONOTHING;
}

TWindow * podwrite_mh_cursor(ttk_menu_item * item)
{
	podwrite_mode = 1;
	return TTK_MENU_UPONE;
}

TWindow * podwrite_mh_scroll(ttk_menu_item * item)
{
	podwrite_mode = 2;
	return TTK_MENU_UPONE;
}

TWindow * podwrite_mh_quit(ttk_menu_item * item)
{
	if (podwrite_filename) {
		free(podwrite_filename);
		podwrite_filename = 0;
	}
	pz_close_window(podwrite_menu->win);
	pz_close_window(podwrite_win);
	return TTK_MENU_DONOTHING;
}

TWindow * podwrite_mh_input(ttk_menu_item * item)
{
	ttk_menu_item * timenu = pz_get_menu_item("/Settings/Text Input");
	if (timenu) {
		return pz_mh_sub(timenu);
	} else {
		return TTK_MENU_DONOTHING;
	}
}

TWindow * podwrite_mh_clear(ttk_menu_item * item)
{
	int cl = pz_dialog(_("Clear"), _("Clear all text?"), 2, 0, _("No"), _("Yes"));
	if (cl == 1) {
		ti_set_buffer(podwrite_buf, "");
		podwrite_wid->dirty = 1;
		return podwrite_mh_return(item);
	}
	return TTK_MENU_DONOTHING;
}

ttk_menu_item * podwrite_new_menu_item(const char * name, TWindow * (*func)(ttk_menu_item *), int flags)
{
	ttk_menu_item * mi = (ttk_menu_item *)calloc(1, sizeof(ttk_menu_item));
	if (mi) {
		mi->name = name;
		mi->makesub = func;
		mi->flags = flags;
	}
	return mi;
}

PzWindow * new_podwrite_menu_window()
{
	PzWindow * w;
	podwrite_menu = ttk_new_menu_widget(0, ttk_menufont, ttk_screen->w, ttk_screen->h-ttk_screen->wy);
	if (!podwrite_menu) return 0;
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Return to PodWrite"), podwrite_mh_return, 0));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Move Cursor"), podwrite_mh_cursor, 0));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Scroll"), podwrite_mh_scroll, 0));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Clear All Text"), podwrite_mh_clear, 0));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Save..."), podwrite_mh_save, 0));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Change Input Method"), podwrite_mh_input, TTK_MENU_ICON_SUB));
	ttk_menu_append(podwrite_menu, podwrite_new_menu_item(N_("Quit PodWrite"), podwrite_mh_quit, 0));
	w = pz_new_menu_window(podwrite_menu);
	if (w) w->title = _("PodWrite");
	return w;
}

/* PodWrite Event Handling */

int podwrite_callback(TWidget * wid, char * txt)
{
	PzWindow * w = new_podwrite_menu_window();
	if (w) pz_show_window(w);
	return 0;
}

int podwrite_check_bounds(void)
{
	int s = podwrite_scroll;
	if (podwrite_linecount < podwrite_screenlines) {
		if (podwrite_scroll != 0) { podwrite_scroll = 0; }
	} else {
		if (podwrite_scroll > (podwrite_linecount-podwrite_screenlines)) { podwrite_scroll = podwrite_linecount-podwrite_screenlines; }
		if (podwrite_cursor_out_of_bounds < 0) {
			podwrite_scroll -= 2;
			if (podwrite_scroll < 0) { podwrite_scroll = 0; }
		} else if (podwrite_cursor_out_of_bounds > 0) {
			podwrite_scroll += 2;
			if (podwrite_scroll > (podwrite_linecount-podwrite_screenlines)) { podwrite_scroll = podwrite_linecount-podwrite_screenlines; }
			if (podwrite_scroll < 0) { podwrite_scroll = 0; }
		}
	}
	return (podwrite_scroll == s)?0:1;
}

int podwrite_widget_input(TWidget * wid, int ch)
{
	int ret;
	if ( (ret = ti_widget_input_ml(wid,ch)) != TTK_EV_UNUSED ) {
		if (podwrite_check_bounds()) {
			wid->dirty = 1;
		}
	}
	return ret;
}

void podwrite_widget_draw(TWidget * wid, ttk_surface srf)
{
	int h = wid->h - (((TiBuffer *)wid->data)->idata[2]) - 1;
	
	ttk_ap_fillrect(srf, ti_ap_getx(0), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	ti_multiline_text(srf, ttk_textfont, wid->x+5, wid->y+5, wid->w-15, wid->h-10-(((TiBuffer *)wid->data)->idata[2]),
		ti_ap_get(1), ((TiBuffer *)wid->data)->text, ((TiBuffer *)wid->data)->cpos,
		((podwrite_linecount > podwrite_screenlines)?podwrite_scroll:0),
		&podwrite_linecount, &podwrite_screenlines, &podwrite_cursor_out_of_bounds);
	
	if (podwrite_linecount > podwrite_screenlines) {
	ttk_ap_fillrect (srf, ttk_ap_get ("scroll.bg"), wid->x + wid->w - 10,
			 wid->y + ttk_ap_getx ("header.line") -> spacing,
			 wid->x + wid->w, wid->y + h);
	ttk_ap_rect (srf, ttk_ap_get ("scroll.box"), wid->x + wid->w - 10,
		     wid->y + ttk_ap_getx ("header.line") -> spacing,
		     wid->x + wid->w, wid->y + h);
	ttk_ap_fillrect (srf, ttk_ap_get ("scroll.bar"), wid->x + wid->w - 10,
			 wid->y + ttk_ap_getx ("header.line") -> spacing + ((podwrite_scroll) * (h-2) / podwrite_linecount),
			 wid->x + wid->w,
			 wid->y - ttk_ap_getx ("header.line") -> spacing + ((podwrite_scroll + podwrite_screenlines) * (h-2) / podwrite_linecount) );	
    }
}

int podwrite_widget_scroll(TWidget * wid, int dir)
{
	if (podwrite_mode == 0) { return ti_widget_start(wid); }
	
	TTK_SCROLLMOD (dir,4);
	switch (podwrite_mode) {
	case 1:
		ti_buffer_cmove((TiBuffer *)wid->data, dir);
		podwrite_check_bounds();
		wid->dirty = 1;
		break;
	case 2:
		podwrite_scroll += dir;
		if (podwrite_scroll < 0) podwrite_scroll = 0;
		if (podwrite_scroll > (podwrite_linecount - podwrite_screenlines)) podwrite_scroll = (podwrite_linecount - podwrite_screenlines);
		wid->dirty = 1;
		break;
	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

int podwrite_widget_button(TWidget * wid, int btn, int t)
{
	// KERIPO MOD
	// Replaced "new" PodWrite code with "old" PodRead code to fix nano menu bug
	
 	//if (btn == TTK_BUTTON_MENU ||
 	//	btn == TTK_BUTTON_PLAY ||
	if (btn == TTK_BUTTON_PLAY ||
 		btn == TTK_BUTTON_ACTION ||
 		podwrite_mode == 0) { return ti_widget_start(wid); }
	
	if (btn == TTK_BUTTON_MENU){
		PzWindow * w = new_podwrite_menu_window();
		if (w) pz_show_window(w);
		wid->dirty = 1;
		return TTK_EV_CLICK;
	}
	switch (podwrite_mode) {
	case 1:
		if (btn == TTK_BUTTON_PREVIOUS) {
			((TiBuffer *)wid->data)->cpos = 0;
			podwrite_scroll = 0;
			wid->dirty = 1;
		} else if (btn == TTK_BUTTON_NEXT) {
			((TiBuffer *)wid->data)->cpos = ((TiBuffer *)wid->data)->usize;
			podwrite_scroll = (podwrite_linecount - podwrite_screenlines);
			wid->dirty = 1;
		} else {
			return TTK_EV_UNUSED;
		}
		break;
	case 2:
		if (btn == TTK_BUTTON_PREVIOUS) {
			podwrite_scroll = 0;
			wid->dirty = 1;
		} else if (btn == TTK_BUTTON_NEXT) {
			podwrite_scroll = (podwrite_linecount - podwrite_screenlines);
			wid->dirty = 1;
		} else {
			return TTK_EV_UNUSED;
		}
		break;
	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

/* PodWrite Initialization */

PzWindow * new_podwrite_window_with_text(char * dt)
{
	PzWindow * ret;
	TWidget * wid;
	
	/* set podwrite's state */
	podwrite_mode = 0;
	podwrite_linecount = 0;
	podwrite_screenlines = 0;
	podwrite_scroll = 0;
	podwrite_cursor_out_of_bounds = 0;
	podwrite_filename = 0;
	
	/* create the window, widget, and buffer */
	podwrite_win = ret = pz_new_window(_("PodWrite"), PZ_WINDOW_NORMAL);
	podwrite_wid = wid = ti_new_text_widget(0, 0, ret->w, ret->h, 1, dt, podwrite_callback, podwrite_widget_draw, podwrite_widget_input, 0);
	podwrite_buf = (TiBuffer *)wid->data;
	wid->scroll = podwrite_widget_scroll;
	wid->button = podwrite_widget_button;
	
	/* move cursor to beginning */
	ti_buffer_cset(podwrite_buf, 0);
	
	/* set up window */
	ttk_add_widget(ret, wid);
	ret = pz_finish_window(ret);
	ti_widget_start(wid);
	
	/* return */
	return ret;
}

PzWindow * new_podwrite_window_with_file(char * fn)
{
	PzWindow * w;
	FILE * f;
	char * buf;
	int fl;
	
	f = fopen(fn, "rb");
	if (!f) return 0;
	fseek(f, 0, SEEK_END);
	fl = ftell(f);
	if (fl<0) {
		fclose(f);
		return 0;
	}
	fseek(f, 0, SEEK_SET);
	buf = (char *)malloc(fl+1);
	if (!buf) {
		fclose(f);
		return 0;
	}
	fl = fread(buf, 1, fl, f);
	buf[fl]=0;
	fclose(f);
	
	w = new_podwrite_window_with_text(buf);
	podwrite_filename = strdup(fn);
	free(buf);
	return w;
}

PzWindow * new_podwrite_window()
{
	return new_podwrite_window_with_text("");
}

/* Module & File Browser Extension Initialization */

int podwrite_openable(const char * filename)
{
	struct stat st;
	stat(filename, &st);
	return !S_ISDIR(st.st_mode);
}

PzWindow * podwrite_open_handler(ttk_menu_item * item)
{
	return new_podwrite_window_with_file(item->data);
}

/*
void podwrite_mod_init(void)
{
	// This menu path is canonically "/Extras/Productivity/PodWrite",
	// but it may be different in the svn repository depending on the
	// current structure of podzilla2's Extras menu.                 
	pz_menu_add_action("/Extras/Utilities/PodWrite", new_podwrite_window);
	
	podwrite_fbx.name = N_("Open with PodWrite");
	podwrite_fbx.makesub = podwrite_open_handler;
	pz_browser_add_action (podwrite_openable, &podwrite_fbx);
}

PZ_MOD_INIT(podwrite_mod_init)
*/


/////////////////////////////////
// Final Initiation
/////////////////////////////////

void init_text_input_pack()
{
	ti_module = pz_register_module("TextInput-Pack", 0);

// Note: "Scroll" is selected by default instead of "Serial", which is removed
//void ti_init()
//{
	int i;
	//ti_module = pz_register_module("textinput", 0);
	ti_conf = pz_load_config(pz_module_get_cfgpath(ti_module, "textinput.conf"));
	for (i=0; i<64; i++) {
		ti_tim_creators[i]=0;
		ti_tim_ncreators[i]=0;
	}
	if (pz_get_setting(ti_conf, TI_SETTING_SEL_TIM)) {
		ti_selected_tim = ti_saved_tim = pz_get_int_setting(ti_conf, TI_SETTING_SEL_TIM);
	} else {
		ti_selected_tim = ti_saved_tim = TI_DEFAULT_TIM;
	}
#ifdef IPOD
	if (!ti_selected_tim) {
		//pz_warning (_("Serial text input is selected. Make sure a keyboard is attached or select a different input method."));
		
		ti_selected_tim=2;
	}
#endif
	//ti_register(ti_serial_create, ti_serial_create, N_("Serial"), 0);
//}

// Note: No demos to conserve space
/*
void init_textinput_demos()
{
	module = pz_register_module ("tiwidgets", 0);
	pz_menu_add_action ("/Settings/Text Input/Text Input Demo/Standard", new_standard_text_demo_window);
	pz_menu_add_action ("/Settings/Text Input/Text Input Demo/Numeric", new_numeric_text_demo_window);
	pz_menu_add_action ("/Settings/Text Input/Text Input Demo/Password", new_password_text_demo_window);
	pz_menu_add_action ("/Settings/Text Input/Text Input Demo/Multiline", new_multiline_text_demo_window);
}
*/

// Note: Copy/Paste functions removed - don't seem to work ; /
// Run function has also been removed
//void init_ti_extensions()
//{
	//module = pz_register_module ("tixtensions", 0);
	//tix_file_clipboard = 0;
	//tix_file_clipboard_name = 0;
	//tix_file_clipboard_copy = 0;
	
	//pz_menu_add_action ("/Run...", new_run_window);
	
	tix_rename_menuitem.name = N_("Rename");
	tix_rename_menuitem.makesub = new_rename_window;
	pz_browser_add_action (tix_true, &tix_rename_menuitem);
	
	tix_mkdir_menuitem.name = N_("Make Directory");
	tix_mkdir_menuitem.makesub = new_mkdir_window;
	pz_browser_add_action (tix_true, &tix_mkdir_menuitem);
	
	/*
	tix_cut_menuitem.name = N_("Cut");
	tix_cut_menuitem.makesub = tix_cut_handler;
	pz_browser_add_action (tix_true, &tix_cut_menuitem);
	
	tix_copy_menuitem.name = N_("Copy");
	tix_copy_menuitem.makesub = tix_copy_handler;
	pz_browser_add_action (tix_true, &tix_copy_menuitem);
	
	tix_paste_menuitem.name = N_("Paste");
	tix_paste_menuitem.makesub = tix_paste_handler;
	tix_paste_menuitem.visible = tix_paste_visible;
	pz_browser_add_action (tix_true, &tix_paste_menuitem);
	
	tix_pastelink_menuitem.name = N_("Paste Link");
	tix_pastelink_menuitem.makesub = tix_pastelink_handler;
	tix_pastelink_menuitem.visible = tix_paste_visible;
	pz_browser_add_action (tix_true, &tix_pastelink_menuitem);
	*/
//}

//void ti_dial_init()
//{
	//module = pz_register_module("tidial", 0);
	// Note: shortened for nanos, no predict text (doesn't work)
	//ti_register(ti_dial_stcreate, ti_dial_stncreate, N_("Scroll-Through"), 2);
	ti_register(ti_dial_stcreate, ti_dial_stncreate, N_("Scroll"), 2);
	//ti_register(ti_dial_dtcreate, ti_dial_dtncreate, N_("Scroll with Return"), 8);
	ti_register(ti_dial_dtcreate, ti_dial_dtncreate, N_("Scroll (Return)"), 8);
	//ti_register(ti_dial_ftcreate, ti_dial_ftncreate, N_("Scroll with Fixed Layout"), 20);
	ti_register(ti_dial_ftcreate, ti_dial_ftncreate, N_("Scroll (Fixed)"), 20);
	//ti_register(ti_dial_ptcreate, ti_dial_ptncreate, N_("Scroll with Prediction"), 21);
	//ti_register(ti_dial_ptcreate, ti_dial_ptncreate, N_("Scroll (Prediction)"), 21);
	//ti_dial_predict_load(pz_module_get_datapath(module, "predict.txt"));
//}

//void podwrite_mod_init(void)
//{
	// This menu path is canonically "/Extras/Productivity/PodWrite",
	// but it may be different in the svn repository depending on the
	// current structure of podzilla2's Extras menu.                 
	//pz_menu_add_action("/Extras/Utilities/PodWrite", new_podwrite_window);
	pz_menu_add_action("/Tools/PodWrite", new_podwrite_window);
	
	podwrite_fbx.name = N_("Open with PodWrite");
	podwrite_fbx.makesub = podwrite_open_handler;
	pz_browser_add_action (podwrite_openable, &podwrite_fbx);
//}

}

PZ_MOD_INIT(init_text_input_pack)
