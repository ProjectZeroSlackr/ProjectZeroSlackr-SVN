#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pz.h"
#include "ipod.h"
#include "mlist.h"

void battery_graphical();
void battery_voltage();
int file_exists(char*);

static GR_WINDOW_ID battery_wid;
static GR_GC_ID battery_gc;
static menu_st *bmenu;

static item_st battery_menu[] = {
	{"This will change the", NULL, NULL},
	{"battery meter in", NULL, NULL},
	{"Apple's Firmware", NULL, NULL},
	{"----Choose one----", NULL, NULL},
	{"Graphical (default)", battery_graphical, ACTION_MENU},
	{"Voltage Meter", battery_voltage, ACTION_MENU},
	{0}
};

static void menu_do_draw()
{
	/* window is focused */
	if(battery_wid == GrGetFocus()) {
		pz_draw_header(bmenu->title);
		menu_draw(bmenu);
	}
}

static int menu_do_keystroke(GR_EVENT * event)
{
	int ret = 0;

	switch (event->type) {
	case GR_EVENT_TYPE_TIMER:
		menu_draw_timer(bmenu);
		break;
	case GR_EVENT_TYPE_KEY_DOWN:
		switch (event->keystroke.ch) {
		case '\r':
		case '\n':
			bmenu = menu_handle_item(bmenu, bmenu->sel);
			menu_do_draw();
			ret |= KEY_CLICK;
			break;
		case 'l':
			if (menu_shift_selected(bmenu, -1)) {
				menu_draw(bmenu);
				ret |= KEY_CLICK;
			}
			break;
		case 'r':
			if (menu_shift_selected(bmenu, 1)) {
				menu_draw(bmenu);
				ret |= KEY_CLICK;
			}
			break;
		case 'm':
			pz_close_window (battery_wid);
            GrDestroyGC(battery_gc);
			break;
		case 'q':
			menu_destroy_all(bmenu);
			pz_close_window(battery_wid);
			GrDestroyGC(battery_gc);
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

void new_battery_window()
{
	GrGetScreenInfo(&screen_info);

	battery_gc = pz_get_gc(1);
	GrSetGCUseBackground(battery_gc, GR_TRUE);
	GrSetGCForeground(battery_gc, BLACK);
	GrSetGCBackground(battery_gc, WHITE);

	battery_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols,
			screen_info.rows - (HEADER_TOPLINE + 1), menu_do_draw,
			menu_do_keystroke);

	GrSelectEvents(battery_wid, GR_EVENT_MASK_EXPOSURE| GR_EVENT_MASK_KEY_UP|
			GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_TIMER);
	
	bmenu = menu_init(battery_wid,  "Battery", 0, 1,
			screen_info.cols, screen_info.rows -
			(HEADER_TOPLINE + 1), NULL, battery_menu, ASCII);

	GrMapWindow(battery_wid);
}

void battery_graphical() {
	if (file_exists("hp/iPod_Control/Device/_show_voltage")) {
		if (!file_exists("hp/iPod_Control/Device/_show_voltage_")) {
			rename("hp/iPod_Control/Device/_show_voltage", "hp/iPod_Control/Device/_show_voltage_");
		} else {
			remove("hp/iPod_Control/Device/_show_voltage");
		}
	} else {
		if (!file_exists("hp/iPod_Control/Device/_show_voltage_")) {
			FILE * pFile = fopen("hp/iPod_Control/Device/_show_voltage_", "w");
			fclose(pFile);
		}
	}
}

void battery_voltage() {
	if (file_exists("hp/iPod_Control/Device/_show_voltage_")) {
		if (!file_exists("hp/iPod_Control/Device/_show_voltage")) {
			rename("hp/iPod_Control/Device/_show_voltage_", "hp/iPod_Control/Device/_show_voltage");
		} else {
			remove("hp/iPod_Control/Device/_show_voltage_");
		}
	} else {
		if (!file_exists("hp/iPod_Control/Device/_show_voltage")) {
			FILE * pFile = fopen("hp/iPod_Control/Device/_show_voltage", "w");
			fclose(pFile);
		}
	}
}

int file_exists(char *file) {
	FILE * pFile;
	pFile = fopen(file, "r");
	fclose(pFile);
	if (pFile != NULL) {
		return 1;
	} else {
		return 0;
	}
}
