/*
 * video_x.c
 *
 * display screen management
 */

/* $Id: video_x.c,v 1.26 2000/09/16 16:01:59 nyef Exp $ */

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/XShm.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "tool.h"
#include "ui.h"

/* the joypad (only one, I could care less about more) */
struct joypad *ui_joypad;

/* the keypad (only one, I could care less about more) */
struct keypad *ui_keypad;

int video_active = 0;
int deblevel = 0;

Display *display;
Window nes_window;
GC nes_gc;
Colormap nes_cmap;
Screen *nes_screen;

int buffer_x;
int buffer_y;

struct image_buffer {
    int is_shm;
    char *data;
    XImage *image;
} image_buffers[2], *cur_image_buffer;

typedef void ((*xlatfunc)(void *, int));

xlatfunc vidxlate;
void vidxlate16 (char *dest, int size);
typedef void (*video_setpal_t)(int, int *r, int *g, int *b);
video_setpal_t video_setpal_true;
void init_xlate8(int colors, int *red, int *green, int *blue);
void init_xlate16(int colors, int *red, int *green, int *blue);
void init_xlate32(int colors, int *red, int *green, int *blue);
unsigned char *vid_pre_xlat;

/* FIXME: ui_x.c interface -- move to separate header. */
extern Widget drawbox;
extern Widget manager;
void ui_set_drawbox_size(int width, int height);

void do_key_event(Widget, XtPointer, XEvent *, Boolean *);

/*
 * Thanks to Sam Lantinga of SDL and the GDK team for XShm references.
 */
int use_shm = 0;
int shm_error = 0;
int (*old_x_handler)(Display *, XErrorEvent *);
int alloc_shm_surface(struct image_buffer *, int, int, int, int);
int shm_error_handler(Display*, XErrorEvent *);

void video_init()
{
    int is_local_display;

    display = XtDisplay(drawbox);
    nes_screen = XtScreen(drawbox);
    nes_window = XtWindow(drawbox);

    ui_set_drawbox_size(256, 240);

    nes_gc = DefaultGCOfScreen(nes_screen);
    XtVaGetValues(drawbox, XtNcolormap, &nes_cmap, NULL);

    XtAddEventHandler(manager, KeyPressMask | KeyReleaseMask, False, do_key_event, NULL);
    
    XFlush(display);

    if (0 == strncmp(XDisplayName(NULL), ":", 1)) {
	is_local_display = True;
    } else {
	is_local_display = False;
    }

    if (is_local_display) {
	use_shm = XShmQueryExtension(display);
    } else {
	use_shm = 0;
    }

#ifdef FORCE_NOXSHM
    use_shm = 0;
#endif
    
    if (DefaultDepthOfScreen(nes_screen) == 8) {
	video_setpal_true = init_xlate8;
    } else if (DefaultDepthOfScreen(nes_screen) == 16) {
	video_setpal_true = init_xlate16;
    } else if (DefaultDepthOfScreen(nes_screen) == 24) {
	video_setpal_true = init_xlate32;
    } else {
	fprintf(stderr, "display depth %d unsupported.\n", DefaultDepthOfScreen(nes_screen));
	exit(1);
    }
}

void video_shutdown(void)
{
}

void video_allocate_image_buffer(struct image_buffer *buffer, int bit_depth, int bytes_per_pixel, int x, int y)
{
    if (use_shm) {
	if (alloc_shm_surface(buffer, bit_depth, bytes_per_pixel, x, y)) {
	    return;
	} else {
	    deb_printf("shm failure, falling back.\n");
	    use_shm = 0;
	}
    }
    
    buffer->data = malloc(x * y * bytes_per_pixel);
    buffer->image = XCreateImage(display, DefaultVisualOfScreen(nes_screen),
				 bit_depth, ZPixmap, 0, buffer->data, x, y, 8, 0);
    buffer->is_shm = 0;
}

void video_setsize(int x, int y)
{
    buffer_x = x;
    buffer_y = y;

    ui_set_drawbox_size(x, y);
    
    if (DefaultDepthOfScreen(nes_screen) == 8) {
	video_allocate_image_buffer(&image_buffers[0], 8, 1, x, y);
	video_allocate_image_buffer(&image_buffers[1], 8, 1, x, y);
    } else if (DefaultDepthOfScreen(nes_screen) == 16) {
	video_allocate_image_buffer(&image_buffers[0], 16, 2, x, y);
	video_allocate_image_buffer(&image_buffers[1], 16, 2, x, y);
    } else if (DefaultDepthOfScreen(nes_screen) == 24) {
	video_allocate_image_buffer(&image_buffers[0], 24, 4, x, y);
	video_allocate_image_buffer(&image_buffers[1], 24, 4, x, y);
    }

    cur_image_buffer = &image_buffers[0];
}

