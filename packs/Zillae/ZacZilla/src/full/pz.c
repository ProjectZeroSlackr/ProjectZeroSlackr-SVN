/*
 * Copyright (C) 2004 Bernard Leach
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "pz.h"
#include "browser.h"
#include "slider.h"
#include "ipod.h"
#include "vectorfont.h"
#ifdef MPDC
#include "mpdc/mpdc.h"
#endif

int pz_setting_debounce = 0;
extern int gettxt(void);
/* globals */
GR_SCREEN_INFO screen_info;
long hw_version;

GR_WINDOW_ID root_wid;
GR_GC_ID root_gc;

struct pz_window {
	GR_WINDOW_ID wid;
	void (*draw)(void);
	int (*keystroke)(GR_EVENT * event);
};

/* int BACKLIGHT_TIMER = 10; // in seconds */

#ifdef IPOD
int pz_startup_contrast = -1;

#endif
//static unsigned long int last_keypress_event = 0;

int usb_connected = 0;
int fw_connected = 0;
ttk_timer connection_timer = 0;

/*
 * polyfont - vector polyline font
 *
 *  Copyright (C) 2005 Scott Lawrence and Joshua Oreman
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




/** The points are specified as follows: 0xXXYY
 * XX and YY indicate where the point is. The values
 * are 00 for the top / left or ff for the bottom / right.

  00 40 80 b0 ff
  --------------+
  00 27 01 28 02| 00
  18 -- -- -- 17| 2a
  03 vv 04 -- 05| 56 <-+ Between these rows: 22 between 3 and 6,
  06 ^^ 34 07 08| 80 <-+ 23 between vv and ^^
  19 -- 09 -- 10| aa
  11 24 12 25 13| d6 <-+ Between these rows: 32 between 24 and 29,
  14 29 15 30 16| ff <-+ 26 between 12 and 15, 31 between 25 and 30
*/
#define PT0  0x0000
#define PT1  0x8000
#define PT2  0xff00
#define PT3  0x0056
#define PT4  0x8056
#define PT5  0xff56
#define PT6  0x0080
#define PT7  0xb080
#define PT8  0xff80
#define PT9  0x80aa
#define PT10 0xffaa
#define PT11 0x00d6
#define PT12 0x80d6
#define PT13 0xffd6
#define PT14 0x00ff
#define PT15 0x80ff
#define PT16 0xffff
#define PT17 0xff2a
#define PT18 0x002a
#define PT19 0x00aa
#define PT22 0xb064
#define PT23 0x4064
#define PT24 0xb0d6
#define PT25 0x40d6
#define PT26 0x80e4
#define PT27 0x4000
#define PT28 0xb000
#define PT29 0x40ff
#define PT30 0xb0ff
#define PT31 0xb0e4
#define PT32 0x40e4
/* PT33 is out-of-bounds */
#define PT34 0x8080

#define SKIP 0x10000

struct vector_polystruct 
{
    int nv;
    int v[16];
};

/* digits for text rendering */
static struct vector_polystruct V_digits[] = {
    /* 0 */ {5, {PT0, PT2, PT16, PT14, PT0, PT4|SKIP, PT9}},
    /* 1 */ {2, {PT1, PT15}},
    /* 2 */ {6, {PT0, PT2, PT8, PT6, PT14, PT16}},
    /* 3 */ {6, {PT0, PT2, PT16, PT14, PT6|SKIP, PT8}},
    /* 4 */ {5, {PT0, PT6, PT8, PT2|SKIP, PT16}},
    /* 5 */ {6, {PT2, PT0, PT6, PT8, PT16, PT14}},
    /* 6 */ {5, {PT0, PT14, PT16, PT8, PT6}},
    /* 7 */ {3, {PT0, PT2, PT16}},
    /* 8 */ {8, {PT8, PT2, PT0, PT6, PT8, PT16, PT14, PT6}},
    /* 9 */ {5, {PT8, PT6, PT0, PT2, PT16}},
};

