/*
 * video_svga.c
 *
 * display screen management
 */

/* $Id: video_svga.c,v 1.12 2000/09/16 23:45:10 nyef Exp $ */

#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
/* #include <vgajoystick.h> */

#include <stdio.h>
#include <stdlib.h>

#include "video.h"
#include "tool.h"
#include "ui.h"

#define BUFFER_X 256
#define BUFFER_Y 240
#define BUFFER_SIZE (BUFFER_X * BUFFER_Y)


/* the joypad (only one, I could care less about more) */
struct joypad *ui_joypad;

int video_active = 0;
int deblevel = 0;

unsigned char *vid_pre_xlat;

unsigned char *nes_image;

int middle_x=0,
    middle_y=0;
int buffer_x;
int buffer_y;
int image_x;
int image_y;
int offset_x;
int offset_y;

int svga_mode = G640x480x256;
int use_joystick = 1;
int joy_up, joy_down, joy_left, joy_right,
    joy_a, joy_b, joy_start, joy_select;

void init_xlate8(int colors, int *red, int *green, int *blue);
unsigned char *vbpfunc8(int line);

void video_events(void);

__inline__
void input_update()
{
    keyboard_update();
    
    if (keyboard_keypressed(SCANCODE_ESCAPE)) {
	system_flags |= F_QUIT;
    } else {
	system_flags &= ~F_QUIT;
    }
    
    joy_a = (keyboard_keypressed(SCANCODE_S) || keyboard_keypressed(SCANCODE_LEFTALT));
    joy_b = (keyboard_keypressed(SCANCODE_A) || keyboard_keypressed(SCANCODE_LEFTCONTROL));
    joy_select = (keyboard_keypressed(SCANCODE_BRACKET_LEFT) || keyboard_keypressed(SCANCODE_TAB));
    joy_start  = (keyboard_keypressed(SCANCODE_BRACKET_RIGHT) || keyboard_keypressed(SCANCODE_ENTER));
    joy_up     = keyboard_keypressed(SCANCODE_CURSORUP);
    joy_down   = keyboard_keypressed(SCANCODE_CURSORDOWN);
    joy_left   = keyboard_keypressed(SCANCODE_CURSORLEFT);
    joy_right  = keyboard_keypressed(SCANCODE_CURSORRIGHT);
    
#if 0
    if (use_joystick) {
	joystick_update();
	
	joy_up    = (joystick_x(0) > 0) ? 1 : joy_up;
	joy_down  = (joystick_x(0) < 0) ? 1 : joy_down;
	joy_left  = (joystick_y(0) < 0) ? 1 : joy_left;
	joy_right = (joystick_y(0) > 0) ? 1 : joy_right;
    }
#endif
}

void video_setsize(int x, int y)
{
    middle_x = (vga_getxdim() - x) / 2;
    middle_y = (vga_getydim() - y) / 2;

    if (middle_x < 0) {
	offset_x = -middle_x;
	middle_x = 0;
    }
    
    if (middle_y < 0) {
	offset_y = -middle_y;
	middle_y = 0;
    }
    
    buffer_x = x;
    buffer_y = y;

    if (buffer_x > vga_getxdim()) {
	image_x = vga_getxdim();
    } else {
	image_x = buffer_x;
    }
    
    if (buffer_y > vga_getydim()) {
	image_y = vga_getydim();
    } else {
	image_y = buffer_y;
    }
    
    if (nes_image) {
	free(nes_image);
    }

    nes_image = malloc(x * y);

    if (!nes_image) {
	printf("unable to allocate image buffer.\n");
	exit(-1);
    }
}

void video_enter_deb(void)
{
    if (video_active && (!deblevel)) {
	vga_flip();
    }
    deblevel++;
}

void video_leave_deb(void)
{
    deblevel--;
    if (video_active && (!deblevel)) {
	vga_flip();
    }
}

__inline__
void video_display_buffer()
{
    if ((buffer_x == image_x) && (buffer_y == image_y)) {
	gl_putbox(middle_x, middle_y, buffer_x, buffer_y, nes_image);
    } else {
	gl_putboxpart(middle_x, middle_y, image_x, image_y, buffer_x, buffer_y, nes_image, offset_x, offset_y);
    }
    video_events();
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
    input_update();

    if (ui_joypad) {
	switch (ui_joypad->button_template->num_buttons) {
	case 8:
	    if (joy_start) {
		ui_joypad->data |= ui_joypad->button_template->buttons[7];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[7];
	    }
	case 7:
	    if (joy_select) {
		ui_joypad->data |= ui_joypad->button_template->buttons[6];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[6];
	    }
	case 6:
	    if (joy_b) {
		ui_joypad->data |= ui_joypad->button_template->buttons[5];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[5];
	    }
	case 5:
	    if (joy_a) {
		ui_joypad->data |= ui_joypad->button_template->buttons[4];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[4];
	    }
	case 4:
	    if (joy_right) {
		ui_joypad->data |= ui_joypad->button_template->buttons[3];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[3];
	    }
	case 3:
	    if (joy_left) {
		ui_joypad->data |= ui_joypad->button_template->buttons[2];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[2];
	    }
	case 2:
	    if (joy_down) {
		ui_joypad->data |= ui_joypad->button_template->buttons[1];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[1];
	    }
	case 1:
	    if (joy_up) {
		ui_joypad->data |= ui_joypad->button_template->buttons[0];
	    } else {
		ui_joypad->data &= ~ui_joypad->button_template->buttons[0];
	    }
	default:
	    break;
	}
    }
}

void video_setpal(int colors, int *red, int *green, int *blue)
{
    int i;
    
    vid_pre_xlat = (unsigned char*) malloc(colors);
    
    for (i = 0; i < 64; i++) {
	gl_setpalettecolor(i+64, red[i] >> 2, green[i] >> 2, blue[i] >> 2);
	vid_pre_xlat[i] = i + 64;
    }
}


unsigned char *video_get_vbp(int line)
{
    return nes_image+(line*buffer_x);
}

/*
 * $Log: video_svga.c,v $
 * Revision 1.12  2000/09/16 23:45:10  nyef
 * moved video_shutdown() out to ui_svga.c
 *
 * Revision 1.11  2000/06/03 17:24:48  nyef
 * fixed (hopefully) to work with the new joypad interface
 *
 * Revision 1.10  2000/05/31 01:25:00  nyef
 * fixed (hopefully) to work with the new video interface
 *
 * Revision 1.9  2000/01/19 01:40:27  nyef
 * added dummy functions for the keypad interface
 *
 * Revision 1.8  1999/07/11 20:57:31  nyef
 * fixed buffer display clipping to display from the center of the buffer
 *
 * Revision 1.7  1999/07/05 01:42:22  nyef
 * added preliminary clipping for when a video buffer is larger than the
 * current screen resolution
 *
 * Revision 1.6  1999/07/01 02:18:47  nyef
 * moved video_init() out to ui_svga.c
 *
 * Revision 1.5  1999/06/06 20:30:16  nyef
 * converted to new joypad spec
 *
 * Revision 1.4  1999/02/07 17:06:16  nyef
 * added video_setsize() interface
 *
 * Revision 1.3  1999/02/06 16:49:57  nyef
 * added a call io video_events() in video_display_buffer().
 *
 * Revision 1.2  1999/01/18 22:08:18  nyef
 * added a cleaned up version of the changes by Toby Deshane.
 *
 * Revision 1.1  1998/07/11 22:19:21  nyef
 * Initial revision
 *
 */