int shm_error_handler(Display* d, XErrorEvent* e)
{
    if (BadAccess == e->error_code) {
	shm_error = 1;
	return 0;
    }

    return old_x_handler(d, e);
}

int alloc_shm_surface(struct image_buffer *buffer, int depth, int bytes_per_pixel, int width, int height)
{
    XShmSegmentInfo *shm_info;
    Screen *screen = XtScreen(drawbox);

    shm_info = calloc(1, sizeof(XShmSegmentInfo));

    shm_info->shmid = shmget(IPC_PRIVATE, width * height * bytes_per_pixel,
			     IPC_CREAT | 0777);

    if (shm_info->shmid < 0) {
	perror("darcnes: shmget()");
	use_shm = 0;
	return 0;
    }

    shm_info->shmaddr = (char *)shmat(shm_info->shmid, 0, 0);
    shm_info->readOnly = False;
 
    if ((char *) -1 == shm_info->shmaddr) {
	perror("darcnes: shmat()");

	/* destroy the SHM block now, before we forget */
	if (shmctl(shm_info->shmid, IPC_RMID, NULL) < 0) {
	    perror("darcnes: shmctl(RMID)");
	    /* No, this isn't fatal enough to cause us to quit */
	}
	
	use_shm = 0;
	return 0;
    }

    shm_error = 0;
    /*
     * We set this to grab anything bad that happens during the
     * attach and (especially) sync calls.
     */
    old_x_handler = XSetErrorHandler(shm_error_handler);
    XShmAttach(display, shm_info);
    XSync(display, False);
    /* And then reset the old handler here. */
    XSetErrorHandler(old_x_handler);
    
    /* Automatically destroy surface on detach. */
    if (shmctl(shm_info->shmid, IPC_RMID, NULL) < 0) {
	perror("darcnes: shmctl(RMID)");
	/* No, this isn't fatal enough to cause us to quit */
    }
    
    if (shm_error) {
	fprintf(stderr, "darcnes: error during SHM attach.\n");
	shmdt(shm_info->shmaddr);
	use_shm = 0;
	return 0;
    }
    
    buffer->data = shm_info->shmaddr;
    buffer->image = XShmCreateImage(display, DefaultVisualOfScreen(screen), depth,
				    ZPixmap, shm_info->shmaddr, shm_info, width, height);
    buffer->is_shm = 1;
    
    return 1;
}

void video_display_buffer(void)
{
    if (vidxlate) {
	vidxlate(cur_image_buffer->data, buffer_x * buffer_y);
    }

    if (cur_image_buffer->is_shm) {
	XShmPutImage(display, nes_window, nes_gc, cur_image_buffer->image,
		     0, 0, 0, 0, buffer_x, buffer_y, False);
    } else {
	XPutImage(display, nes_window, nes_gc, cur_image_buffer->image, 0, 0, 0, 0, buffer_x, buffer_y);
    }

    XFlush(display);

    if (cur_image_buffer == &image_buffers[0]) {
	cur_image_buffer = &image_buffers[1];
    } else {
	cur_image_buffer = &image_buffers[0];
    }
}

/* joypad emulation */

int ui_register_joypad(struct joypad *pad)
{
    if (ui_joypad) {
	return 0;
    }
    
    ui_joypad = pad;
    return 1;
}

void ui_update_joypad(struct joypad *pad)
{
    /* NOTE: does nothing. may want to do stuff when using real gamepads */
}

void video_events(void)
{
}

char joypad_key_state[12];

struct {
    KeySym keysym;
    int state;
} joypad_keys[] = {
    {XK_s, 8}, {XK_S, 8},
    {XK_a, 9}, {XK_A, 9},
    {XK_bracketleft, 10}, {XK_braceleft, 10},
    {XK_bracketright, 11}, {XK_braceright, 11},
    {XK_Up, 0}, {XK_KP_Up, 4},
    {XK_Down, 1}, {XK_KP_Down, 5},
    {XK_Left, 2}, {XK_KP_Left, 6},
    {XK_Right, 3}, {XK_KP_Right, 7},
};