/* letters for alphabetical (English latin font) */
static struct vector_polystruct V_alpha[] =
{
    /* A */ {7, {PT14, PT3, PT1, PT5, PT16, PT6|SKIP, PT8}},
    /* B */ {10, {PT0, PT2, PT5, PT7, PT6, PT7, PT10, PT16, PT14, PT0}},
    /* C */ {4, {PT2, PT0, PT14, PT16}},
    /* D */ {7, {PT14, PT0, PT1, PT17, PT13, PT15, PT14}},
    /* E */ {6, {PT2, PT0, PT14, PT16, PT6|SKIP, PT7}},
    /* F */ {5, {PT2, PT0, PT14, PT6|SKIP, PT7}},
    /* G */ {7, {PT5, PT2, PT0, PT14, PT16, PT10, PT9}},
    /* H */ {6, {PT0, PT14, PT6|SKIP, PT8, PT2|SKIP, PT16}},
    /* I */ {6, {PT0, PT2, PT1|SKIP, PT15, PT14|SKIP, PT16}},
    /* J */ {6, {PT0, PT2, PT28|SKIP, PT30, PT14, PT11}},
    /* K */ {5, {PT0, PT14, PT2|SKIP, PT6, PT16}},
    /* L */ {3, {PT0, PT14, PT16}},
    /* M */ {5, {PT14, PT0, PT4, PT2, PT16}},
    /* N */ {4, {PT14, PT0, PT16, PT2}},
    /* O */ {5, {PT0, PT2, PT16, PT14, PT0}},
    /* P */ {5, {PT14, PT0, PT2, PT8, PT6}},
    /* Q */ {8, {PT0, PT2, PT13, PT15, PT14, PT0, PT12|SKIP, PT16}},
    /* R */ {6, {PT14, PT0, PT2, PT8, PT6, PT16}},
    /* S */ {6, {PT2, PT0, PT6, PT8, PT16, PT14}},
    /* T */ {4, {PT0, PT2, PT1|SKIP, PT15}},
    /* U */ {4, {PT0, PT14, PT16, PT2}},
    /* V */ {3, {PT0, PT15, PT2}},
    /* W */ {5, {PT0, PT14, PT12, PT16, PT2}},
    /* X */ {4, {PT0, PT16, PT2|SKIP, PT14}},
    /* Y */ {5, {PT0, PT4, PT2, PT4|SKIP, PT15}},
    /* Z */ {4, {PT0, PT2, PT14, PT16}},
};

/* punctuation */
static struct vector_polystruct V_punctuation[] =
{
    /* ! */ {8, {PT9, PT1, PT2, PT9, PT26|SKIP, PT29, PT30, PT26}},
    /* : */ {10, {PT1, 0xc02a, PT4, 0x402a, PT1,
		SKIP | 0x80ab, 0xc0d5, 0x80fc, 0x40d5, 0x80ab}},
    /* / */ {2, {PT14, PT2}},
    /* \ */ {2, {PT16, PT0}},
    /* _ */ {2, {PT14, PT16}},
    /* . */ {5, {PT12, PT31, PT15, PT32, PT12}},
    /* , */ {6, {PT12, PT31, PT15, PT29, PT32, PT12}},
    /* [ */ {4, {PT27, PT0, PT14, PT29}},
    /* ] */ {4, {PT28, PT2, PT16, PT30}},
    /* ' */ {4, {PT0, PT1, PT3, PT0}},
    /* - */ {2, {PT6, PT8}},
    /* & */ {7, {PT16, PT3, PT1, PT5, PT11, PT15, PT31}},
    /* ( */ {4, {PT27, PT18, PT11, PT29}},
    /* ) */ {4, {PT28, PT17, PT13, PT30}},
    /* % */ {12, {PT0, 0x6000, 0x6060, 0x0060, PT0,  PT16|SKIP, 0xa0ff, 0xa0a0, 0xffa0, PT16,  PT2|SKIP, PT14}},
    /* ~ */ {4, {PT6, PT23, 0xb096, PT8}},
    /* ` */ {4, {PT18, PT4, 0x902a, PT27}},
    /* @ */ {10, {PT16, PT2, PT0, PT14, PT16, 0xb0aa, 0xb056, 0x4056, 0x40aa, 0xb0aa}},
    /* # */ {8, {0x4000, 0x40ff, 0xc000|SKIP, 0xc0ff, 0x0060|SKIP, 0xff60, 0x00a0|SKIP, 0xffa0}},
    /* $ */ {8, {PT17, PT18, PT6, PT8, PT13, PT11, PT1|SKIP, PT15}},
    /* ^ */ {3, {PT3, PT1, PT5}},
    /* * */ {4, {0xb056, 0x40aa, 0x4056|SKIP, 0xb0aa}},
    /* ; */ {11, {PT1, 0xc02a, PT4, 0x402a, PT1,  PT9|SKIP, 0xc0d6, PT15, 0x40ff, 0x40d6, PT9}},
    /* ? */ {9, {PT18, PT1, PT17, PT34, PT9,  PT12|SKIP, PT29, PT30, PT12}},
    /* = */ {4, {PT3, PT5,  PT19|SKIP, PT10}},
    /* + */ {4, { 0x8822, 0x88dd, 0x0088|SKIP, 0xff88 }},
};

