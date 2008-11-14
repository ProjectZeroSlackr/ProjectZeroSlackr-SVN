/*
 * Last updated: Nov 13, 2008
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

#include "../../launch/browser-ext.h"

static PzModule *module;
static ttk_menu_item browser_extension_terminal;
static ttk_menu_item browser_extension_pz_exec_kill;
static int backlight;

// String manipulators
const char *get_filename(const char *file)
{
	char *c = strrchr(file, '/');
	if (c != NULL) {
		static char ret[256];
		sprintf(ret, "%s", c+1);
		return ret;
	} else {
		return file;
	}
}
const char *get_dirname(const char *file)
{
	if (strrchr(file, '/') != NULL) {
		static char ret[256];
		int filename, fullname;
		filename = strlen(get_filename(file));
		fullname = strlen(file);
		snprintf(ret, fullname-filename+1, "%s", file);
		return ret;
	} else {
		return "";
	}
}

// Convenient checkers
int check_file_ext(const char *file, const char *ext)
{
	struct stat st;
	stat (file, &st);
	if (S_ISDIR(st.st_mode)) return 0;
	char *file_ext = strrchr(file, '.');
	int length = strlen(file_ext);
	if (length != strlen(ext)) return 0;
	int i;
	for (i = 0; i < length; i++) {
		if ( file_ext[i] != ext[i] && file_ext[i] != ext[i]-32 ){
			return 0;
		}
	}
	return 1;
}
int check_is_dir(const char *file)
{
	struct stat st;
 	stat(file, &st);
 	return S_ISDIR(st.st_mode);
}
int check_is_file(const char *file)
{
	return !check_is_dir(file);
}
int check_nothing(const char *file)
{
	return 1;
}

// Backlight
void toggle_backlight()
{
	if (backlight == -2) {
		backlight = pz_get_int_setting (pz_global_config, BACKLIGHT_TIMER);
		pz_set_backlight_timer(-2);
	} else {
		pz_set_backlight_timer(backlight);
		backlight = -2;
	}
}
PzWindow *toggle_backlight_window()
{
	toggle_backlight();
	return NULL;
}

// CPU speed
// See browser-ext.h for postscalar constants
#define outl(a, b) \
	(*(volatile unsigned int *)(b) = (a))
#define inl(a) \
	(*(volatile unsigned int *)(a))
#define CLOCK_SCALER	0x60006034
#define CLOCK_POLICY	0x60006020
#define RUN_CLK(x) (0x20000000 | ((x) <<  4))
#define RUN_GET(x) ((inl(CLOCK_POLICY) & 0x0fffff8f) | RUN_CLK(x))
#define RUN_SET(x) outl(RUN_GET(x), CLOCK_POLICY)
void set_cpu_speed(int postscalar)
{
	outl(inl(0x70000020) | (1 << 30), 0x70000020);
	RUN_SET(0x2);
	outl(0xaa020008 | (postscalar << 8), CLOCK_SCALER);
	int x;
	for (x = 0; x < 10000; x++);
	RUN_SET(0x7);
}

// Binary executer
static int check_is_binary(const char *file)
{
	FILE *fp;
	unsigned char header[12];
	struct stat st;

	stat(file, &st); 
	if(S_ISBLK(st.st_mode)||S_ISCHR(st.st_mode))
		return 0;
	if((fp=fopen(file, "r"))==NULL) {
		perror(file);
		return 0;
	}

	fread(header, sizeof(char), 12, fp);
	
	fclose(fp);
	if(strncmp((const char *)header, "bFLT", 4)==0)
		return 1;
	if(strncmp((const char *)header, "#!/bin/sh", 9)==0)
		return 1;
	return 0;
}
static TWindow *pz_exec_kill_binary(const char *file)
{
	pz_exec_kill(file);
	return NULL;
}
static PzWindow *load_pz_exec_kill_handler(ttk_menu_item * item)
{
	return pz_exec_kill_binary(item->data);
}
static PzWindow *terminal_exec_binary(const char *file)
{
	static char script[256];
	snprintf(script, 256, "stty erase \"^H\"; %s;"
		"read -p \"Press any key to exit...\"; exit", file);
	const char *const cmd[] = {"sh", "-c", script, NULL};
	return new_terminal_window_with(
		"/bin/sh",
		(char *const *)cmd
		);
}
static PzWindow *load_terminal_handler(ttk_menu_item * item)
{
	return terminal_exec_binary(item->data);
}

// Mini-browser mod (code from browser module)
#ifndef MAXPATHLEN
#define MAXPATHLEN 512
#endif
typedef struct _Entry_mod
{
	char *name;
	mode_t mode;
} Entry_mod;
TWidget *read_directory_mod(const char *dirname,
	int check(const char *file),TWindow *handler(ttk_menu_item *item))
{
	TWidget *ret;
	DIR *dir;
	struct stat st;
	struct dirent *subdir;

	ret = ttk_new_menu_widget(NULL, ttk_menufont, ttk_screen->w -
			ttk_screen->wx, ttk_screen->h - ttk_screen->wy);
	ttk_menu_set_i18nable(ret, 0);
	dir = opendir(dirname);

	while ((subdir = readdir(dir))) {
		ttk_menu_item *item;
		Entry_mod *entry;
		if (strncmp(subdir->d_name,".", strlen(subdir->d_name)) == 0 ||
			strncmp(subdir->d_name,"..",strlen(subdir->d_name)) == 0
			) {
			continue;
		}
		
		if (check(subdir->d_name) == 1) {
			stat(subdir->d_name, &st);
			
			item = (ttk_menu_item *)calloc(1, sizeof(ttk_menu_item));
			entry = (Entry_mod *)malloc(sizeof(Entry_mod));
			entry->name = (char *)strdup(subdir->d_name);
			entry->mode = st.st_mode;
			
			item->name = (char *)entry->name;
			char path[MAXPATHLEN];
			snprintf(path, MAXPATHLEN-1, "%s/%s", dirname, item->name);
			item->data = (char *)strdup(path);
			item->free_data = 1;
			item->free_name = 1;
			item->makesub = handler;
			ttk_menu_append(ret, item);
		} else {
			continue;
		}
	}
	closedir(dir);
	return ret;
}
TWindow *open_directory_title_mod(const char *filename, const char *title,
	int check(const char *file), TWindow *handler(ttk_menu_item *item))
{
	TWindow *ret;
	TWidget *menu;
	int lwd;

	lwd = open(".", O_RDONLY);

	chdir(filename);
	ret = pz_new_window(_(title), PZ_WINDOW_NORMAL);

	menu = read_directory_mod(filename, check, handler);
	menu->data2 = malloc(sizeof(int));
	*(int *)menu->data2 = lwd;
	ttk_add_widget(ret, menu);

	ret->data = 0x12345678;
	return pz_finish_window(ret);
}

// General
static void cleanup()
{
	pz_browser_remove_handler(check_is_binary);
}
static void init_launch()
{
	module = pz_register_module("browser-ext", cleanup);
	backlight = -2;
	
	browser_extension_terminal.name = N_("Execute in terminal");
	browser_extension_terminal.makesub = load_terminal_handler;
	pz_browser_add_action(check_is_binary, &browser_extension_terminal);
	browser_extension_pz_exec_kill.name = N_("Exit and execute");
	browser_extension_pz_exec_kill.makesub = load_pz_exec_kill_handler;
	pz_browser_add_action(check_is_binary, &browser_extension_pz_exec_kill);
}

PZ_MOD_INIT(init_launch)
