/*
 * Clickwheel Services for iPodLinux
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

#ifndef __CLICKWHEEL_H__
#define __CLICKWHEEL_H__

#define clickwheel_inl(p) (*(volatile unsigned long *) (p))

int clickwheel_ipod_gen(void);
int clickwheel_down(void);
int clickwheel_position(void);
int clickwheel_octrant(void);
int clickwheel_division(int n);
void clickwheel_create_timer(GR_WINDOW_ID wid);
void clickwheel_destroy_timer(void);
int is_clickwheel_timer(GR_EVENT * event);
int clickwheel_change(int i);

#endif
