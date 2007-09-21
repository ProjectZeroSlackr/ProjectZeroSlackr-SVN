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
#include "textinput.h"

/* browser.c */
extern void new_exec_window(char *filename);


void new_run_window_callback(void)
{
	char * fn;
	fn = text_get_buffer();
	if (fn[0] != 0) {
		new_exec_window(fn);
	}
}

void new_run_window(void)
{
	GrGetScreenInfo(&screen_info);
	new_text_box(10, 40, 0, 0, "", new_run_window_callback);
}
