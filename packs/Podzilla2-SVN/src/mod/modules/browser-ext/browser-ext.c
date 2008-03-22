/*
 * Last updated: March 13, 2008
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

#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern void pz_exec();
extern PzWindow *new_terminal_window_with();

static PzModule *module;
static ttk_menu_item browser_extension;

const char *get_filename(const char *file)
{
	static char ret[256];
	sprintf(ret, "%s", (strrchr(file, '/')+1));
	return ret;
}

const char *get_dirname(const char *file)
{
	int full_length = strlen(file);
	int filename_length = strlen(get_filename(file));
	int length = full_length-filename_length;
	char dir[length+1];
	strncpy(dir, file, (length));
	strcat(dir, "\0");
	static char ret[256];
	sprintf(ret, "%s", dir);
	return ret;
}

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

int check_is_file(const char *file)
{
	struct stat st;
	stat(file, &st);
	return !S_ISDIR(st.st_mode);
}

int check_is_dir(const char *file)
{
	return check_is_file(file);
}

int check_nothing(const char *file)
{
	return 1;
}

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

static TWindow *pz_exec_binary(const char *file)
{
	pz_exec(file);
	return NULL;
}

static PzWindow *terminal_exec_binary(const char *file)
{
	const char *f = (const char *)get_filename(file);
	const char *d = (const char *)get_dirname(file);
	chdir(d);
	char script[256];
	sprintf(script, "cd %s; exec %s; exit", d, f);
	const char *const cmd[] = {"sh", "-c", script, NULL};
	return new_terminal_window_with("/bin/sh", (char *const *)cmd);	
}

static PzWindow *load_terminal_handler(ttk_menu_item * item)
{
	return terminal_exec_binary(item->data);
}

static void cleanup()
{
	pz_browser_remove_handler(check_is_binary);
}

static void init_launch()
{
	module = pz_register_module("browser-ext", cleanup);

	browser_extension.name = N_("Execute in terminal");
	browser_extension.makesub = load_terminal_handler;
	pz_browser_add_action(check_is_binary, &browser_extension);
	pz_browser_set_handler(check_is_binary, pz_exec_binary);
}

PZ_MOD_INIT(init_launch)
