/*
 * Copyright (C) 2006 Jonathynne Bettencourt (jonrelay)
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
#include "menudata.h"
#include "iconmenu.h"
#include "icon2menu.h"
#include "bigiconmenu.h"
#include "bigicon2menu.h"
#include "dockmenu.h"
#include "smdockmenu.h"
#include "mdockmenu.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


ttk_font iconui_font;

#define iconui_setting_iconsize 1
#define iconui_setting_iconcache 2
static const char * iconui_setting_iconsize_options[] = {"Off", "Small List", "Large List", "Small Array", "Large Array", "Small Dock", "Magnifying Dock", "Large Dock", 0};
static const char * iconui_setting_iconcache_options[] = {"Off", "On", 0};

typedef struct iconui_cache_record {
	char * name;
	ttk_surface icon;
	struct iconui_cache_record * next;
} iconui_cache_record;

static iconui_cache_record * iconui_cache[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//#ifdef IPOD
//#define COLOR_SCHEME_PATH "/usr/share/schemes/default.cs"
//#else
//#define COLOR_SCHEME_PATH "schemes/default.cs"
//#endif

static ttk_surface iconui_copy_surface(ttk_surface srf)
{
	int w, h;
	ttk_surface nsrf;
	
	ttk_surface_get_dimen(srf, &w, &h);
	nsrf = ttk_new_surface(w, h, ttk_screen->bpp);
	ttk_blit_image(srf, nsrf, 0, 0);
	return nsrf;
}

static char * iconui_munge(char * s)
{
	char * munge = malloc(strlen(s)*3);
	char * d = munge;
	while (*s) {
		/* FAT32 doesn't support these characters in file names, 'cause it's stupid. */
		switch (*s) {
		case '/':
			(*(d++)) = '_';
			(*(d++)) = 'S';
			(*(d++)) = 'L'; /* SLash */
			break;
		case '\\':
			(*(d++)) = '_';
			(*(d++)) = 'B';
			(*(d++)) = 'S'; /* BackSlash */
			break;
		case ':':
			(*(d++)) = '_';
			(*(d++)) = 'C';
			(*(d++)) = 'L'; /* CoLon */
			break;
		case '*':
			(*(d++)) = '_';
			(*(d++)) = 'A';
			(*(d++)) = 'S'; /* ASterisk */
			break;
		case '?':
			(*(d++)) = '_';
			(*(d++)) = 'Q';
			(*(d++)) = 'U'; /* QUestion mark */
			break;
		case '"':
			(*(d++)) = '_';
			(*(d++)) = 'Q';
			(*(d++)) = 'T'; /* QuoTation mark */
			break;
		case '<':
			(*(d++)) = '_';
			(*(d++)) = 'L';
			(*(d++)) = 'T'; /* Less Than */
			break;
		case '>':
			(*(d++)) = '_';
			(*(d++)) = 'G';
			(*(d++)) = 'T'; /* Greater Than */
			break;
		case '|':
			(*(d++)) = '_';
			(*(d++)) = 'V';
			(*(d++)) = 'B'; /* Vertical Bar */
			break;
		case '.':
			if ( ((*(s+1))=='.') && ((*(s+2))=='.') ) {
				(*(d++)) = '_';
				(*(d++)) = 'E';
				(*(d++)) = 'L'; /* ELipsis */
				s+=2;
				break;
			}
		default:
			(*(d++)) = (*s);
			break;
		}
		s++;
	}
	(*d) = 0;
	return munge;
}

void iconui_cache_clear()
{
	int i;
	iconui_cache_record * rcd;
	for (i=0; i<32; i++) {
		rcd = iconui_cache[i];
		while (rcd) {
			iconui_cache_record * nr = rcd->next;
			if (rcd->name) free(rcd->name);
			if (rcd->icon) ttk_free_surface(rcd->icon);
			free(rcd);
			rcd = nr;
		}
		iconui_cache[i] = 0;
	}
}

ttk_surface iconui_cache_get(char * name)
{
	iconui_cache_record * rcd;
	rcd = iconui_cache[(*name) & 0x1F];
	while (rcd) {
		if (rcd->name && !strcmp(name, rcd->name)) {
			return iconui_copy_surface(rcd->icon);
		} else {
			rcd = rcd->next;
		}
	}
	return 0;
}

