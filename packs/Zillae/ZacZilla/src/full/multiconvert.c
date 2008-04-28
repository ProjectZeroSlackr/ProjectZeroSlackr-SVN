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
#include "textinput.h"

typedef struct multiconvert_unit_ {
	char * name;
	double value;
	char * svalue;
	double preadd;
	double multiplier;
	double postadd;
} multiconvert_unit;

typedef struct multiconvert_unit_type_ {
	char *              name;
	int                 invert;
	int                 num_units;
	multiconvert_unit * units;
} multiconvert_unit_type;

extern void new_message_window(char * message);

// KERIPO MOD
//const char * multiconvert_file = "/etc/MultiConvert.tabl";
const char * multiconvert_file = "/opt/Zillae/ZacZilla/Data/MultiConvert.tabl"; /* feel free to change this to whatever you want */

int                      multiconvert_num_unit_types = 0;
multiconvert_unit_type * multiconvert_unit_types;
int                      multiconvert_sel = -1;
int                      multiconvert_start_row = 0;
int                      multiconvert_num_rows = 1;
int                      multiconvert_curr_type = 0;

GR_WINDOW_ID             multiconvert_wid;
GR_WINDOW_ID             multiconvert_bufwid;
GR_GC_ID                 multiconvert_gc;
int                      multiconvert_push_u;

double multiconvert(double n, double spra, double sm, double spoa,
                    double dpra, double dm, double dpoa, int inv)
{
	double i;
	if (inv != 0) {
		i = ((n+spra)/sm)+spoa;
		return ((i-dpoa)*dm)-dpra;
	} else {
		i = ((n+spra)*sm)+spoa;
		return ((i-dpoa)/dm)-dpra;
	}
	return 0;
}

void multiconvert_calculate(int ut, int u)
{
	double n1, pa1, m1, po1, n2, pa2, m2, po2;
	char sn2[13]; /* was "dsr forever." */
	int i;
	n1 = multiconvert_unit_types[ut].units[u].value;
	pa1 = multiconvert_unit_types[ut].units[u].preadd;
	m1 = multiconvert_unit_types[ut].units[u].multiplier;
	po1 = multiconvert_unit_types[ut].units[u].postadd;
	for (i=0; i<multiconvert_unit_types[ut].num_units; i++) {
		pa2 = multiconvert_unit_types[ut].units[i].preadd;
		m2 = multiconvert_unit_types[ut].units[i].multiplier;
		po2 = multiconvert_unit_types[ut].units[i].postadd;
		n2 = multiconvert(n1, pa1, m1, po1, pa2, m2, po2, multiconvert_unit_types[ut].invert);
		snprintf(sn2, 13, "%g", n2);
		multiconvert_unit_types[ut].units[i].value = n2;
		multiconvert_unit_types[ut].units[i].svalue = strdup(sn2);
	}
}

int multiconvert_add_unit_type(char * name, int invert)
{
//	if (multiconvert_num_unit_types >= 24) { return 0; }
	if (multiconvert_num_unit_types < 1) {
		if (( multiconvert_unit_types = (multiconvert_unit_type *)malloc(sizeof(multiconvert_unit_type)) ) == NULL ) { return 1; }
	} else {
		if (( multiconvert_unit_types = (multiconvert_unit_type *)realloc(multiconvert_unit_types, sizeof(multiconvert_unit_type) * (multiconvert_num_unit_types+1)) ) == NULL ) { return 1; }
	}
	multiconvert_unit_types[multiconvert_num_unit_types].name = name;
	multiconvert_unit_types[multiconvert_num_unit_types].invert = invert;
	multiconvert_unit_types[multiconvert_num_unit_types].num_units = 0;
	multiconvert_unit_types[multiconvert_num_unit_types].units = NULL;
	multiconvert_num_unit_types++;
	return 0;
}

