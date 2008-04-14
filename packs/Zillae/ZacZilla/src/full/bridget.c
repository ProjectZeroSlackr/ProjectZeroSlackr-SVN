/*
 * Copyright (C) 2006 Jonathynne Bettencourt (jonrelay)
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

#define B_MAX(a,b) (((a)>(b))?(a):(b))
#define B_MIN(a,b) (((a)<(b))?(a):(b))

typedef enum bridget_tile {
	t_unusable,	/* unusable playing square */
	t_ohonly,	/* o goes horizontally only */
	t_xvonly,	/* x goes vertically only */
	t_xhov,		/* x goes horizontally, o goes vertically */
	t_ohxv,		/* o goes horizontally, x goes vertically */
	t_x,		/* x marker */
	t_o,		/* o marker */
	t_number,	/* column/row number */
	t_horiz,	/* horizontally marked */
	t_vert		/* vertically marked */
} bridget_tile;

static const char * bridget_help_text__ =
	"In this game, the first player is designated as O and the "
	"second player as X. Using a grid of tiles, the player puts "
	"a connecting line between two horizontally or vertically "
	"adjacent tiles of their symbol. The O player attempts to "
	"form a connected bridge from the top to the bottom of the "
	"board, while the X player attempts to form a bridge from "
	"the left to the right. The first one to form a bridge wins."
;
static char * bridget_help_text;

static int          bridget_base_x;
static int          bridget_base_y;
static int          bridget_cell_tsize;
static int          bridget_cell_size;
static int          bridget_cell_spacing;
static bridget_tile bridget_cells[12][12];
static int          bridget_text_x;
static int          bridget_text_y;
static int          bridget_text_w;
static int          bridget_text_h;

static int bridget_cursor_x;
static int bridget_cursor_y;
static int bridget_xs_turn;
static int bridget_over;

static void bridget_draw_x(ttk_surface srf, int x, int y, int size, ttk_color col)
{
	ttk_line(srf, x, y, x+size-1, y+size-1, col);
	ttk_line(srf, x+size-1, y, x, y+size-1, col);
}

static void bridget_draw_o(ttk_surface srf, int x, int y, int size, ttk_color col)
{
	ttk_ellipse(srf, x+size/2, y+size/2, size/2, size/2, col);
}

static int bridget_ok(int x, int y)
{
	return (
		bridget_cells[x][y] == t_xhov ||
		bridget_cells[x][y] == t_ohxv ||
		bridget_cells[x][y] == (bridget_xs_turn ? t_xvonly : t_ohonly)
	);
}

static void bridget_mark(int x, int y)
{
	if (bridget_xs_turn) {
		if (bridget_cells[x][y] == t_xhov) {
			bridget_cells[x][y] = t_horiz;
		} else if (bridget_cells[x][y] == t_ohxv || bridget_cells[x][y] == t_xvonly) {
			bridget_cells[x][y] = t_vert;
		}
	} else {
		if (bridget_cells[x][y] == t_ohxv || bridget_cells[x][y] == t_ohonly) {
			bridget_cells[x][y] = t_horiz;
		} else if (bridget_cells[x][y] == t_xhov) {
			bridget_cells[x][y] = t_vert;
		}
	}
}

static int bridget_fixnext()
{
	int ox = bridget_cursor_x;
	int oy = bridget_cursor_y;
	while (!bridget_ok(bridget_cursor_x, bridget_cursor_y)) {
		bridget_cursor_x++;
		if (bridget_cursor_x >= 12) {
			bridget_cursor_x = 0;
			bridget_cursor_y++;
		}
		if (bridget_cursor_y >= 12) {
			bridget_cursor_y = 0;
		}
		if (bridget_cursor_x == ox && bridget_cursor_y == oy) {
			bridget_over = 1;
			return 0; //no valid moves
		}
	}
	return 1;
}

static int bridget_next()
{
	bridget_cursor_x++;
	if (bridget_cursor_x >= 12) {
		bridget_cursor_x = 0;
		bridget_cursor_y++;
	}
	if (bridget_cursor_y >= 12) {
		bridget_cursor_y = 0;
	}
	return bridget_fixnext();
}

static int bridget_fixprev()
{
	int ox = bridget_cursor_x;
	int oy = bridget_cursor_y;
	while (!bridget_ok(bridget_cursor_x, bridget_cursor_y)) {
		bridget_cursor_x--;
		if (bridget_cursor_x < 0) {
			bridget_cursor_x = 11;
			bridget_cursor_y--;
		}
		if (bridget_cursor_y < 0) {
			bridget_cursor_y = 11;
		}
		if (bridget_cursor_x == ox && bridget_cursor_y == oy) {
			bridget_over = 1;
			return 0; //no valid moves
		}
	}
	return 1;
}

