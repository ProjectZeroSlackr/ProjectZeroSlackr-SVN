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
#include <dirent.h>
#include "video.h"
#include "tool.h"
#include "ui.h"
#include "../hotdog/hotdog.h"
#include "cop.h"
#include <sys/soundcard.h>
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
int roffset_x=40;
int roffset_y=84;
int movemode=0;
char sfilename[50];
int use_joystick = 1;
unsigned int joy_up, joy_down, joy_left, joy_right,
    joy_a, joy_b, joy_start, joy_select;
extern int qui;
unsigned int tjoy_up, tjoy_down, tjoy_left, tjoy_right,
    tjoy_a, tjoy_b, tjoy_start, tjoy_select;
void init_xlate8(int colors, int *red, int *green, int *blue);
unsigned char *vbpfunc8(int line);

unsigned char  *vid_pre_xlat=NULL;
int  vid_pre_xlat2[128];
int  vid_pre_xlat2r[128*128];
int  vid_pre_xlat2g[128*128];
int  vid_pre_xlat2b[128*128];
unsigned char *nes_image=NULL;
unsigned char *nes_imageb=NULL;

struct spritedata {
    unsigned char vpos;
    unsigned char tile;
    unsigned char flags;
    unsigned char xpos;
};
void video_events(void);
int fskip=2,fr;
int volume=48;
int selected=0;
int totalentries=2;
#define MAINMENU 0
#define CFGMENU  1
#define ROMSMENU 2
#define KEYSMENU 3
int mline=0;
int whereinmenu=0;
static int consoleFd = -1;
static struct termios old;
//char *keynames = {""};
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
int W,H,type,ver;
char z[13];
int START_INPUT(void)
{
	  struct termios new;
FILE *fp;
/*int kb;
char pad;
if((fp=fopen("idarcnes.cfg","r"))==0) 
z[13]=1;
else {
int i;
for(i=0;i<=7;i++)
{
  fscanf(fp,"kb%i=PAD_%c\n",&kb,&pad);
  z[kb]=pad;
}
for(i=8;i<=12;i++)
{
  fscanf(fp,"pd%i=PAD_%c\n",&kb,&pad);
  z[kb]=pad;
}
fscanf(fp,"fskip=%i\n",&fskip);
fclose(fp);
}
free(kb);
free(pad);*/
  
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

/*unsigned char touch_map[8] = {
	        PAD_UP,
	        PAD_A,
	        PAD_RIGHT,
	        PAD_START,
	        PAD_DOWN,
	        PAD_SELECT,
	        PAD_LEFT,
	        PAD_B
};*/
int nicescale=1;
char scaletoggle(void);
int scaling=2;
int but_mod=0;
int but_mod2=0;
extern unsigned int *kb[8], *menu, *sel, *play,
    *rev, *fwd;
    extern unsigned char kbc[8], menuc, selc, playc,
    revc, fwdc;
unsigned int joy_up, joy_down, joy_left, joy_right,
    joy_a, joy_b, joy_start, joy_select;
extern void nes_run(rom_file romfile);
extern void nes_shutdown(void);
extern void nes_save_battery_file(void);
extern unsigned char PPU_sprite_ram[0x100];
extern rom_file romfile;
extern char* filename;
extern int mixer;
int up_but=0;
int actcfg=0;
int outcfg=1;
int butcfgwd[14]= {0,4,3,7,1,6,2,5,0,2,8,3,1,8};
int butcfgwod[14]={0,9,3,9,1,9,2,9,0,2,8,3,1,8};
int *butcfg=butcfgwd;
int input_updated=0;
int fsprnum=0;
int fsprnumt=0;
int diags=0;
void rescale(void);
void ssave(void);
void keyc(char z,unsigned int *y);
void ssave()
{
  FILE*fp=fopen("/opt/Emulators/iDarcNES/Conf/iDarcNES.cfg","w");
  int i;
  for(i=0;i<=7;i++)
  fprintf(fp,"kb%i=%c\n",i,kbc[i]);
  
  fprintf(fp,"play=%c\n",playc);
  fprintf(fp,"menu=%c\n",menuc);
  fprintf(fp,"select=%c\n",selc);
  fprintf(fp,"fwd=%c\n",fwdc);
  fprintf(fp,"rev=%c\n",revc);
  fprintf(fp,"screen=%i\n",scaling);
 fclose(fp);
}
void keyc(char z,unsigned int *y)
{
  if(z=='A')
{    z='B';
    *y=&joy_b;
    return 0; }
      if(z=='B')
{ 
    z='S';
    *y=&joy_start;
    return 0; }
    if(z=='S')
{ 
    z='E';
    *y=&joy_select;
    return 0; }
    if(z=='E')
{ 
    z='U';
    *y=&joy_up;
    return 0; }
     if(z=='U')
{ 
    z='D';
    *y=&joy_down;
    return 0; }
     if(z=='D')
{ 
    z='R';
    *y=&joy_right;
    return 0; }
    if(z=='R')
{ 
    z='L';
    *y=&joy_left;
    return 0;
    }
  if(z=='L')
{ 
    z='A';
    *y=&joy_a;
    return 0; }
  
}
void cfg(int k){
    switch(k)
    {
	    case KH_KEY_MENU: selected--; if (selected<0) selected=totalentries;
			      break;
	    case KH_KEY_PLAY: selected++; if (selected>totalentries) selected=0;
			      break;
	    case KH_KEY_REWIND:whereinmenu=MAINMENU; selected=0; 
			      break; 
	    case KH_KEY_ACTION:
		switch(whereinmenu)
		{
		case MAINMENU:
				if (selected==0) { whereinmenu=CFGMENU; selected=0; }
				if (selected==1) { whereinmenu=ROMSMENU; selected=0; } 
				// All defunct at the moment
				// ~Keripo
				//if (selected==2) {  nes_save_battery_file(); }
				//if(selected==3) {saves();}
				//if(selected==4) {loads();}
				if (selected==2) {HD_LCD_Quit();free(nes_imageb);cop_end(); nes_shutdown();STOP_INPUT();/*free(romfile);free(screen);free(nes_image);*/PPU_shutdown();exit(0);qui=0;}
			break;
		case CFGMENU:
				if (selected==0) { scaletoggle(); }
				if (selected==1) { nicescale^=1; }
				if (selected==2) { whereinmenu=KEYSMENU; selected=0; totalentries=2; }
				if (selected==3) { fsprnum++; 
    if (fsprnum<0) fsprnum=63;
    if (fsprnum>63) fsprnum=0;
    			struct spritedata *nsprites;
    			nsprites = (struct spritedata *)PPU_sprite_ram;
			fsprnumt=nsprites[fsprnum].tile;
				
				}
				if (selected==4) { volume=(volume+10); 
						   if (volume>80) volume=0;
						  int vol = volume << 8 | volume;
						  int ret = ioctl(mixer, SOUND_MIXER_WRITE_PCM, &vol);
				}
				if(selected==5) ssave();
			break;
		case ROMSMENU:
				cop_end();
				filename=strdup(sfilename);
				system_flags |= F_QUIT;
				system_flags &= ~F_GUI;
				memset(screen,0,W*H*2);
			break;
		case KEYSMENU:
				if (selected==0) keyc(&menuc,&menu);
				if (selected==1) keyc(&selc,&sel);
				if (selected==2) keyc(&playc,&play);
				if (selected==3) keyc(&fwdc,&fwd);
				if (selected==4) keyc(&revc,&rev);
				if (selected==5) keyc(&kbc[0],&kb[0]);
				if (selected==6) keyc(&kbc[1],&kb[1]);
				if (selected==7) keyc(&kbc[2],&kb[2]);
				if (selected==8) keyc(&kbc[3],&kb[3]);
				if (selected==9) keyc(&kbc[4],&kb[4]);
				if (selected==10) keyc(&kbc[5],&kb[5]);
				if (selected==11) keyc(&kbc[6],&kb[6]);
				if (selected==12) keyc(&kbc[7],&kb[7]);
			break;
				
	}
    		
    } 
}
extern int lastframe;
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
       if (tjoy_b) { tjoy_b = fals; joy_b = fals; } 
    if (tjoy_up) { tjoy_up = fals; joy_up = fals; } 
    if (tjoy_a) { tjoy_a = fals; joy_a = fals; } 
    if (tjoy_right) { tjoy_right = fals; joy_right = fals; } 
    if (tjoy_start) { tjoy_start = fals; joy_start = fals; } 
    if (tjoy_down) { tjoy_down = fals; joy_down = fals; } 
    if (tjoy_select) { tjoy_select = fals; joy_select = fals; } 
    if (tjoy_left) { tjoy_left = fals; joy_left = fals; } 
 
/*    if (!(joy_a || joy_b)) {
    joy_a = fals;
    joy_b = fals;
    joy_select = fals;
    joy_start  = fals;
    joy_up     = fals;
    joy_down   = fals;
    joy_left   = fals;
    joy_right  = fals;}*/
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
int k;
	int in,st;
	unsigned char touch,button;
        in = inl(0x7000C140);
//        if(in == 0) enter_menu();       // FIXME: hold - is there a better way?
        st = (in & 0xff000000) >> 24;
        if(st == 0xc0)
        {       // touched
                touch = (in & 0x007F0000 ) >> 16;
                touch+=6;
                touch/=12;
                if(touch>7) touch=0;
                k=touch;//touch_map[touch]);
 
   /* if (k == 0) {*kb[0]=tru; } 
    if (k == 1) { *kb[1]=tru;} 
    if (k == 2) { *kb[2]=tru; } 
    if (k == 3) { *kb[3]=tru; } 
    if (k == 4) {*kb[4]=tru; } 
    if (k == 5) { *kb[5]=tru;} 
    if (k == 6) {*kb[6]=tru;} 
    if (k == 7) { *kb[7]=tru; } */
    if(k>=0 && k<=7) *kb[k]=tru;
    
    
        }
//        else if(st != 0x80) return;
//
//	        PAD_UP,
//	        PAD_A,
//	        PAD_RIGHT,
//	        PAD_START,
//	        PAD_DOWN,
//	        PAD_SELECT,
//	        PAD_LEFT,
//	        PAD_B
					
  k=GET_KEY();
    while (k!=-1)
    {
    int up=k & 0x80;
    k=KEYCODE(k);
//    if (k == KH_KEY_MENU) { STOP_INPUT(); exit(0); }
   if (up ==0) { 
    if (k == KH_KEY_MENU) {*menu=tru; selected--; if (selected<0) selected=totalentries;}
    if (k == KH_KEY_PLAY) { *play=tru;selected++; if (selected>totalentries) selected=0;} 
    if (k == KH_KEY_REWIND) { *rev=tru; whereinmenu=MAINMENU; selected=0; } 
    if (k == KH_KEY_FORWARD) { *fwd=tru;} 
    if (k == KH_KEY_ACTION) { *sel=tru; if(!(inl(COP_STATUS) & COP_LINE_REQ)) cfg(k);} 
    if (k == KH_KEY_HOLD) { while ((inl(COP_STATUS) & COP_LINE_REQ) != 0);if(system_flags & F_GUI)system_flags &= ~F_GUI;else system_flags |= F_GUI; } 
//    if (k == KH_WHEEL_L) { joy_a = tru; } 
//    if (k == KH_WHEEL_R) { joy_b = tru; } 

 
    }
    else
    {
if (k == KH_KEY_MENU) {*menu=fals; }
    if (k == KH_KEY_PLAY) { *play=fals;} 
    if (k == KH_KEY_REWIND) { *rev=fals; } 
    if (k == KH_KEY_FORWARD) { *fwd=fals;} 
    if (k == KH_KEY_ACTION) { *sel=fals; } 
  //  if (k == KH_KEY_HOLD) { while ((inl(COP_STATUS) & COP_LINE_REQ) != 0);if(system_flags & F_GUI)system_flags &= ~F_GUI;else system_flags |= F_GUI; } 
//    if (k == KH_WHEEL_L) { joy_a = tru; } 
//    if (k == KH_WHEEL_R) { joy_b = tru; } 
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*void input_update()
{
if (input_started!=1) { START_INPUT(); input_started=1; }

/*  keyboard_update();
    
    if (keyboard_keypressed(SCANCODE_ESCAPE)) {
	system_flags |= F_QUIT;
    } else {
	system_flags &= ~F_QUIT;
    }*/
  /* if (!(joy_a && joy_b)) {
    *menu = fals;
    *play = fals;
    *fwd = fals;
    *rev  = fals;
 //   *sel     = fals;
    *kb[0]   = fals;
    *kb[1]   = fals;
    *kb[2]  = fals;
    *kb[3]  = fals;
    *kb[4]  = fals;
    *kb[5]  = fals;
    *kb[6]  = fals;
    *kb[7]  = fals;}
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
    flipper=(flipper +1) % 9;*//*
int k;
	int in,st;
	unsigned char touch,button;
        in = inl(0x7000C140);
//        if(in == 0) enter_menu();       // FIXME: hold - is there a better way?
        st = (in & 0xff000000) >> 24;
        if(st == 0xc0)
        {       // touched
                touch = (in & 0x007F0000 ) >> 16;
                touch+=6;
                touch/=12;
                if(touch>7) touch=0;
                k=touch;//touch_map[touch]);

    if (k == 0) { *kb[0]=tru; } 
    if (k == 1) {*kb[1]=tru; } 
    if (k == 2) {*kb[2]=tru;} 
    if (k == 3) { *kb[3]=tru; } 
    if (k == 4) { *kb[4]=tru; } 
    if (k == 5) { *kb[5]=tru;} 
    if (k == 6) { *kb[6]=tru; } 
    if (k == 7) { *kb[7]=tru; } 
  }
        
//        else if(st != 0x80) return;
//
//	        PAD_UP,
//	        PAD_A,
//	        PAD_RIGHT,
//	        PAD_START,
//	        PAD_DOWN,
//	        PAD_SELECT,
//	        PAD_LEFT,
//	        PAD_B
					
  k=GET_KEY();
    while (k!=-1)
    {
    int up=k & 0x80;
    k=KEYCODE(k);
//    if (k == KH_KEY_MENU) { STOP_INPUT(); exit(0); }
//    if (k == KH_KEY_HOLD) { STOP_INPUT(); exit(0); }
    if (up ==0) { 
    if (k == KH_KEY_MENU) {*menu=tru; selected--; if (selected<0) selected=totalentries;}
    if (k == KH_KEY_PLAY) { *play=tru;selected++; if (selected>totalentries) selected=0;} 
    if (k == KH_KEY_REWIND) { *rev=tru; whereinmenu=MAINMENU; selected=0; } 
    if (k == KH_KEY_FORWARD) { *fwd=tru;} 
    if (k == KH_KEY_ACTION) { *sel=tru; if(!(inl(COP_STATUS) & COP_LINE_REQ)) cfg(k);} 
    if (k == KH_KEY_HOLD) { while ((inl(COP_STATUS) & COP_LINE_REQ) != 0);if(system_flags & F_GUI)system_flags &= ~F_GUI;else system_flags |= F_GUI; } 
//    if (k == KH_WHEEL_L) { joy_a = tru; } 
//    if (k == KH_WHEEL_R) { joy_b = tru; } 

 
    }
    else
    {
if (k == KH_KEY_MENU) {*menu=fals; }
    if (k == KH_KEY_PLAY) { *play=fals;} 
    if (k == KH_KEY_REWIND) { *rev=fals; } 
    if (k == KH_KEY_FORWARD) { *fwd=fals;} 
    if (k == KH_KEY_ACTION) { *sel=fals; } 
  //  if (k == KH_KEY_HOLD) { while ((inl(COP_STATUS) & COP_LINE_REQ) != 0);if(system_flags & F_GUI)system_flags &= ~F_GUI;else system_flags |= F_GUI; } 
//    if (k == KH_WHEEL_L) { joy_a = tru; } 
//    if (k == KH_WHEEL_R) { joy_b = tru; } 
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
*/
int scalex[320];
int scaley[240];
int screeny[240];
int NW,NH;
extern void snd_output_4_waves_();
int vga_getxdim() { return 256; } 
int vga_getydim() { return 240; } 
void video_display_buffer_();
extern unsigned char font[];
void putChar(short *bufp,int r, int c, char x,int invert)
{
	char rc;
	char l;
	if (x>=0x61) { rc=1+(x-65-32); l=0; } else 
	if (x>=65) { rc=1+(x-65); l=0; } else
	if (x>=48) { rc=16+(x-48); l=1; } else
	if (x>32) { rc=(x-32); l=1; } else
	{ rc=0; l=0; } 
	int i,j;
	for (j=0; j<8; j++)
	for (i=0; i<8; i++)
		bufp[r*8+(c*8+j)*W+i]=font[i+rc*8+256*(j+l*8)]!=invert?0xFFFF:0;
		
}
void putChar2(short *bufp,int r, int c, char x)
{
	char rc;
	char l=0;
	rc=x;
	
	rc=x % 32;
	l=x/32;
	int i,j;
	for (j=0; j<8; j++)
	for (i=0; i<8; i++)
		bufp[r*8+(c*8+j)*W+i]=font[i+rc*8+256*(j+l*8)]!=0?0xFFFF:0;
		
}
void print(short *bufp,int r,int c,char *s,int invert)
{
	int i=0;
	while (s[i]!=0)
	{
		putChar(bufp,r+i,c,s[i],invert);
		i++;
	}
}

void rescale()
{
int k;
	for (k=0; k<320; k++) scalex[k]=0;
	for (k=0; k<240; k++) scaley[k]=0;
	for (k=0; k<240; k++) screeny[k]=0;
if(scaling==1) {
switch(W) {
case 320:
NH=240;
NW=225;
break;
case 220:
NH=176;
NW=165;
break;
case 176:
NH=132;
NW=124;
break;
}

}

else {NH=H;NW=W;}
	
	for (k=0; k<NW; k++) scalex[k]=k*buffer_x/NW;
	for (k=0; k<NH; k++) scaley[k]=k*buffer_y/NH;
	for (k=0; k<NH; k++) screeny[k]=k*W;
	print(screen,1,1,NW,0);
	print(screen,1,2,NH,0);
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
    int ft=0;
    if (!nes_image) {
    nes_image =malloc(x * y);
    ft=1;
    }

    if (!nes_imageb) {
    nes_imageb =malloc(x * y);
    }
    int hw_ver;
    int lcd_width;
    int lcd_height;
    int lcd_type;
   
    HD_LCD_GetInfo (&hw_ver, &lcd_width, &lcd_height, &lcd_type);
    ver=hw_ver;
    W=lcd_width;
    NW=W;
    H=lcd_height;
    NH=H;
    type=lcd_type;
    if (!screen) {
    screen = malloc(lcd_width*lcd_height*2);
    }
    if (!nes_image) {
	printf("unable to allocate image buffer.\n");
	exit(-1);
    }
	rescale();
	cop_begin(video_display_buffer_);
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

int frameskip=0;


extern int waveptr;
extern unsigned short final_wave[];
extern int sound_fd;
extern int sound_pl;
int cycles_left;
u32 updatelr;
u32 lraddr;
extern int tframeskip,avgframe;
#define RED 63488 
#define GREEN 2016
#define BLUE 31
extern int fstatus1,fstatus2,fstatus3;
extern int n0,n1,n2,n3,n4,n5,n6,n7;
int xp=0,yp=0,framenum=0;
void video_display_buffer_()
{
	int j,i,p=0;
	short* bufp=screen;

	u8 *nesimageb=inl(0x40015030);
	if (W>255)
	{
//	bufp+=((W-buffer_x)>>1);	
	/*int mW=256;
	for (j=0; j<NH; j++)
	{
		for (i=0; i<NW; i++)
		{
			
			*bufp=vid_pre_xlat2[nesimageb[p]];
			bufp++;
			p++;
		}
		//p+=buffer_x-mW;
		bufp+=W-buffer_x;
	
	}*/
		for (j=0; j<NH; j++)
	{
		int sy=scaley[j]<<8;
		for (i=0; i<NW; i++)
		{
			int spos=screeny[j]+i;
			int sx=scalex[i];
			bufp[spos]=vid_pre_xlat2[nesimageb[sy+sx]];
		}
	}
	}
	else
	if (scaling==0)
	{
	int mW=W>255?256:W;
    	struct spritedata *nsprites;
    	nsprites = (struct spritedata *)PPU_sprite_ram;
	
	if (nsprites[fsprnum].tile==fsprnumt) { 
	yp=nsprites[fsprnum].vpos-(H>>1);
	yp=yp<0?0:yp;
	yp=yp>240-H?240-H:yp;
	xp=nsprites[fsprnum].xpos-(W>>1);
	xp=xp<0?0:xp;
	xp=xp>256-W?256-W:xp;
	}
	p+=yp<<8;
	p+=xp;
	for (j=0; j<H; j++)
	{
		for (i=0; i<mW; i++)
		{
			
			*bufp=vid_pre_xlat2[nesimageb[p]];
			bufp++;
			p++;
		}
		if (W>buffer_x) bufp+=W-buffer_x;
		
		p+=buffer_x-mW;
	}
	}
	else
	if (nicescale==1)
	{
	for (j=0; j<NH; j++)
	{
		int sy=scaley[j]<<8;
		int dy=scaley[j+1]-sy;
		for (i=0; i<NW; i++)
		{
			int spos=screeny[j]+i;
			int sx=scalex[i];
			int dx,dy;
			dx=scalex[i+1]-sx;
			int dd=(dx>1?1:0) | (dy>1?2:0);
			unsigned char r1,r2,g1,g2,b1,b2;
			unsigned char p1,p2,p3,p4;
			unsigned int xp1,xp2;
			unsigned char nr,ng,nb;
			int p=sy+sx;
			/*switch (dd)
			{
			case 0:*/
			p1=nesimageb[p];
			p2=nesimageb[p+1];
			p3=nesimageb[p+buffer_x];
			p4=nesimageb[p+buffer_x+1];
			/*break;
			case 1:
			p1=nes_imageb[p];
			p2=nes_imageb[p+1];
			p3=nes_imageb[p+buffer_x];
			p4=nes_imageb[p+buffer_x+1];
			break;
			case 2:
			p1=nes_imageb[p];
			p2=nes_imageb[p+1];
			p3=nes_imageb[p+buffer_x];
			p4=nes_imageb[p+(buffer_x<<1)+1];
			break;
			case 3:
			p1=nes_imageb[p];
			p2=nes_imageb[p+2];
			p3=nes_imageb[p+(buffer_x<<1)];
			p4=nes_imageb[p+(buffer_x<<1)+2];
			break;
			
			//bufp[spos]=vid_pre_xlat2[nes_imageb[sy+sx]];
			}*/	
			xp1=(p1<<6)+p3;
			xp2=(p2<<6)+p4;
			r1=vid_pre_xlat2r[xp1];
			r2=vid_pre_xlat2r[xp2];
			g1=vid_pre_xlat2g[xp1];
			g2=vid_pre_xlat2g[xp2];
			b1=vid_pre_xlat2b[xp1];
			b2=vid_pre_xlat2b[xp2];
			nr=(r1+r2);
			ng=(g1+g2);
			nb=(b1+b2);
			bufp[spos]=(nr<<11) | (ng<<5)| nb;
		}
	}
	}
	else
	{
	for (j=0; j<NH; j++)
	{
		int sy=scaley[j]<<8;
		for (i=0; i<NW; i++)
		{
			int spos=screeny[j]+i;
			int sx=scalex[i];
			bufp[spos]=vid_pre_xlat2[nesimageb[sy+sx]];
		}
	}
	}
/*        char sp[200];
        int line=0;
        sprintf(sp,"%x", tframeskip);
        print(screen,0,line++,sp,0);
        sprintf(sp,"%x", avgframe);
        print(screen,0,line++,sp,0);
						 
        sprintf(sp,"s%x", n2);
        print(screen,0,line++,sp,0);
        sprintf(sp,"d%x", n7);
        print(screen,0,line++,sp,0);
        sprintf(sp,"O%x", fstatus1);
        print(screen,0,line++,sp,0);
        sprintf(sp,"Z%x", fstatus2);
        print(screen,0,line++,sp,0);
        sprintf(sp,"V%x", fstatus3);
        print(screen,0,line++,sp,0);
	fstatus1=0;
	fstatus2=0;
	fstatus3=0;
	n2=0;
	n7=0;
	fstatus1=0;
	fstatus2=0;
	fstatus3=0;
	n2=0;
	n7=0;*/
/*	char sp[200];
	DIR* dir = opendir("/bin");
	if (!dir) {
		    sprintf(sp,"PERROR\n");
		    print(screen,0,0,sp);
	}
	int line=0;
	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL) {
		    sprintf(sp,"%s\n", entry->d_name);
		    print(screen,0,line++,sp);
		    if (line>8) break;
	}*/
	
	if (!(system_flags & F_GUI)) HD_LCD_Update (screen,0,0,W,H);
//	}
/*    if ((buffer_x == image_x) && (buffer_y == image_y)) {
	gl_putbox(middle_x, middle_y, buffer_x, buffer_y, nes_image);
    } else {
	gl_putboxpart(middle_x, middle_y, image_x, image_y, buffer_x, buffer_y, nes_image, offset_x, offset_y);
    }*/
}
void printMenu(char *mentry,char *status, int selected)
{
	char sp[200];
	sprintf(sp,"%s %s", mentry,status);
	print(screen,0,mline,sp,selected);
	mline++;
}
int doRomMenu(char *folder)
{
	char sp[200];
	int cfil=0;
	DIR* dir = opendir(folder);
	if (dir) {
	int line=0;
	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name+strlen(entry->d_name)-4,".nes")==0)
		{	
		    if ((line<(H>>3)) && (cfil>=selected))
		    {
		    sprintf(sp,"%s", entry->d_name);
		    print(screen,0,line,sp,cfil==selected);
		    line++;
		    }
		    if (cfil==selected) sprintf(sfilename,"%s/%s",folder,entry->d_name);
		    cfil++;
		}
	}
	totalentries=cfil-1;
	closedir(dir);
	}
	if(!qui) printMenu("HIHI","HIHI",0);
	return cfil;
}
void startMenu() { mline=0; }
char scaletoggle()
{
  
  switch(scaling) {
  case 0:
  scaling=1;
 rescale();
  return "ON";
  break;
  case 1:
  scaling=2;
 rescale();
  return "STRETCH";
  break;
  case 2:
  scaling=0;
 rescale();
  return "OFF";
  break;
  }
}

char ul,ur,ll,lr;
char *keycfgtype[3]={"A","B","A+B"};
void doMenu()
{
	startMenu();
	switch(whereinmenu)
	{
		case MAINMENU:
			totalentries=2;		
			printMenu("CONFIG","",selected==0);
			printMenu("LOAD ROM","",selected==1);
			// All defunct at the moment
			// ~Keripo
			//printMenu("SAVE BATTERY","",selected==2);
			//printMenu("SAVE STATE","",selected==3);
			//printMenu("LOAD","",selected==4);
			printMenu("QUIT","",selected==2);
			break;
		case CFGMENU:
			totalentries=5;		
			printMenu("SCALE",scaling,selected==0);
			printMenu("SMOOTH",nicescale==1?"ON":"OFF",selected==1);
			printMenu("KEYS","",selected==2);	
			char sprnum[3];
			sprintf(sprnum,"%d",fsprnum);
			printMenu("FOLLOW SPRITE",sprnum,selected==3);	
			char volstr[11];
			int i=0;
			for (i=0; i<9; i++)
				if (volume>=i*10) volstr[i]='*';
				else
						volstr[i]=' ';
			volstr[i]=0;
			printMenu("VOLUME",volstr,selected==4);	
    			struct spritedata *nsprites;
    			nsprites = (struct spritedata *)PPU_sprite_ram;
			i=fsprnum;
//			for (i=0; i<64; i++)
			{
				sprintf(sprnum,"%d",i);
				if (scaling==0)
				{
				putChar(screen,(nsprites[i].xpos-xp)>>3,(nsprites[i].vpos-yp)>>3,sprnum[0],0);
				if (sprnum[1]) putChar(screen,1+(nsprites[i].xpos-xp)>>3,(nsprites[i].vpos-yp)>>3,sprnum[1],0);
				}
				else
				{
				putChar(screen,(nsprites[i].xpos*W/buffer_x)>>3,(nsprites[i].vpos*H/buffer_y)>>3,sprnum[0],0);
				if (sprnum[1]) putChar(screen,1+(nsprites[i].xpos*W/buffer_x)>>3,(nsprites[i].vpos*H/buffer_y)>>3,sprnum[1],0);
				}
			}
			printMenu("SAVE","",selected==5);
			break;
		case ROMSMENU: 
			if (doRomMenu("Roms")==0)
				doRomMenu("/opt/Emulators/iDarcNES/Roms");
			break;
		case KEYSMENU:
			totalentries=12;		
			printMenu("MENU",menuc,selected==0);
			printMenu("ACTION",selc,selected==1);
			printMenu("PLAY",playc,selected==2);
			printMenu("FORWARD",fwdc,selected==3);
			printMenu("REVERSE",revc,selected==4);
			printMenu("kB0",kbc[0],selected==5);
			printMenu("KB1",kbc[1],selected==6);
			printMenu("KB2",kbc[2],selected==7);
			printMenu("KB3",kbc[3],selected==8);
			printMenu("KB4",kbc[4],selected==9);
			printMenu("KB5",kbc[5],selected==10);
			printMenu("KB6",kbc[6],selected==11);
			printMenu("KB7",kbc[7],selected==12);
			break;

	}
	//printMenu("WIDTH:",NW,0);
	//printMenu("HEIGHT:",NH,0);
}

void video_display_buffer() 
{
    struct spritedata *nsprites;
    nsprites = (struct spritedata *)PPU_sprite_ram;
    if (/*nsprites[fsprnum].tile!=fsprnumt && */!(system_flags & F_GUI))
    {
	int i;
	for (i=0; i<64; i++)
	{
		if (nsprites[i].tile==fsprnumt) { fsprnum=i; }
	}
    }
    if (!(system_flags & F_GUI))
    {
    if ((inl(COP_STATUS) & COP_LINE_REQ) == 0)
    {
//op_set(video_display_buffer_);
	char *timage=nes_image;
	nes_image=nes_imageb;
	nes_imageb=timage;
	outl(nes_imageb,0x40015030);
	outl(inl(COP_STATUS) | COP_LINE_REQ, COP_STATUS); 
    }
    }
    else
    {
	video_display_buffer_();
	doMenu();	
	HD_LCD_Update (screen,0,0,W,H);
    }
    video_events();
//    if (menumode) system_flags |= F_GUI; else system_flags &=~F_GUI;
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
 
	    if (joy_start||tjoy_start) {
		ui_joypad->data |= ui_joypad->button_template->buttons[7];
	    } else  {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[7]);
	    }
//	case 7:
	    if (joy_select||tjoy_select) {
		ui_joypad->data |= ui_joypad->button_template->buttons[6];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[6]);
	    }
//	case 6:
    if (joy_b||tjoy_b) {
		ui_joypad->data |= ui_joypad->button_template->buttons[5];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[5]);
	    }
