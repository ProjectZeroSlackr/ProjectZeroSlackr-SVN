/*
 * video_svga.c
 *
 * display screen management
 */

/* $Id: video_svga.c,v 1.12 2000/09/16 23:45:10 nyef Exp $ */

//#include <SDL.h>
/* #include <vgajoystick.h> */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
#include "video.h"
#include "tool.h"
#include "ui.h"
#include "../hotdog/hotdog.h"
#define BUFFER_X 256
#define BUFFER_Y 240
#define BUFFER_SIZE (BUFFER_X * BUFFER_Y)


/* the joypad (only one, I could care less about more) */
struct joypad *ui_joypad;

int video_active = 0;
int deblevel = 0;


int middle_x=0,
    middle_y=0;
int buffer_x;
int buffer_y;
int image_x;
int image_y;
int offset_x;
int offset_y;
int tru=1;
int fals=0;

int use_joystick = 1;
unsigned int joy_up, joy_down, joy_left, joy_right,
    joy_a, joy_b, joy_start, joy_select;

void init_xlate8(int colors, int *red, int *green, int *blue);
unsigned char *vbpfunc8(int line);

unsigned char  *vid_pre_xlat;
int  *vid_pre_xlat2;
unsigned char *nes_image;

void video_events(void);


static int consoleFd = -1;
static struct termios old;
#define  KEYCODE(a) (a & 0x7f)

#define KH_KEY_MENU    50
#define KH_KEY_REWIND  17
#define KH_KEY_PLAY    32
#define KH_KEY_FORWARD 33

#define KH_KEY_ACTION  28
#define KH_KEY_HOLD    35

#define KH_WHEEL_L     38
#define KH_WHEEL_R     19

#define EV_SCROLL        0x1000
#define EV_TAP           0x2000
#define EV_PRESS         0x4000
#define EV_RELEASE       0x8000
#define EV_TOUCH         0x0100
#define EV_LIFT          0x0200
#define EV_MASK          0xff00

#define BTN_ACTION       0x0001
#define BTN_NEXT         0x0002
#define BTN_PREVIOUS     0x0004
#define BTN_PLAY         0x0008
#define BTN_MENU         0x0010
#define BTN_HOLD         0x0020
#define BTN_MASK         0x00ff

#define SCROLL_LEFT      0x0080
#define SCROLL_RIGHT     0x0000
#define SCROLL_MASK      0x007f
#define TAP_MASK         0x007f
#define TOUCH_MASK       0x007f
#define LIFT_MASK        0x007f

int START_INPUT(void)
{
	  struct termios new;

	    if ((consoleFd = open("/dev/console",O_RDONLY | O_NONBLOCK)) < 0)
		        fprintf(stderr, "Could not open /dev/console");

	      if (tcgetattr(consoleFd, &old) < 0)
		          fprintf(stderr, "Could not save old termios");

	        new = old;

		  new.c_lflag    &= ~(ICANON | ECHO  | ISIG);
		    new.c_iflag    &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON | BRKINT);
		      new.c_cc[VMIN]  = 0;
		        new.c_cc[VTIME] = 0;

			  if (tcsetattr(consoleFd, TCSAFLUSH, &new) < 0)
				      exit(0);

			    if (ioctl(consoleFd, KDSKBMODE, K_MEDIUMRAW) < 0)
				        exit(0);

			      return 0;
}

void STOP_INPUT(void)
{
	  if (tcsetattr(consoleFd, TCSAFLUSH, &old) < 0)
		      fprintf(stderr, "Could not reset old termios");

	    if (consoleFd > 2)
		        if (close(consoleFd) < 0)
				      fprintf(stderr, "Could not close console");
}


int GET_KEY(void)
{
	  int c = 0;

	    if (read(consoleFd, &c, 1) != 1)
		        c = -1;

	      return c;
}

int input_started=0;

