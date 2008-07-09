/*
 * Last updated: July 9, 2008
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

/*
 * Uses iniParser for parsing initiation file
 * Note that if you know the proper way to do the loop code in the #if 0 / #endif
 * sections, please contact me. A loop is better than hardcoded.
 */

#include "ipod_common.h"
#include "iniparser/iniparser.h"

// iPod Tweaks and Options
extern int ipod_scale_type;
extern int ipod_smooth_type;
extern u32 no_alpha;
extern u32 synchronize_flag;
// Hardware settings
extern int ipod_cpu_speed;
//extern int ipod_contrast;
extern int ipod_brightness;
extern int ipod_backlight;
// Input settings
extern int ipod_menu_scrolling;
extern int ipod_map_triggers;
extern int ipod_rapid_fire;
#ifndef NOSOUND
// Frameskip and Sound Options
extern int ipod_volume;
#endif

static char *ini_path;

// Doesn't work ; /
#if 0
static const char *keys[] = {
	// iPod Tweaks and Options
	"tweaks:scale_type",
	"tweaks:smooth_type",
	"tweaks:no_alpha",
	"tweaks:synchronize_flag",
	// Hardware settings
	"hardware:cpu_speed",
	//"hardware:contrast",
	"hardware:brightness",
	"hardware:backlight",
	// Input settings
	"input:menu_scrolling",
	"input:map_triggers",
	"input:rapid_fire",
	#ifndef NOSOUND
	// Frameskip and Sound Options
	"sound:volume",
	#endif
	NULL
};

static int *values[] = {
	// iPod Tweaks and Options
	&ipod_scale_type,
	&ipod_smooth_type,
	(int *)(&no_alpha),
	(int *)(&synchronize_flag),
	// Hardware settings
	&ipod_cpu_speed,
	//&ipod_contrast,
	&ipod_brightness,
	&ipod_backlight,
	// Input settings
	&ipod_menu_scrolling,
	&ipod_map_triggers,
	&ipod_rapid_fire,
	#ifndef NOSOUND
	// Frameskip and Sound Options
	&ipod_volume,
	#endif
	NULL
};

static int defaults[] = {
	// iPod Tweaks and Options
	3,	// Scale to width
	1,	// Type 1
	0,	// Alpha blending on
	1,	// Synchronize on
	// Hardware settings
	2,	// 78MHz
	//101,// Default constrast level
	16,	// Default brightness level
	1,	// Backlight on
	// Input settings
	0,	// Menu scrolling off
	0,	// Map triggers off
	0,	// Rapid fire off
	#ifndef NOSOUND
	// Frameskip and Sound Options
	70,	// Good volume level
	#endif
	0
};

#endif

static const char *igpsp_conf_filenames[] = {
	"igpSP.ini",
	"Conf/igpSP.ini",
	"/igpSP.ini",
	"/mnt/igpSP.ini",
	"/mnt/igpSP/igpSP.ini",
	"/mnt/igpSP/Conf/igpSP.ini",
	NULL
};

static void ipod_create_conf(char *path)
{
	FILE *f;
	f = fopen(path, "w");
	// Just the sections - values set later during parsing
	// Current version of iniparser does not support comments
	fprintf(f,
		//"# igpSP Initiation File\n"
		//"# Format: setting = value\n"
		//"# Use '#' for comments\n"
		"\n[tweaks]\n"
		"\n[hardware]\n"
		"\n[input]\n"
		"\n[sound]\n"
		);
	fclose(f);
}

static dictionary *ipod_find_ini()
{
	int i;
	dictionary *ini;
	for (i = 0; igpsp_conf_filenames[i]; i++) {
		ini = iniparser_load(igpsp_conf_filenames[i]);
		if (ini != NULL && iniparser_getnsec(ini) == 4) {
			ini_path = (char *)igpsp_conf_filenames[i];
			break;
		}
	}
	if (ini == NULL) {
		ini_path = (char *)igpsp_conf_filenames[0];
		ipod_create_conf(ini_path);
		ini = iniparser_load(ini_path);
	}
	return ini;
}

static void ipod_set_setting(dictionary *ini, char *setting, int new_value)
{
	char value[4];
	snprintf(value, 4, "%i", new_value);
	iniparser_set(ini, setting, value);
}