/* specials */
static struct vector_polystruct V_specials[] =
{
    /* ^ */ {4, {0x00ee, 0x8888, 0xffee, 0x00ee}},
    /* < */ {4, {0xff22, 0x6688, 0xffdd, 0xff22}},
    /* v */ {4, {0x0011, 0x8888, 0xff11, 0x0011}},
    /* > */ {4, {0x0022, 0xbb88, 0x00dd, 0x0022}},
};

static struct vector_polystruct *lookup[] = {
    [0x30] = 	V_digits + 0,  V_digits + 1,  V_digits + 2,  V_digits + 3, 
		V_digits + 4,  V_digits + 5,  V_digits + 6,  V_digits + 7,
		V_digits + 8,  V_digits + 9,

    [0x41] = 	V_alpha + 0,  V_alpha + 1,  V_alpha + 2,  V_alpha + 3, 
		V_alpha + 4,  V_alpha + 5,  V_alpha + 6,  V_alpha + 7, 
		V_alpha + 8,  V_alpha + 9,  V_alpha + 10, V_alpha + 11,
             	V_alpha + 12, V_alpha + 13, V_alpha + 14, V_alpha + 15, 
		V_alpha + 16, V_alpha + 17, V_alpha + 18, V_alpha + 19, 
		V_alpha + 20, V_alpha + 21, V_alpha + 22, V_alpha + 23,
             	V_alpha + 24, V_alpha + 25,

    [0x61] = 	V_alpha + 0,  V_alpha + 1,  V_alpha + 2,  V_alpha + 3, 
		V_alpha + 4,  V_alpha + 5,  V_alpha + 6,  V_alpha + 7, 
		V_alpha + 8,  V_alpha + 9,  V_alpha + 10, V_alpha + 11,
             	V_alpha + 12, V_alpha + 13, V_alpha + 14, V_alpha + 15, 
		V_alpha + 16, V_alpha + 17, V_alpha + 18, V_alpha + 19, 
		V_alpha + 20, V_alpha + 21, V_alpha + 22, V_alpha + 23,
             	V_alpha + 24, V_alpha + 25,

    ['!'] = V_punctuation + 0, 		[':'] = V_punctuation + 1,
    ['/'] = V_punctuation + 2,		['\\'] = V_punctuation + 3, 
    ['_'] = V_punctuation + 4, 		['.'] = V_punctuation + 5,
    [','] = V_punctuation + 6, 		['['] = V_punctuation + 7, 
    [']'] = V_punctuation + 8,		['\''] = V_punctuation + 9, 
    ['-'] = V_punctuation + 10, 	['&'] = V_punctuation + 11,
    ['('] = V_punctuation + 12, 	[')'] = V_punctuation + 13, 
    ['%'] = V_punctuation + 14, 	['~'] = V_punctuation + 15, 
    ['`'] = V_punctuation + 16, 	['@'] = V_punctuation + 17,
    ['#'] = V_punctuation + 18, 	['$'] = V_punctuation + 19, 
    ['^'] = V_punctuation + 20, 	['*'] = V_punctuation + 21, 
    [';'] = V_punctuation + 22, 	['?'] = V_punctuation + 23,
    ['='] = V_punctuation + 24,		['+'] = V_punctuation + 25,

    [250] = 	V_specials + 0, V_specials + 1, V_specials + 2, V_specials + 3
};


