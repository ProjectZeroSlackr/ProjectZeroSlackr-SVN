/*
 * keyboard_x.c
 *
 * keyboard emulation
 */

/* $Id: keyboard_x.c,v 1.4 2000/11/25 15:01:06 nyef Exp $ */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>

#include "types.h"

/* hack interface to ui code */
typedef void (*keyhook)(Display *display, XEvent *event);
void ui_set_keyboard_hook(keyhook hook);

/* hack interface to apple2 code */
extern void *apple2;
void apple2_press_keyboard(void *apple2, u8 key);
void sc3k_keyboard_event(u8 code);
void msx_keyboard_event(u8 code);

u8 keysym_translate[0x100] = {
     0,  0,  0,  0,  0,  0,  0,  0, /* 00-07 */
     0,  0,  0,  0,  0,  0,  0,  0, /* 08-0f */
     0,  0,  0,  0,  0,  0,  0,  0, /* 10-17 */
     0,  0,  0,  0,  0,  0,  0,  0, /* 18-1f */
    55,  0,  0,  0,  0,  0,  0,  0, /* 20-27 */
     0,  0,  0,  0, 50, 11, 51, 52, /* 28-2f */
    10,  1,  2,  3,  4,  5,  6,  7, /* 30-37 */
     8,  9,  0, 38,  0, 12,  0,  0, /* 38-3f */
     0, 29, 47, 45, 31, 17, 32, 33, /* 40-47 */
    34, 22, 35, 36, 37, 49, 48, 23, /* 48-4f */
    24, 15, 18, 30, 19, 21, 46, 16, /* 50-57 */
    44, 20, 43,  0,  0,  0,  0,  0, /* 58-5f */
     0, 29, 47, 45, 31, 17, 32, 33, /* 60-67 */
    34, 22, 35, 36, 37, 49, 48, 23, /* 68-6f */
    24, 15, 18, 30, 19, 21, 46, 16, /* 70-77 */
    44, 20, 43,  0,  0,  0,  0,  0, /* 78-7f */
};

/*
 * FIXME: This table is specific to the Apple ][ driver, and really belongs
 * there (as it stands, it's duplicated in every OS-dependant keyboard
 * interface file, which is wrong).
 */

u8 decode_key[56][4] = {
    {0x00, 0x00, 0x00, 0x00}, /* 00 - not used */
    { '1',  '!',  '1',  '!'}, /* 01 */
    { '2',  '"',  '2',  '"'}, /* 02 */
    { '3',  '#',  '3',  '#'}, /* 03 */
    { '4',  '$',  '4',  '$'}, /* 04 */
    { '5',  '%',  '5',  '%'}, /* 05 */
    { '6',  '&',  '6',  '&'}, /* 06 */
    { '7',  '\'',  '7',  '\''}, /* 07 */
    { '8',  '(',  '8',  '('}, /* 08 */
    { '9',  ')',  '9',  ')'}, /* 09 */
    { '0',  '0',  '0',  '0'}, /* 10 */
    { ':',  '*',  ':',  '*'}, /* 11 */
    { '-',  '=',  '-',  '='}, /* 12 */
    {0x00, 0x00, 0x00, 0x00}, /* 13 - reset (not used) */
    {0x1b, 0x1b, 0x1b, 0x1b}, /* 14 */
    { 'Q',  'Q', 0x11, 0x11}, /* 15 */
    { 'W',  'W', 0x17, 0x17}, /* 16 */
    { 'E',  'E', 0x05, 0x05}, /* 17 */
    { 'R',  'R', 0x12, 0x12}, /* 18 */
    { 'T',  'T', 0x14, 0x14}, /* 19 */
    { 'Y',  'Y', 0x19, 0x19}, /* 20 */
    { 'U',  'U', 0x15, 0x15}, /* 21 */
    { 'I',  'I', 0x09, 0x09}, /* 22 */
    { 'O',  'O', 0x0f, 0x0f}, /* 23 */
    { 'P',  '@', 0x10, 0x10}, /* 24 */
    {0x00, 0x00, 0x00, 0x00}, /* 25 - repeat (not used) */
    {0x0d, 0x0d, 0x0d, 0x0d}, /* 26 */
    {0x00, 0x00, 0x00, 0x00}, /* 27 - ??? (can't find it) */
    {0x00, 0x00, 0x00, 0x00}, /* 28 - control (not used) */
    { 'A',  'A', 0x01, 0x01}, /* 29 */
    { 'S',  'S', 0x13, 0x13}, /* 30 */
    { 'D',  'D', 0x04, 0x04}, /* 31 */
    { 'F',  'F', 0x06, 0x06}, /* 32 */
    { 'G',  'G', 0x07, 0x07}, /* 33 */
    { 'H',  'H', 0x08, 0x08}, /* 34 */
    { 'J',  'J', 0x0a, 0x0a}, /* 35 */
    { 'K',  'K', 0x0b, 0x0b}, /* 36 */
    { 'L',  'L', 0x0c, 0x0c}, /* 37 */
    { ';',  '+',  ';',  '+'}, /* 38 */
    {0x08, 0x08, 0x08, 0x08}, /* 39 */
    {0x15, 0x15, 0x15, 0x15}, /* 40 */
    {0x00, 0x00, 0x00, 0x00}, /* 41 - ??? (can't find it) */
    {0x00, 0x00, 0x00, 0x00}, /* 42 - ??? (can't find it) */
    { 'Z',  'Z', 0x1a, 0x1a}, /* 43 */
    { 'X',  'X', 0x18, 0x18}, /* 44 */
    { 'C',  'C', 0x03, 0x03}, /* 45 */
    { 'V',  'V', 0x16, 0x16}, /* 46 */
    { 'B',  'B', 0x02, 0x02}, /* 47 */
    { 'N',  '^', 0x0e, 0x0e}, /* 48 */
    { 'M',  ']', 0x0d, 0x1d}, /* 49 */
    { ',',  '<',  ',',  '<'}, /* 50 */
    { '.',  '>',  '.',  '>'}, /* 51 */
    { '/',  '?',  '/',  '?'}, /* 52 */
    {0x00, 0x00, 0x00, 0x00}, /* 53 - ??? (can't find it) */
    {0x00, 0x00, 0x00, 0x00}, /* 54 - ??? (can't find it) */
    { ' ',  ' ',  ' ',  ' '}, /* 55 */
};

