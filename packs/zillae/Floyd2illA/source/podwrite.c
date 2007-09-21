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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pz.h"
#include "mlist.h"
#include "appearance.h"
#include "textinput.h"

/* START FUNCTION PROTOTYPES */

/* These were in podwrite.h, but I decided to rid
   myself of that file to make my life easier.    */

void podwrite_input_mode(void);
void podwrite_move_mode(void);
void podwrite_scroll_mode(void);
void podwrite_exit(void);

void podwrite_multiline_GrText(GR_WINDOW_ID wid, GR_GC_ID gc, int x, int y, int w, int h, char *t, int cursorpos, int scroll);
void podwrite_draw_scrollbar(GR_WINDOW_ID wid, GR_GC_ID gc, int y, int w, int h, int screen_items, int num_items, int sel);

void podwrite_save_callback(void);
void podwrite_save(void);

void podwrite_close_menu(void);
static void podwrite_menu_do_draw();
static int podwrite_menu_do_keystroke(GR_EVENT * event);
void podwrite_new_menu_window();
void podwrite_open_menu(void);

void podwrite_text_exit(void);
void podwrite_text_output_char(int ch);
void podwrite_draw(void);
int podwrite_handle_event(GR_EVENT *event);

void new_podwrite_window_with_text(char * t);
void new_podwrite_window_with_file(char * filename);
void new_podwrite_window(void);

/* END FUNCTION PROTOTYPES */

static int podwrite_mode = 0;
static int podwrite_linecount = 0;
static int podwrite_screenlines = 0;
static int podwrite_scroll = 0;
static int podwrite_cursor_out_of_bounds = 0;
static char * podwrite_saved_text;
static char * podwrite_saved_filename = 0;
static int podwrite_saved_stuff[4];
static GR_WINDOW_ID podwrite_menu_wid;
static GR_GC_ID podwrite_menu_gc;
static menu_st *podwrite_menuz;
static int podwrite_menu_no_auto_close = 0;

void podwrite_input_mode(void)
{
	podwrite_mode = 0;
}

void podwrite_move_mode(void)
{
	podwrite_mode = 1;
}

void podwrite_scroll_mode(void)
{
	podwrite_mode = 2;
}

void podwrite_exit(void)
{
	if (podwrite_saved_filename) {
		free(podwrite_saved_filename);
		podwrite_saved_filename = 0;
	}
	text_exit_continue();
}

void podwrite_clear(void)
{
	if (DIALOG_MESSAGE_T2(_("Clear"), _("Clear all text?"), _("No"), _("Yes"), 10)==1) {
		text_clear_buffer();
		text_set_iposition(0);
		podwrite_linecount = 0;
		podwrite_scroll = 0;
		podwrite_cursor_out_of_bounds = 0;
	} else {
		podwrite_menu_no_auto_close = 1;
	}
}

/*
	Multi-line GrText - just like GrText, but draws multiple lines.
*/