// Returns the number of pixels wide it was.
static void render_polystruct (ttk_surface srf, struct vector_polystruct *v, int x, int y, int cw, int ch,
			       ttk_color col) 
{
    int i;
    int lx = -1, ly = -1;

    cw--; ch--; // if a char is 5px wide the left edge will be at x+0 and right at x+4... cw is used below as
                // x+cw being right side, etc.

    for (i = 0; i < v->nv; i++) {
	int vv = v->v[i] & ~SKIP;
	if (v->v[i] & SKIP)
	    lx = ly = -1;
	if (lx != -1 && ly != -1) {
	    ttk_line (srf, lx, ly, x + (vv >> 8) * cw / 0xff, y + (vv & 0xff) * ch / 0xff, col);
	}
	lx = x + (vv >> 8) * cw / 0xff;
	ly = y + (vv & 0xff) * ch / 0xff;
    }
}

void pz_vector_string (ttk_surface srf, const char *string, int x, int y, int cw, int ch, int kern, ttk_color col) 
{
    int sx = x;
    while (*string) {
	if (*string == '\n') {
	    x = sx;
	    y += ch + (ch/4);
	} else {
	    if (lookup[*(unsigned const char *)string])
		render_polystruct (srf, lookup[*(unsigned const char *)string],
				x, y, cw, ch, col);
	    x += cw + (cw/8) + 1 + kern;
	}
	string++;
    }
}

void draw_lack_of_text_input_method (TWidget *this, ttk_surface srf) 
{
    ttk_text (srf, ttk_menufont, this->x + 3, this->y + 1, ttk_makecol (BLACK),
              _("No text input method selected."));
    ttk_text (srf, ttk_menufont, this->x + this->w - ttk_text_width (ttk_menufont, _("[press a key]")) - 3,
              this->y + 3 + ttk_text_height (ttk_menufont), ttk_makecol (BLACK), _("[press a key]"));
}

int close_lack_of_text_input_method (TWidget *this, int button, int time) 
{
    ttk_input_end();
    return 0;
}

TWidget *new_lack_of_text_input_method() 
{
    TWidget *ret = ttk_new_widget (0, 0);
    ret->w = ttk_screen->w;
    ret->h = 2 * (ttk_text_height (ttk_menufont) + 2);
    ret->draw = draw_lack_of_text_input_method;
    ret->button = close_lack_of_text_input_method;
    return ret;
}

static TWidget *(*handler)();
static TWidget *(*handler_n)();

void pz_register_input_method (TWidget *(*h)()) { handler = h; }
void pz_register_input_method_n (TWidget *(*h)()) { handler_n = h; }
int pz_start_input() { return ttk_input_start (handler()); }
int pz_start_input_n() { return ttk_input_start (handler_n()); }
int pz_start_input_for (TWindow *win) { return ttk_input_start_for (win, handler()); }
int pz_start_input_n_for (TWindow *win) { return ttk_input_start_for (win, handler_n()); }
void pz_vector_string_center (ttk_surface srf, const char *string, int x, int y, int cw, int ch, int kern, ttk_color col)
{
    int w = pz_vector_width (string, cw, ch, kern);
    x -= w/2;
    y -= ch/2;
    pz_vector_string (srf, string, x, y, cw, ch, kern, col);
}

int pz_vector_width (const char *string, int cw, int ch, int kern)
{
    return (strlen (string) * cw) + ((strlen (string) - 1) * (cw/8 + kern + 1));
}




void check_connection() 
{
    int temp;
    
    if ((temp = usb_is_connected()) != usb_connected) {
	usb_connected = temp;
	usb_check_goto_diskmode();
    }
    if ((temp = fw_is_connected()) != fw_connected) {
	fw_connected = temp;
	fw_check_goto_diskmode();
    }
    connection_timer = ttk_create_timer (1000, check_connection);
}

int hold_is_on = 0;

extern void new_menu_window();
extern void load_font();
extern void beep(void);

extern void header_update_hold_status( int curr );
void header_timer_update( void );

void poweroff_ipod(void)
{
#ifdef IPOD
	ipod_touch_settings();
#ifdef MPDC
	mpdc_destroy();
#endif
	GrClose();
	printf("\nPowering down.\nPress action to power on.\n");
	execl("/bin/poweroff", "poweroff", NULL);

	printf("No poweroff binary available.  Rebooting.\n");
	execl("/bin/reboot", "reboot", NULL);
	exit(0);
#else
	pz_error ("I don't think you want to reboot your desktop...");
#endif
}

void reboot_ipod(void)
{
#ifdef IPOD
	ipod_touch_settings();
#ifdef MPDC
	mpdc_destroy();
#endif
	GrClose();
	execl("/bin/reboot", "reboot", NULL);
	exit(0);
#else
	pz_error ("I don't think you want me to reboot your desktop...");
#endif
}

