/***************************************************************************
 *   Copyright (C) 2006 by Jacob Hoffman-Andrews                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <pz.h>
#include "search.h"
#include "encyclopodiareader.h"

extern "C" {
	extern TWidget * ti_new_standard_text_widget(int x, int y,
			int w, int h, int absheight, char * dt,
			int (*callback)(TWidget *, char *));
}

namespace encyclopodia {

void draw_text_box(TWidget* text_bar, ttk_surface srf) {
        ttk_ap_fillrect(srf, ttk_ap_getx("window.bg"), text_bar->x, text_bar->y, text_bar->w, text_bar->h);
}

int do_search(TWidget* wid, char* search_text) {
	TWindow* window = wid->win;
	Ebook* book = (Ebook*)window->data;
	if (book)
		search_ebook(book, search_text);
	pz_close_window(window);
	return 0;
}

void make_text_bar(TWindow* window) {
        TWidget* text_box = ttk_new_widget(10, 5);
        text_box->w = window->w - 20;
        text_box->h = ttk_text_height(ttk_menufont)*((ttk_screen->w < 160 || ttk_screen->w >= 320)?2:3);
        text_box->draw = draw_text_box;

        int x = 10;
        int y = 10;
        int width = window->w - 20;
        int height = 10 + ttk_text_height(ttk_textfont);
        TWidget* text_input = ti_new_standard_text_widget(x, y, width, height, 0, "", do_search);

	ttk_add_widget(window, text_input);
        ttk_add_widget(window, text_box);
}

TWindow* create_search_window(ttk_menu_item* menu_item) {
	TWindow* search_window = pz_new_window(_("Search"), PZ_WINDOW_NORMAL);
	make_text_bar(search_window);
	if (menu_item->data) {
		/* Danger Will Robinson!  Casting a pointer into an int. */
		/* This is not 64-bit safe.  Good thing the ARM is 32 */
		/* bit!  Anyhow, why is TWindow.data an int? */
		search_window->data = (int)menu_item->data;
	}
	pz_start_input_for(search_window);
	return search_window;
}

} // end namespace encyclopodia