void podwrite_multiline_GrText(GR_WINDOW_ID wid, GR_GC_ID gc, int x, int y, int w, int h, char *t, int cursorpos, int scroll)
{
	char * sot; /* start of text */
	char * curtextptr;
	int currentLine = 0;
	int screenlines = 0;
	int coob = 0; /* Cursor Out Of Bounds, or iPodLinux dev; you decide */
	GR_SIZE width, height, base;
	GrGetGCTextSize(gc, " ", 1, GR_TFUTF8, &width, &height, &base);
	screenlines = h / height;

	sot = t;
	curtextptr = t;
	if ((cursorpos >= 0) && (*curtextptr == '\0')) {
		/* text is empty, draw the cursor at initial position */
		GrSetGCForeground(gc, GRAY);
		GrLine(wid, gc, x, y-base, x, y+height-base);
		GrSetGCForeground(gc, appearance_get_color(CS_FG));
	}
	while (*curtextptr != '\0') {
		char * sol; /* start of line */
		sol = curtextptr;
		/* find the line break */
		while (1) {
			if ((*curtextptr == '\n')||(*curtextptr == '\r')) {
				curtextptr++;
				break;
			} else if (*curtextptr == '\0') {
				break;
			}
			GrGetGCTextSize(gc, sol, curtextptr - sol + 1, GR_TFASCII, &width, &height, &base);
			if (width > w) {
				/* backtrack to the last word */
				char *optr;
				for (optr=curtextptr; optr>sol; optr--) {
					if (isspace(*optr)||(*optr=='\t')||(*optr=='-')) {
						curtextptr=optr;
						curtextptr++;
						break;
					}
				}
				break;
			}
			curtextptr++;
		}
		/* if the line is in the viewable area, draw it */
		if ( ((currentLine - scroll) >= 0) && ((currentLine - scroll) < screenlines) ) {
			GrSetGCForeground(gc, appearance_get_color(CS_FG));
			if ((*(curtextptr-1) == '\n')||(*(curtextptr-1) == '\r')) {
				/* line ends with a newline, so don't display the newline */
				GrText(wid, gc, x, y + ((currentLine-scroll)*height), sol, curtextptr-sol-1, GR_TFUTF8);
			} else {
				/* display the whole line */
				GrText(wid, gc, x, y + ((currentLine-scroll)*height), sol, curtextptr-sol, GR_TFUTF8);
			}
			GrSetGCForeground(gc, GRAY);
			if ((cursorpos >= sol-sot) && (cursorpos < curtextptr-sot)) {
				/* cursor is on the line, but not at the very end */
				if (cursorpos == (sol-sot)) {
					width = 0;
				} else {
					GrGetGCTextSize(gc, sol, cursorpos-(sol-sot), GR_TFUTF8, &width, &height, &base);
				}
				GrLine(wid, gc, x+width, y+((currentLine-scroll)*height)-base, x+width, y+((currentLine-scroll+1)*height)-base);
			} else if ((cursorpos == curtextptr-sot) && (*curtextptr == '\0')) {
				/* cursor is not only at the very end of the line, but the very end of the text */
				if ((*(curtextptr-1) == '\n')||(*(curtextptr-1) == '\r')) {
					/* last character of the text is a newline, so display the cursor on the next line */
					GrLine(wid, gc, x, y+((currentLine-scroll+1)*height)-base, x, y+((currentLine-scroll+2)*height)-base);
					if ((currentLine - scroll + 1) == screenlines) { coob = 1; }
				} else {
					/* display the cursor normally */
					GrGetGCTextSize(gc, sol, cursorpos-(sol-sot), GR_TFUTF8, &width, &height, &base);
					GrLine(wid, gc, x+width, y+((currentLine-scroll)*height)-base, x+width, y+((currentLine-scroll+1)*height)-base);
				}
			}
			GrSetGCForeground(gc, appearance_get_color(CS_FG));
			
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
	
	/* podwrite-specific; remove or edit if using in another context */
	podwrite_linecount = currentLine;
	podwrite_screenlines = screenlines;
	podwrite_cursor_out_of_bounds = coob;
}

void podwrite_draw_scrollbar(GR_WINDOW_ID wid, GR_GC_ID gc, int y, int w, int h, int screen_items, int num_items, int sel)
{
	/* ripped from mlist.c with minor changes */
	int per = screen_items * 100 / num_items;
	int height = (h-2) * (per < 3 ? 3 : per) / 100;
	int y_top = ((((h-3) - height) * 100) * sel / (num_items-1)) / 100 + y + 1;
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
	/* return to black */
	GrSetGCForeground(gc, BLACK);
}

/*
	PodWrite open/save handling.
*/

void podwrite_save_callback(void)
{
	/* write to the file */
	char * buf;
	FILE * textout;
	buf = strdup(text_get_buffer());
	if (podwrite_saved_filename) {
		free(podwrite_saved_filename);
		podwrite_saved_filename = 0;
	}
	
	if (buf[0]) {
		textout = fopen(buf, "w");
		fprintf(textout, "%s", podwrite_saved_text);
		fclose(textout);
	}
	/* restore podwrite window */
	new_podwrite_window_with_text(podwrite_saved_text);
	/* restore podwrite state */
	if ( (podwrite_saved_filename = (char *)malloc(strlen(buf)+2)) != NULL ) {
		strcpy(podwrite_saved_filename, buf);
	}
	podwrite_mode = 0;
	podwrite_linecount = podwrite_saved_stuff[0];
	podwrite_scroll = podwrite_saved_stuff[1];
	text_set_iposition(podwrite_saved_stuff[2]);
	text_draw();
	
	free(buf);
}

void podwrite_save(void)
{
	/* hold on tight, we're going to call a text window from inside a text window */
	
	/*
		Normally the menu handler would close the menu right after calling
		this function. In this case we can't close the menu window right
		after calling this because it causes a Podzilla window management
		catastrophe. Podzilla requires that windows be closed in the order
		they are created, because it uses a stack for event handlers I think.
		At this point, the window order is Podzilla Menu; PodWrite; PodWrite
		Menu. So we have to close the PodWrite Menu, then PodWrite, then
		create a new text box. Setting the following variable prevents the
		menu handler from attempting to close the menu again.
	*/
	podwrite_menu_no_auto_close = 1;
	/* save podwrite state */
	podwrite_saved_text = strdup(text_get_buffer());
	podwrite_saved_stuff[0] = podwrite_linecount;
	podwrite_saved_stuff[1] = podwrite_scroll;
	podwrite_saved_stuff[2] = text_get_iposition();
	/* close menu window */
	podwrite_close_menu();
	/* close podwrite window */
	text_exit_continue();
	/* now bring up a normal text box for save command */
	pz_draw_header(_("Save to..."));
	if (podwrite_saved_filename) {
		new_text_box(0, 0, 0, 0, podwrite_saved_filename, podwrite_save_callback);
	} else {
		new_text_box(0, 0, 0, 0, "/Notes/", podwrite_save_callback);
	}
}

/*
	PodWrite menu handling.
	Uses mlist.c for the actual handling, but otherwise separate from
	the main Podzilla menu system.
	/!\ This menu is programmed to close after selecting an item,
	so putting things like submenus and options in it is completely useless.
*/

void podwrite_close_menu(void)
{
	menu_destroy_all(podwrite_menuz);
	pz_close_window(podwrite_menu_wid);
	GrDestroyGC(podwrite_menu_gc);
}

static item_st podwrite_menu[] = {
	{N_("Return to PodWrite"), podwrite_input_mode, ACTION_MENU},
	{N_("Move Cursor"), podwrite_move_mode, ACTION_MENU},
	{N_("Scroll"), podwrite_scroll_mode, ACTION_MENU},
	{N_("Clear"), podwrite_clear, ACTION_MENU},
	{N_("Save..."), podwrite_save, ACTION_MENU},
	{N_("Exit PodWrite"), podwrite_exit, ACTION_MENU},
	{0}
};

static void podwrite_menu_do_draw()
{
	/* window is focused */
	if (podwrite_menu_wid == GrGetFocus()) {
		pz_draw_header(podwrite_menuz->title);
		menu_draw(podwrite_menuz);
	}
}

static int podwrite_menu_do_keystroke(GR_EVENT * event)
{
	int ret = 0;

	switch (event->type) {
	case GR_EVENT_TYPE_TIMER:
		menu_draw_timer(podwrite_menuz);
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		case '\r':
		case '\n':
			podwrite_menu_no_auto_close = 0;
			podwrite_menuz = menu_handle_item(podwrite_menuz, podwrite_menuz->sel);
			podwrite_menu_do_draw();
			ret |= KEY_CLICK;
			if (podwrite_menu_no_auto_close == 0) {
				podwrite_close_menu();
			}
			break;
		case 'l':
			if (menu_shift_selected(podwrite_menuz, -1)) {
				menu_draw(podwrite_menuz);
				ret |= KEY_CLICK;
			}
			break;
		case 'r':
			if (menu_shift_selected(podwrite_menuz, 1)) {
				menu_draw(podwrite_menuz);
				ret |= KEY_CLICK;
			}
			break;
		case 'm':
			menu_destroy_all(podwrite_menuz);
			pz_close_window(podwrite_menu_wid);
			GrDestroyGC(podwrite_menu_gc);
			break;
		default:
			ret |= KEY_UNUSED;
			break;
		}
		break;
	default:
		ret |= EVENT_UNUSED;
		break;
	}
	return ret;
}

void podwrite_new_menu_window()
{
	GrGetScreenInfo(&screen_info);

	podwrite_menu_gc = pz_get_gc(1);
	GrSetGCUseBackground(podwrite_menu_gc, GR_TRUE);
	GrSetGCForeground(podwrite_menu_gc, appearance_get_color(CS_FG));
	GrSetGCBackground(podwrite_menu_gc, appearance_get_color(CS_BG));

	podwrite_menu_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols,
			screen_info.rows - (HEADER_TOPLINE + 1), podwrite_menu_do_draw,
			podwrite_menu_do_keystroke);

	GrSelectEvents(podwrite_menu_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP|
			GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_TIMER);

	podwrite_menuz = menu_init(podwrite_menu_wid, _("PodWrite"), 0, 1,
			screen_info.cols, screen_info.rows -
			(HEADER_TOPLINE + 1), NULL, podwrite_menu, ASCII);

	GrMapWindow(podwrite_menu_wid);
}

