/* Browseradmin, a podzilla2 module adding chmod functionality to the browser.
Copyright (C) 2006  David Greenberg

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pz.h"

static ttk_menu_item browseradmin_chmod_item;
//the two below may be implement at some time in the future, if guid and uid can be easily enumerated
// browseradmin_chown_item, browseradmin_chgrp_item;

void cleanup_browseradmin()
{
}

int browseradmin_openable(const char * filename)
{
	/* Everything that isn't a pipe/fifo stream can be chmodded */
	struct stat st;
	stat(filename, &st);
	return !S_ISFIFO(st.st_mode);
}

void browseradmin_chmod_usr_labels(const char * fn, char * chmod_r, char * chmod_w, char * chmod_x)
{
	strcpy(chmod_r, "Toggle read; it is currently ");
	strcpy(chmod_w, "Toggle write; it is currently ");
	strcpy(chmod_x, "Toggle execute; it is currently ");
	//test for reading
	if (!access(fn, R_OK)) //see if usr can read
		strcat(chmod_r, "true"); //if can read, it is readable
	else	
		strcat(chmod_r, "false"); //if can read, it isn't readable
	//test for writing
	if (!access(fn, W_OK)) //see if usr can write
		strcat(chmod_w, "true"); //if can read, it is writable
	else
		strcat(chmod_w, "false"); //if can read, it isn't writable
	//test for execute
	if (!access(fn, X_OK)) //see if usr can exec
		strcat(chmod_x, "true"); //if can read, it is executable
	else
		strcat(chmod_x, "false"); //if can read, it isn't executable
}

ttk_menu_item * browseradmin_new_menu_item(const char * name, TWindow * (*func)(ttk_menu_item *), char * data)
{
	ttk_menu_item * mi = (ttk_menu_item *)calloc(1, sizeof(ttk_menu_item));
	if (mi) {
		mi->name = name;
		mi->makesub = func;
		mi->data = data;
	}
	return mi;
}

PzWindow * browseradmin_chmod_usr_r(ttk_menu_item * item)
{
	struct stat st;
	stat(item->data, &st);//get the file mode
	if (!access(item->data, R_OK)) //can usr read?
		//usr can read
		//chmod that masks read to zero
		chmod(item->data, st.st_mode & (~S_IRUSR));
	else
		//usr can't read
		//chmod that "OR"'s read to one
		chmod(item->data, st.st_mode | S_IRUSR);

	return TTK_MENU_UPONE; //go back to the pz window
}

PzWindow * browseradmin_chmod_usr_w(ttk_menu_item * item) //see above
{
	struct stat st;
	stat(item->data, &st);
	if (!access(item->data, W_OK))
		chmod(item->data, st.st_mode & (~S_IWUSR));
	else
		chmod(item->data, st.st_mode | S_IWUSR);

	return TTK_MENU_UPONE;
}

PzWindow * browseradmin_chmod_usr_x(ttk_menu_item * item) //see above
{
	struct stat st;
	stat(item->data, &st);
	if (!access(item->data, X_OK))
		chmod(item->data, st.st_mode & (~S_IXUSR));
	else
		chmod(item->data, st.st_mode | S_IXUSR);

	return TTK_MENU_UPONE;
}


PzWindow * browseradmin_chmod_window(char * fn)
{
	PzWindow * w;
	TWidget * menu;
	//make a window for the menuitems to go into
	menu = ttk_new_menu_widget(0, ttk_menufont, ttk_screen->w, ttk_screen->h-ttk_screen->wy);
	if (!menu) return 0; //menu wasn't created

	//create strings for menuitems
	char chmod_r[40], chmod_w[40], chmod_x[40];
	browseradmin_chmod_usr_labels(fn, chmod_r, chmod_w, chmod_x);
	
	ttk_menu_append(menu, browseradmin_new_menu_item(chmod_r, browseradmin_chmod_usr_r, fn));
	ttk_menu_append(menu, browseradmin_new_menu_item(chmod_w, browseradmin_chmod_usr_w, fn));
	ttk_menu_append(menu, browseradmin_new_menu_item(chmod_x, browseradmin_chmod_usr_x, fn));
	
	w = pz_new_menu_window(menu);
	if (w) w->title = "chmod";
	w->titlefree = 0;
	
	return w;
}

PzWindow * brosweradmin_chmod_func(ttk_menu_item * item)
{
	PzWindow * w;
	w = browseradmin_chmod_window((char*)item->data);
	w->data = 0x12345678; //mark the window as needing to be recreated next time
	return w;
}

void init_browseradmin()
{
	/* This registers the admin functions with the filebrowser */
	module = pz_register_module("browseradmin", cleanup_browseradmin);
	browseradmin_chmod_item.name = "chmod";
	browseradmin_chmod_item.makesub = brosweradmin_chmod_func;
	pz_browser_add_action (browseradmin_openable, &browseradmin_chmod_item);
}

PZ_MOD_INIT(init_browseradmin)
