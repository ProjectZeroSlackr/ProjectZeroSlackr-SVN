/*
 * Clickwheel Services for iPodLinux
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *
 * Somebody please help me fill in these functions for the other gens!
 * (I know 1g, 2g, 3g, 1g mini won't report clickwheel position, but
 * they can still return clickwheel down.)
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
#include "ipod.h"

#define clickwheel_inl(p) (*(volatile unsigned long *) (p))

static GR_TIMER_ID clickwheel_timer = 0;
static int clickwheel_ps = -1;

/* MESS WITH THIS STUFF */

int clickwheel_ipod_gen(void)
{
#ifdef IPOD
	return (ipod_get_hw_version() / 10000);
#else
	return 0;
#endif
}

int clickwheel_down(void)
{
#ifdef IPOD
	int in, st;
	switch (clickwheel_ipod_gen()) {
	case 1: /* 1st gen */
	case 2: /* 2nd gen */
	case 3: /* 3rd gen */
	case 4: /* 1st gen mini */
		return 0;
		break;
	case 5: /* 4th gen */
	case 6: /* 2nd gen mini */
	case 7: /* photo/color */
		in = clickwheel_inl(0x7000C140);
		st = (in & 0xFF000000) >> 24;
		return (st == 0xC0);
		break;
	case 12: /* nano */
		return 0;
		break;
	default:
		return 0;
		break;
	}
#else
	return 0;
#endif
}

int clickwheel_position(void)
{
#ifdef IPOD
	int in, st, touch;
	switch (clickwheel_ipod_gen()) {
	case 1: /* 1st gen */
	case 2: /* 2nd gen */
	case 3: /* 3rd gen */
	case 4: /* 1st gen mini */
		return -1;
		break;
	case 5: /* 4th gen */
	case 6: /* 2nd gen mini */
	case 7: /* photo/color */
		in = clickwheel_inl(0x7000C140);
		st = (in & 0xFF000000) >> 24;
		if (st == 0xC0) {
			touch = (in & 0x007F0000) >> 16;
			return touch;
		} else {
			return -1;
		}
		break;
	case 12: /* nano */
		return -1;
		break;
	default:
		return -1;
		break;
	}
#else
	return -1;
#endif
}

/* DON'T MESS WITH THIS STUFF */

int clickwheel_octrant(void)
{
	int touch = clickwheel_position();
	if (touch >= 0) {
		touch += 6;
		touch /= 12;
		if (touch > 7) { touch = 0; }
	}
	return touch;
}

int clickwheel_division(int n)
{
	/* click wheel is evenly divisible by 2, 3, 4, 6, 8, 12, 16, 24, and 48 */
	int touch = clickwheel_position();
	if (touch >= 0) {
		touch += (48/n);
		touch /= (96/n);
		if (touch >= n) { touch = 0; }
	}
	return touch;
}

void clickwheel_create_timer(GR_WINDOW_ID wid)
{
	if (clickwheel_timer) {
		GrDestroyTimer(clickwheel_timer);
	}
	clickwheel_ps = -1;
	clickwheel_timer = GrCreateTimer(wid, 20);
}

void clickwheel_destroy_timer(void)
{
	if (clickwheel_timer) {
		GrDestroyTimer(clickwheel_timer);
	}
	clickwheel_ps = -1;
	clickwheel_timer = 0;
}

int is_clickwheel_timer(GR_EVENT * event)
{
	return (((GR_EVENT_TIMER *)event)->tid == clickwheel_timer);
}

int clickwheel_change(int i)
{
	int x = (clickwheel_ps != i);
	clickwheel_ps = i;
	return x;
}