static int bridget_prev()
{
	bridget_cursor_x--;
	if (bridget_cursor_x < 0) {
		bridget_cursor_x = 11;
		bridget_cursor_y--;
	}
	if (bridget_cursor_y < 0) {
		bridget_cursor_y = 11;
	}
	return bridget_fixprev();
}

static void bridget_reset(int winw, int winh)
{
	int x,y;
	for (x=0; x<12; x++) {
		bridget_cells[x][0] = t_number;
		bridget_cells[0][x] = t_number;
	}
	for (y=1; y<12; y+=2) {
		for (x=2; x<12; x+=2) {
			bridget_cells[x][y] = t_o;
		}
	}
	for (y=2; y<12; y+=2) {
		for (x=1; x<12; x+=2) {
			bridget_cells[x][y] = t_x;
		}
	}
	for (y=2; y<12; y+=2) {
		for (x=2; x<12; x+=2) {
			bridget_cells[x][y] = t_xhov;
		}
	}
	for (y=1; y<12; y+=2) {
		for (x=1; x<12; x+=2) {
			bridget_cells[x][y] = t_ohxv;
		}
	}
	for (x=1; x<12; x+=2) {
		bridget_cells[x][1] = t_ohonly;
		bridget_cells[x][11] = t_ohonly;
		bridget_cells[1][x] = t_xvonly;
		bridget_cells[11][x] = t_xvonly;
	}
	bridget_cells[1][1] = t_unusable;
	bridget_cells[1][11] = t_unusable;
	bridget_cells[11][1] = t_unusable;
	bridget_cells[11][11] = t_unusable;
	
	bridget_cell_tsize = B_MIN(winw, winh) / 12;
	bridget_cell_size = B_MIN(bridget_cell_tsize-2, ((B_MIN(winw,winh) < 180) ? 5 : 7));
	bridget_cell_spacing = (bridget_cell_tsize - bridget_cell_size)/2;
	bridget_base_x = bridget_base_y = (B_MIN(winw,winh) - bridget_cell_tsize*12)/2;
	if (winh < winw) {
		bridget_text_x = bridget_base_x + bridget_cell_tsize*12 + 8;
		bridget_text_y = bridget_base_y;
	} else {
		bridget_text_x = bridget_base_x;
		bridget_text_y = bridget_base_y + bridget_cell_tsize*12 + 8;
	}
	bridget_text_w = winw - bridget_text_x - bridget_base_x;
	bridget_text_h = winh - bridget_text_y - bridget_base_y;
	
	bridget_cursor_x = 0;
	bridget_cursor_y = 0;
	bridget_xs_turn = (rand() & 1);
	bridget_over = 0;
	bridget_fixnext();
}

static void bridget_help()
{
	TWindow *w=ttk_new_window();
	w->title="Help";
	new_message_window("In this game, the first player is designated as O and thesecond player as X.");
	new_message_window("Using a grid of tiles, the player puts a connecting line between two horizontally or vertically adjacent tiles of their symbol.");
	new_message_window("The O player attempts to form a connected bridge from the top to the bottom of the board,");
	new_message_window("while the X player attempts to form a bridge from the left to the right.");
	new_message_window("The first one to form a bridge wins.");
	if (!w) {
		new_message_window(_("Failed to read directions.")); /* ... like many n00bs. */
	} else {
		//ttk_show_window(w);
	}
}

