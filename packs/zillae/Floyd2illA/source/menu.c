/*
 * Copyright (C) 2005 Courtney Cavin
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

// KERIPO MOD
#include "_mods.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pz.h"
#include "ipod.h"
#include "mlist.h"

extern void new_contrast_window(void);
extern void new_browser_window(void);
extern void toggle_backlight(void);
extern void set_wheeldebounce(void);
extern void set_buttondebounce(void);
#ifdef __linux__
extern void new_record_mic_window(void);
extern void new_record_line_in_window(void);
extern void new_playback_browse_window(void);
#endif /* __linux__ */
extern void new_calendar_window(void);
extern void new_clock_window(void);
extern void new_world_clock_window(void);
extern void new_Set_Time_window(void);
extern void new_Set_DateTime_window(void);
extern void new_oth_window(void);
extern void new_steroids_window(void);
extern void new_bluecube_window(void);
extern void new_mandel_window(void);
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
extern void new_font_window(menu_st *);
extern void new_vortex_window(void);
extern void new_wumpus_window(void);
extern void about_podzilla(void);
extern void show_credits(void);
extern void new_text_box_np(void);
extern void new_text_box_nppw(void);
extern void new_run_window(void);
extern void new_podwrite_window(void);
extern void new_multiconvert_window(void);
extern void new_chopper_window(void);
extern void new_battery_window(void);
extern void new_memoryg_window(void);
extern void new_snake_window(void);
extern void new_tstep_window(void);
extern void new_conv_window(void);
extern void new_stopwatch_window(void);
extern void new_keyman_window(void);
extern void manic_miner_start(void);
extern void new_credits_window(void);
extern void new_generator_window(void);
/*
extern void new_iboy4g_window(void);
extern void new_iboyphoto_window(void);
extern void new_iboy1mini_window(void);
extern void new_iboy2mini_window(void);
extern void new_iboycolor_window(void);
extern void new_iboynano_window(void);
*/
// KERIPO MOD
// obsolete via ZeroSlackr
//extern void new_iboy_window(void);
extern void new_idw_window(void);
extern void new_factor_window(void);
extern void new_metronome_window(void);
extern void new_sudoku_window(void);
//extern void new_idoom_window(void);
extern void new_periodic_window(void);
extern void new_blackjack_window(void);
extern void new_kaboom_window(void);

#ifdef MIKMOD
extern void new_mikmod_window(void);
#endif

#ifdef MPDC
extern void mpd_currently_playing(void);
extern item_st mpdc_menu[];
#endif /* MPDC */

extern item_st lights_menu[];

extern void quit_podzilla(void);
extern void poweroff_ipod(void);
extern void reboot_ipod(void);
static void change_font(void);

static GR_WINDOW_ID menu_wid;
static GR_GC_ID menu_gc;
static menu_st *menuz;

static item_st tuxchess_menu[] = {
	{N_("Load Last Game"), last_tuxchess_window, ACTION_MENU},
	{N_("---------------"), NULL, NULL},
	{N_("Start New Game"), new_tuxchess_window, ACTION_MENU},
	{0}
};

static item_st arcade_menu[] = {
	{N_("Asteroids"), new_steroids_window, ACTION_MENU},
	{N_("Invaders"), new_invaders_window, ACTION_MENU},
	{N_("KabooM!"), new_kaboom_window, ACTION_MENU},
	{N_("Pong"), new_pong_window, ACTION_MENU},
	{N_("Stepmania"), new_tstep_window, ACTION_MENU},
	{N_("Tetris"), new_bluecube_window, ACTION_MENU},
	{N_("Vortex(Demo)"), new_vortex_window, ACTION_MENU},
	{0}
};

static item_st action_menu[] = {
	{N_("Chopper"), new_chopper_window, ACTION_MENU},
	{N_("Hunt The Wumpus"), new_wumpus_window, ACTION_MENU},
	{N_("KeyMan"), new_keyman_window, ACTION_MENU},
	{N_("Manic Miner"), manic_miner_start, ACTION_MENU},
	{N_("Snake"), new_snake_window, ACTION_MENU},
	{N_("Tunnel"), new_tunnel_window, ACTION_MENU},	
	{0}
};

