/*
 * Copyright (C) 2005 Courtney Cavin and Joshua Oreman
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

#include "pz.h"
#include "ipod.h"
TWindow *ret;
int curvespeed;
int curveplay;
extern void new_contrast_window(void);
extern void new_browser_window(void);
extern void toggle_backlight(void);
extern void set_wheeldebounce(void);
extern void set_buttondebounce(void);
#ifdef __linux__
/*extern void new_record_mic_window(void);
extern void new_record_line_in_window(void);
extern void new_playback_browse_window(void);
*/#endif /* __linux__ */
extern void new_calendar_window(void);
extern void new_clock_window(void);
extern void new_world_clock_window(void);
extern void new_Set_Time_window(void);
extern void new_Set_DateTime_window(void);
extern void new_oth_window(void);
//extern void new_steroids_window(void);
extern void new_bluecube_window(void);
#ifndef MPDC
extern void new_itunes_track(void);
extern void new_itunes_artist(void);
extern void new_itunes_album(void);
extern void new_itunes_plist(void);
#endif /* !MPDC */
extern void new_pong_window(void);
extern void new_mines_window(void);
extern void new_tictactoe_window(void);
extern void new_tunnel_window(void);
extern void new_tuxchess_window(void);
extern void last_tuxchess_window(void);
extern void new_calc_window(void);
extern void new_dialer_window(void);
extern void new_poddraw_window(void);
extern void new_cube_window(void);
extern void new_matrix_window(void);
extern void new_ipobble_window(void);
extern void new_invaders_window(void);
extern void new_chopper_window(void);
extern TWindow *new_font_window(ttk_menu_item *);
extern void new_vortex_window(void);
extern void browser_ps2(void);


extern void new_kaboom_window(void);
extern void new_periodic_window(void);
extern void new_blackjack_window(void);
extern void new_credits_window(void);
extern TWindow *new_factor_window(void);
//extern void new_sudoku_window(void);
extern void new_memoryg_window(void);

//MODULES:

extern TWindow *new_plasma_window(void);
//extern void new_podwrite_window(void);
extern TWindow *new_starfield_window(void);
extern TWindow *new_about_window();
extern TWindow *show_credits(ttk_menu_item *);
extern TWindow *mystify_new_window(void);
extern TWindow *capture_start(void);
extern TWindow *new_1dtetris_window(void);
extern TWindow *new_chopper2_window(void);
extern TWindow *new_chopper2_highscore(void);
//extern TWindow *new_brickm_window(void);
extern TWindow *new_bridget_window(void);
//extern TWindow *new_connect4_window(void);
extern TWindow *new_craps_window(void);
extern TWindow *new_colormixer_window(void);
//extern TWindow *new_duckhunt_window(void);
//extern TWindow *new_duckhuntHS_window(void);
extern TWindow *new_invaders2_window(void);
extern TWindow *new_reflex_window(void);
//extern TWIndow *new_curve_window(void);
extern TWindow *new_ouch_window(void);
extern TWindow *new_deal_window(void);
extern TWindow *wumpus_new_game(void);
//extern TWindow *iconui_new_menu_window(void);
extern TWindow *new_izilla_window(void);
extern TWindow *new_usvsthem_window(void);
extern TWindow *new_piezomaker(void);
//extern TWindow *new_avalanche_window(void);
extern TWindow * new_podwrite_window(void);
//extern TWindow *new_iracer_window(void);
//extern TWindow * new_terminal_window(void);
//extern TWindow *new_e3d_window(void);
#ifdef MIKMOD
extern void new_mikmod_window(void);
#endif
#ifdef MPDC
extern void mpd_currently_playing(void);
extern void settxt(int set);
extern ttk_menu_item mpdc_menu[];
#endif /* MPDC */
extern ttk_menu_item lights_menu[];
extern void quit_podzilla(void);
extern void poweroff_ipod(void);
extern void reboot_ipod(void);
#include "ipod.h"
#define ACTION_MENU      0
#define SUB_MENU_HEADER  TTK_MENU_ICON_SUB
#define MENU_SETTING(c,s) .choices=c, .cdata=s, .choicechanged=menu_set_setting, .choiceget=menu_get_setting
#define MENU_BOOL(s) .choices=boolean_options, .cdata=s, .choicechanged=menu_set_setting, .choiceget=menu_get_setting
static ttk_menu_item tuxchess_menu[] = {
	{N_("Last Game"), {pz_mh_legacy}, 0, last_tuxchess_window},
	{N_("New Game"), {pz_mh_legacy}, 0, new_tuxchess_window},
	{0}
};

