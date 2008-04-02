/*
 * GTK front-end/blitter for DarcNES
 * (c) Michael K Vance, 1999
 *     mkv102@psu.edu
 */

/* $Id: ui_gtk.c,v 1.10 2000/08/22 02:06:48 nyef Exp $ */

#include "ui_gtk.h"
#include "menu.h"

/* Callbacks galore. */
unsigned char* vid_pre_xlat;
shutdown_t dn_shutdown;
void (* timeslice )( void* );
void* timeslice_data;

/* Needed for our Gdk blit later on. */
GtkWidget* nes_gtk_window;
GdkGC* gc;
/* Useful for all manners of depth checking, etc. */
GdkVisual* visual;
GdkColormap* color_map;

/* These can have any bit depth. Use image->bpp to get bit depth. */
GdkImage* image_1;
GdkImage* image_2;
GdkImage* cur_image;

/* These always carry 8-bit index values. */
unsigned char* video_buffer_1;
unsigned char* video_buffer_2;
unsigned char* cur_video_buffer;

/* Our palette for any 16-bit drawing. */
unsigned short* translation_buffer_16;

/* Video buffer height, width. */
int video_buffer_height;
int video_buffer_width;

/* Our ROM name. */
char rom_name[128];

/* Joystick data. */
struct joypad *ui_joypad;

/*
 * Bootstrap our UI, get our buffers, etc.
 */
int main( int argc, char* argv[] )
{
    extern int nes_psg_quality; /* FIXME: cheap hack, copied from nes_psg.h */
    
  translation_buffer_16 = NULL;
  vid_pre_xlat = NULL;

  timeslice = NULL;
  timeslice_data = NULL;
  nes_psg_quality = 2;

  image_1 = image_2 = NULL;
  video_buffer_1 = video_buffer_2 = NULL;

  /* Get Gtk up and running. */
  gtk_init( &argc, &argv );

  nes_gtk_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_window_set_title( GTK_WINDOW( nes_gtk_window ), "DarcNES" );
  gtk_widget_set_usize( nes_gtk_window, 256, 240 );
  gtk_signal_connect( GTK_OBJECT( nes_gtk_window ), "destroy",
		      GTK_SIGNAL_FUNC( destroy ), NULL );
  gtk_signal_connect( GTK_OBJECT( nes_gtk_window ), "key_press_event",
		      GTK_SIGNAL_FUNC( key_press ), NULL );
  gtk_signal_connect( GTK_OBJECT( nes_gtk_window ), "key_release_event",
		      GTK_SIGNAL_FUNC( key_release ), NULL );
  gtk_widget_set_events( nes_gtk_window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK );
  gtk_widget_show( nes_gtk_window );

  visual = gdk_window_get_visual( nes_gtk_window->window );
  gc = gdk_gc_new( nes_gtk_window->window );
  color_map = gdk_window_get_colormap( nes_gtk_window->window );

  if( argc > 1 )
  {
      int system_type;
      rom_file romfile;
      
      strncpy( rom_name, argv[1], 127 );
      romfile = read_romimage(rom_name);
      system_type = guess_system(romfile);
      activate_system(system_type, romfile);
  }
  else
  {
    show_open_dialog( NULL, NULL );
  }

  /* Using GTK_PRIORITY_HIGH causes Gtk events to not occur. */
  gtk_idle_add( (GtkFunction) emulate_timeslice, NULL );

  gtk_main( );

  return 0;
}

void build_menus( GtkWidget* window )
{
  GtkWidget* menu_bar;
  GtkWidget* file_menu;
  GtkWidget* file_item;
  GtkWidget* file_open;
  GtkWidget* file_quit;

  menu_bar = gtk_menu_bar_new( );

  file_menu = gtk_menu_new( );

  file_open = gtk_menu_item_new_with_label( "Open" );
  gtk_signal_connect_object( GTK_OBJECT( file_open ), "activate",
			     GTK_SIGNAL_FUNC( show_open_dialog ), NULL );
  gtk_menu_append( GTK_MENU( file_menu ), file_open );
  gtk_widget_show( file_open );

  file_quit = gtk_menu_item_new_with_label( "Quit" );
  gtk_signal_connect_object( GTK_OBJECT( file_quit ), "activate",
			     GTK_SIGNAL_FUNC( query_quit ), NULL );
  gtk_menu_append( GTK_MENU( file_menu ), file_quit );
  gtk_widget_show( file_quit );

  file_item = gtk_menu_item_new_with_label( "File" );
  gtk_menu_item_set_submenu( GTK_MENU_ITEM( file_item ), file_menu );
  gtk_widget_show( file_item );

  gtk_menu_bar_append( GTK_MENU_BAR( menu_bar ), file_item );
  gtk_container_add( GTK_CONTAINER( window ), menu_bar );
  gtk_widget_show( menu_bar );
}