void podwrite_open_menu(void)
{
	podwrite_new_menu_window();
}

/* PodWrite Event Handling */

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

void podwrite_text_exit(void)
{
	/* text_output_byte(10);
	text_draw();
	if (podwrite_check_bounds() != 0) { text_draw(); } */
	podwrite_open_menu();
}

void podwrite_text_output_char(int ch)
{
	if ((ch == 10) || (ch == 13)) {
		text_output_byte(10);
		text_draw();
		if (podwrite_check_bounds() != 0) { text_draw(); }
	} else {
		text_output_char_continue(ch);
	}
}

/*
	We used to have our text_exit replacement function
	produce a newline, and no text_output_char replacement.
	The default text_output_char calls text_exit when
	called with a newline.
	But the hold switch method for bringing up PodWrite's
	menu was causing somebody grief, so I decided to fix
	things and make PodWrite produce a newline in its
	text_output_char replacement, and bring up its menu
	in its text_exit replacement.
*/

int podwrite__scroll_sel(void)
{
	if (podwrite_linecount <= podwrite_screenlines) {
		return 0;
	} else if ((podwrite_linecount > 46000) && (podwrite_scroll > podwrite_screenlines)) {
		/* on the VERY rare occasion that you have loads of lines, prevent any chance of numeric overflow */
		return podwrite_scroll+podwrite_screenlines-1;
	} else {
		return (podwrite_scroll*(podwrite_linecount-1))/(podwrite_linecount-podwrite_screenlines);
	}
}