void kb_event(Display *display, XEvent *event)
{
    KeySym keysym;
    Modifiers dummy;
    u8 key_index;
    int modifiers;

    if ((event->type != KeyPress) && (event->type != KeyRelease)) {
	return;
    }

    modifiers = event->xkey.state & ShiftMask;
    modifiers |= (event->xkey.state & ControlMask) >> 1;
    
    XtTranslateKeycode(display, event->xkey.keycode, 0, &dummy, &keysym);

    key_index = 0;
    
    if ((keysym & 0xff00) == 0x0000) { /* Latin1 charset */
	if (!(keysym < 0x80)) {
	    return;
	}
	key_index = keysym_translate[keysym & 0xff];
    }

    if ((keysym & 0xff00) == 0xff00) { /* miscellaneous keys */
	if (keysym == XK_Escape) {
	    key_index = 14;
	} else if (keysym == XK_Return) {
	    key_index = 26;
	} else if ((keysym == XK_Left) || (keysym == XK_KP_Left)) {
	    key_index = 39;
	} else if ((keysym == XK_Right) || (keysym == XK_KP_Right)) {
	    key_index = 40;
	}
    }

    if (key_index) {
	/*
	 * NOTE: We can get away with calling both handler functions because
	 * they both operate on static data. We really should be using a
	 * registered callback, but that would require more work than I feel
	 * like doing at the present time.
	 */
	if (event->type == KeyPress) {
	    apple2_press_keyboard(apple2, decode_key[key_index][modifiers]);
	}
	sc3k_keyboard_event(key_index | ((event->type != KeyPress) << 7));
	msx_keyboard_event(key_index | ((event->type != KeyPress) << 7));
    }
}

void kb_init(void)
{
    ui_set_keyboard_hook(kb_event);
}

/*
 * $Log: keyboard_x.c,v $
 * Revision 1.4  2000/11/25 15:01:06  nyef
 * added support for the msx keyboard
 *
 * Revision 1.3  2000/11/24 18:56:26  nyef
 * added support for the sc3k keyboard
 *
 * Revision 1.2  2000/03/06 03:01:49  nyef
 * fixed space bar key
 *
 * Revision 1.1  2000/03/06 00:38:13  nyef
 * Initial revision
 *
 */
