/*
 * GTK front-end/blitter for DarcNES
 * (c) Michael K Vance, 1999
 *     mkv102@psu.edu
 */

/* $Id: ui_gtk.h,v 1.1 1999/02/15 03:39:41 nyef Exp $ */

#ifndef UI_GTK_H
#define UI_GTK_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "ui.h"
#include "system.h"
#include "video.h"
#include "tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILENAME_SIZE 256

void build_menus( GtkWidget* );
void show_open_dialog( GtkWidget*, gpointer );
void query_quit( GtkWidget*, gpointer );
void nothing( void );
void destroy( void );
void file_selected( GtkWidget*, GtkFileSelection* );
unsigned char* get_video_buffer( int );
void video_setsize( int, int );
void video_set_palette( int, int*, int*, int* );
gboolean emulate_timeslice( void );
void begin_emulation( void );
gint key_press( GtkWidget*, GdkEventKey*, gpointer );
gint key_release( GtkWidget*, GdkEventKey*, gpointer );

#ifdef __cplusplus
}
#endif

#endif

/*
 * $Log: ui_gtk.h,v $
 * Revision 1.1  1999/02/15 03:39:41  nyef
 * Initial revision
 *
 */