ttk_surface iconui_cache_set(char * name, ttk_surface icon)
{
	iconui_cache_record * rcd;
	rcd = iconui_cache[(*name) & 0x1F];
	if (!rcd) {
		rcd = iconui_cache[(*name) & 0x1F] = (iconui_cache_record *)calloc(1, sizeof(iconui_cache_record));
		if (rcd) {
			rcd->name = strdup(name);
			rcd->icon = iconui_copy_surface(icon);
			rcd->next = 0;
		}
		return icon;
	} else {
		while (rcd) {
			if (rcd->name && !strcmp(name, rcd->name)) {
				if (rcd->icon) ttk_free_surface(rcd->icon);
				rcd->icon = iconui_copy_surface(icon);
				return icon;
			} else if (!rcd->next) {
				rcd->next = (iconui_cache_record *)calloc(1, sizeof(iconui_cache_record));
				rcd = rcd->next;
				if (rcd) {
					rcd->name = strdup(name);
					rcd->icon = iconui_copy_surface(icon);
					rcd->next = 0;
				}
				return icon;
			} else {
				rcd = rcd->next;
			}
		}
	}
	return icon;
}

ttk_surface iconui_get_icon(char * name)
{
	static int mnt_na = 1; /* 1 - unchecked, 0 - not accessible, 2 - accessible */
	static int hp_na = 1;
	static int share_na = 1;
	static int icons_na = 1;
#ifdef NEVER
	static int cs_na = 1;
	static int epoch = 0;
#endif
	char path[256];
	char * s = iconui_munge(name);
	int iconcache = ipod_get_setting((76));
	ttk_surface iconsrf;
#ifdef NEVER
	if (ttk_epoch > epoch) {
		cs_na = 1;
		epoch = ttk_epoch;
	}
#endif
	if (iconcache && (iconsrf = iconui_cache_get(s))) {
		return iconsrf;
	}
#ifdef NEVER
	if (cs_na) {
		//char * p = realpath(COLOR_SCHEME_PATH, path);
		//if (!p) {
		//	cs_na = 0;
		//} else {
			path[strlen(path)-3] = 0;
			strcat(path, "-icons/");
			if (cs_na == 1 && access(path, R_OK)) {
				cs_na = 0;
			} else {
				cs_na = 2;
				strcat(path, s);
				strcat(path, ".png");
				if (!access(path, R_OK)) {
					if (iconcache) {
						iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
					} else {
						iconsrf = ttk_load_image((const char *)path);
					}
					free(s);
					return iconsrf;
				}
			}
		//}
	}
#endif
	if (mnt_na) {
		strcpy(path, "/mnt/icons/");
		if (mnt_na == 1 && access(path, R_OK)) {
			mnt_na = 0;
		} else {
			mnt_na = 2;
			strcat(path, s);
			strcat(path, ".png");
			if (!access(path, R_OK)) {
				if (iconcache) {
					iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
				} else {
					iconsrf = ttk_load_image((const char *)path);
				}
				free(s);
				return iconsrf;
			}
		}
	}
	if (hp_na) {
		strcpy(path, "/hp/icons/");
		if (hp_na == 1 && access(path, R_OK)) {
			hp_na = 0;
		} else {
			hp_na = 2;
			strcat(path, s);
			strcat(path, ".png");
			if (!access(path, R_OK)) {
				if (iconcache) {
					iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
				} else {
					iconsrf = ttk_load_image((const char *)path);
				}
				free(s);
				return iconsrf;
			}
		}
	}
	if (share_na) {
		strcpy(path, "/usr/share/icons/");
		if (share_na == 1 && access(path, R_OK)) {
			share_na = 0;
		} else {
			share_na = 2;
			strcat(path, s);
			strcat(path, ".png");
			if (!access(path, R_OK)) {
				if (iconcache) {
					iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
				} else {
					iconsrf = ttk_load_image((const char *)path);
				}
				free(s);
				return iconsrf;
			}
		}
	}
	if (icons_na) {
		strcpy(path, "/opt/ZacZilla/Data/icons/");
		if (icons_na == 1 && access(path, R_OK)) {
			icons_na = 0;
		} else {
			icons_na = 2;
			strcat(path, s);
			strcat(path, ".png");
			if (!access(path, R_OK)) {
				if (iconcache) {
					iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
				} else {
					iconsrf = ttk_load_image((const char *)path);
				}
				free(s);
				return iconsrf;
			}
		}
	}
	sprintf(path,"/opt/ZacZilla/Data/icons/%c", s);
	strcat(path, ".png");
	if (!access(path, R_OK)) {
		if (iconcache) {
			iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
		} else {
			iconsrf = ttk_load_image((const char *)path);
		}
		free(s);
		return iconsrf;
	}
#ifdef NEVER
	if (cs_na) {
		//char * p = realpath(COLOR_SCHEME_PATH, path);
		//if (p) {
			path[strlen(path)-3] = 0;
			strcat(path, "-icons/Default.png");
			if (!access(path, R_OK)) {
				if (iconcache) {
					iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
				} else {
					iconsrf = ttk_load_image((const char *)path);
				}
				free(s);
				return iconsrf;
			}
		//}
	}
#endif
	if (mnt_na) {
		strcpy(path, "/mnt/icons/Default.png");
		if (!access(path, R_OK)) {
			if (iconcache) {
				iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
			} else {
				iconsrf = ttk_load_image((const char *)path);
			}
			free(s);
			return iconsrf;
		}
	}
	if (hp_na) {
		strcpy(path, "/hp/icons/Default.png");
		if (!access(path, R_OK)) {
			if (iconcache) {
				iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
			} else {
				iconsrf = ttk_load_image((const char *)path);
			}
			free(s);
			return iconsrf;
		}
	}
	if (share_na) {
		strcpy(path, "/usr/share/icons/Default.png");
		if (!access(path, R_OK)) {
			if (iconcache) {
				iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
			} else {
				iconsrf = ttk_load_image((const char *)path);
			}
			free(s);
			return iconsrf;
		}
	}
	if (icons_na) {
		sprintf(path, "/opt/ZacZilla/Data/icons/Default.png");
		if (!access(path, R_OK)) {
			if (iconcache) {
				iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
			} else {
				iconsrf = ttk_load_image((const char *)path);
			}
			free(s);
			return iconsrf;
		}
	}
	strcpy(path, "/opt/ZacZilla/Data/icons/Default.png");
	if (!access(path, R_OK)) {
		if (iconcache) {
			iconsrf = iconui_cache_set(s, ttk_load_image((const char *)path) );
		} else {
			iconsrf = ttk_load_image((const char *)path);
		}
		free(s);
		return iconsrf;
	}
	free(s);
	return ttk_new_surface(32, 32, ttk_screen->bpp);
}

