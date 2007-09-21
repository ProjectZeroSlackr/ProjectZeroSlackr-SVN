/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 * Four-Button Keyboard concept by Jonathan Bettencourt (jonrelay)
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

/*
	/!\ Careful!
	This fourbtnkbd.c is *not* the same as jonrelay's original fourbtnkbd.c!
*/

#include <string.h>
#include "../pz.h"
#include "../textinput.h"

const unsigned long int text_fbkMSlongs[] = {
	0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10, 0x11000012, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x13141500, 0x16171819, 0x1A1B1C1D,
	0x1E1F2021, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x22232425, 0x00000000,
	0x00000000, 0x00000000, 0x26272829, 0x2A2B2C2D, 0x2E2F302D, 0x31323334, 0x35363738,
	0x00000000, 0x00000000, 0x00003F40, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x393A3B2D, 0x3C3D3E2D, 0x00000000, 0x00004143, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00004200, 0x00000000, 0x00000000, 0x00004445,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	
	0x4748494A, 0x46464646, 0x46464646, 0x46464646, 0x4B4C4646, 0x46464646, 0x46464646 };
const unsigned long int text_fbkCOlongs[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200800, 0x41614262, 0x43634464,
	0x45654666, 0x47674868, 0x49694A6A, 0x4B6B4C6C, 0x4D6D4E6E, 0x4F6F5070, 0x51715272,
	0x53735474, 0x55755676, 0x57775878, 0x59795A7A, 0x0000000A, 0x00000000, 0x00000000,
	0x00000000, 0x30313233, 0x34353637, 0x38394142, 0x43444546, 0x2E2C213F, 0x3A3B2722,
	0x2D2F2829, 0x5B5D7B7D, 0x2B2D2A5E, 0x40232526, 0x24A2A3A5, 0x00000000, 0x3C3E3DB1,
	0x2F5C7C5F, 0x7E60B4A8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xC0E0C8E8, 0xCCECD2F2, 0xD9F90000, 0xD0F0DEFE, 0xC1E1C9E9, 0xCDEDD3F3, 0xDAFADDFD,
	0x00000000, 0x00000000, 0xC3E3D5F5, 0xC6E60000, 0xC5E5C7E7, 0xD1F1D8F8, 0xDFA7AEA9,
	0xA1BFABBB, 0xC4E4CBEB, 0xCFEFD6F6, 0xDCFC00FF, 0xC2E2CAEA, 0xCEEED4F4, 0xDBFB0000,
	0xB0B9B2B3, 0xBCBDBEB7, 0xB7B6AFAD, 0xD7F7A6AC, 0xA4B5BAAA, 0xB8AFB0B7, 0x090B0D7F,
	
	0x00000000, 0x30313233, 0x34353637, 0x38392E2D, 0x0000080A, 0x452E2B2D, 0x2B2A2F5E };
const char * text_fbkModeNames[] = {
	"Home",		"A-H",		"I-P",		"Q-X",		"YZEtc",	"AB",		"CD",
	"EF",		"GH",		"IJ",		"KL",		"MN",		"OP",		"QR",
	"ST",		"UV",		"WX",		"YZ",		"Etc",		"Num",		"Punct",
	"Symb",		"0123",		"4567",		"89AB",		"CDEF",		".,!?",		":;\'\"",
	"-/()",		"[]{}",		"+-*^",		"@#%&",		"$\xA2\xA3\xA5", "Etc2", "<>=\xB1",
	"/\\|_",	"~`\xB4\xA8", "Accen",	"`",		"\xB4",		"\xA8^~&",	"\xE5\xF1\xDF\xBF\xA9",
	"\xE0\xE8", "\xEC\xF2", "\xF9Supr", "\xF0\xFE", "\xE1\xE9", "\xED\xF3", "\xFA\xFD",
	"\xA8",		"^",		"\xE3\xF5", "LgPtX",	"\xE5\xE7", "\xF1\xF8", "\xDF\xA7\xAE\xA9",
	"IPnct",	"\xE4\xEB", "\xEF\xF6", "\xFC\xFFMth", "\xE2\xEA", "\xEE\xF4", "\xFB""AcXC",
	"Super",	"Fract",	"PnctX",	"MathX",	"PctX2",	"AccX",		"Ctrl",
	
	"#Home",	"0123",		"4567",		"89.-",		"Etc",		"Symb",		"Math" };
