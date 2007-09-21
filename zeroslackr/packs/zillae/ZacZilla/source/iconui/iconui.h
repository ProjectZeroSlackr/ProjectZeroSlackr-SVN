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

#ifndef ____ICONUI_H____
#define ____ICONUI_H____

#include "../pz.h"

extern ttk_font iconui_font;

void iconui_cache_clear();
ttk_surface iconui_cache_get(char * name);
ttk_surface iconui_cache_set(char * name, ttk_surface icon);

ttk_surface iconui_get_icon(char * s);
ttk_surface iconui_get_medium_icon(char * s);
ttk_surface iconui_get_small_icon(char * s);

void iconui_draw_icon(ttk_surface srf, char * s, int x, int y);
void iconui_draw_medium_icon(ttk_surface srf, char * s, int x, int y);
void iconui_draw_small_icon(ttk_surface srf, char * s, int x, int y);

TWindow * iconui_mh_sub(ttk_menu_item * item);
TWindow * iconui_new_menu_window(TWidget * menu_wid);

void iconui_free();
void iconui_init();

#endif