void show_open_dialog( GtkWidget* w, gpointer data )
{
  GtkWidget* open_dialog;

  open_dialog = gtk_file_selection_new( "Select ROM" );
  gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( open_dialog )->ok_button ), "clicked",
		      (GtkSignalFunc) file_selected, open_dialog );
  gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( open_dialog )->cancel_button ),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT( open_dialog ) );
  gtk_widget_show( open_dialog );
}

void query_quit( GtkWidget* w, gpointer data )
{
  /* PENDING( are you sure, etc ) */
  gtk_exit( 0 );
}

void file_selected( GtkWidget* w, GtkFileSelection* fs )
{
    rom_file romfile;
    int system_type;
    
    strncpy( rom_name, gtk_file_selection_get_filename( GTK_FILE_SELECTION( fs ) ), 127 );
    gtk_widget_destroy( GTK_WIDGET( fs ) );
    romfile = read_romimage(rom_name);
    system_type = guess_system(romfile);
    activate_system( system_type, romfile);
}

void destroy( void )
{

  if( dn_shutdown )
  {
    dn_shutdown( );
  }

  gtk_exit( 0 );
}

/* per-driver menus */

void menu_init(struct ui_menu *menu)
{
    /* dummy implementation */
}

void menu_rename_item(struct ui_menu *item)
{
    /* dummy implementation */
}

void menu_disable_item(struct ui_menu *item)
{
    /* dummy implementation */
}

void menu_enable_item(struct ui_menu *item)
{
    /* dummy implementation */
}

void menu_file_open_box(ui_open_callback callback, void *data, char *filter)
{
    /* dummy implementation */
}


void deb_printf( const char* format, ... )
{
  va_list args;

  va_start( args, format );
  vprintf( format, args );
  va_end( args );
}

void set_timeslice( void (* proc)( void* ), void* data )
{
  timeslice = proc;
  timeslice_data = data;
}

void unset_timeslice( void )
{
  timeslice = NULL;
  timeslice_data = NULL;
}

gboolean emulate_timeslice( void )
{

  if( timeslice )
  {
    timeslice( timeslice_data );
  }

  if( system_flags & F_QUIT )
  {
    destroy( );
  }

  return TRUE;
}

void video_setsize( int width, int height )
{
  video_buffer_width = width;
  video_buffer_height = height;

  if( video_buffer_1 )
  {
    free( video_buffer_1 );
  }

  if( image_1 )
  {
    gdk_image_destroy( image_1 );
  }

  if( video_buffer_2 )
  {
    free( video_buffer_2 );
  }

  if( image_2 )
  {
    gdk_image_destroy( image_2 );
  }

  gtk_widget_set_usize( GTK_WIDGET( nes_gtk_window ), width, height );

  /* Buffer allocation. */
  video_buffer_1 = (unsigned char*) malloc( width * height * 8 );
  image_1 = gdk_image_new( GDK_IMAGE_FASTEST, visual, width, height );
  video_buffer_2 = (unsigned char*) malloc( width * height * 8 );
  image_2 = gdk_image_new( GDK_IMAGE_FASTEST, visual, width, height );

  cur_image = image_1;
  cur_video_buffer = video_buffer_1;
}

void video_setpal( int colors, int* red, int* green, int* blue )
{
  int i;
  GdkColor color;

  vid_pre_xlat = (unsigned char*) malloc( colors );

  switch( visual->depth )
  {
  case 8:

    for( i = 0; i < colors; ++i )
    {
      color.red = red[i] + ( red[i] << 8 );
      color.green = green[i] + ( green[i] << 8 );
      color.blue = blue[i] + ( blue[i] << 8 );

      if( gdk_colormap_alloc_color( color_map, &color, TRUE, TRUE ) )
      {
	vid_pre_xlat[i] = color.pixel;
      }

    }
    
    break;
  case 16:
    translation_buffer_16 = (unsigned short*) malloc( colors * 2 );

    for( i = 0; i < colors; ++i )
    {
      vid_pre_xlat[i] = i;
      color.red = red[i] + ( red[i] << 8 );
      color.green = green[i] + ( green[i] << 8 );
      color.blue = blue[i] + ( blue[i] << 8 );

      if( gdk_colormap_alloc_color( color_map, &color, TRUE, TRUE ) )
      {
	translation_buffer_16[i] = color.pixel;
      }

    }

    break;
  default:
    fprintf( stderr, "darcnes: unsupported display depth %d in video_set_palette()\n",
	     visual->depth );
    break;
  }

}

/*
 * The video buffer is always indexed--the GdkImage however
 * can have a different depth.
 */
unsigned char* video_get_vbp( int line )
{
  return cur_video_buffer + ( video_buffer_width * line );
}

/*
 * Since the NES does everything in eight bit index mode,
 * we need to translate that indexed framebuffer into our
 * native format for display.
 */
