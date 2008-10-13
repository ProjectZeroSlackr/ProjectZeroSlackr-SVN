/*
 * Last updated: Oct 12, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo
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

#define KEY_MENU	50 // Up
#define KEY_PLAY	32 // Down
#define KEY_REWIND	17 // Left
#define KEY_FORWARD	33 // Right
#define KEY_ACTION	28 // Select
#define KEY_HOLD	35 // Hold
#define KEY_REC		46 // Record (Sansa e200 only)
#define KEY_POWER	25 // Power (Sansa e200 only)
#define SCROLL_L	38 // Counter-clockwise
#define SCROLL_R	19 // Clockwise

#define TOUCH_U		 0 // North
#define TOUCH_UR	 1 // North-East
#define TOUCH_R		 2 // East
#define TOUCH_DR	 3 // South-East
#define TOUCH_D		 4 // South
#define TOUCH_DL	 5 // South-West
#define TOUCH_L		 6 // West
#define TOUCH_UL	 7 // North-West

#define KEY_NULL	-1 // No key event
#define TOUCH_NULL	-1 // No wheel touch event

#define KEYCODE(a)	(a & 0x7f) // Use to get keycode of scancode.
#define KEYSTATE(a)	(a & 0x80) // Check if key is pressed or lifted

// For wheel scrolling
#define SCROLL_MOD_NUM	13 // 100 / 8 = 12.5

#define SCROLL_MOD(n) \
  ({ \
    static int scroll_count = 0; \
    int use = 0; \
    if (++scroll_count >= n) { \
      scroll_count -= n; \
      use = 1; \
    } \
    (use == 1); \
  })

// For rapid key-presses
#define RAPID_FIRE_NUM	5 // Arbitrary number (very rapid)

#define RAPID_FIRE(n) \
  ({ \
    static int press_count = 0; \
    int use = 0; \
    if (++press_count >= n) { \
      press_count -= n; \
      use = 1; \
    } \
    (use == 1); \
  })

