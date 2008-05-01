/*
 * Last updated: May 1, 2008
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

#define PATH "/opt/Tools/CmdLine-Tools/Bin/"
#define JOHN "/opt/Tools/CmdLine-Tools/John"

static PzModule *module;
static ttk_menu_item
browser_extension_zip,
browser_extension_unzip, browser_extension_unrar,
browser_extension_tar_c, browser_extension_tar_d,
browser_extension_bzip2_c, browser_extension_bzip2_d,
browser_extension_gzip_c, browser_extension_gzip_d;

static int check_ext_zip(const char *file)
{
	return check_file_ext(file, ".zip");
}

static int check_ext_rar(const char *file)
{
	return check_file_ext(file, ".rar");
}

static int check_ext_tar(const char *file)
{
	return check_file_ext(file, ".tar");
}

static int check_ext_bz2(const char *file)
{
	return check_file_ext(file, ".bz2");
}

static int check_ext_gz(const char *file)
{
	return check_file_ext(file, ".gz");
}

static PzWindow *zip(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	char archive[strlen(f)+5];
	sprintf(archive, "%s.zip", f);
	const char *const cmd[] = {"zip", "-r", archive, f, NULL};
	pz_execv(
		PATH "zip",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_zip(ttk_menu_item *item)
{
	return zip(item->data);
}

static PzWindow *unzip(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"unzip", "-o", f, NULL};
	pz_execv(
		PATH "unzip",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_unzip(ttk_menu_item *item)
{
	return unzip(item->data);
}

static PzWindow *unrar(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"unrar", "x", "-o+", "-y", f, NULL};
	pz_execv(
		PATH "unrar",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_unrar(ttk_menu_item *item)
{
	return unrar(item->data);
}

static PzWindow *tar_c(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	char archive[strlen(f)+5];
	sprintf(archive,"%s.tar", f);
	const char *const cmd[] = {"tar", "-cf", archive, f, NULL};
	pz_execv(
		PATH "tar",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_tar_c(ttk_menu_item *item)
{
	return tar_c(item->data);
}

static PzWindow *tar_d(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"tar", "-xf", f, NULL};
	pz_execv(
		PATH "tar",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_tar_d(ttk_menu_item *item)
{
	return tar_d(item->data);
}

static PzWindow *bzip2_c(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"bzip2", "-vz9", f, NULL};
	pz_execv(
		PATH "bzip2",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_bzip2_c(ttk_menu_item *item)
{
	return bzip2_c(item->data);
}

static PzWindow *bzip2_d(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"bzip2", "-vd", f, NULL};
	pz_execv(
		PATH "bzip2",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_bzip2_d(ttk_menu_item *item)
{
	return bzip2_d(item->data);
}

static PzWindow *gzip_c(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"gzip", "-v9", f, NULL};
	pz_execv(
		PATH "gzip",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_gzip_c(ttk_menu_item *item)
{
	return gzip_c(item->data);
}

static PzWindow *gzip_d(const char *file)
{
	const char *f = get_filename(file);
	const char *d = get_dirname(file);
	chdir(d);
	const char *const cmd[] = {"gzip", "-vd", f, NULL};
	pz_execv(
		PATH "gzip",
		(char *const *)cmd
		);
	return TTK_MENU_UPONE;
}
static PzWindow *load_file_handler_gzip_d(ttk_menu_item *item)
{
	return gzip_d(item->data);
}

static void cleanup()
{
	pz_browser_remove_handler(check_is_file);
	pz_browser_remove_handler(check_nothing);
	pz_browser_remove_handler(check_ext_zip);
	pz_browser_remove_handler(check_ext_rar);
	pz_browser_remove_handler(check_ext_tar);
	pz_browser_remove_handler(check_ext_bz2);
	pz_browser_remove_handler(check_ext_gz);
}

static void init_launch()
{
	module = pz_register_module("CmdLine-Tools", cleanup);
	
	char path[256];
	sprintf(
		path, "%s:%s",
		getenv("PATH"),
		PATH
		);	
	setenv("PATH", path, 1);
	setenv("JOHN", JOHN, 1);
	
	browser_extension_zip.name = N_("Compress (zip)");
	browser_extension_zip.makesub = load_file_handler_zip;
	pz_browser_add_action(check_nothing, &browser_extension_zip);
	
	browser_extension_unzip.name = N_("Decompress (unzip)");
	browser_extension_unzip.makesub = load_file_handler_unzip;
	pz_browser_add_action(check_ext_zip, &browser_extension_unzip);
	
	browser_extension_unrar.name = N_("Decompress (unrar)");
	browser_extension_unrar.makesub = load_file_handler_unrar;
	pz_browser_add_action(check_ext_rar, &browser_extension_unrar);
	
	browser_extension_tar_c.name = N_("Compress (tar)");
	browser_extension_tar_c.makesub = load_file_handler_tar_c;
	pz_browser_add_action(check_nothing, &browser_extension_tar_c);
	
	browser_extension_tar_d.name = N_("Decompress (tar)");
	browser_extension_tar_d.makesub = load_file_handler_tar_d;
	pz_browser_add_action(check_ext_tar, &browser_extension_tar_d);
	
	browser_extension_bzip2_c.name = N_("Compress (bzip2)");
	browser_extension_bzip2_c.makesub = load_file_handler_bzip2_c;
	pz_browser_add_action(check_is_file, &browser_extension_bzip2_c);
	
	browser_extension_bzip2_d.name = N_("Decompress (bzip2)");
	browser_extension_bzip2_d.makesub = load_file_handler_bzip2_d;
	pz_browser_add_action(check_ext_bz2, &browser_extension_bzip2_d);
	
	browser_extension_gzip_c.name = N_("Compress (gzip)");
	browser_extension_gzip_c.makesub = load_file_handler_gzip_c;
	pz_browser_add_action(check_is_file, &browser_extension_gzip_c);
	
	browser_extension_gzip_d.name = N_("Decompress (gzip)");
	browser_extension_gzip_d.makesub = load_file_handler_gzip_d;
	pz_browser_add_action(check_ext_gz, &browser_extension_gzip_d);
}

PZ_MOD_INIT(init_launch)