void quit_podzilla(void)
{
	ipod_touch_settings();
#ifdef MPDC
	mpdc_destroy();
#endif
	GrClose();
	exit(0);
}

void set_wheeldebounce(void)
{
	pz_setting_debounce = 1;
	ttk_set_scroll_multiplier (1, 1); // for selecting the speed
	new_settings_slider_window(_("Wheel Sensitivity"),
				   WHEEL_DEBOUNCE, 0, 19);
	ttk_windows->w->data = 0x12345678;
}

void pz_event_handler (t_GR_EVENT *ev) 
{
    int sdir = 0;
    if (ttk_windows->w->focus) {
	if (ev->keystroke.ch == 'r')
	    sdir = 1;
	else if (ev->keystroke.ch == 'l')
	    sdir = -1;
	
	switch (ev->type) {
	case GR_EVENT_TYPE_KEY_DOWN:
	    if (sdir)
		ttk_windows->w->focus->scroll (ttk_windows->w->focus, sdir);
	    else
		ttk_windows->w->focus->down (ttk_windows->w->focus, ev->keystroke.ch);
	    break;
	case GR_EVENT_TYPE_KEY_UP:
	    if (!sdir)
		ttk_windows->w->focus->button (ttk_windows->w->focus, ev->keystroke.ch, 0);
	    break;
	}
    }
}

extern void header_fix_hold (void);


static ttk_timer bloff_timer = 0;
static int bl_forced_on = 0;

static void backlight_off() { if (!bl_forced_on) ipod_set_setting (BACKLIGHT, 0); bloff_timer = 0; }
static void backlight_on()  { ipod_set_setting (BACKLIGHT, 1); }

// Reads high score from file in SAVEFILE
int readHighScore(char *SAVEFILE)
{
	int highScore;
	FILE *input;
	if ((input = fopen(SAVEFILE, "r")) == NULL)
	{
		perror(SAVEFILE);
		return -1;
	}
	fscanf(input, "%d", &highScore); 
	fclose(input);
	return highScore;
}

// Writes high score to file in SAVEFILE
void writeHighScore(char *SAVEFILE,int highScore)
{
	FILE *output;
	if ((output = fopen(SAVEFILE, "w")) == NULL)
	{
		perror(SAVEFILE);
		return;
	}
	fprintf(output, "%d", highScore);
	fclose(output);
}

void pz_set_backlight_timer (int sec) 
{
    static int last = BL_OFF;
    if (sec != BL_RESET) last = sec;

    if (last == BL_OFF) {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = 0;
	backlight_off();
    } else if (last == BL_ON) {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = 0;
	backlight_on();
    } else {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = ttk_create_timer (1000*last, backlight_off);
	backlight_on();
    }
}

int held_times[128] = { ['m'] = 500, ['d'] = 1000 }; // key => ms
int held_ignores[128]; // set a char to 1 = ignore its UP event once.
ttk_timer held_timers[128]; // the timers

void backlight_toggle() 
{
    bl_forced_on = !bl_forced_on;
    held_timers['m'] = 0;
    held_ignores['m']++;
}

void (*held_handlers[128])() = { ['m'] = backlight_toggle, ['d'] = /* sleep_ipod */ 0 }; // key => fn

