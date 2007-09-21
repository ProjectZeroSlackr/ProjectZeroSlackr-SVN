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
#include <stdio.h>
#include <stdlib.h>
#include "pz.h"
#include "appearance.h"

GR_GC_ID factor_gc;
GR_WINDOW_ID factor_wid;
unsigned long int factor_number = 2;
int factor_textascent;
int factor_textheight;

void factor(unsigned long int * factors, unsigned long int num)
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
			factors[p]=i;
			p++;
			factors[p]=0;
			while (!(n % i)) {
				n /= i;
				factors[p]++;
			}
			p++;
		}
		i++;
	}
}

void factor_print(unsigned long int * factors, unsigned long int num, GR_WINDOW_ID wid, GR_GC_ID gc, int x, int y)
{
	unsigned long int i;
	int myx = x;
	int tw, th;
	int tb = 0;
	char fn[13];
	char fp[13];
	for (i=0; i<factors[0]; i++) {
		if (factors[i+i+1]) {
			snprintf(fn, 13, "%u", (unsigned int)factors[i+i+1]);
			snprintf(fp, 13, "%u", (unsigned int)factors[i+i+2]);
			if (tb) {
				GrText(wid, gc, myx, y-3, ".", -1, GR_TFASCII);
				GrGetGCTextSize(gc, ".", -1, GR_TFASCII, &tw, &th, &tb);
				myx += tw;
			}
			GrText(wid, gc, myx, y, fn, -1, GR_TFASCII);
			GrGetGCTextSize(gc, fn, -1, GR_TFASCII, &tw, &th, &tb);
			myx += tw;
			if (factors[i+i+2] != 1) {
				GrText(wid, gc, myx, y-tb/2, fp, -1, GR_TFASCII);
				GrGetGCTextSize(gc, fp, -1, GR_TFASCII, &tw, &th, &tb);
				myx += tw;
			}
		}
	}
}

void factor_action(void)
{
	unsigned long int * factors;
	int top = 10+(factor_textheight*2);
	GrSetGCBackground(factor_gc, appearance_get_color(CS_BG));
	GrSetGCForeground(factor_gc, appearance_get_color(CS_BG));
	GrFillRect(factor_wid, factor_gc, 0, top, screen_info.cols, screen_info.rows-60);
	GrSetGCForeground(factor_gc, appearance_get_color(CS_FG));
	if ((  factors = (unsigned long int *)malloc(sizeof(unsigned long int)*25)  ) == NULL) {
		GrText(factor_wid, factor_gc, 10, top+factor_textascent+factor_textheight, _("Not enough memory to factorize."), -1, GR_TFASCII);
	} else {
		factor(factors, factor_number);
		factor_print(factors, factor_number, factor_wid, factor_gc, 10, top+factor_textascent+factor_textheight);
		free(factors);
	}
}

void factor_draw(void)
{
	char n[13];
	GrSetGCBackground(factor_gc, appearance_get_color(CS_BG));
	GrSetGCForeground(factor_gc, appearance_get_color(CS_BG));
	GrFillRect(factor_wid, factor_gc, 0, 0, screen_info.cols, screen_info.rows);
	GrSetGCForeground(factor_gc, appearance_get_color(CS_FG));
	snprintf(n, 13, "%u", (unsigned int)factor_number);
	GrText(factor_wid, factor_gc, 10, 10+factor_textascent, _("Number to factor:"), -1, GR_TFASCII);
	GrText(factor_wid, factor_gc, 10, 10+factor_textascent+factor_textheight, n, -1, GR_TFASCII);
}

void factor_draw_all(void)
{
	pz_draw_header(_("Factor"));
	factor_draw();
}

int factor_event(GR_EVENT * event)
{
	if (event->type == GR_EVENT_TYPE_KEY_DOWN) {
		switch (event->keystroke.ch) {
		case 'l':
			factor_number--;
			if (factor_number < 2) { factor_number = 2; }
			factor_draw();
			break;
		case 'r':
			factor_number++;
			if (factor_number > 0xFFFFFFF0) { factor_number = 0xFFFFFFF0; }
			factor_draw();
			break;
		case '\n':
		case '\r':
			factor_action();
			break;
		case 'm':
			pz_close_window(factor_wid);
			/* GrDestroyWindow(factor_bufwid); */
			GrDestroyGC(factor_gc);
			break;
		default:
			return 0;
			break;
		}
		return 1;
	} else {
		return 0;
	}
}

void new_factor_window(void)
{
	int tw;
	factor_gc = pz_get_gc(1);
	GrGetGCTextSize(factor_gc, "x", -1, GR_TFASCII, &tw, &factor_textheight, &factor_textascent);
	GrSetGCUseBackground(factor_gc, GR_TRUE);
	GrSetGCForeground(factor_gc, appearance_get_color(CS_FG));
	GrSetGCBackground(factor_gc, appearance_get_color(CS_BG));
	factor_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), factor_draw_all, factor_event);
	/* factor_bufwid = GrNewPixmap(screen_info.cols, screen_info.rows - HEADER_TOPLINE, NULL); */
	GrSelectEvents(factor_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP| GR_EVENT_MASK_KEY_DOWN);
	GrMapWindow(factor_wid);
}