int flipper=0;
void input_update()
{
/*    SDL_Event keyevent;
  while  (SDL_PollEvent(&keyevent)) {
          switch(keyevent.type) {
                 case SDL_KEYDOWN:
			switch(keyevent.key.keysym.sym){
			        case SDLK_h:
			            system_flags |= F_QUIT;
			            break;
				case SDLK_m:
				    joy_up=tru;
				    break;
			        case SDLK_w:
			            joy_left=tru;
			             break;
				case SDLK_f:
				    joy_right=tru;
				     break;
				case SDLK_r:
				    joy_a=tru;
				     break;
				case SDLK_l:
				    joy_b=tru;
				case SDLK_d:
				    joy_down=tru;
				     break;
				case SDLK_RETURN:
				     joy_start=tru;
				     break;
				default:
				     break;
			}
			break;
                 case SDL_KEYUP:
			switch(keyevent.key.keysym.sym){
			        case SDLK_h:
			            system_flags |= F_QUIT;
			            break;
				case SDLK_m:
				    joy_up=fals;
				    break;
			        case SDLK_w:
			            joy_left=fals;
			             break;
				case SDLK_f:
				    joy_right=fals;
				     break;
				case SDLK_r:
				    joy_a=fals;
				     break;
				case SDLK_l:
				    joy_b=fals;
				case SDLK_d:
				    joy_down=fals;
				     break;
				case SDLK_RETURN:
				     joy_start=fals;
				     break;
				default:
				     break;
			}
			break;
	  }
  }*/
  if (input_started!=1) { START_INPUT(); input_started=1; }

/*  keyboard_update();
    
    if (keyboard_keypressed(SCANCODE_ESCAPE)) {
	system_flags |= F_QUIT;
    } else {
	system_flags &= ~F_QUIT;
    }*/
  joy_a = fals;
    joy_b = fals;
/*    joy_select = fals;
    joy_start  = fals;
    joy_up     = fals;
    joy_down   = fals;
    joy_left   = fals;
    joy_right  = fals;*/
/*    switch (flipper)
    {
	    case 1:
	    joy_a = tru; break;
	    case 2:
	    joy_b = tru; break;
	    case 3:
	    joy_up = tru; break;
	    case 4:
	    joy_down = tru; break;
	    case 5:
	    joy_left = tru; break;
	    case 6:
	    joy_right = tru; break;
	    case 7:
	    joy_select = tru; break;
	    case 8:
	    joy_start = tru; break;
    }
    flipper=(flipper +1) % 9;*/
    int k=GET_KEY();
    while (k!=-1)
    {
    int up=k & 0x80;
    k=KEYCODE(k);
//    if (k == KH_KEY_MENU) { STOP_INPUT(); exit(0); }
    if (up ==0) { 
    if (k == KH_KEY_MENU) { joy_up = tru; } 
    if (k == KH_KEY_PLAY) { joy_down = tru; } 
    if (k == KH_KEY_REWIND) { joy_left = tru; } 
    if (k == KH_KEY_FORWARD) { joy_right = tru; } 
    if (k == KH_KEY_ACTION) { joy_start = tru; } 
    if (k == KH_WHEEL_L) { joy_a = tru; } 
    if (k == KH_WHEEL_R) { joy_b = tru; } 
    }
    else
    {
    if (k == KH_KEY_MENU) { joy_up = fals; } 
    if (k == KH_KEY_PLAY) { joy_down = fals; } 
    if (k == KH_KEY_REWIND) { joy_left = fals; } 
    if (k == KH_KEY_FORWARD) { joy_right = fals; } 
    if (k == KH_KEY_ACTION) { joy_start = fals; } 
//    if (k == KH_WHEEL_L) { joy_a = fals; } 
//    if (k == KH_WHEEL_R) { joy_b = fals; } 
    }
    k=GET_KEY(); 
    }

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

int vga_getxdim() { return 256; } 
int vga_getydim() { return 240; } 

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
    int hw_ver;
    int lcd_width;
    int lcd_height;
    int lcd_type;
	       
    HD_LCD_GetInfo (&hw_ver, &lcd_width, &lcd_height, &lcd_type);
	     
    screen = malloc(lcd_width*lcd_height*2);
    if (!nes_image) {
	printf("unable to allocate image buffer.\n");
	exit(-1);
    }
}

void video_enter_deb(void)
{
    if (video_active && (!deblevel)) {
//	SDL_Flip(screen);
    }
    deblevel++;
}

void video_leave_deb(void)
{
    deblevel--;
    if (video_active && (!deblevel)) {
//	SDL_Flip(screen);
    }
}

__inline__
void video_display_buffer()
{
	int j,i,p=0;
	short* bufp=screen;
//	for (j=0; j<buffer_y>>1; j++)
//	{
//		for (i=0; i<buffer_x>>1; i++)
//		{
//			*bufp=vid_pre_xlat2[nes_image[p]];
//			bufp++;
//			p+=2;
//		}
//		bufp+=176-(buffer_x>>1);
//		p+=buffer_x;
//	
//	}
        p+=buffer_x*54+40;	
	for (j=0; j<132; j++)
	{
		for (i=0; i<176; i++)
		{
			*bufp=vid_pre_xlat2[nes_image[p]];
			bufp++;
			p++;
		}
//bufp+=176-(buffer_x>>1);
		p+=buffer_x-176;
	
	}
	HD_LCD_Update (screen,0,0,176,132);
/*    if ((buffer_x == image_x) && (buffer_y == image_y)) {
	gl_putbox(middle_x, middle_y, buffer_x, buffer_y, nes_image);
    } else {
	gl_putboxpart(middle_x, middle_y, image_x, image_y, buffer_x, buffer_y, nes_image, offset_x, offset_y);
    }*/
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

//    if (ui_joypad) 
  //  {
//	switch (ui_joypad->button_template->num_buttons) {
//	case 8:

	    if (joy_start) {
		ui_joypad->data |= ui_joypad->button_template->buttons[7];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[7]);
	    }
//	case 7:
	    if (joy_select) {
		ui_joypad->data |= ui_joypad->button_template->buttons[6];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[6]);
	    }
//	case 6:
	    if (joy_b) {
		ui_joypad->data |= ui_joypad->button_template->buttons[5];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[5]);
	    }
//	case 5:
	    if (joy_a) {
		ui_joypad->data |= ui_joypad->button_template->buttons[4];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[4]);
	    }
//	case 4:
	    if (joy_right) {
		ui_joypad->data |= ui_joypad->button_template->buttons[3];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[3]);
	    }
//	case 3:
	    if (joy_left) {
		ui_joypad->data |= ui_joypad->button_template->buttons[2];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[2]);
	    }
//	case 2:
	    if (joy_down) {
		ui_joypad->data |= ui_joypad->button_template->buttons[1];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[1]);
	    }
//	case 1:
	    if (joy_up) {
		ui_joypad->data |= ui_joypad->button_template->buttons[0];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[0]);
	    }
//	default:
//	    break;
//	}
  //  }
}

void video_setpal(int colors, int *red, int *green, int *blue)
{
    int i;
    
    vid_pre_xlat = (unsigned char*) malloc(colors);
    vid_pre_xlat2 = (int*) malloc(sizeof(int)*colors*4);
    
    for (i = 0; i < 64; i++) {
	vid_pre_xlat[i] =  i+64; //SDL_MapRGB(screen->format, red[i], green[i], blue[i]);
	vid_pre_xlat2[i+64] = ((red[i]>>3)<<11) + ((green[i]>>2)<<5)+ ((blue[i]>>3));
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