int multiconvert_add_unit(int ut, char * name, double pa, double m, double po)
{
//	if (multiconvert_unit_types[ut].num_units >= 64) { return 0; }
	if (multiconvert_unit_types[ut].num_units < 1) {
		if (( multiconvert_unit_types[ut].units = (multiconvert_unit *)malloc(sizeof(multiconvert_unit)) ) == NULL ) { return 1; }
	} else {
		if (( multiconvert_unit_types[ut].units = (multiconvert_unit *)realloc(multiconvert_unit_types[ut].units, sizeof(multiconvert_unit) * (multiconvert_unit_types[ut].num_units+1)) ) == NULL ) { return 1; }
	}
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].name = name;
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].value = 0;
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].svalue = "0";
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].preadd = pa;
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].multiplier = m;
	multiconvert_unit_types[ut].units[multiconvert_unit_types[ut].num_units].postadd = po;
	multiconvert_unit_types[ut].num_units++;
	return 0;
}

int multiconvert_init(void)
{
	FILE * fp;
	char currline[128];
	char * sof; /* start of field */
	char * fpos; /* field position */
	char * currfield;
	double pa, m, po;
	multiconvert_num_unit_types = 0;
	multiconvert_start_row = 0;
	multiconvert_sel = -1;
	if ( (fp = fopen(multiconvert_file, "r")) == NULL ) {
		return 1;
	}
	clearerr(fp);
	while ( (fgets(currline, 120, fp) != NULL) && !feof(fp) ) {
		if (currline[strlen(currline)-1] == 10) { currline[strlen(currline)-1] = 0; }
		if ((currline[0] != '#') && (currline[0] != 0)) {
			if ((currline[0] == '/') && (currline[1] == '/')) {
				if (currline[2] == '~') {
					if ( multiconvert_add_unit_type( strdup(currline + 3), 1 ) != 0 ) { return 1; }
				} else {
					if ( multiconvert_add_unit_type( strdup(currline + 2), 0 ) != 0 ) { return 1; }
				}
			} else if (currline[0] != '/') {
				fpos = currline;
				sof = fpos;
				while (((*fpos) != ',') && ((*fpos) != 0)) { fpos++; }
				if (( currfield = (char *)malloc((fpos - sof + 1) * sizeof(char)) ) == NULL) { return 1; }
				memcpy(currfield, sof, fpos-sof);
				currfield[fpos-sof] = 0;
				pa = atof(currfield);
				free(currfield);
				fpos++;
				sof = fpos;
				while (((*fpos) != ',') && ((*fpos) != 0)) { fpos++; }
				if (( currfield = (char *)malloc((fpos - sof + 1) * sizeof(char)) ) == NULL) { return 1; }
				memcpy(currfield, sof, fpos-sof);
				currfield[fpos-sof] = 0;
				m = atof(currfield);
				free(currfield);
				fpos++;
				sof = fpos;
				while (((*fpos) != ',') && ((*fpos) != 0)) { fpos++; }
				if (( currfield = (char *)malloc((fpos - sof + 1) * sizeof(char)) ) == NULL) { return 1; }
				memcpy(currfield, sof, fpos-sof);
				currfield[fpos-sof] = 0;
				po = atof(currfield);
				free(currfield);
				fpos++;
				sof = fpos;
				if ( multiconvert_add_unit(multiconvert_num_unit_types-1, strdup(sof), pa, m, po) != 0 ) { return 1; }
			}
		}
	}
	fclose(fp);
	return 0;
}

void multiconvert_free(void)
{
	int ut;
	for (ut = multiconvert_num_unit_types-1; ut >= 0; ut--) {
		free(multiconvert_unit_types[ut].units);
	}
	free(multiconvert_unit_types);
	multiconvert_num_unit_types = 0;
}

