/*
 * ui_allegro.c
 *
 * user interface for DJGPP/Allegro
 */

/* $Id: ui_allegro.c,v 1.16 2000/08/22 02:06:44 nyef Exp $ */

#include <stdio.h>
#include <stdarg.h>
#include <allegro.h>
#include "ui.h"
#include "system.h"
#include "video.h"
#include "tool.h"
#include "menu.h"

/* the joypad (only one, I could care less about more) */
struct joypad *ui_joypad;

void run_system(void);
shutdown_t dn_shutdown;

int main(int argc, char *argv[])
{
    extern int nes_psg_quality; /* FIXME: cheap hack, copied from nes_psg.h */
    rom_file romfile;
    int system_type;
    
    if (argc != 2) {
	printf("usage: %s <romfile>\n", argv[0]);
	exit(0x42);
    }
    
    video_init();

    nes_psg_quality = 2;

    romfile = read_romimage(argv[1]);

    if (romfile) {
	system_type = guess_system(romfile);
	activate_system(system_type, romfile);
    } else {
	printf("error reading romfile.\n");
	return 0x41;
    }
	 
    run_system();

    return 0;
}


/* debug console handling */

void deb_printf(const char *fmt, ...)
{
#if 0
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}


/* keyboard emulation */

void kb_init(void)
{
    /* FIXME: dummy function. keyboards don't work */
}


/* keypad emulation */

int keypad_register(struct keypad *pad)
{
    /* FIXME: dummy function. keypads don't work */
    return 0;
}


/* joypad emulation */

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

void video_events(void)
{
#if 0 /* this locks up the system. Oops. */
    if (key[KEY_F1]) {
	system_flags |= F_BREAK;
    } else {
	system_flags &= ~F_BREAK;
    }
#endif
    
    if (key[KEY_ESC]) {
	system_flags |= F_QUIT;
    } else {
	system_flags &= ~F_QUIT;
    }

    if (ui_joypad) {
	switch (ui_joypad->button_template->num_buttons) {
	case 8:
	    if (key[KEY_CLOSEBRACE]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[7];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[7];
	    }
	case 7:
	    if (key[KEY_OPENBRACE]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[6];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[6];
	    }
	case 6:
	    if (key[KEY_A]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[5];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[5];
	    }
	case 5:
	    if (key[KEY_S]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[4];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[4];
	    }
	case 4:
	    if (key[KEY_RIGHT]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[3];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[3];
	    }
	case 3:
	    if (key[KEY_LEFT]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[2];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[2];
	    }
	case 2:
	    if (key[KEY_DOWN]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[1];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[1];
	    }
	case 1:
	    if (key[KEY_UP]) {
		ui_joypad->data |= ui_joypad->button_template->buttons[0];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[0];
	    }
	default:
	    break;
	}
    }
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


/* emulation timeslicing */

void (*timeslice)(void *) = NULL;
void *timeslice_data;

void set_timeslice(void (*proc)(void *), void *data)
{
    timeslice = proc;
    timeslice_data = data;
}

void unset_timeslice(void)
{
    timeslice = NULL;
}

void run_system(void)
{
    while ((!(system_flags & F_QUIT)) && timeslice) {
	video_events();
	timeslice(timeslice_data);
    }
    if (dn_shutdown) {
	dn_shutdown();
    }
}

/*
 * $Log: ui_allegro.c,v $
 * Revision 1.16  2000/08/22 02:06:44  nyef
 * added dummy implementation of menu_file_open_box()
 *
 * Revision 1.15  2000/07/04 23:19:44  nyef
 * fixed compile problems with the dummy menu implementation
 *
 * Revision 1.14  2000/07/02 02:43:23  nyef
 * added dummy implementation of the per-driver menu interface
 *
 * Revision 1.13  2000/06/03 17:24:18  nyef
 * added dummy keypad and keyboard emulation functions
 *
 * Revision 1.12  1999/11/26 20:08:18  nyef
 * moved sound quality control in from the game system layer
 *
 * Revision 1.11  1999/11/25 01:49:58  nyef
 * fixed to not crash when the rom file cannot be loaded
 *
 * Revision 1.10  1999/11/21 03:10:49  nyef
 * converted to new rom file interface (not tested)
 *
 * Revision 1.9  1999/06/05 03:24:15  nyef
 * converted to use new joypad interface
 *
 * Revision 1.8  1999/04/17 20:12:26  nyef
 * changed shutdown() to dn_shutdown().
 *
 * Revision 1.7  1999/03/06 21:20:45  nyef
 * added inclusion of allegro.h (oops)
 *
 * Revision 1.6  1999/02/09 02:11:13  nyef
 * moved video_events() in from video_allegro.c
 * added call to video_events in main timeslice loop
 *
 * Revision 1.5  1999/01/01 20:50:51  nyef
 * fixed shutdown callback to only be called if it gets set up.
 *
 * Revision 1.4  1998/12/21 02:58:08  nyef
 * added a shutdown callback.
 *
 * Revision 1.3  1998/12/18 04:17:14  nyef
 * changed usage message to use argv[0] for name of program.
 *
 * Revision 1.2  1998/08/04 01:57:59  nyef
 * fixed stipid mistake in main(). (forgot a 'return 0')
 *
 * Revision 1.1  1998/08/02 19:35:45  nyef
 * Initial revision
 *
 */