const char * text_fbkCtrlCharNames[] = {
	"Null", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "Bell",
	"Bksp", "Tab", "Newln", "VTab", "FrmFd", "CRtn", "SOut", "SIn",
	"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
	"CAN", "EM", "SUB", "Esc", "FS", "GS", "RS", "US", "Space" };
unsigned long int text_fbkCurrModeSwitch = 0x01020304;
unsigned long int text_fbkCurrCharOutput = 0;

void text_fbk_print_status(GR_GC_ID gc, char * s, int x, int y)
{
	GrText(text_get_bufwid(), gc, x, y, s, -1, GR_TFASCII);
}

void text_fbk_print_mode_status(GR_GC_ID gc)
{
	unsigned long int c, ch, m, mh;
	int i;
	char cho[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-16, text_get_width(), text_get_height()-16);
	m = text_fbkCurrModeSwitch;
	c = text_fbkCurrCharOutput;
	for (i=0; i<4; i++) {
		ch = (c / 16777216);
		mh = (m / 16777216);
		if (ch != 0) {
			if (ch < 33) {
				text_fbk_print_status(gc, (char *)text_fbkCtrlCharNames[ch], (text_get_width()/4)*i+1, text_get_height()-4);
			} else {
				cho[0]=ch;
				cho[1]=0;
				text_fbk_print_status(gc, cho, (text_get_width()/4)*i+1, text_get_height()-4);
			}
		} else {
			text_fbk_print_status(gc, (char *)text_fbkModeNames[mh], (text_get_width()/4)*i+1, text_get_height()-4);
		}
		m = (m % 16777216)*256;
		c = (c % 16777216)*256;
	}
}

void text_fbk_print_mode_status_4g(GR_GC_ID gc)
{
	unsigned long int c, ch, m, mh;
	int i, x, y;
	char cho[2];
	GrLine(text_get_bufwid(), gc, 0, text_get_height()-28, text_get_width(), text_get_height()-28);
	m = text_fbkCurrModeSwitch;
	c = text_fbkCurrCharOutput;
	for (i=0; i<4; i++) {
		switch (i) {
		case 0: x=                       1; y=text_get_height()-10; break;
		case 1: x=(text_get_width()/3)  +1; y=text_get_height()-16; break;
		case 2: x=(text_get_width()/3)  +1; y=text_get_height()- 4; break;
		case 3: x=(text_get_width()/3)*2+1; y=text_get_height()-10; break;
		}
		ch = (c / 16777216);
		mh = (m / 16777216);
		if (ch != 0) {
			if (ch < 33) {
				text_fbk_print_status(gc, (char *)text_fbkCtrlCharNames[ch], x, y);
			} else {
				cho[0]=ch;
				cho[1]=0;
				text_fbk_print_status(gc, cho, x, y);
			}
		} else {
			text_fbk_print_status(gc, (char *)text_fbkModeNames[mh], x, y);
		}
		m = (m % 16777216)*256;
		c = (c % 16777216)*256;
	}
}

void text_fbkSetMode(int m)
{
	if ((m < 77) && (m >= 0)) {
		text_fbkCurrModeSwitch = text_fbkMSlongs[m];
		text_fbkCurrCharOutput = text_fbkCOlongs[m];
	} else {
		text_fbkCurrModeSwitch = (text_get_numeric_mode() != 0)?0x4748494A:0x01020304;
		text_fbkCurrCharOutput = 0;
	}
}

void text_fbkPushButton(int b)
{
	unsigned long int c, ch, m;
	int i;
	m = text_fbkCurrModeSwitch;
	c = text_fbkCurrCharOutput;
	for (i=b; i<3; i++) {
		m /= 256;
		c /= 256;
	}
	text_fbkSetMode(m % 256);
	ch = (c % 256);
	if (ch != 0) {
		text_output_char(ch);
	}
}