/*
static ttk_menu_item duckhunt_menu[]={
  {N_("Start Game"),{pz_mh_legacy},0,new_duckhunt_window},
  {N_("High Scores"),{pz_mh_legacy},0,new_duckhuntHS_window},
  {0}
};
*/

static ttk_menu_item chopper_menu[]={
  {N_("Start Game"),{pz_mh_legacy},0,new_chopper2_window},
  {N_("High Scores"),{pz_mh_legacy},0,new_chopper2_highscore},
  {0}
};

//static ttk_menu_item curve_menu[]={
 // {N_("Start Game"),{pz_mh_legacy},0,new_curve_window},
 // {N_("1 Player"),{pz_mh_legacy},0,curve1play},
 // {N
static ttk_menu_item games_menu[] = {
	{N_("1D Tetris"),{pz_mh_legacy},0,new_1dtetris_window},
	//{N_("Avalanche"),{pz_mh_legacy},0,new_avalanche_window},
	{N_("BlackJack"), {pz_mh_legacy}, 0, new_blackjack_window},
	{N_("BlueCube"), {pz_mh_legacy}, 0, new_bluecube_window},
	//{N_("BrickMania"), {pz_mh_legacy},0,new_brickm_window},
	{N_("Bridget"),{pz_mh_legacy},0,new_bridget_window},
	{N_("Chopper"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, chopper_menu},
	//{N_("Connect 4"),{pz_mh_legacy},0,new_connect4_window},
	{N_("Craps"),{pz_mh_legacy},0,new_craps_window},
	//{N_("Curve"),{ttk_mh_sub},TTK_MENU_ICON_SUB,curve_menu},
	//{N_("Duckhunt"),{ttk_mh_sub},TTK_MENU_ICON_SUB,duckhunt_menu},
	{N_("Hunt The Wumpus"), {pz_mh_legacy}, 0, wumpus_new_game},
	{N_("iDeal or No Deal"),{pz_mh_legacy},0,new_deal_window},
	{N_("Invaders"), {pz_mh_legacy}, 0, new_invaders_window},
	{N_("Invaders2"), {pz_mh_legacy}, 0, new_invaders2_window},
	{N_("iPobble"), {pz_mh_legacy}, 0, new_ipobble_window},
	//{N_("iPod Racer"), {pz_mh_legacy}, 0, new_iracer_window},
	{N_("Kaboom!"), {pz_mh_legacy}, 0, new_kaboom_window},
	{N_("Lights"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, lights_menu},
	{N_("Memory"),{pz_mh_legacy},0,new_memoryg_window},
	{N_("Minesweeper"), {pz_mh_legacy}, 0, new_mines_window},
	{N_("Othello"), {pz_mh_legacy}, 0, new_oth_window},
	{N_("Pong"), {pz_mh_legacy}, 0, new_pong_window},
	{N_("Reflex"),{pz_mh_legacy},0,new_reflex_window},
	//{N_("Steroids"), {pz_mh_legacy}, 0, new_steroids_window},
	//{N_("Sudoku"), {pz_mh_legacy}, 0, new_sudoku_window},
	{N_("Tic-Tac-Toe"), {pz_mh_legacy}, 0, new_tictactoe_window},
	{N_("Tunnel"), {pz_mh_legacy}, 0, new_tunnel_window},
	{N_("TuxChess"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, tuxchess_menu},
	{N_("Us Vs Them Demo"),{pz_mh_legacy},0,new_usvsthem_window},
	{N_("Vortex Demo"), {pz_mh_legacy}, 0, new_vortex_window},
	{0}
};

static ttk_menu_item stuff_menu[] = {
	{N_("Cube"), {pz_mh_legacy}, 0, new_cube_window},
	{N_("Dialer"), {pz_mh_legacy}, 0, new_dialer_window},
	//{N_("Engine 3D"), {pz_mh_legacy}, 0, new_e3d_window},
	{N_("Factor"), {pz_mh_legacy}, 0, new_factor_window},
	{N_("Matrix"), {pz_mh_legacy}, 0, new_matrix_window},
	{N_("Mystify"),{pz_mh_legacy},0,mystify_new_window},
	{N_("Plasma"), {pz_mh_legacy}, 0, new_plasma_window},
	{N_("StarField"),{pz_mh_legacy},0,new_starfield_window},
	{0}
};

static ttk_menu_item apps_menu[] = {
	{N_("Calendar"), {pz_mh_legacy}, 0, new_calendar_window},
	{N_("Calculator"), {pz_mh_legacy}, 0, new_calc_window},
	{N_("ColorMixer"),{pz_mh_legacy},0,new_colormixer_window},
	{N_("Periodic table"), {pz_mh_legacy}, 0, new_periodic_window},
	{N_("Piezo Maker"), {pz_mh_legacy}, 0, new_piezomaker},
	{N_("PodDraw"), {pz_mh_legacy}, 0, new_poddraw_window},
	{N_("PodWrite"), {pz_mh_legacy}, 0, new_podwrite_window},
	//{N_("Terminal"), {pz_mh_legacy}, 0, new_terminal_window},
	//{N_("Screen Capture"),{pz_mh_legacy},0,capture_start},
	{0}
};

static const char *backlight_options[] = {
	N_("Off"), N_("1 sec"), N_("2 secs"), N_("5 secs"), N_("10 secs"),
	N_("30 secs"), N_("1 min"), N_("On"), 0
};

static const char *sample_rates[] = {
    "8 kHz", "32 kHz", "44.1 kHz", "88.2 kHz", "96 kHz", 0
};

static const char *shuffle_options[] = {
    N_("Off"), N_("Songs"), 0
};

static const char *repeat_options[] = {
    N_("Off"), N_("One"), N_("All"), 0
};

static const char *boolean_options[] = {
    N_("Off"), N_("On"), 0
};


static void menu_set_setting (ttk_menu_item *item, int cdata) 
{
    ipod_set_setting (cdata, item->choice);
}
static int menu_get_setting (ttk_menu_item *item, int cdata) 
{
    int ret = ipod_get_setting (cdata);
    if (ret > item->nchoices) {
	fprintf (stderr, "Warning! Setting %d set to %d on a %d scale - using 0\n", cdata, ret, item->nchoices);
	ret = 0;
    }
    return ret;
}

static ttk_menu_item recording_menu[] = {
#ifdef __linux__
/*	{N_("Mic Record"), {pz_mh_legacy}, 0, new_record_mic_window},
	{N_("Line In Record"), {pz_mh_legacy}, 0, new_record_line_in_window},
	{N_("Playback"), {pz_mh_legacy}, 0, new_playback_browse_window},
*/#endif /* __linux__ */
	{N_("Sample Rate"), MENU_SETTING (sample_rates, DSPFREQUENCY)},
	{0}
};

extern const char * clocks_timezones[]; /* for the timezones.  in clocks.c */
extern const char * clocks_dsts[];	  /* for dst display.  in clocks.c */

static ttk_menu_item world_clock_menu[] = {
	{N_("Local Clock"), {pz_mh_legacy}, 0, new_clock_window},
	{N_("World Clock"), {pz_mh_legacy}, 0, new_world_clock_window},
	{N_("TZ"), MENU_SETTING (clocks_timezones, TIME_WORLDTZ)},
	{N_("DST"), MENU_SETTING (clocks_dsts, TIME_WORLDDST)},
	{0}
};

static ttk_menu_item extras_menu[] = {
	{N_("Recordings"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, recording_menu},
	{N_("Clock"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, world_clock_menu},
	{N_("Stuff"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, stuff_menu},
	{N_("Games"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, games_menu},
	{N_("Apps"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, apps_menu},
	{0}
};

static TWindow *menu_reset_settings (ttk_menu_item *item) { ipod_reset_settings(); return TTK_MENU_UPONE; }

static ttk_menu_item reset_menu[] = {
	{N_("No"), { .sub = (TWindow *)TTK_MENU_UPONE }, TTK_MENU_MADESUB},
	{N_("Yes"), {menu_reset_settings}, TTK_MENU_ICON_EXE, 0},
	{0}
};

static const char * time1224_options[] = { N_("12-hour"), N_("24-hour"), 0 };

static ttk_menu_item clocks_menu[] = {
        { N_("Clock"), {pz_mh_legacy}, 0, new_clock_window },
        { N_("Set Time"), {pz_mh_legacy}, 0, new_Set_Time_window },
        { N_("Set Time & Date"), {pz_mh_legacy}, 0, new_Set_DateTime_window },
/* -- future expansion --
	{ N_("Set Alarm"), NULL, SUB_MENU_PREV },
	{ N_("Set Sleep Timer"), NULL, SUB_MENU_PREV },
	{ N_("Time In Title"), NULL, BOOLEAN_MENU, TIME_IN_TITLE },
*/
	{ N_("TZ"), MENU_SETTING (clocks_timezones, TIME_ZONE) },
	{ N_("DST"), MENU_SETTING (clocks_dsts, TIME_DST) },
	{ N_("Time"), MENU_SETTING (time1224_options, TIME_1224) },
	{ N_("Time Tick Noise"), MENU_BOOL (TIME_TICKER) },
        { 0 }
};


static const char * transit_options[] = { N_("Off"), N_("Slow"), N_("Fast"), N_("Video Fast"), 0 };
static const char * icon_settings[] = {"Off", "Small List", "Large List", "Small Array", "Large Array", "Small Dock", "Magnifying Dock", "Large Dock", 0};
static const char * iconc[] = {"Off", "On", 0};
static ttk_menu_item appearance_menu[] = {
	{N_("Color Scheme"), {appearance_select_color_scheme} },
     	{N_("Decorations"), MENU_SETTING (appearance_decorations, DECORATIONS) },
	{N_("Battery Digits"), MENU_BOOL (BATTERY_DIGITS) },
	{N_("Display Load Average"), MENU_BOOL (DISPLAY_LOAD) },
	{N_("Menu Transition"), MENU_SETTING (transit_options, SLIDE_TRANSIT) },
	{N_("Font"), {new_font_window}, TTK_MENU_ICON_SUB},
	{N_("Icon Style"),MENU_SETTING(icon_settings,51)},
	{N_("Icon Cache"),MENU_SETTING(iconc,52)},
	{ 0 }
};
extern TWindow * ti_ttkselect0(ttk_menu_item * item);
extern TWindow * ti_ttkselect1(ttk_menu_item * item);
extern TWindow * ti_ttkselect2(ttk_menu_item * item);

static ttk_menu_item txtin_menu[] = {
  {N_("Serial"),{ti_ttkselect0}},
  {N_("Scroll Through"),{ti_ttkselect1}},
  {N_("TUP"),{ti_ttkselect2}},
  {0}
};
extern TWindow *new_standard_text_demo_window();
static const char *txtin_options[] = {
	N_("Off"), N_("Serial"), N_("Scroll Through"), N_("On Screen Keyboard"), N_("Morse Code"),
	N_("Cursive"), N_("WheelBoard"), N_("4-Button Keyboard"), N_("Dial Type"),N_("Telephone Keypad (4g+)"),N_("ThumbScript (4g+)"),
	N_("4-Button Telephone Keypad"), 0
};
// KERIPO MOD
//extern TWindow * new_run_window();
static ttk_menu_item settings_menu[] = {
	{N_("About"), {pz_mh_legacy},0,new_about_window},
	{N_("Credits"), {pz_mh_legacy}, 0, new_credits_window},
	{N_("Date & Time"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, clocks_menu},
	{N_("Repeat"), MENU_SETTING (repeat_options, REPEAT)},
	{N_("Shuffle"), MENU_SETTING (shuffle_options, SHUFFLE)},
	{N_("Contrast"), {pz_mh_legacy}, 0, set_contrast},
	{N_("Wheel Sensitivity"), {pz_mh_legacy}, 0, set_wheeldebounce},
	{N_("Backlight Timer"), MENU_SETTING (backlight_options, BACKLIGHT_TIMER)},
	{N_("Clicker"), MENU_BOOL (CLICKER)},
	{N_("Appearance"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, appearance_menu},
	{N_("Text Input"),{ttk_mh_sub},TTK_MENU_ICON_SUB,txtin_menu},
	{N_("Text Input Test"),{pz_mh_legacy},0,new_standard_text_demo_window},
	{N_("Browser Path Display"), MENU_BOOL (BROWSER_PATH)},
	{N_("Show Hidden Files"), MENU_BOOL (BROWSER_HIDDEN)},
	{N_("Exit Without Saving"), {.sub=TTK_MENU_UPONE}, TTK_MENU_MADESUB},
	{N_("Reset All Settings"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, reset_menu},
	{0}
};

static ttk_menu_item reboot_menu[] = {
	{N_("No"), { .sub = (TWindow *)TTK_MENU_UPONE }},
	{N_("Yes"), {pz_mh_legacy}, 0, reboot_ipod},
	{0}
};

static ttk_menu_item turnoff_menu[] = {
	{N_("No"), { .sub = (TWindow *)TTK_MENU_UPONE }},
	{N_("Yes"), {pz_mh_legacy}, 0, poweroff_ipod},
	{0}
};

#ifndef MPDC
static ttk_menu_item itunes_menu[] = {
	{N_("Playlists"), {pz_mh_legacy}, TTK_MENU_ICON_SUB, new_itunes_plist},
	{N_("Artists"), {pz_mh_legacy}, TTK_MENU_ICON_SUB, new_itunes_artist},
	{N_("Albums"), {pz_mh_legacy}, TTK_MENU_ICON_SUB, new_itunes_album},
	{N_("Songs"), {pz_mh_legacy}, TTK_MENU_ICON_SUB, new_itunes_track},
#ifdef MIKMOD
	{N_("MikMod"), {pz_mh_legacy}, 0, new_mikmod_window},
#endif
	{0}
};
#endif /* !MPDC */

static ttk_menu_item power_menu[] = {
	{N_("Quit ZacZilla"), {pz_mh_legacy}, 0, quit_podzilla},
	{N_("Reboot iPod"), {ttk_mh_sub}, 0, reboot_menu},
#ifdef NEVER /* just to show where this should go */
	{N_("Sleep iPod"), {pz_mh_legacy}, 0, sleep};
#endif
	{N_("Turn off iPod"), {ttk_mh_sub}, 0, turnoff_menu},
	{0}
};

int settings_button (TWidget *this, int button, int time)
{
    if (button == TTK_BUTTON_MENU)
       ipod_save_settings();
    return ttk_menu_button (this, button, time);
}

TWindow *pz_settings_sub (ttk_menu_item *item)
{
    TWindow *reet = ttk_new_window();
    TWidget *menu = ttk_new_menu_widget (item->data, ttk_menufont, item->menuwidth, item->menuheight);
    ttk_window_title (reet, item->name);
    menu->draw (menu, reet->srf);
    menu->button = settings_button;
    ttk_add_widget (reet, menu);
    return reet;
}

static ttk_menu_item main_menu[] = {
#ifdef MPDC
	{N_("Music"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, mpdc_menu},
#else
	{N_("Music"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, itunes_menu},
#endif /* MPDC */
	{N_("Extras"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, extras_menu},
	{N_("Settings"), {pz_settings_sub}, TTK_MENU_ICON_SUB, settings_menu},
#ifdef MPDC
	{N_("Now Playing"), {pz_mh_legacy}, 0, mpd_currently_playing},
#endif /* MPDC */
	{N_("File Browser"), {pz_mh_legacy},TTK_MENU_ICON_SUB,new_browser_window},
	{N_("Power"), {ttk_mh_sub}, TTK_MENU_ICON_SUB, power_menu},
	// KERIPO MOD
	// obsolete
	//{N_("Run"),{pz_mh_legacy},0,new_run_window},

	{0}
};
extern ttk_menu_item empty_menu[];
//TWindow *(*pz_new_menu_window)(TWidget *) = iconui_new_menu_window;
void new_menu_window()
{
    
//  TWidget *menu = ttk_new_menu_widget (empty_menu, ttk_menufont, ret->w, ret->h);
    //ret=ttk_new_window();
   // TWidget *menu = ttk_new_widget (0,0);
  //  iconui_menu_append(menu,main_menu+5);
   //ttk_show_window(pz_new_menu_window(menu));
  //  //ttk_show_window (pz_new_menu_window(main_menu));
    ret=ttk_new_window();
    TWidget *menu = ttk_new_menu_widget (main_menu, ttk_menufont, ret->w, ret->h);
    ttk_menu_set_closeable (menu, 0);
    ttk_add_widget (ret, menu);
    ttk_window_set_title (ret, "ZacZilla");
    ttk_show_window (ret);
  
}