void podwrite_draw(void)
{
	GrGetScreenInfo(&screen_info);
	/* pz_draw_header(_("PodWrite")); */
    GrClearWindow(text_get_bufwid(), GR_FALSE);
    GrSetGCBackground(text_get_gc(), appearance_get_color(CS_BG));
    GrSetGCForeground(text_get_gc(), appearance_get_color(CS_BG));
    GrFillRect(text_get_bufwid(), text_get_gc(), 0, 0, text_get_width(), text_get_height());
    GrSetGCForeground(text_get_gc(), appearance_get_color(CS_FG));
    GrRect(text_get_bufwid(), text_get_gc(), 0, 0, text_get_width(), text_get_height());
	podwrite_multiline_GrText(text_get_bufwid(), text_get_gc(), 5, 15,
		screen_info.cols-18, screen_info.rows-text_optimum_height()-2,
		text_get_buffer(), text_get_iposition(), podwrite_scroll);
	podwrite_draw_scrollbar(text_get_bufwid(), text_get_gc(), 2,
		screen_info.cols+1, screen_info.rows-text_optimum_height(),
		podwrite_screenlines, podwrite_linecount, podwrite__scroll_sel());
	text_draw_status(text_get_gc());
	/* copy the buffer to the actual window */
	GrCopyArea(text_get_wid(), text_get_gc(), 0, 0, text_get_width()+1, text_get_height()+1, text_get_bufwid(), 0, 0, MWROP_SRCCOPY);
}