int pz_new_event_handler (int ev, int earg, int time)
{
    static int vtswitched = 0;

    pz_set_backlight_timer (BL_RESET);

    if (pz_setting_debounce && (ttk_windows->w->focus->draw != ttk_slider_draw)) {
	pz_setting_debounce = 0;
	ipod_set_setting (WHEEL_DEBOUNCE, ipod_get_setting (WHEEL_DEBOUNCE));
    }

    switch (ev) {
    case TTK_BUTTON_DOWN:
	switch (earg) {
	case TTK_BUTTON_HOLD:
	    hold_is_on = 1;
	    header_fix_hold();
	    break;
	case TTK_BUTTON_MENU:
	    vtswitched = 0;

	    break;
	}
	if (held_times[earg] && held_handlers[earg]) {
	    if (held_timers[earg]) ttk_destroy_timer (held_timers[earg]);
	    held_timers[earg] = ttk_create_timer (held_times[earg], held_handlers[earg]);
	}
	break;
    case TTK_BUTTON_UP:
	if (held_timers[earg]) {
	    ttk_destroy_timer (held_timers[earg]);
	    held_timers[earg] = 0;
	}
	if (held_ignores[earg]) {
	    held_ignores[earg] = 0;
	    return 1;
	}
	switch (earg) {
	case TTK_BUTTON_HOLD:
	    hold_is_on = 0;
	    header_fix_hold();
	    break;
	case TTK_BUTTON_MENU:
	    if (time > 500 && time < 1000) {
		bl_forced_on = !bl_forced_on;
		if (bl_forced_on)
		    backlight_on();
		return 1;
	    }
	    break;
	case TTK_BUTTON_PREVIOUS:
	    if (ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PLAY)) {
		// vt switch code <<
		printf ("VT SWITCH <<\n");
		vtswitched = 1;
		return 1;
	    } else if (ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_NEXT)) {
		// vt switch code [0]
		printf ("VT SWITCH 0 (N-P)\n");
		vtswitched = 1;
		return 1;
	    } else if (ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		TWindowStack *lastwin = ttk_windows;
		while (lastwin->next) lastwin = lastwin->next;
		if (lastwin->w != ttk_windows->w) {
		//    ttk_move_window (lastwin->w, 0, TTK_MOVE_ABS);
		    printf ("WINDOW CYCLE >>\n");
		} else
		    printf ("WINDOW CYCLE >> DIDN'T\n");
		return 1;
	    }
	    break;
	case TTK_BUTTON_NEXT:
	    if (ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PLAY)) {
		// vt switch code >>
		printf ("VT SWITCH >>\n");
		vtswitched = 1;
		return 1;
	    } else if (ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PREVIOUS)) {
		// vt switch code [0]
		printf ("VT SWITCH 0 (P-N)\n");
		vtswitched = 1;
		return 1;
	    } else if (ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		printf ("WINDOW CYCLE <<\n");
		if (ttk_windows->next) {
		  //  ttk_move_window (ttk_windows->w, 0, TTK_MOVE_END);
		    return 1;
		}
	    }
	    break;
	case TTK_BUTTON_PLAY:
	    if (ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		printf ("WINDOW MINIMIZE\n");
		if (ttk_windows->next) {
		    ttk_windows->minimized = 1;
		    return 1;
		}
	    }
	}
	break;
    }
    return 0; // keep event
}


int pz_unused_handler (int ev, int earg, int time) 
{
    switch (ev) {
    case TTK_BUTTON_UP:
	switch (earg) {
#ifdef MPDC
	case 'f':
	    mpdc_next (mpdz);
	    break;
	case 'w':
	    mpdc_prev (mpdz);
	    break;
	case 'd':
	    mpdc_playpause (mpdz);
	    break;
#endif
	default:
	    break;
	}
	break;
    }
    return 0; // no clickiness
}


typedef struct 
{
    void (*draw)();
    int (*key)(GR_EVENT *);
} legacy_data;

#define _MAKETHIS legacy_data *data = (legacy_data *)this->data

void pz_legacy_construct_GR_EVENT (GR_EVENT *ev, int type, int arg) 
{
    ev->type = type;
    if (ev->type == GR_EVENT_TYPE_KEY_UP || ev->type == GR_EVENT_TYPE_KEY_DOWN) {
	if (arg == TTK_BUTTON_ACTION)
	    arg = '\r';
	ev->keystroke.ch = arg;
	ev->keystroke.scancode = 0;
    }
}
void pz_legacy_draw (TWidget *this, ttk_surface srf) 
{
    _MAKETHIS;
    data->draw(); // it'll be on the window we returned from pz_new_window
}
int pz_legacy_button (TWidget *this, int button, int time)
{
    GR_EVENT ev;
    _MAKETHIS;
    pz_legacy_construct_GR_EVENT (&ev, GR_EVENT_TYPE_KEY_UP, button);
    this->dirty++;
    return data->key (&ev);
}
int pz_legacy_down (TWidget *this, int button)
{
    GR_EVENT ev;
    _MAKETHIS;
    pz_legacy_construct_GR_EVENT (&ev, GR_EVENT_TYPE_KEY_DOWN, button);
    this->dirty++;
    return data->key (&ev);
}
int pz_legacy_scroll (TWidget *this, int dir)
{
#ifdef IPOD
#define SPER 4
    static int sofar = 0;
    sofar += dir;
    if (sofar > -SPER && sofar < SPER) return 0;
    dir = sofar / SPER;
    sofar -= SPER*dir;
#endif

    GR_EVENT ev;
    int key = 'r';
    int ret = 0;
    _MAKETHIS;

    if (dir < 0) {
	key = 'l';
	dir = -dir;
    }
    
    while (dir) {
	pz_legacy_construct_GR_EVENT (&ev, GR_EVENT_TYPE_KEY_DOWN, key);
	ret |= data->key (&ev);
	pz_legacy_construct_GR_EVENT (&ev, GR_EVENT_TYPE_KEY_UP, key);
	ret |= data->key (&ev);
	dir--;
    }
    this->dirty++;
    return ret;
}
int pz_legacy_timer (TWidget *this)
{
    GR_EVENT ev;
    _MAKETHIS;
    pz_legacy_construct_GR_EVENT (&ev, GR_EVENT_TYPE_TIMER, 0);
    this->dirty++;
    return data->key (&ev);
}
void pz_legacy_destroy (TWidget *this)
{
    free (this->data);
}


