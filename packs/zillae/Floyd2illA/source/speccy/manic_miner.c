/*
 * Copyright (C) 2005 Matthew Westcott
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

#include "../pz.h"
#include "speccy.h"

#include "manic_miner_sna.h"

#define KEMPSTON_FIRE   0x10
#define KEMPSTON_UP     0x08
#define KEMPSTON_DOWN   0x04
#define KEMPSTON_LEFT   0x02
#define KEMPSTON_RIGHT  0x01

static byte manic_miner_kempston_port; /* bitfield of current directions of kempston joystick */

void manic_miner_draw_header() {
	pz_draw_header("Manic Miner");
}

void manic_miner_out_z80(register word Port,register byte Value)
{
}

byte manic_miner_in_z80(register word Port)
{
	if (Port & 0x00e0) {
		/* ignore all ports except for Kempston joystick */
		return 0xff;
	} else {
		/* Kempston joystick responds to IN requests where bits 7,6,5 are reset */
		return manic_miner_kempston_port;
	}
}

static int manic_miner_handle_key_event(GR_EVENT *event)
{
    int ret = 0;
    switch (event->type) {
    case GR_EVENT_TYPE_KEY_DOWN:
        switch (event->keystroke.ch) {
        case 'm': /* Menu button */
			speccy_end();
			break;
        case '\r': /* Action button */
			manic_miner_kempston_port |= KEMPSTON_FIRE;
			break;
        case 'w': /* Rewind button */
			manic_miner_kempston_port |= KEMPSTON_LEFT;
			break;
        case 'f': /* Fast forward button */
			manic_miner_kempston_port |= KEMPSTON_RIGHT;
			break;
        case 'l': /* Wheel left */
        case 'r': /* Wheel right */
        case 'd': /* Play/pause button */
        default:
            ret |= KEY_UNUSED; /* allow a default key action */
            break;
        }
        break;
    case GR_EVENT_TYPE_KEY_UP:
        switch (event->keystroke.ch) {
        case '\r': /* Action button */
			manic_miner_kempston_port &= ~KEMPSTON_FIRE;
			break;
        case 'w': /* Rewind button */
			manic_miner_kempston_port &= ~KEMPSTON_LEFT;
			break;
        case 'f': /* Fast forward button */
			manic_miner_kempston_port &= ~KEMPSTON_RIGHT;
			break;
		default:
            ret |= KEY_UNUSED;
            break;
        }
    default:
        ret |= EVENT_UNUSED; /* we didn't ask for this event, go away */
    }
    return ret;
}

void manic_miner_setup()
{
	manic_miner_kempston_port = 0;
	speccy_load_sna(manic_miner_sna);
}

static SPECCY_GAME manic_miner = {
	&manic_miner_setup,
	&manic_miner_draw_header,
	&manic_miner_out_z80,
	&manic_miner_in_z80,
	&manic_miner_handle_key_event
};

void manic_miner_start()
{
	new_speccy_window(manic_miner);
}
