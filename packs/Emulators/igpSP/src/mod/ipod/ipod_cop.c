/*
 * Last updated: Jun 2, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo, Various
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

#include "ipod_common.h"

#define COP_HANDLER			0x4001501C
#define COP_STATUS			0x40015020
#define COP_CONTROL			0x60007004

#define COP_RUNNING			(1 << 0)
#define COP_LINE_REQ		(1 << 1)
#define COP_LINE_REQ_CLEAR	(2 << 1)

#define ipod_cop_clear_frameready() \
	outl(inl(COP_STATUS) &~ COP_LINE_REQ_CLEAR, COP_STATUS)

// Waits for main update_screen to call "ipod_cop_update_screen();"
static void ipod_cop_sync_screen()
{
	while (inl(COP_STATUS) & COP_RUNNING) // COP is active
	{
		// Get stuck in loop until told to update screen
		while ((inl(COP_STATUS) & COP_LINE_REQ) == 0);
		ipod_update_screen();
		outl(inl(COP_STATUS) &~ COP_LINE_REQ, COP_STATUS);
	}	
}

void ipod_cop_update_screen()
{
	// Envokes ipod_cop_sync_screen() to synchronize
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}

void ipod_cop_execute(void (*function)())
{
	outl((unsigned int)function, COP_HANDLER);
	outl(0x0, COP_CONTROL);
}

void ipod_init_cop()
{
	ipod_cop_clear_frameready();
	ipod_cop_execute(ipod_cop_sync_screen);	
	outl(COP_RUNNING, COP_STATUS); // Start COP?
}

void ipod_exit_cop()
{
	outl(inl(COP_STATUS) &~ COP_RUNNING, COP_STATUS); // Stop COP
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS);
}