void video_display_buffer( void )
{
  int i;
  unsigned short* buffer_16;
  int num_pixels;

  switch( cur_image->bpp )
  {
  case 1:
    /* Empty. */
    break;
  case 2:
    buffer_16 = (unsigned short*) cur_image->mem;
    num_pixels = video_buffer_width * video_buffer_height;

    for( i = 0; i < num_pixels; ++i )
    {
      buffer_16[i] = translation_buffer_16[ (int) cur_video_buffer[i] ];
    }
    
    break;
  default:
    fprintf( stderr, "darcnes: unknown display depth %d in video_display_buffer()\n",
	     cur_image->bpp );
    break;
  }

  gdk_draw_image( nes_gtk_window->window, gc, cur_image, 0, 0, 0, 0, 
		  video_buffer_width, video_buffer_height );
  gdk_flush( );

  cur_image = ( cur_image == image_1 ) ? image_2 : image_1;
  cur_video_buffer = ( cur_video_buffer == video_buffer_1 ) ? video_buffer_2 : video_buffer_1;
}

void video_enter_deb( void ) { }

void video_leave_deb( void ) { }

void kb_init(void)
{
    /* FIXME: dummy function. keyboards don't work */
}

int keypad_register(struct keypad *pad)
{
    /* FIXME: dummy function. keypads don't work */
    return 0;
}

int ui_register_joypad(struct joypad *pad)
{
    if (!ui_joypad) {
	ui_joypad = pad;
	return 1;
    } else {
	return 0;
    }
}

void ui_update_joypad(struct joypad *pad)
{
    /* NOTE: does nothing. may want to do stuff when using real gamepads */
}

gint key_press (GtkWidget* w, GdkEventKey* e, gpointer unused)
{
    switch (e->keyval) {
    case GDK_Escape:
	system_flags |= F_QUIT;
	break;
    case GDK_F1:
	system_flags |= F_BREAK;
	break;
    case GDK_S:
    case GDK_s:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 4)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[4];
	}
	break;
    case GDK_A:
    case GDK_a:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 5)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[5];
	}
	break;
    case GDK_bracketleft:
    case GDK_braceleft:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 6)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[6];
	}
	break;
    case GDK_bracketright:
    case GDK_braceright:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 7)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[7];
	}
	break;
    case GDK_Up:
    case GDK_KP_Up:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 0)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[0];
	}
	break;
    case GDK_Down:
    case GDK_KP_Down:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 1)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[1];
	}
	break;
    case GDK_Left:
    case GDK_KP_Left:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 2)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[2];
	}
	break;
    case GDK_Right:
    case GDK_KP_Right:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 3)) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[3];
	}
	break;
    }
    
    return 0;
}

gint key_release (GtkWidget* w, GdkEventKey* e, gpointer unused)
{
    switch (e->keyval) {
    case GDK_S:
    case GDK_s:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 4)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[4];
	}
	break;
    case GDK_A:
    case GDK_a:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 5)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[5];
	}
	break;
    case GDK_bracketleft:
    case GDK_braceleft:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 6)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[6];
	}
	break;
    case GDK_bracketright:
    case GDK_braceright:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 7)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[7];
	}
	break;
    case GDK_Up:
    case GDK_KP_Up:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 0)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[0];
	}
	break;
    case GDK_Down:
    case GDK_KP_Down:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 1)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[1];
	}
	break;
    case GDK_Left:
    case GDK_KP_Left:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 2)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[2];
	}
	break;
    case GDK_Right:
    case GDK_KP_Right:
	if (ui_joypad && (ui_joypad->button_template->num_buttons > 3)) {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[3];
	}
	break;
    }
    
    return 0;
}

/*
 * $Log: ui_gtk.c,v $
 * Revision 1.10  2000/08/22 02:06:48  nyef
 * added dummy implementation of menu_file_open_box()
 *
 * Revision 1.9  2000/07/04 23:19:41  nyef
 * fixed compile problems with the dummy menu implementation
 *
 * Revision 1.8  2000/07/02 02:43:18  nyef
 * added dummy implementation of the per-driver menu interface
 *
 * Revision 1.7  2000/06/03 17:25:14  nyef
 * fixed (hopefully) to work with the new joypad interface
 * added dummy functions for keypad and keyboard emulation
 *
 * Revision 1.6  2000/05/31 01:22:39  nyef
 * fixed (hopefully) to work with the new video interface
 *
 * Revision 1.5  1999/11/26 20:08:21  nyef
 * moved sound quality control in from the game system layer
 *
 * Revision 1.4  1999/11/21 03:09:49  nyef
 * converted to use new rom file interface (not tested)
 *
 * Revision 1.3  1999/06/14 15:33:04  nyef
 * converted to use new joypad interface
 *
 * Revision 1.2  1999/04/17 20:11:49  nyef
 * changed shutdown() to dn_shutdown().
 *
 * Revision 1.1  1999/02/15 03:39:46  nyef
 * Initial revision
 *
 */