static item_st card_games[] = {
	{N_("BlackJack"), new_blackjack_window, ACTION_MENU},
	{N_("Memory"), new_memoryg_window, ACTION_MENU},
	{0}
};

static item_st strategy_menu[] = {
	{N_("iPobble"), new_ipobble_window, ACTION_MENU},
	{N_("Lights"), lights_menu, SUB_MENU_HEADER},
	{N_("Minesweeper"), new_mines_window, ACTION_MENU},
	{N_("Othello"), new_oth_window, ACTION_MENU},
	{N_("SuDoKu"), new_sudoku_window, ACTION_MENU},
	{N_("Tic-Tac-Toe"), new_tictactoe_window, ACTION_MENU},
	{N_("TuxChess"), tuxchess_menu, SUB_MENU_HEADER},
	{N_("DrugWars"), new_idw_window, ACTION_MENU},
	{0}
};

static item_st games_menu[] = {
	{N_("Card Games"), card_games, SUB_MENU_HEADER},
	{N_("Action Games"), action_menu, SUB_MENU_HEADER},
	{N_("Arcade Games"), arcade_menu, SUB_MENU_HEADER},
	{N_("Strategy Games"), strategy_menu, SUB_MENU_HEADER},
	{0}
};

static char *backlight_options[] = {
	N_("Off"), N_("1 sec"), N_("2 secs"), N_("5 secs"), N_("10 secs"),
	N_("30 secs"), N_("1 min"), N_("On")
};

static char *sample_rates[] = {
	"8 kHz", "32 kHz", "44.1 kHz", "88.2 kHz", "96 kHz"
};

static char *shuffle_options[] = {
	N_("Off"), N_("Songs")
};

static char *repeat_options[] = {
	N_("Off"), N_("One"), N_("All")
};

static item_st recording_menu[] = {
#ifdef __linux__
	{N_("Mic Record"), new_record_mic_window, ACTION_MENU},
	{N_("Line In Record"), new_record_line_in_window, ACTION_MENU},
	{N_("Playback"), new_playback_browse_window, ACTION_MENU},
#endif /* __linux__ */
	{N_("Sample Rate"), sample_rates, OPTION_MENU, DSPFREQUENCY, 5},
	{0}
};

extern char * clocks_timezones[]; /* for the timezones.  in clocks.c */
extern char * clocks_dsts[];	  /* for dst display.  in clocks.c */

static item_st world_clock_menu[] = {
	{N_("Local Clock"), new_clock_window, ACTION_MENU},
	{N_("World Clock"), new_world_clock_window, ACTION_MENU},
	{N_("TZ"), clocks_timezones, OPTION_MENU, TIME_WORLDTZ, 39},
	{N_("DST"), clocks_dsts, OPTION_MENU, TIME_WORLDDST, 3},
	{0}
};

static item_st tools_menu[] = {
	{N_("Calculator"), new_calc_window, ACTION_MENU},
	{N_("Calendar"), new_calendar_window, ACTION_MENU},
	{N_("Clock"), world_clock_menu, SUB_MENU_HEADER},
	{N_("PodDraw"), new_poddraw_window, ACTION_MENU},
	{N_("PodWrite"), new_podwrite_window, ACTION_MENU},
	{N_("Timer"), new_stopwatch_window, ACTION_MENU},
	{0}
};

static item_st apps_menu[] = {
	{N_("Factor Calc"), new_factor_window, ACTION_MENU},
	{N_("Metronome"), new_metronome_window, ACTION_MENU},
	{N_("MultiConvert"), new_multiconvert_window, ACTION_MENU},
	{N_("Periodic Table"), new_periodic_window, ACTION_MENU},
	{N_("Temp Converter"), new_conv_window, ACTION_MENU},
	{N_("Tone Dialer"), new_dialer_window, ACTION_MENU},
	{N_("Tone Generator"), new_generator_window, ACTION_MENU},
	{0}
};