static void bridget_draw(TWidget * wid, ttk_surface srf)
{
	int x, y, px, py;
	ttk_color col;
	int th, tw;
	char buf[4];
	col = ttk_ap_getx("window.fg")->color;
	th = ttk_text_height(ttk_textfont);
	tw = ttk_text_width(ttk_textfont,"10");
	ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), wid->x, wid->y, wid->x+wid->w, wid->y+wid->h);
	for (y=0; y<12; y++) {
		for (x=0; x<12; x++) {
			px = bridget_base_x + bridget_cell_tsize * x;
			py = bridget_base_y + bridget_cell_tsize * y;
			switch (bridget_cells[x][y]) {
			case t_unusable:
			case t_ohonly:
			case t_xvonly:
			case t_xhov:
			case t_ohxv:
				/* draw nothing */
				break;
			case t_x:
				bridget_draw_x(srf, px + bridget_cell_spacing, py + bridget_cell_spacing, bridget_cell_size, col);
				break;
			case t_o:
				bridget_draw_o(srf, px + bridget_cell_spacing, py + bridget_cell_spacing, bridget_cell_size, col);
				break;
			case t_number:
				if (x == 0 && y == 0) {
					buf[0] = (bridget_xs_turn ? 'X' : 'O');
					buf[1] = 0;
				} else if (x == 0) {
					snprintf(buf, 4, "%d", y);
				} else if (y == 0) {
					snprintf(buf, 4, "%d", x);
				}
				if (tw < bridget_cell_tsize || th < bridget_cell_tsize) {
					ttk_text(srf, ttk_textfont, px + (bridget_cell_tsize - ttk_text_width(ttk_textfont, (const char *)buf))/2,
						py + (bridget_cell_tsize - th)/2, col, (const char *)buf);
				} else {
					/*pz_vector_string(srf, buf, px + bridget_cell_spacing, py + bridget_cell_spacing,
						bridget_cell_size / strlen(buf), bridget_cell_size, 0, col);*/
				}
				break;
			case t_horiz:
				ttk_line(srf, px - bridget_cell_spacing + 1, py + (bridget_cell_tsize-1)/2,
					px + bridget_cell_tsize + bridget_cell_spacing - 2, py + (bridget_cell_tsize-1)/2, col);
				break;
			case t_vert:
				ttk_line(srf, px + (bridget_cell_tsize-1)/2, py - bridget_cell_spacing + 1,
					px + (bridget_cell_tsize-1)/2, py + bridget_cell_tsize + bridget_cell_spacing - 2, col);
				break;
			}
		}
	}
	if (bridget_ok(bridget_cursor_x, bridget_cursor_y)) {
		px = bridget_base_x + bridget_cursor_x * bridget_cell_tsize;
		py = bridget_base_y + bridget_cursor_y * bridget_cell_tsize;
		ttk_rect(srf,
			px + bridget_cell_spacing/2,
			py + bridget_cell_spacing/2,
			px + bridget_cell_spacing + bridget_cell_size + bridget_cell_spacing/2,
			py + bridget_cell_spacing + bridget_cell_size + bridget_cell_spacing/2,
			col);
	}
	if (bridget_over) {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y, col, _("No Moves."));
	} else if (bridget_xs_turn) {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y, col, _("X's Turn."));
	} else {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y, col, _("O's Turn."));
	}
	if (bridget_text_y > bridget_base_y) {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*1, col,
			_("Hold select button for help."));
	} else if (bridget_text_w > 50) {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*1+th/2, col, _("Hold select"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*2+th/2, col, _("button for"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*3+th/2, col, _("help."));
	} else {
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*1+th/2, col, _("Hold"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*2+th/2, col, _("select"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*3+th/2, col, _("button"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*4+th/2, col, _("for"));
		ttk_text(srf, ttk_textfont, bridget_text_x, bridget_text_y + th*5+th/2, col, _("help."));
	}
}

static int bridget_scroll(TWidget * wid, int dir)
{
	TTK_SCROLLMOD(dir, 4);
	if (bridget_over) return TTK_EV_DONE;
	if (dir < 0) {
		while (dir) {
			bridget_prev();
			dir++;
		}
	} else {
		while (dir) {
			bridget_next();
			dir--;
		}
	}
	wid->dirty = 1;
	return TTK_EV_CLICK;
}

static int bridget_button(TWidget * wid, int btn, int time)
{
	switch (btn) {
	case TTK_BUTTON_ACTION:
		if (time < 500) {
			if (!bridget_over) {
				bridget_mark(bridget_cursor_x, bridget_cursor_y);
				bridget_xs_turn = !bridget_xs_turn;
				bridget_fixnext();
				wid->dirty = 1;
			}
		} else {
			bridget_help();
		}
		break;
	case TTK_BUTTON_MENU:
		pz_close_window(wid->win);
		break;
	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

TWindow * new_bridget_window(void)
{
	TWindow * win;
	TWidget * wid;
	bridget_help_text = _(bridget_help_text__);
	win = ttk_new_window();
	win->title="Bridget";
	wid = ttk_new_widget(0,0);
	wid->w = ttk_screen->w;
	wid->h = ttk_screen->h - ttk_screen->wy;
	wid->draw = bridget_draw;
	wid->scroll = bridget_scroll;
	wid->button = bridget_button;
	wid->focusable = 1;
	bridget_reset(wid->w, wid->h);
	ttk_add_widget(win, wid);
	ttk_show_window(win);
}


