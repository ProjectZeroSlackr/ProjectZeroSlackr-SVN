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

#include "pz.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define NUM_FACTORS 25
#define NUM_FACTORZ 255
static unsigned long int factor_factors[NUM_FACTORS] = {0};
static unsigned long int factor_factorz[NUM_FACTORZ] = {0};
static int factor_exited;
static int factor_tiwh;
static TWidget * factor_widget;
static TWidget * factor_twidget;

/* Dependencies */
typedef struct TiBuffer TiBuffer;
extern TWidget * ti_new_numeric_text_widget(int x, int y, int w, int h, int absheight, char * dt, int (*callback)(TWidget *, char *));
extern int ti_widget_input_n(TWidget * wid, int ch);
extern void ti_widget_draw(TWidget * wid, ttk_surface srf);
extern int ti_widget_start(TWidget * wid);
extern char * ti_get_buffer(TiBuffer * buf);

/* Factorization Routines */

void factor(unsigned long int * factors, unsigned long int num, int m)
{
	/*
		/!\ 'factors' must be malloced before calling this function
	*/
	unsigned long int i = 2;
	unsigned long int n = num;
	int p = 1;
	factors[0]=0;
	while (n != 1) {
		if (!(n % i)) {
			factors[0]++;
			factors[p++]=i;
			factors[p]=0;
			while (!(n % i)) {
				n /= i;
				factors[p]++;
			}
			p++;
			if (p>=m) { break; }
		}
		i++;
	}
}

void factor_print(ttk_surface srf, unsigned long int * factors, int x, int y)
{
	unsigned long int i;
	int myx = x;
	int first = 1;
	char fn[13];
	char fp[13];
	for (i=0; i<factors[0]; i++) {
		if (factors[i+i+1]) {
			snprintf(fn, 13, "%u", (unsigned int)factors[i+i+1]);
			snprintf(fp, 13, "%u", (unsigned int)factors[i+i+2]);
			if (!first) {
				ttk_text_lat1(srf, ttk_textfont, myx, y, ttk_ap_getx("window.fg")->color, "\xB7");
				myx += ttk_text_width_lat1(ttk_textfont, "\xB7");
			} else {
				first = 0;
			}
			ttk_text_lat1(srf, ttk_textfont, myx, y, ttk_ap_getx("window.fg")->color, fn);
			myx += ttk_text_width_lat1(ttk_textfont, fn);
			if (factors[i+i+2] != 1) {
				ttk_text_lat1(srf, ttk_textfont, myx, y-ttk_text_height(ttk_textfont)/2, ttk_ap_getx("window.fg")->color, fp);
				myx += ttk_text_width_lat1(ttk_textfont, fp);
			}
		}
	}
}

void factor2(unsigned long int * factors, unsigned long int num, int m)
{
	/*
		/!\ 'factors' must be malloced before calling this function
	*/
	unsigned long int i;
	unsigned long int n = num;
	unsigned long int s = (unsigned long int)sqrt((float)n);
	int p = 1;
	factors[0]=0;
	for (i=1; i<=s; i++) {
		if (!(n % i)) {
			factors[0]++;
			factors[p++]=i;
			factors[p++]=(n/i);
			if (p>=m) { break; }
		}
	}
}

void factor2_print(ttk_surface srf, unsigned long int * factors, int x, int y, int w, int h)
{
	unsigned long int i;
	int myx = x;
	int myy = y;
	int Maxw = 0;
	int maxw = 0;
	int tw;
	int th = ttk_text_height(ttk_textfont);
	char fz[30];
	if (h < th) return;
	for (i=0; i<factors[0]; i++) {
		if (factors[i+i+1] && factors[i+i+2]) {
			snprintf(fz, 30, "%u\xB7%u", (unsigned int)factors[i+i+1], (unsigned int)factors[i+i+2]);
			ttk_text_lat1(srf, ttk_textfont, myx, myy, ttk_ap_getx("window.fg")->color, fz);
			tw = ttk_text_width_lat1(ttk_textfont, fz);
			if (tw>Maxw) Maxw=tw;
			if (tw>maxw) maxw=tw;
			myy += th;
			if ( (myy+th) > (y+h) ) {
				myy = y;
				myx += (maxw + 10);
				maxw = 0;
				if ( (myx+Maxw) > (x+w) ) {
					return;
				}
			}
		}
	}
}

/* Factorization Program */

void factor_draw(TWidget * wid, ttk_surface srf)
{
	int y;
	
	ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	
	/* only the topmost widget gets drawn, apparently, so here we do both */
	ti_widget_draw(factor_twidget, srf);
	
	ttk_text(srf, ttk_textfont, 10, 5, ttk_ap_getx("window.fg")->color, _("Number to factor:"));
	
	y = factor_twidget->y + factor_tiwh + 5;
	factor_print(srf, factor_factors, 10, y+ttk_text_height(ttk_textfont)/2+1);
	
	y += ttk_text_height(ttk_textfont) + ttk_text_height(ttk_textfont)/2 + 5;
	factor2_print(srf, factor_factorz, 10, y, wid->w-20, wid->h-y-5);
}

int factor_input(TWidget * wid, int ch)
{
	int ret;
	char * stuff;
	unsigned long int i;
	factor_widget->dirty = 0;
	ret = ti_widget_input_n(wid, ch);
	if (!factor_exited) {
		if (ret != TTK_EV_UNUSED) {
			stuff = ti_get_buffer((TiBuffer *)wid->data);
			if (stuff[0]) {
				sscanf(stuff, "%lu", &i);
				if (i<2) {
					factor_factors[0]=0;
					factor_factorz[0]=0;
				} else {
					factor(factor_factors, i, NUM_FACTORS);
					factor2(factor_factorz, i, NUM_FACTORZ);
				}
			} else {
				factor_factors[0]=0;
				factor_factorz[0]=0;
			}
			factor_widget->dirty = 1;
		}
	}
	return ret;
}

int factor_input_callback(TWidget * wid, char * txt)
{
	factor_exited = 1;
	pz_close_window(wid->win);
	return 0;
}

TWindow * new_factor_window(void)
{
	TWindow * ret;
	TWidget * wid;
	TWidget * twid;
	
	factor_exited = 0;
	factor_factors[0]=0;
	factor_factorz[0]=0;
	
	ret = ttk_new_window(_("Factor"));
	
	factor_widget = wid = ttk_new_widget(0,0);
	wid->w = ttk_screen->w;
	wid->h = ttk_screen->h-ttk_screen->wy;
	wid->draw = factor_draw;
	
	factor_twidget = twid = ti_new_numeric_text_widget(10, 10+ttk_text_height(ttk_textfont), ret->w-20, 10+ttk_text_height(ttk_textfont), 0, "", factor_input_callback);
	twid->input = factor_input;
	twid->draw = factor_draw;
	
	ttk_add_widget(ret, twid);
	ttk_add_widget(ret, wid);
	ttk_show_window(ret);
	ti_widget_start(twid);
	factor_tiwh = twid->h + ret->input->h;
	return ret;
}


