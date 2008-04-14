/*
 * ui_x.c
 *
 * user interface for the X Window System.
 */

/* $Id: ui_x.c,v 1.31 2000/08/22 01:28:59 nyef Exp $ */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>

#include <stdio.h>
#include <stdarg.h>
#include "ui.h"
#include "system.h"
#include "video.h"
#include "tool.h"
#include "menu.h"

#define APPLICATION_NAME "darcnes"
#define APPLICATION_CLASS "DarcNES"

/* Overhead for Xt and main window */
XtAppContext context;
Widget toplevel;
Widget manager;

/* Menus */
Widget menubar;
Widget menubutton_file;
Widget menu_file;
Widget menu_file_exit;

/* the drawing area */
Widget drawbox;

/* the shutdown callback */
shutdown_t dn_shutdown;

/* miscellany */
void do_file_exit(Widget, XtPointer, XtPointer);
Boolean emulation_timeslice(XtPointer);

void menu_callback(Widget widget, XtPointer menu, XtPointer call_data)
{
    struct ui_menu *item;

    item = (struct ui_menu *) menu;
    item->callback(item->callback_data);
}

void menu_init(struct ui_menu *menu)
{
    Widget menubutton;
    Widget themenu;
    Widget menuitem;
    int i;

    menubutton = XtCreateManagedWidget(menu[0].name, menuButtonWidgetClass, menubar, NULL, 0);
    themenu = XtCreatePopupShell("drivermenu", simpleMenuWidgetClass, menubutton, NULL, 0);
    XtVaSetValues(menubutton, XtNmenuName, "drivermenu", NULL);

    for (i = 1; menu[i].name; i++) {
	if (menu[i].flags & MF_CHILD) {
	    /* don't create child menus */
	    continue;
	}
	menuitem = XtCreateManagedWidget(menu[i].name, smeBSBObjectClass, themenu, NULL, 0);
	XtAddCallback(menuitem, XtNcallback, menu_callback, &menu[i]);
	menu[i].ui_data = menuitem;
    }
}

void menu_rename_item(struct ui_menu *item)
{
    Widget item_widget;

    item_widget = item->ui_data;
    XtVaSetValues(item_widget, XtNlabel, item->name, NULL);
}

void menu_disable_item(struct ui_menu *item)
{
    /* dummy implementation */
}

void menu_enable_item(struct ui_menu *item)
{
    /* dummy implementation */
}

void init_menus(void)
{
    /* the menubar */
    menubar = XtVaCreateManagedWidget("menubar", boxWidgetClass, manager, XtNorientation, XtorientHorizontal, XtNborderWidth, 0, NULL);

    /* the file menu */
    menubutton_file = XtCreateManagedWidget("File", menuButtonWidgetClass, menubar, NULL, 0);
    menu_file = XtCreatePopupShell("fileMenu", simpleMenuWidgetClass, menubutton_file, NULL, 0);
    XtVaSetValues(menubutton_file, XtNmenuName, "fileMenu", NULL);
    menu_file_exit = XtCreateManagedWidget("Exit", smeBSBObjectClass, menu_file, NULL, 0);
    XtAddCallback(menu_file_exit, XtNcallback, do_file_exit, NULL);
}

void menu_file_open_box(ui_open_callback callback, void *data, char *filter)
{
    /* dummy implementation */
}

void initialize_window(Display *display)
{
    toplevel = XtVaAppCreateShell(APPLICATION_NAME, APPLICATION_CLASS, applicationShellWidgetClass, display, XtNallowShellResize, TRUE, NULL);

    manager = XtVaCreateManagedWidget("manager", boxWidgetClass, toplevel, XtNhSpace, 0, XtNvSpace, 0, NULL);

    init_menus();
    
    drawbox = XtVaCreateManagedWidget("drawbox", boxWidgetClass, manager, XtNwidth, 512, XtNheight, 256, NULL);

    XtRealizeWidget(toplevel);
    
    video_init();
}

void usage(char *progname)
{
    printf("usage: %s [--system=<system>] [--nosound] [filename]\n", progname);
}

int main(int argc, char *argv[])
{
    extern int nes_psg_quality; /* FIXME: cheap hack, copied from nes_psg.h */
    rom_file romfile;
    int system_type;
    int i;
    Display *display; /* so as not to conflict with the one in video_x.c */

    XtToolkitInitialize();

    context = XtCreateApplicationContext();

    display = XtOpenDisplay(context, NULL, APPLICATION_NAME, APPLICATION_CLASS, NULL, 0, &argc, argv);

    if (argc == 1) {
	usage(argv[0]);
	return 1;
    }

    system_type = ST_NONE;
    romfile = NULL;
    
    nes_psg_quality = 2;

    for (i = 1; i < argc; i++) {
	if (!strncasecmp(argv[i], "--system=", 9)) {
	    system_type = parse_system_name(argv[i] + 9);
	} else if (!strcasecmp(argv[i], "--nosound")) {
	    nes_psg_quality = 0;
	} else if (romfile) {
	    printf("rom file \"%s\" already loaded, ignoring \"%s\"\n", romfile->filename, argv[i]);
	} else if ((romfile = read_romimage(argv[i]))) {
	    if (system_type == ST_NONE) {
		system_type = guess_system(romfile);
	    }
	} else {
	    printf("error loading rom file \"%s\"\n", argv[i]);
	}
    }

    if (system_type == ST_NONE) {
	printf("no system specified or unable to guess system.\n");
	return 1;
    }

    initialize_window(display);

    activate_system(system_type, romfile);

    XtAppMainLoop(context);
    
    return 0;
}