void multiconvert_draw_scrollbar(GR_WINDOW_ID wid, GR_GC_ID gc, int y, int w, int h, int screen_items, int num_items, int sel)
{
	/* ripped from mlist.c with minor changes */
	int per = screen_items * 100 / num_items;
	int height = (h-2) * (per < 3 ? 3 : per) / 100;
	int y_top = ((((h-3) - height) * 100) * sel / (num_items-1)) / 100 + 2;
	int y_bottom = y_top + height;
	/* only draw if appropriate */
	if(screen_items >= num_items) { return; }
	/* draw the containing box */
	GrSetGCForeground(gc, appearance_get_color(CS_SCRLBDR));
	GrRect(wid, gc, w - 8, y, 8, h - 1);
	/* erase the scrollbar */
	GrSetGCForeground(gc, appearance_get_color(CS_SCRLCTNR));
	GrFillRect(wid, gc, w-7, y+1, 6, y_top - (y+1));
	GrFillRect(wid, gc, w-7, y_bottom, 6, (h-3) - y_bottom);
	/* draw the bar */
	GrSetGCForeground(gc, appearance_get_color(CS_SCRLKNOB));
	GrFillRect(wid, gc, w-7, y_top, 6, height);
	/* return to GR_RGB(0,0,0) */
	GrSetGCForeground(gc, GR_RGB(0,0,0));
}

void multiconvert_draw(void)
{
	GR_SIZE width, height, base;
	int midpoint, numrows, startrow, maxrow, i, j;
	multiconvert_unit u;
	pz_draw_header(_("MultiConvert"));
	GrGetGCTextSize(multiconvert_gc, " ", 1, GR_TFASCII, &width, &height, &base);
	height += 4;
	base += 1;
	GrGetScreenInfo(&screen_info);
	midpoint = screen_info.cols/2 - 4;
	multiconvert_num_rows = numrows = (screen_info.rows - 22 - height) / height;
	startrow = multiconvert_start_row;
	maxrow = multiconvert_unit_types[multiconvert_curr_type].num_units;
	
	GrClearWindow(multiconvert_bufwid, GR_FALSE);
	GrSetGCBackground(multiconvert_gc, appearance_get_color(CS_BG));
	GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_BG));
	GrFillRect(multiconvert_bufwid, multiconvert_gc, 0, 0, screen_info.cols, screen_info.rows);
	GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_FG));
	/* draw unit type */
	if (multiconvert_sel < 0) {
		GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_SELBG));
		GrFillRect(multiconvert_bufwid, multiconvert_gc, 0, 1, screen_info.cols, height);
		GrSetGCBackground(multiconvert_gc, appearance_get_color(CS_SELBG));
		GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_SELFG));
	}
	GrText(multiconvert_bufwid, multiconvert_gc, 4, base+1, _("Units:"), -1, GR_TFASCII);
	GrText(multiconvert_bufwid, multiconvert_gc, midpoint+4, base+1, multiconvert_unit_types[multiconvert_curr_type].name, -1, GR_TFASCII);
	/* draw a line */
	GrSetGCForeground(multiconvert_gc, GR_RGB(160,160,160));
	GrLine(multiconvert_bufwid, multiconvert_gc, 0, height+1, screen_info.cols, height+1);
	/* draw the units */
	for (i = startrow, j = 1; i<(startrow+numrows) && i<maxrow; i++, j++) {
		if (i == multiconvert_sel) {
			GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_SELBG));
			GrFillRect(multiconvert_bufwid, multiconvert_gc, 0, (height*j)+2, screen_info.cols, height);
			GrSetGCBackground(multiconvert_gc, appearance_get_color(CS_SELBG));
			GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_SELFG));
		} else {
			GrSetGCBackground(multiconvert_gc, appearance_get_color(CS_BG));
			GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_FG));
		}
		u = multiconvert_unit_types[multiconvert_curr_type].units[i];
		GrText(multiconvert_bufwid, multiconvert_gc, 4, (height*j)+2+base, u.svalue, -1, GR_TFASCII);
		GrText(multiconvert_bufwid, multiconvert_gc, midpoint+4, (height*j)+2+base, u.name, -1, GR_TFASCII);
	}
	/* draw another line just for the heck of it */
	GrSetGCForeground(multiconvert_gc, GR_RGB(160,160,160));
	GrLine(multiconvert_bufwid, multiconvert_gc, midpoint, 0, midpoint, screen_info.rows-20);
	/* draw a scroll bar */
	multiconvert_draw_scrollbar(multiconvert_bufwid, multiconvert_gc, 1, screen_info.cols, screen_info.rows-21, numrows+1, maxrow+1, multiconvert_sel+1);
	/* copy the buffer to the actual window */
	GrCopyArea(multiconvert_wid, multiconvert_gc, 0, 0, screen_info.cols, screen_info.rows - HEADER_TOPLINE, multiconvert_bufwid, 0, 0, MWROP_SRCCOPY);
}