//	case 5:
	    if (joy_a||tjoy_a) {
		ui_joypad->data |= ui_joypad->button_template->buttons[4];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[4]);
	    }
//	case 4:
	    if (joy_right||tjoy_right) {
		ui_joypad->data |= ui_joypad->button_template->buttons[3];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[3]);
	    }
//	case 3:
	    if (joy_left||tjoy_left) {
		ui_joypad->data |= ui_joypad->button_template->buttons[2];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[2]);
	    }
//	case 2:
	    if (joy_down||tjoy_down) {
		ui_joypad->data |= ui_joypad->button_template->buttons[1];
	    } else {
		ui_joypad->data &= ~(ui_joypad->button_template->buttons[1]);
	    }
//	case 1:
	    if (joy_up||tjoy_up) {
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
    int i,j;
    if (!vid_pre_xlat)
    {

    vid_pre_xlat = (unsigned char*) malloc(512);
    memset(vid_pre_xlat,0,512);
//  vid_pre_xlat2 = (int*) calloc(1,512*4);
/*    vid_pre_xlat2r = (int*) calloc(4,64*64);
    memset(vid_pre_xlat2r,0,64*64*4);
    vid_pre_xlat2g = (int*) calloc(4,64*64);
    memset(vid_pre_xlat2g,0,64*64*4);
    vid_pre_xlat2b = (int*) calloc(4,64*64);
    memset(vid_pre_xlat2b,0,64*64*4);*/
    
    for (i = 0; i < colors; i++) {
	vid_pre_xlat[i] =  i+64; //SDL_MapRGB(screen->format, red[i], green[i], blue[i]);
	vid_pre_xlat2[i+64] = ((red[i]>>3)<<11) + ((green[i]>>2)<<5)+ ((blue[i]>>3));
   }	 
    for (j=0; j<colors; j++)
    {
	   for (i=0; i<colors; i++)
	   {
		   vid_pre_xlat2r[((j+64)<<6)+i+64]=((red[i]+red[j])>>5);
		   vid_pre_xlat2g[((j+64)<<6)+i+64]=((green[i]+green[j])>>4);
		   vid_pre_xlat2b[((j+64)<<6)+i+64]=((blue[i]+blue[j])>>5);
	   }
    }
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