/* debug console handling */

void deb_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


/* emulation timeslicing */

void (*timeslice)(void *) = NULL;
void *timeslice_data;

void set_timeslice(void (*proc)(void *), void *data)
{
    if (!timeslice) {
	XtAppAddWorkProc(context, emulation_timeslice, NULL);
    }
    
    timeslice = proc;
    timeslice_data = data;
}

void unset_timeslice(void)
{
    timeslice = NULL;
    timeslice_data = NULL;
}

Boolean emulation_timeslice(XtPointer client_data)
{
    if (timeslice) {
	timeslice(timeslice_data);
    }
    
    if ((system_flags == F_NONE) && timeslice) {
	return False;
    } else {
	return True;
    }
}


/* menu callbacks */

void do_file_exit(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (dn_shutdown) {
	dn_shutdown();
    }
    exit(0);
}


/* video interface */

void ui_set_drawbox_size(int width, int height)
{
    XtVaSetValues(drawbox, XtNwidth, width, XtNheight, height, NULL);
}

/*
 * $Log: ui_x.c,v $
 * Revision 1.31  2000/08/22 01:28:59  nyef
 * added dummy implementation of menu_file_open_box()
 *
 * Revision 1.30  2000/07/01 15:43:14  nyef
 * fixed to not dump core when encountering a child menu
 *
 * Revision 1.29  2000/06/29 01:24:56  nyef
 * renamed the menu functions more appropriately
 *
 * Revision 1.28  2000/06/29 01:05:39  nyef
 * moved menu interface out from ui.h to menu.h
 *
 * Revision 1.27  2000/06/25 19:22:11  nyef
 * added dummy implementations of ui_{en,dis}able_item()
 *
 * Revision 1.26  2000/06/25 18:57:09  nyef
 * added support for parameters with menu callbacks
 * added support for changing the label on a menu
 *
 * Revision 1.25  2000/06/25 17:20:09  nyef
 * added per-driver menu interface
 *
 * Revision 1.24  2000/06/10 02:20:49  nyef
 * fixed --nosound option (silly bug in args parser)
 *
 * Revision 1.23  2000/06/09 00:11:31  nyef
 * added --nosound option
 *
 * Revision 1.22  2000/03/25 18:23:02  nyef
 * broke up toolkit initialization
 * moved toplevel window creation to initialize_window()
 *
 * Revision 1.21  2000/03/06 01:29:59  nyef
 * extracted most of the X initialization code from main()
 * rebuilt the command argument parser (added "--system=" option)
 *
 * Revision 1.20  2000/02/21 23:16:09  nyef
 * fixed toplevel window to resize when the drawbox is resized
 *
 * Revision 1.19  2000/01/01 04:15:14  nyef
 * added disabled code to activate apple2 emulation
 *
 * Revision 1.18  1999/12/04 06:43:22  nyef
 * stripped out a lot of dead code
 *
 * Revision 1.17  1999/11/26 20:08:24  nyef
 * moved sound quality control in from the game system layer
 *
 * Revision 1.16  1999/11/20 05:27:19  nyef
 * fixed to work with new rom loading interface
 *
 * Revision 1.15  1999/04/17 20:12:46  nyef
 * changed shutdown() to dn_shutdown().
 *
 * Revision 1.14  1999/03/03 02:53:05  nyef
 * changed the initial size of the drawbox to "really big"
 * as a workaround for the resize problem
 *
 * Revision 1.13  1999/02/13 17:25:34  nyef
 * disabled file->nes.
 * added code to accept a filename on the command line.
 * changed to start running game immediately after starting.
 *
 * Revision 1.12  1999/01/08 02:45:25  nyef
 * disabled file->zarzon.
 *
 * Revision 1.11  1998/12/24 04:56:51  nyef
 * moved for loop from emulation_timeslice() to nes.c.
 *
 * Revision 1.10  1998/12/24 01:23:18  nyef
 * removed debug menu (it was at best useless, and at worst dangerous).
 * added two file menu items ("Nes", and "Zarzon") to start the system.
 * fixed file->quit to call exit(3).
 * fixed background task to only run when we need an emulation timeslice.
 *
 * Revision 1.9  1998/12/21 02:58:03  nyef
 * added a shutdown callback.
 *
 * Revision 1.8  1998/12/18 04:22:25  nyef
 * changed application class to "DarcNES".
 *
 * Revision 1.7  1998/10/28 01:50:00  nyef
 * changed emulation_timeslice() to return aproximately once per screen
 * refresh. this sped up the emulation appreciably but does not appear
 * to have degraded the key response.
 *
 * Revision 1.6  1998/10/02 01:22:07  nyef
 * changed timeslice function to remove itself when emulation halts.
 *
 * Revision 1.5  1998/08/24 01:25:41  nyef
 * started working on new interface.
 *
 * Revision 1.4  1998/08/02 04:01:56  nyef
 * added function deb_printf() for debug console support.
 * added ui.h to the list of includes.
 *
 * Revision 1.3  1998/08/02 03:56:28  nyef
 * added comments grouping functions into related sections.
 *
 * Revision 1.2  1998/08/02 02:31:47  nyef
 * added a separate menubar widget.
 * added a "Debug" menu with one item. ("Show console", which calls a stub.)
 *
 * Revision 1.1  1998/08/02 02:04:26  nyef
 * Initial revision
 *
 */