static int ipod_get_setting(dictionary *ini, char *setting, int default_value)
{
	int i;
	i = iniparser_getint(ini, setting, -1);
	if (i == -1) {
		ipod_set_setting(ini, setting, default_value);
		i = iniparser_getint(ini, setting, -1);
	}
	return i;
}

static void ipod_parse_conf(dictionary *ini)
{
// Doesn't work ; /
#if 0
	int i, size;
	for (i = 0; i < keys[i]; i++) {
		values[i] = ipod_get_setting(ini, keys[i], defaults[i]);
	}
#endif
	
	// iPod Tweaks and Options
	ipod_scale_type =
		ipod_get_setting(ini, "tweaks:scale_type", 3);
	ipod_smooth_type =
		ipod_get_setting(ini, "tweaks:smooth_type", 1);
	no_alpha =
		(u32)ipod_get_setting(ini, "tweaks:no_alpha", 0);
	synchronize_flag =
		(u32)ipod_get_setting(ini, "tweaks:synchronize", 1);
	// Hardware settings
	ipod_cpu_speed =
		ipod_get_setting(ini, "hardware:cpu_speed", 2);
	//ipod_contrast =
		//ipod_get_setting(ini, "hardware:contrast", 101);
	ipod_brightness =
		ipod_get_setting(ini, "hardware:brightness", 16);
	ipod_backlight =
		ipod_get_setting(ini, "hardware:backlight", 1);
	// Input settings
	ipod_menu_scrolling =
		ipod_get_setting(ini, "input:menu_scrolling", 0);
	ipod_map_triggers =
		ipod_get_setting(ini, "input:map_triggers", 0);
	ipod_rapid_fire =
		ipod_get_setting(ini, "input:rapid_fire", 0);
	#ifndef NOSOUND
	// Frameskip and Sound Options
	ipod_volume =
		ipod_get_setting(ini, "sound:volume", 70);
	#endif
}

static void ipod_save_conf()
{
	dictionary *ini;
	ini = iniparser_load(ini_path);
	
// Doesn't work ; /
#if 0
	int i, size;
	for (i = 0; i < keys[i]; i++) {
		ipod_set_setting(ini, keys[i], values[i]);
	}
#endif
	
	// iPod Tweaks and Options
	ipod_set_setting(ini, "tweaks:scale_type", ipod_scale_type);
	ipod_set_setting(ini, "tweaks:smooth_type", ipod_smooth_type);
	ipod_set_setting(ini, "tweaks:no_alpha", (int)(no_alpha));
	ipod_set_setting(ini, "tweaks:synchronize", (int)(synchronize_flag));
	// Hardware settings
	ipod_set_setting(ini, "hardware:cpu_speed", ipod_cpu_speed);
	//ipod_set_setting(ini, "hardware:contrast", ipod_contrast);
	ipod_set_setting(ini, "hardware:brightness", ipod_brightness);
	ipod_set_setting(ini, "hardware:backlight", ipod_backlight);
	// Input settings
	ipod_set_setting(ini, "input:menu_scrolling", ipod_menu_scrolling);
	ipod_set_setting(ini, "input:map_triggers", ipod_map_triggers);
	ipod_set_setting(ini, "input:rapid_fire", ipod_rapid_fire);
	#ifndef NOSOUND
	// Frameskip and Sound Options
	ipod_set_setting(ini, "sound:volume", ipod_volume);
	#endif
	
	iniparser_dump_ini(ini, fopen(ini_path, "w"));
	iniparser_freedict(ini);
}

void ipod_update_settings()
{
	ipod_update_cpu_speed();
	//ipod_update_contrast(); // No support for monochrome iPods - see ipod_video.c
	ipod_update_brightness();
	ipod_update_backlight();
	ipod_update_scale_type();
#ifndef NOSOUND
	ipod_update_volume();
#endif
}

void ipod_init_conf()
{
	dictionary *ini;
	ini = ipod_find_ini();
	
	ipod_parse_conf(ini);
	iniparser_freedict(ini);
}


void ipod_exit_conf()
{
	ipod_save_conf();
}