ttk_surface iconui_get_medium_icon(char * s)
{
	ttk_surface orig = iconui_get_icon(s);
	ttk_surface sm = ttk_scale_surface(orig, 0.75);
	ttk_free_surface(orig);
	return sm;
}

ttk_surface iconui_get_small_icon(char * s)
{
	ttk_surface orig = iconui_get_icon(s);
	ttk_surface sm = ttk_scale_surface(orig, 0.5);
	ttk_free_surface(orig);
	return sm;
}

void iconui_draw_icon(ttk_surface srf, char * s, int x, int y)
{
	ttk_surface icon = iconui_get_icon(s);
	ttk_blit_image(icon, srf, x, y);
	ttk_free_surface(icon);
}

void iconui_draw_medium_icon(ttk_surface srf, char * s, int x, int y)
{
	ttk_surface icon = iconui_get_medium_icon(s);
	ttk_blit_image(icon, srf, x, y);
	ttk_free_surface(icon);
}

void iconui_draw_small_icon(ttk_surface srf, char * s, int x, int y)
{
	ttk_surface icon = iconui_get_small_icon(s);
	ttk_blit_image(icon, srf, x, y);
	ttk_free_surface(icon);
}

void iconui_widget_update(struct header_info *hdr)
{
//	if (!hdr || !hdr->widg) return;
	//hdr->widg->w = 18;
}