int podwrite_handle_event(GR_EVENT *event)
{
	if (podwrite_mode == 0) {
		int ret;
		ret = text_handle_event_continue(event);
		GrFlush();
		if (podwrite_check_bounds() != 0) { text_draw(); GrFlush(); }
		return ret;
	} else if (podwrite_mode == 1) {
		if (event->type == GR_EVENT_TYPE_KEY_DOWN) {
			switch (event->keystroke.ch) {
			case 'l':
			case 28:
				if (text_get_iposition() > 0) {
					text_output_char(28);
					if (podwrite_check_bounds() != 0) { text_draw(); }
				} else { return 0; }
				break;
			case 'r':
			case 29:
				if (text_get_iposition() < text_get_length()) {
					text_output_char(29);
					if (podwrite_check_bounds() != 0) { text_draw(); }
				} else { return 0; }
				break;
			case '\n':
			case '\r':
			case 'd':
				podwrite_input_mode();
				break;
			case 'w':
			case 8:
				text_set_iposition(0);
				podwrite_scroll = 0;
				text_draw();
				break;
			case 'f':
			case 127:
				text_set_iposition(text_get_length());
				podwrite_scroll = podwrite_linecount-podwrite_screenlines;
				if (podwrite_scroll < 0) { podwrite_scroll = 0; }
				text_draw();
				if (podwrite_check_bounds() != 0) { text_draw(); }
				break;
			case 'm':
			/* case 'h': */
			case 27:
				podwrite_input_mode();
				podwrite_open_menu();
				break;
			}
			return 1;
		}
	} else if (podwrite_mode == 2) {
		if (event->type == GR_EVENT_TYPE_KEY_DOWN) {
			switch (event->keystroke.ch) {
			case 'l':
			case 28:
				if (podwrite_scroll>0) {
					podwrite_scroll--;
					text_draw();
				} else { return 0; }
				break;
			case 'r':
			case 29:
				if ( (podwrite_linecount > podwrite_screenlines) && (podwrite_scroll < (podwrite_linecount-podwrite_screenlines)) ) {
					podwrite_scroll++;
					text_draw();
				} else { return 0; }
				break;
			case '\n':
			case '\r':
			case 'd':
				podwrite_input_mode();
				break;
			case 'w':
				podwrite_scroll = 0;
				text_draw();
				break;
			case 'f':
				podwrite_scroll = podwrite_linecount-podwrite_screenlines;
				if (podwrite_scroll < 0) { podwrite_scroll = 0; }
				text_draw();
				break;
			case 'm':
			/* case 'h': */
			case 27:
				podwrite_input_mode();
				podwrite_open_menu();
				break;
			}
			return 1;
		}
	}
	return 0;
}

void new_podwrite_window_with_text(char * t)
{
	podwrite_mode = 0;
	podwrite_linecount = 0;
	podwrite_scroll = 0;
	podwrite_cursor_out_of_bounds = 0;
	
	if (podwrite_saved_filename) {
		free(podwrite_saved_filename);
		podwrite_saved_filename = 0;
	}
	
	GrGetScreenInfo(&screen_info);
	new_text_box_x(-1, 20, screen_info.cols+2, screen_info.rows-19, t, NULL,
		podwrite_draw, podwrite_text_output_char, podwrite_handle_event, podwrite_text_exit);
	pz_draw_header(_("PodWrite"));
}

void new_podwrite_window_with_file(char * filename)
{
	char *buf;
	char tmp[512];
	FILE *fp;
	size_t file_len;
	if ((fp = fopen(filename,"r"))==NULL) {
		return;
	}
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	rewind(fp);
	if(file_len==0) {
		buf = '\0';
		while(fgets(tmp, 512, fp)!=NULL) {
			if((buf = realloc(buf, ((buf=='\0'?0:strlen(buf)) +
						512) * sizeof(char)))==NULL) {
				fclose(fp);
				return;
			}
			if(file_len==0) {
				strncpy(buf, tmp, 512);
				file_len=1;
			} else
				strncat(buf, tmp, 512);
		}
	}
	else {
		if((buf = calloc(file_len+1, 1))==NULL) {
			fclose(fp);
			return;
		}
		if(fread(buf, 1, file_len, fp)!=file_len)
			pz_error(_("unknown read error, continuing"));
	}
	fclose(fp);
	
	podwrite_mode = 0;
	podwrite_linecount = 0;
	podwrite_scroll = 0;
	podwrite_cursor_out_of_bounds = 0;
	
	if (podwrite_saved_filename) { free(podwrite_saved_filename); }
	if ( (podwrite_saved_filename = (char *)malloc(strlen(filename)+2)) != NULL ) {
		strcpy(podwrite_saved_filename, filename);
	} else { podwrite_saved_filename = 0; }
	
	GrGetScreenInfo(&screen_info);
	new_text_box_x(-1, 20, screen_info.cols+2, screen_info.rows-19, buf, NULL,
		podwrite_draw, podwrite_text_output_char, podwrite_handle_event, podwrite_text_exit);
	text_set_iposition(0);
	pz_draw_header(_("PodWrite"));
	
	free(buf);
}

void new_podwrite_window(void)
{
	podwrite_mode = 0;
	podwrite_linecount = 0;
	podwrite_scroll = 0;
	podwrite_cursor_out_of_bounds = 0;
	
	if (podwrite_saved_filename) {
		free(podwrite_saved_filename);
		podwrite_saved_filename = 0;
	}
	
	GrGetScreenInfo(&screen_info);
	new_text_box_x(-1, 20, screen_info.cols+2, screen_info.rows-19, "", NULL,
		podwrite_draw, podwrite_text_output_char, podwrite_handle_event, podwrite_text_exit);
	pz_draw_header(_("PodWrite"));
}