// Make widget 0 by 0 -- many old games draw to window only when they need to.
// ttk_run() blanks a WxH region before calling draw(), and draw() might
// only draw a few things.
TWidget *pz_new_legacy_widget (void (*do_draw)(), int (*do_keystroke)(GR_EVENT *))
{
    TWidget *ret = ttk_new_widget (0, 0);
    ret->w = 0;
    ret->h = 0;
    ret->data = calloc (1, sizeof(legacy_data));
    ret->focusable = 1;
    ret->dirty = 1;
    ret->draw = pz_legacy_draw;
    ret->button = pz_legacy_button;
    ret->down = pz_legacy_down;
    ret->scroll = pz_legacy_scroll;
    ret->timer = pz_legacy_timer;
    ret->destroy = pz_legacy_destroy;

    legacy_data *data = (legacy_data *)ret->data;
    data->draw = do_draw;
    data->key = do_keystroke;

    return ret;
}

TWindow *pz_mh_legacy (ttk_menu_item *item) 
{
    TWindow *old = ttk_windows->w;
    void (*newwin)() = (void (*)())item->data;
    (*newwin)();
    if (ttk_windows->w != old) {
	item->sub = ttk_windows->w;
	return TTK_MENU_ALREADYDONE;
    } else {
	item->flags &= ~TTK_MENU_MADESUB;
	return TTK_MENU_DONOTHING;
    }
}

GR_GC_ID pz_get_gc(int copy)
{
    return (copy ? GrCopyGC(root_gc) : root_gc);
}

// for knowing what to change the header of
TWindow *pz_last_window = 0;
extern char *pz_next_header;

GR_WINDOW_ID pz_new_window (int x, int y, int w, int h, void(*do_draw)(void), int(*do_keystroke)(GR_EVENT * event))
{
    // KERIPO MOD
    // We know ; P
    //fprintf (stderr, "Legacy code alert!\n");

    TWindow *ret = ttk_new_window();
    ttk_fillrect (ret->srf, 0, 0, ret->w, ret->h, ttk_makecol (255, 255, 255));
    ret->x = x;
    ret->y = y;
    if ((y == HEADER_TOPLINE) || (y == HEADER_TOPLINE + 1))
	ret->y = ttk_screen->wy, ret->h = ttk_screen->h - ttk_screen->wy;
    else if (!y)
	ret->show_header = 0;
    ret->w = w;
    ret->h = h;
    ttk_add_widget (ret, pz_new_legacy_widget (do_draw, do_keystroke));

    pz_last_window = ret;

    if (pz_next_header) {
	ttk_window_set_title (ret, pz_next_header);
	pz_next_header = 0;
    }

    return ret;
}

void
pz_close_window(GR_WINDOW_ID wid)
{
    ttk_hide_window (wid);
    // pick new top legacy window:
    if (ttk_windows->w->focus && ttk_windows->w->focus->draw == pz_legacy_draw)
	pz_last_window = ttk_windows->w; // pick new top window
    else
	pz_last_window = 0;
    wid->data = 0x12345678; // hey menu: free it & recreate
}