void iconui_widget_draw(struct header_info * hdr, ttk_surface srf)
{
//	if (!hdr || !hdr->widg || !srf) return;
	//char * title = (char *)ttk_windows->w->title;
	//TWidget * wid = hdr->widg;
//	iconui_draw_small_icon( srf, title, wid->x + (wid->w - 16)/2, wid->y + (wid->h - 16)/2 );
}

TWindow * iconui_mh_sub(ttk_menu_item * item)
{
	TWindow * ret;
	TWidget * menu_wid = (TWidget *)item->data;
	int iconsize = 1;//ipod_get_setting((75));
	int iconcache = 1;//ipod_get_setting((76));
	iconsize=1;
	iconcache=1;
	int i = 0;
	while (1) {
		ttk_menu_item * mi = ttk_menu_get_item(menu_wid, i);
		if (!mi) break;
		if (mi->makesub == ttk_mh_sub) mi->makesub = iconui_mh_sub;
		if (iconcache) ttk_free_surface(iconui_get_icon((char *)mi->name));
		i++;
	}
	
	switch (iconsize) {
	case 1: /* small list */
		menu_wid->draw = iconui_menu_draw;
		menu_wid->frame = iconui_menu_frame;
		menu_wid->down = iconui_menu_down;
		menu_wid->button = iconui_menu_button;
		menu_wid->scroll = iconui_menu_scroll;
		menu_wid->destroy = iconui_menu_free;
		//menu_wid->focusable=1;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 2: /* large list */
		menu_wid->draw = bigiconui_menu_draw;
		menu_wid->frame = bigiconui_menu_frame;
		menu_wid->down = bigiconui_menu_down;
		menu_wid->button = bigiconui_menu_button;
		menu_wid->scroll = bigiconui_menu_scroll;
		menu_wid->destroy = bigiconui_menu_free;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 3: /* small array */
		menu_wid->draw = icon2ui_menu_draw;
		menu_wid->frame = icon2ui_menu_frame;
		menu_wid->down = icon2ui_menu_down;
		menu_wid->button = icon2ui_menu_button;
		menu_wid->scroll = icon2ui_menu_scroll;
		menu_wid->destroy = icon2ui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 4: /* large array */
		menu_wid->draw = bigicon2ui_menu_draw;
		menu_wid->frame = bigicon2ui_menu_frame;
		menu_wid->down = bigicon2ui_menu_down;
		menu_wid->button = bigicon2ui_menu_button;
		menu_wid->scroll = bigicon2ui_menu_scroll;
		menu_wid->destroy = bigicon2ui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 5: /* small dock */
		menu_wid->draw = smdockiconui_menu_draw;
		menu_wid->frame = smdockiconui_menu_frame;
		menu_wid->down = smdockiconui_menu_down;
		menu_wid->button = smdockiconui_menu_button;
		menu_wid->scroll = smdockiconui_menu_scroll;
		menu_wid->destroy = smdockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 6: /* mag dock */
		menu_wid->draw = mdockiconui_menu_draw;
		menu_wid->frame = mdockiconui_menu_frame;
		menu_wid->down = mdockiconui_menu_down;
		menu_wid->button = mdockiconui_menu_button;
		menu_wid->scroll = mdockiconui_menu_scroll;
		menu_wid->destroy = mdockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 7: /* large dock */
		menu_wid->draw = dockiconui_menu_draw;
		menu_wid->frame = dockiconui_menu_frame;
		menu_wid->down = dockiconui_menu_down;
		menu_wid->button = dockiconui_menu_button;
		menu_wid->scroll = dockiconui_menu_scroll;
		menu_wid->destroy = dockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	}
	
	ret = ttk_new_window();
	ttk_add_widget(ret, menu_wid);
	ttk_window_title(ret, item->name);
	ret->widgets->v->draw (ret->widgets->v, ret->srf);
	return ret;
}