static item_st stuff_menu[] = {
	{N_("Battery Toggle [AF]"), new_battery_window, ACTION_MENU},
	{N_("Cube"), new_cube_window, ACTION_MENU},
	{N_("MandelPod"), new_mandel_window, ACTION_MENU},
	{N_("Matrix"), new_matrix_window, ACTION_MENU},
	{N_("Text Input Password Demo"), new_text_box_nppw, ACTION_MENU},
	{0}
};

/*
//No Longer Needed
static item_st iboy_menu[] = {
	{N_("iBoy- Photo iPod"), new_iboyphoto_window, ACTION_MENU},
	{N_("iBoy- Color iPod"), new_iboycolor_window, ACTION_MENU},
	{N_("iBoy- 4th Gen iPod"), new_iboy4g_window, ACTION_MENU},
	{N_("iBoy- 1st Gen Mini"), new_iboy1mini_window, ACTION_MENU},
	{N_("iBoy- 2nd Gen Mini"), new_iboy2mini_window, ACTION_MENU},
	{N_("iBoy- Nano iPod"), new_iboynano_window, ACTION_MENU},
	{0}
};
*/

static item_st extras_menu[] = {
	{N_("Recordings"), recording_menu, SUB_MENU_HEADER},
	{N_("Games"), games_menu, SUB_MENU_HEADER},
	{N_("Tools"), tools_menu, SUB_MENU_HEADER},
	{N_("Apps"), apps_menu, SUB_MENU_HEADER},
	{N_("Other"), stuff_menu, SUB_MENU_HEADER},
	//{N_("DooM"), new_idoom_window, ACTION_MENU},
	// KERIPO MOD
	// obsolete via ZeroSlackr
	//{N_("iBoy"), new_iboy_window, ACTION_MENU},
	{0}
};

static item_st reset_menu[] = {
	{N_("NO"), NULL, SUB_MENU_PREV},
	{N_("----"), NULL, NULL},
	{N_("YES"), ipod_reset_settings, ACTION_MENU | SUB_MENU_PREV},
	{0}
};

static char * time1224_options[] = { N_("12-hour"), N_("24-hour") };

static item_st clocks_menu[] = {
        { N_("Clock"), new_clock_window, ACTION_MENU },
        { N_("Set Time"), new_Set_Time_window, ACTION_MENU },
        { N_("Set Time & Date"), new_Set_DateTime_window, ACTION_MENU },
/* -- future expansion --
	{ N_("Set Alarm"), NULL, SUB_MENU_PREV },
	{ N_("Set Sleep Timer"), NULL, SUB_MENU_PREV },
	{ N_("Time In Title"), NULL, BOOLEAN_MENU, TIME_IN_TITLE },
*/
	{ N_("TZ"), clocks_timezones, OPTION_MENU, TIME_ZONE, 39 },
	{ N_("DST"), clocks_dsts, OPTION_MENU, TIME_DST, 3},
	{ N_("Time"), time1224_options, OPTION_MENU, TIME_1224, 2 },
	{ N_("Time Tick Noise"), NULL, BOOLEAN_MENU, TIME_TICKER },
        { 0 }
};

static item_st textinput_menu[] = {
	{N_("OFF"), NULL, SETTING_ITEM, TEXT_INPUT, 0},
	{N_("Serial"), NULL, SETTING_ITEM, TEXT_INPUT, 1},
	{N_("Scroll Through"), NULL, SETTING_ITEM, TEXT_INPUT, 2},
	{N_("On-Screen Keyboard"), NULL, SETTING_ITEM, TEXT_INPUT, 3},
	{N_("Morse Code"), NULL, SETTING_ITEM, TEXT_INPUT, 4},
	{N_("Cursive"), NULL, SETTING_ITEM, TEXT_INPUT, 5},
	{N_("WheelBoard"), NULL, SETTING_ITEM, TEXT_INPUT, 6},
	{N_("4-Button Keyboard"), NULL, SETTING_ITEM, TEXT_INPUT, 7},
	{N_("Dial Type"), NULL, SETTING_ITEM, TEXT_INPUT, 8},
	{N_("Telephone Keypad (4g)"), NULL, SETTING_ITEM, TEXT_INPUT, 9},
	{N_("Thumbscript (4g)"), NULL, SETTING_ITEM, TEXT_INPUT, 10},
	{N_("4-Button Telephone Keypad"), NULL, SETTING_ITEM, TEXT_INPUT, 11},
	/* {N_("Dasher"), NULL, SETTING_ITEM, TEXT_INPUT, 12}, */
	/* {N_("Speech Recognition"), NULL, SETTING_ITEM, TEXT_INPUT, 13}, */
	{0}
};


