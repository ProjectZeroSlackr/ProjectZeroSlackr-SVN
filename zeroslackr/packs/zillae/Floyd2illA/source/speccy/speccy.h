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

#ifndef SPECCY_H
#define SPECCY_H

#include "Z80.h"
#include "../pz.h"

/* Entry points into the Spectrum emulation framework */

/* Game-specific hooks into emulation runtime */
typedef struct
{
	void (*setup)();
	void (*draw_header)();
	void (*out_z80)(register word Port,register byte Value);
	byte (*in_z80)(register word Port);
	int (*handle_key_event)(GR_EVENT *event);
} SPECCY_GAME;

void new_speccy_window(SPECCY_GAME game); /* Start running emulation of the specified game */
void speccy_end(); /* Shut down */
void speccy_load_sna(unsigned char *sna); /* Load Spectrum state from the supplied buffer containing an SNA format snapshot (48K only) */

#endif /* SPECCY_H */