void multiconvert_push_callback(void)
{
	double v;
	char sv[13]; /* was "courtneycavn" */
	if (text_get_buffer()[0] != 0) {
		v = atof(text_get_buffer());
		snprintf(sv, 13, "%g", v);
		multiconvert_unit_types[multiconvert_curr_type].units[multiconvert_push_u].value = v;
		multiconvert_unit_types[multiconvert_curr_type].units[multiconvert_push_u].svalue = sv;
		multiconvert_calculate(multiconvert_curr_type, multiconvert_push_u);
		multiconvert_draw();
	}
}

void multiconvert_push(int u)
{
	char uvalue[13]; /* was "jon was her." */
	multiconvert_push_u = u;
	if (u < 0) {
		/* switch to a different unit type */
		multiconvert_curr_type++;
		if (multiconvert_curr_type >= multiconvert_num_unit_types) { multiconvert_curr_type = 0; }
		multiconvert_draw();
	} else {
		/* change a unit value */
		snprintf(uvalue, 13, "%g", multiconvert_unit_types[multiconvert_curr_type].units[u].value);
		new_text_box_numeric(10, 40, 0, 0, uvalue, multiconvert_push_callback);
	}
}

int multiconvert_handle_event(GR_EVENT * event)
{
	if (event->type == GR_EVENT_TYPE_KEY_DOWN) {
		switch (event->keystroke.ch) {
		case 'l':
			if (multiconvert_sel >= 0) {
				multiconvert_sel--;
				if (multiconvert_start_row > ((multiconvert_sel<0)?0:multiconvert_sel)) { multiconvert_start_row = ((multiconvert_sel<0)?0:multiconvert_sel); }
				multiconvert_draw();
			} else { return 0; }
			break;
		case 'r':
			if (multiconvert_sel+1 < multiconvert_unit_types[multiconvert_curr_type].num_units) {
				multiconvert_sel++;
				if (multiconvert_start_row+multiconvert_num_rows <= multiconvert_sel) {
					multiconvert_start_row = multiconvert_sel-multiconvert_num_rows+1;
				}
				multiconvert_draw();
			} else { return 0; }
			break;
		case '\n':
		case '\r':
			multiconvert_push(multiconvert_sel);
			break;
		case 'm':
		case 'q':
			multiconvert_free();
			pz_close_window(multiconvert_wid);
			GrDestroyWindow(multiconvert_bufwid);
			GrDestroyGC(multiconvert_gc);
			break;
		case 'f':
			multiconvert_sel = -1;
			multiconvert_curr_type++;
			if (multiconvert_curr_type >= multiconvert_num_unit_types) { multiconvert_curr_type = 0; }
			multiconvert_draw();
			break;
		case 'w':
			multiconvert_sel = -1;
			multiconvert_curr_type--;
			if (multiconvert_curr_type < 0) { multiconvert_curr_type = multiconvert_num_unit_types-1; }
			multiconvert_draw();
			break;
		case 'd':
			new_message_window("© 2005 Jonathan Bettencourt / Kreative Korporation.");
			break;
		}
		return 1;
	} else {
		return 0;
	}
}

void new_multiconvert_window(void)
{
	if ( multiconvert_init() != 0 ) {
		new_message_window(_("MultiConvert failed to load."));
		multiconvert_free();
	} else {
		multiconvert_gc = pz_get_gc(1);
		GrSetGCUseBackground(multiconvert_gc, GR_TRUE);
		GrSetGCForeground(multiconvert_gc, appearance_get_color(CS_FG));
		GrSetGCBackground(multiconvert_gc, appearance_get_color(CS_BG));
		multiconvert_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), multiconvert_draw, multiconvert_handle_event);
		multiconvert_bufwid = GrNewPixmap(screen_info.cols, screen_info.rows - HEADER_TOPLINE, NULL);
		GrSelectEvents(multiconvert_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP| GR_EVENT_MASK_KEY_DOWN);
		GrMapWindow(multiconvert_wid);
	}
}