static item_st appearance_menu[] = {
	{N_("Color Scheme"), colorscheme_names, OPTION_MENU, COLORSCHEME, CS_NSCHEMES },
	{N_("Decorations"), appearance_decorations, OPTION_MENU, DECORATIONS, NDECORATIONS },
	{N_("Gradient Type"), appearance_gradient_names, OPTION_MENU, GRADIENTS, NGRADIENTS },
	{N_("Battery Digits"), NULL, BOOLEAN_MENU, BATTERY_DIGITS },
	{N_("Display Load Average"), NULL, BOOLEAN_MENU, DISPLAY_LOAD },
	{N_("Menu Transition"), NULL, BOOLEAN_MENU, SLIDE_TRANSIT},
	{N_("Font"), change_font, ACTION_MENU},
	{ 0 }
};

static void change_font()
{
	new_font_window(menuz);
}

void menu_adjust_nschemes( int val )
{
	// '0' is the index above for "Color Scheme"
	appearance_menu[0].item_count = val;
}

void menu_adjust_ndecorations( int val )
{
	appearance_menu[1].item_count = val;
}

static item_st settings_menu[] = {
	{N_("About"), about_podzilla, ACTION_MENU},
	{N_("Credits"), show_credits, ACTION_MENU},
	{N_("Date & Time"), clocks_menu, SUB_MENU_HEADER},
	{N_("Repeat"), repeat_options, OPTION_MENU, REPEAT, 3},
	{N_("Shuffle"), shuffle_options, OPTION_MENU, SHUFFLE, 2},
	{N_("Contrast"), set_contrast, ACTION_MENU},
	{N_("Wheel Sensitivity"), set_wheeldebounce, ACTION_MENU},
	{N_("Button Debounce"), set_buttondebounce, ACTION_MENU},
	{N_("Backlight Timer"), backlight_options, OPTION_MENU, BACKLIGHT_TIMER, 8},
	{N_("Clicker"), NULL, BOOLEAN_MENU, CLICKER},
	{N_("Appearance"), appearance_menu, SUB_MENU_HEADER},
	{N_("Text Input"), textinput_menu, SUB_MENU_HEADER},
	{N_("Text Input Test"), new_text_box_np, ACTION_MENU},
	{N_("Browser Path Display"), NULL, BOOLEAN_MENU, BROWSER_PATH},
	{N_("Show Hidden Files"), NULL, BOOLEAN_MENU, BROWSER_HIDDEN},
	{N_("--------------"), NULL, NULL},
	{N_("Save Settings"), ipod_save_settings, ACTION_MENU},
	{N_("--------------"), NULL, NULL},
	{N_("Load Settings"), ipod_load_settings, ACTION_MENU},
	{N_("--------------"), NULL, NULL},
	{N_("Reset All Settings"), reset_menu, SUB_MENU_HEADER},
	{0}
};

static item_st reboot_menu[] = {
	{N_("NO"), NULL, SUB_MENU_PREV},
	{N_("----"), NULL, NULL},
	{N_("YES"), reboot_ipod, ACTION_MENU},
	{0}
};

static item_st turnoff_menu[] = {
	{N_("NO"), NULL, SUB_MENU_PREV},
	{N_("----"), NULL, NULL},
	{N_("YES"), poweroff_ipod, ACTION_MENU},
	{0}
};

#ifndef MPDC
static item_st itunes_menu[] = {
	{N_("Playlists"), new_itunes_plist, ACTION_MENU | ARROW_MENU},
	{N_("Artists"), new_itunes_artist, ACTION_MENU | ARROW_MENU},
	{N_("Albums"), new_itunes_album, ACTION_MENU | ARROW_MENU},
	{N_("Songs"), new_itunes_track, ACTION_MENU | ARROW_MENU},
#ifdef MIKMOD
	{N_("MikMod"), new_mikmod_window, ACTION_MENU},
#endif
	{0}
};
#endif /* !MPDC */