void handle_joypad_key(int type, KeySym keysym)
{
    int i;

    for (i = (sizeof(joypad_keys) / sizeof(joypad_keys[0])) - 1; i >= 0; i--) {
	if (keysym == joypad_keys[i].keysym) {
	    if (type == KeyPress) {
		joypad_key_state[joypad_keys[i].state] = 1;
	    } else {
		joypad_key_state[joypad_keys[i].state] = 0;
	    }
	}
    }
    
    if (!ui_joypad) {
	return;
    }

    switch (ui_joypad->button_template->num_buttons) {
	/* NOTE: all cases fall through */
    case 8:
	if (joypad_key_state[11]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[7];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[7];
	}
	
    case 7:
	if (joypad_key_state[10]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[6];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[6];
	}
	
    case 6:
	if (joypad_key_state[9]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[5];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[5];
	}
	
    case 5:
	if (joypad_key_state[8]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[4];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[4];
	}
	
    case 4:
	if (joypad_key_state[7] || joypad_key_state[3]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[3];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[3];
	}
	
    case 3:
	if (joypad_key_state[6] || joypad_key_state[2]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[2];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[2];
	}
	
    case 2:
	if (joypad_key_state[5] || joypad_key_state[1]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[1];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[1];
	}
	
    case 1:
	if (joypad_key_state[4] || joypad_key_state[0]) {
	    ui_joypad->data |= ui_joypad->button_template->buttons[0];
	} else {
	    ui_joypad->data &= ~ui_joypad->button_template->buttons[0];
	}
	
    default:
    }
}

/* keypad emulation */

int keypad_register(struct keypad *pad)
{
    if (ui_keypad) {
	return 0;
    }
    
    ui_keypad = pad;
    return 1;
}

void keypad_update(struct keypad *pad)
{
    /* NOTE: does nothing. may want to do stuff when using real keypads */
}

u16 keypad_data;

struct {
    KeySym keysym;
    u16 state;
} keypad_keys[12] = {
    {XK_KP_0, KPD_0},
    {XK_KP_1, KPD_1},
    {XK_KP_2, KPD_2},
    {XK_KP_3, KPD_3},
    {XK_KP_4, KPD_4},
    {XK_KP_5, KPD_5},
    {XK_KP_6, KPD_6},
    {XK_KP_7, KPD_7},
    {XK_KP_8, KPD_8},
    {XK_KP_9, KPD_9},
    {XK_KP_Multiply, KPD_STAR},
    {XK_KP_Add, KPD_HASH},
};

void handle_keypad_key(int type, KeySym keysym)
{
    int i;

    for (i = (sizeof(keypad_keys) / sizeof(keypad_keys[0])) - 1; i >= 0; i--) {
	if (keysym == keypad_keys[i].keysym) {
	    if (type == KeyPress) {
		keypad_data |= keypad_keys[i].state;
	    } else {
		keypad_data &= ~keypad_keys[i].state;
	    }
	}
    }
    
    if (!ui_keypad) {
	return;
    }

    ui_keypad->data = keypad_data;
}

/* keyboard emulation */

typedef void (*keyhook)(Display *display, XEvent *event);

keyhook ui_keyhook;

void ui_set_keyboard_hook(keyhook hook)
{
    ui_keyhook = hook;
}

/* key event handler */

void do_key_event(Widget w, XtPointer client_data, XEvent *event, Boolean *continue_dispatch)
{
    KeySym keysym;
    Modifiers dummy;

    XtTranslateKeycode(display, event->xkey.keycode, event->xkey.state, &dummy, &keysym);

    if (ui_keyhook) {
	ui_keyhook(display, event);
    }
    
    handle_joypad_key(event->type, keysym);
    handle_keypad_key(event->type, keysym);
}

short *xlatepal;

void vidxlate16(char *dest, int size)
{
    int i;

    for (i = size; i > 0; i--) {
	*(((short *)dest)+(i-1)) = xlatepal[(int) *(dest+(i-1))];
    }
}

long *xlatepal2;

void vidxlate32(char *dest, int size)
{
    int i;

    for (i = size; i > 0; i--) {
	*(((long *)dest)+(i-1)) = xlatepal2[(int) *(dest+(i-1))];
    }
}

unsigned char *video_get_vbp(int line)
{
    return cur_image_buffer->data + (line * buffer_x);
}

/* #include "nespal.c" */

void init_xlate8(int colors, int *red, int *green, int *blue)
{
    int i;
    XColor color;
    
    vid_pre_xlat = malloc(colors);
    vidxlate = NULL;
    
    for (i = 0; i < colors; i++) {
	color.flags = DoRed | DoGreen | DoBlue;
	color.red = red[i];
	color.green = green[i];
	color.blue = blue[i];
	color.red += color.red << 8;
	color.green += color.green << 8;
	color.blue += color.blue << 8;
	if (XAllocColor(display, nes_cmap, &color)) {
	    vid_pre_xlat[i] = color.pixel;
	}
    }
}

void init_xlate16(int colors, int *red, int *green, int *blue)
{
    int i;
    XColor color;

    xlatepal = malloc(colors << 1);
    vid_pre_xlat = malloc(colors);
    vidxlate = (xlatfunc)&vidxlate16;
    
    for (i = 0; i < colors; i++) {
	vid_pre_xlat[i] = i;
	color.flags = DoRed | DoGreen | DoBlue;
	color.red = red[i];
	color.green = green[i];
	color.blue = blue[i];
	color.red += color.red << 8;
	color.green += color.green << 8;
	color.blue += color.blue << 8;
	if (XAllocColor(display, nes_cmap, &color)) {
	    xlatepal[i] = color.pixel;
	}
    }
}