TWindow * iconui_new_menu_window(TWidget * menu_wid)
{
	TWindow * ret;
	int iconsize = 1;//ipod_get_setting((75));
	int iconcache = 1;//ipod_get_setting((76));
	iconsize=1;
	iconcache=1;
	int i = 0;
	while (1) {
		ttk_menu_item * mi = ttk_menu_get_item(menu_wid, i);
		if (!mi) break;
		if (mi->makesub == ttk_mh_sub) mi->makesub = iconui_mh_sub;
		if (iconcache) ttk_free_surface(iconui_get_icon((char *)mi->name));
		i++;
	}
	
	switch (iconsize) {
	case 1: /* small list */
		menu_wid->draw = iconui_menu_draw;
		menu_wid->frame = iconui_menu_frame;
		menu_wid->down = iconui_menu_down;
		menu_wid->button = iconui_menu_button;
		menu_wid->scroll = iconui_menu_scroll;
		menu_wid->destroy = iconui_menu_free;
		//menu_wid->focusable=1;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 2: /* large list */
		menu_wid->draw = bigiconui_menu_draw;
		menu_wid->frame = bigiconui_menu_frame;
		menu_wid->down = bigiconui_menu_down;
		menu_wid->button = bigiconui_menu_button;
		menu_wid->scroll = bigiconui_menu_scroll;
		menu_wid->destroy = bigiconui_menu_free;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 3: /* small array */
		menu_wid->draw = icon2ui_menu_draw;
		menu_wid->frame = icon2ui_menu_frame;
		menu_wid->down = icon2ui_menu_down;
		menu_wid->button = icon2ui_menu_button;
		menu_wid->scroll = icon2ui_menu_scroll;
		menu_wid->destroy = icon2ui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 4: /* large array */
		menu_wid->draw = bigicon2ui_menu_draw;
		menu_wid->frame = bigicon2ui_menu_frame;
		menu_wid->down = bigicon2ui_menu_down;
		menu_wid->button = bigicon2ui_menu_button;
		menu_wid->scroll = bigicon2ui_menu_scroll;
		menu_wid->destroy = bigicon2ui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 5: /* small dock */
		menu_wid->draw = smdockiconui_menu_draw;
		menu_wid->frame = smdockiconui_menu_frame;
		menu_wid->down = smdockiconui_menu_down;
		menu_wid->button = smdockiconui_menu_button;
		menu_wid->scroll = smdockiconui_menu_scroll;
		menu_wid->destroy = smdockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 6: /* mag dock */
		menu_wid->draw = mdockiconui_menu_draw;
		menu_wid->frame = mdockiconui_menu_frame;
		menu_wid->down = mdockiconui_menu_down;
		menu_wid->button = mdockiconui_menu_button;
		menu_wid->scroll = mdockiconui_menu_scroll;
		menu_wid->destroy = mdockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	case 7: /* large dock */
		menu_wid->draw = dockiconui_menu_draw;
		menu_wid->frame = dockiconui_menu_frame;
		menu_wid->down = dockiconui_menu_down;
		menu_wid->button = dockiconui_menu_button;
		menu_wid->scroll = dockiconui_menu_scroll;
		menu_wid->destroy = dockiconui_menu_free;
		((menu_data *)menu_wid->data)->font = iconui_font;
		((menu_data *)menu_wid->data)->epoch--;
		break;
	}
	
	ret = ttk_new_window();
	ttk_add_widget(ret, menu_wid);
	ttk_window_title(ret, "ZacZilla");
	return ret;
}

void iconui_free()
{
//	pz_save_config(iconui_config);
	//pz_free_config(iconui_config);
	iconui_cache_clear();
}

void iconui_init()
{
//	iconui_module = pz_register_module("iconui", iconui_free);
//	iconui_config = pz_load_config(pz_module_get_cfgpath(iconui_module,"iconui.conf"));
	// KERIPO MOD
	//iconui_font = ttk_get_font("SeaChel", 9);
	iconui_font = ttk_get_font("EspySans-13", 13);
	iconui_cache_clear();
	//pz_menu_add_setting("/Settings/Appearance/Icon Style", iconui_setting_iconsize, iconui_config, iconui_setting_iconsize_options);
	//pz_menu_add_setting("/Settings/Appearance/Icon Cache", iconui_setting_iconcache, iconui_config, iconui_setting_iconcache_options);
	//pz_new_menu_window = iconui_new_menu_window;
	//pz_add_header_widget("Proxy Icon", iconui_widget_update, iconui_widget_draw, "Portland");
}

//_MOD_INIT(iconui_init)