static item_st power_menu[] = {
	{N_("Quit Floyd2illa"), quit_podzilla, ACTION_MENU},
#ifdef NEVER /* just to show where this should go */
	{N_("Sleep iPod"), sleep, ACTION_MENU};
#endif
	{N_("Turn off iPod"), turnoff_menu, SUB_MENU_HEADER},
	{N_("Reboot iPod"), reboot_menu, SUB_MENU_HEADER},
	{0}
};

static item_st main_menu[] = {
#ifdef MPDC
	// KERIPO MOD
	//{N_("Music"), mpdc_menu, SUB_MENU_HEADER},
	{N_("Music Player Daemon"), mpdc_menu, SUB_MENU_HEADER},
#else
	{N_("Music"), itunes_menu, SUB_MENU_HEADER},
#endif /* MPDC */
	{N_("Extras"), extras_menu, SUB_MENU_HEADER},
	{N_("Settings"), settings_menu, SUB_MENU_HEADER},
#ifdef MPDC
	{N_("Now Playing"), mpd_currently_playing, ACTION_MENU},
#endif /* MPDC */
	{N_("File Browser"), new_browser_window, ACTION_MENU | ARROW_MENU},
// KERIPO MOD
// rather obsolete and just freezes things
	//{N_("Run..."), new_run_window, ACTION_MENU},
	{N_("Power"), power_menu, SUB_MENU_HEADER},
	{0}
};

static void menu_do_draw()
{
	/* window is focused */
	if(menu_wid == GrGetFocus()) {
		pz_draw_header(menuz->title);
		menu_draw(menuz);
	}
}

static int menu_do_keystroke(GR_EVENT * event)
{
	int ret = 0;

	switch (event->type) {
	case GR_EVENT_TYPE_TIMER:
		menu_draw_timer(menuz);
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		case '\r':
		case '\n':
			menuz = menu_handle_item(menuz, menuz->sel);
			menu_do_draw();
			ret |= KEY_CLICK;
			break;
		case 'l':
			if (menu_shift_selected(menuz, -1)) {
				menu_draw(menuz);
				ret |= KEY_CLICK;
			}
			break;
		case 'r':
			if (menu_shift_selected(menuz, 1)) {
				menu_draw(menuz);
				ret |= KEY_CLICK;
			}
			break;
		case 'm':
			if(menuz->parent != NULL) {
				menuz = menu_destroy(menuz);
				menu_do_draw();
				ret |= KEY_CLICK;
			}
			break;
		case 'q':
			menu_destroy_all(menuz);
			pz_close_window(menu_wid);
			GrDestroyGC(menu_gc);
			exit(0);
			break;
		default:
			ret |= KEY_UNUSED;
			break;
		}
		break;
	default:
		ret |= EVENT_UNUSED;
		break;
	}
	return ret;
}

void new_menu_window()
{
	GrGetScreenInfo(&screen_info);

	menu_gc = pz_get_gc(1);
	GrSetGCUseBackground(menu_gc, GR_TRUE);
	GrSetGCForeground(menu_gc, BLACK);
	GrSetGCBackground(menu_gc, WHITE);

	menu_wid = pz_new_window(0, HEADER_TOPLINE + 2, screen_info.cols,
			screen_info.rows - (HEADER_TOPLINE + 2), menu_do_draw,
			menu_do_keystroke);

	GrSelectEvents(menu_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP|
			GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_TIMER);

	// KERIPO MOD
	//menuz = menu_init(menu_wid, "[:Floyd2illA Final:]", 0, 0, screen_info.cols,
	menuz = menu_init(menu_wid, PZ_TITLE, 0, 0, screen_info.cols,
			screen_info.rows - (HEADER_TOPLINE + 1), NULL,
			main_menu, ASCII | TRANSLATE);

	GrMapWindow(menu_wid);
}