void pz_set_time_from_file(void)
{
#ifdef IPOD
	struct timeval tv_s;
	struct stat statbuf;

	/* find the last modified time of the settings file */
	stat( IPOD_SETTINGS_FILE, &statbuf );

	/* convert timespec to timeval */
	tv_s.tv_sec  = statbuf.st_mtime;
	tv_s.tv_usec = 0;

	settimeofday( &tv_s, NULL );
#endif
}

#ifdef IPOD
#define SCHEMESDIR "/usr/share/schemes/"
#define SCHEMECONF "/opt/Zillae/ZacZilla/Conf/scheme.conf"
#else
#define SCHEMESDIR "schemes/"
#define SCHEMECONF "scheme.conf"
#endif

int main(int argc, char **argv)
{
	// KERIPO MOD
	if (access(SCHEMECONF, F_OK) < 0) {
		FILE *fconf;
		if ((fconf = fopen(SCHEMECONF, "w")) == NULL) {
			perror(SCHEMECONF);
			return -1;
		}
		fprintf(fconf, "%s%s", SCHEMESDIR, "familiar.cs");
		fclose(fconf);
	}
	
#ifdef IPOD
	FILE *fp;
	pz_startup_contrast = ipod_get_contrast();
#endif
// KERIPO MOD
    //system("rm /mnt/debug*");
	if ((root_wid = ttk_init()) == 0) {
	    fprintf(stderr, _("ttk_init failed"));
	    exit(1);
	}
	ttk_hide_window (root_wid);

	ttk_set_global_event_handler (pz_new_event_handler);
	ttk_set_global_unused_handler (pz_unused_handler);

#ifdef LOCALE
	setlocale(LC_ALL, "");
	bindtextdomain("ZacZilla", LOCALEDIR);
	textdomain("ZacZilla");
#endif
////fp=fopen("/mnt/debug.txt","w");
	root_gc = GrNewGC();
	GrSetGCUseBackground(root_gc, GR_FALSE);
	GrSetGCForeground(root_gc, GR_RGB(0,0,0));
	GrGetScreenInfo(&screen_info);

	hw_version = ipod_get_hw_version();

	if( hw_version && hw_version < 30000 ) { /* 1g/2g only */
		pz_set_time_from_file();
	}
    int temp;
	ipod_load_settings();
	//ipod_set_backlight(1);
	//ipod_set_backlight_timer(4);

	////fprintf(fp,"settings");
	////fclose(fp);
 //fp=//fopen("/mnt/debug2.txt","w");
// KERIPO MOD
	//system("cp Data/schemes/* /usr/share/schemes/");
	//fprintf(fp,"cp");
	//fclose(fp);
 //fp=//fopen("/mnt/debug3.txt","w");
//#ifdef MPDC	
	//system("/opt/MPD/MPD-ke /opt/MPD/Conf/mpd.conf --update-db");
	//system("/opt/MPD/MPD-ke /opt/MPD/Conf/mpd.conf");
//#endif	
	load_font();
	appearance_init();
	//fprintf(fp,"apper");
	//fclose(fp);
 //fp=//fopen("/mnt/debug4.txt","w");
#ifdef MPDC
	mpdc_init();
#endif
	header_init();
	//fprintf(fp,"header");
	//fclose(fp);
 //fp=//fopen("/mnt/debug.txt","w");
	//iconui_init();
ti_init();
//fprintf(fp,"tiinit");
//fclose(fp);
 //fp=//fopen("/mnt/debug5.txt","w");
	if (ipod_get_setting (CONTRAST) < 40) // probably no pz.conf file
	    ipod_set_setting (CONTRAST, 96);
//fprintf(fp,"contrast");
//fclose(fp);
 //fp=//fopen("/mnt/debug6.txt","w");
	new_menu_window();
//fprintf(fp,"menu");
//fclose(fp);
 //fp=//fopen("/mnt/debug7.txt","w");

	connection_timer = ttk_create_timer (1000, check_connection);
	usb_connected = usb_is_connected();
	fw_connected = fw_is_connected();
//fprintf(fp,"hconnect");
//fclose(fp);
 //fp=//fopen("/mnt/debug8.txt","w");
	ttk_run();
     //fprintf(fp,"run");
     //fclose(fp);
 //fp=//fopen("/mnt/debug9.txt","w");
	quit_podzilla();
    //fprintf(fp,"quit");
    //fclose(fp);
// KERIPO MOD
    //system("/mnt/podzilla2");
	return 0;
}
