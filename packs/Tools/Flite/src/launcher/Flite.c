/*
 * Last updated: July 22, 2008
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

#include "browser-ext.h"

#define ENABLE_FLITE 1

static PzModule *module;
static PzConfig *config;
static ttk_menu_item browser_extension_read;
static ttk_menu_item browser_extension_convert;
static pid_t pid;
static const char *path_read, *path_convert, *dir;
static const char *off_on_options[] = {"Off", "On", 0};

// Blatant rip from PZ0's textview.c
static int check_is_ascii_file(const char *filename)
{
	FILE *fp;
	unsigned char buf[20], *ptr;
	long file_len;
	struct stat ftype; 

	stat(filename, &ftype); 
	if(S_ISBLK(ftype.st_mode)||S_ISCHR(ftype.st_mode))
		return 0;
	if((fp=fopen(filename, "r"))==NULL) {
		perror(filename);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	rewind(fp);
	fread(buf, file_len<20?file_len:20, 1, fp);
	fclose(fp);
	
	for(ptr=buf;ptr-buf<(file_len<20?file_len:20);ptr++)
		if(*ptr<7||*ptr>127)
			return 0;
	return 1;
}

static int is_active(ttk_menu_item *item)
{
	if (pid) return 1;
	return 0;
}

static PzWindow *kill_flite()
{
	if (pid) kill(pid, SIGTERM);
	return NULL;
}

static PzWindow *read_file(const char *file)
{
	if (MPD_ACTIVE) {
		pz_error("MPD is active. Unable to launch Flite.");
		return NULL;
	}
	if (strchr(file, ' ') != NULL) {
		pz_error("Sorry, Flite cannot process files with spaces in their names.");
		return NULL;
	}
	const char *const cmd[] = {"Read.sh", file, NULL};
	switch (pid = vfork()) {
		case 0:
			execv(
				path_read,
				(char *const *)cmd
			);
			pz_perror("Flite Read-File.sh execv failed.");
			_exit(0);
		default:
			break;
	}
	if (file != NULL) {
		PzWindow *(*new_podread_window)(const char *f);
		new_podread_window = pz_module_softdep("podread", "new_podread_window_with_file");
		if (new_podread_window) {
			return new_podread_window(file);
		} else {
			return TTK_MENU_UPONE;
		}
	} else {
		return TTK_MENU_UPONE;
	}
}
static PzWindow *load_file_handler_read(ttk_menu_item * item)
{
	return read_file((const char *)(item->data)); // Recast for browser_mod
}

PzWindow *convert_file(const char *file)
{
	if (strchr(file, ' ') != NULL) {
		pz_error("Sorry, Flite cannot process files with spaces in their names.");
		return NULL;
	}
	const char *const cmd[] = {"Convert.sh", file, NULL};
	pz_execv(
		path_convert,
		(char *const *)cmd
	);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_convert(ttk_menu_item * item)
{
	return convert_file(item->data);
}

static PzWindow *browse_texts()
{
	return open_directory_title_mod(dir, "Flite Texts",
		check_is_ascii_file, load_file_handler_read);
}

static void cleanup()
{
	pz_save_config(config);
	kill_flite();
}

static void init_launch() 
{
	module = pz_register_module("Flite", cleanup);	
	config = pz_load_config("/opt/Tools/Flite/Conf/Enable.conf");
	if (!pz_get_setting(config, ENABLE_FLITE))
		pz_set_int_setting(config, ENABLE_FLITE, 0);
	
	pz_menu_add_stub_group("/Tools/Flite", "Accessibility");
	pz_menu_add_setting_group("/Tools/Flite/Enable Flite", "~Settings", ENABLE_FLITE, config, off_on_options);
	
	if (pz_get_int_setting(config, ENABLE_FLITE) == 1) {
		path_read = "/opt/Tools/Flite/Launch/Read.sh";
		path_convert = "/opt/Tools/Flite/Launch/Convert.sh";
		dir = "/opt/Tools/Flite/Text";
		
		pz_menu_add_action_group("/Tools/Flite/Text", "Browse", browse_texts);
		pz_menu_add_action_group("/Tools/Flite/Kill Flite", "~Settings", kill_flite);
		pz_get_menu_item("/Tools/Flite/Kill Flite")->visible = is_active;
		pz_menu_add_action_group("/Tools/Flite/Toggle Backlight", "~Settings", toggle_backlight_window);
		
		browser_extension_read.name = N_("Read with Flite");
		browser_extension_read.makesub = load_file_handler_read;
		pz_browser_add_action (check_is_ascii_file, &browser_extension_read);
		browser_extension_convert.name = N_("Convert to .wav");
		browser_extension_convert.makesub = load_file_handler_convert;
		pz_browser_add_action (check_is_ascii_file, &browser_extension_convert);
		
		// Welcome
		setup_sigchld_handler();
		switch (pid = vfork()) {
			case 0:
				execl(path_read, "Read.sh", NULL);
				_exit(0);
			default:
				break;
		}
		wait(NULL);
	}
	pz_menu_sort("/Tools/Flite");
}

PZ_MOD_INIT(init_launch)