void init_xlate32(int colors, int *red, int *green, int *blue)
{
    int i;
    XColor color;

    xlatepal2 = malloc(colors << 2);
    vid_pre_xlat = malloc(colors);
    vidxlate = (xlatfunc)&vidxlate32;
    
    for (i = 0; i < colors; i++) {
	vid_pre_xlat[i] = i;
	color.flags = DoRed | DoGreen | DoBlue;
	color.red = red[i];
	color.green = green[i];
	color.blue = blue[i];
	color.red += color.red << 8;
	color.green += color.green << 8;
	color.blue += color.blue << 8;
	if (XAllocColor(display, nes_cmap, &color)) {
	    xlatepal2[i] = color.pixel;
	}
    }
}

void video_setpal(int num_colors, int *red, int *green, int *blue)
{
    video_setpal_true(num_colors, red, green, blue);
}

/*
 * $Log: video_x.c,v $
 * Revision 1.26  2000/09/16 16:01:59  nyef
 * removed some (unused) errno stuff
 *
 * Revision 1.25  2000/05/15 01:22:50  nyef
 * changed to work with new video interface
 *
 * Revision 1.24  2000/05/07 00:25:38  nyef
 * fixed to work with new joypad interface names
 *
 * Revision 1.23  2000/03/12 21:43:02  nyef
 * disabled F1 (debugger) and ESC (quit) keys (they didn't work)
 *
 * Revision 1.22  2000/03/06 00:39:33  nyef
 * added keyboard emulation hook
 *
 * Revision 1.21  2000/02/20 22:00:32  nyef
 * added 24/32 bpp support
 *
 * Revision 1.20  2000/02/20 17:12:50  nyef
 * cleaned up the SHM support a bit
 *
 * Revision 1.19  2000/02/20 07:34:50  nyef
 * cleaned up some of the image buffer manipulation code
 *
 * Revision 1.18  2000/01/01 03:22:38  nyef
 * added preliminary keypad implementation
 *
 * Revision 1.17  1999/12/31 16:50:30  nyef
 * changed how the joypad was implemented, especially wrt X keyboard events
 *
 * Revision 1.16  1999/06/05 02:41:39  nyef
 * converted to use new joypad interface
 *
 * Revision 1.15  1999/02/15 03:33:54  nyef
 * added patch from Michael Vance for XShm
 *
 * Revision 1.14  1999/02/13 22:23:51  nyef
 * added support for the keypad arrow keys
 *
 * Revision 1.13  1999/02/07 17:55:51  nyef
 * removed double-buffering code
 * added video_setsize() interface
 *
 * Revision 1.12  1999/01/05 04:28:41  nyef
 * hacked up an interface to allow setting the palette.
 *
 * Revision 1.11  1998/08/29 22:09:37  nyef
 * switched to a double-buffered video system. added routines to support
 * the PPU writing directly into the video buffers. eliminated the 8-bit
 * vidxlate routine. changed vidxlate16 to operate using only one buffer.
 * changed the video buffer size to a symbolic constant.
 *
 * Revision 1.10  1998/08/29 14:52:29  nyef
 * added an identity vid_pre_xlat table for use in 16-bitplane modes.
 *
 * Revision 1.9  1998/08/28 03:02:16  nyef
 * added a "vid_pre_xlat" table so that some of the color translation
 * could be done by the PPU. also moved the 8-bit color translation to
 * the PPU and set the 8-bit xlatfinc to be memcpy. (maybe I should
 * double buffer the display?)
 *
 * Revision 1.8  1998/08/15 15:45:34  nyef
 * reworked vidxlate16 to use a normal loop and to copy longs.
 * this should be slightly faster on a pentium.
 *
 * Revision 1.7  1998/08/15 01:08:15  nyef
 * changed unsupported display depth message to indicate current depth.
 *
 * Revision 1.6  1998/08/02 02:03:08  nyef
 * preliminary GUIfication completed.
 *
 * Revision 1.5  1998/08/01 23:12:45  nyef
 * removed the checks for the symbol "GRAPHICS".
 * removed two commented-out printf statements in the input handler.
 *
 * Revision 1.4  1998/08/01 00:57:13  nyef
 * moved old palette values into nespal.c
 *
 * Revision 1.3  1998/07/26 13:22:55  nyef
 * changed vidxlate16 tu use pointer notation and duff's device.
 *
 * Revision 1.2  1998/07/18 18:54:27  nyef
 * changed vidxlate16 to copy shorts instead of chars for a massive speed increase
 *   in 16 bitplane mode.
 *
 * Revision 1.1  1998/07/11 22:19:25  nyef
 * Initial revision
 *
 */
