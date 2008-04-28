/*
 * ui_svga.c
 *
 * user interface for SVGALib
 */

/* $Id: ui_svga.c,v 1.15 2000/09/16 23:45:24 nyef Exp $ */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ui.h"
#include "system.h"
#include "video.h"
#include "tool.h"
#include "menu.h"
#include "../hotdog/hotdog.h"
#include "tiledraw.h"
#define TEMP_FILENAME "/tmp/darc.temp"

char *filename;

void run_system(void);
shutdown_t dn_shutdown;

short *screen=NULL;

void init_xlate8(int colors, int *red, int *green, int *blue);
unsigned char *vbpfunc8(int line);

int fexists(char *fn)
{
    FILE *F = fopen(fn,"rb");
    
    if (F==NULL) return 0;
    fclose(F);
    return 1;
}

void printusage(char *argv[])
{
    printf("usage: %s <romfile> [-m #] [-h] [--nosound]\n", argv[0]);
}

void listmodes()
{
}

void video_shutdown(void)
{
//    vga_setmode(TEXT);
  //  keyboard_close();
}

void video_init()
{
//    screen = SDL_SetVideoMode (176, 132, 16,SDL_SWSURFACE);
  HD_LCD_Init();
  int hw_ver;
  int lcd_width;
  int lcd_height;
  int lcd_type;
  HD_LCD_GetInfo (&hw_ver, &lcd_width, &lcd_height, &lcd_type);
		
/*     middle_x = (vga_getxdim() / 2) - (BUFFER_X/2); */
/*     middle_y = (vga_getydim() / 2) - (BUFFER_Y/2); */
    
//    keyboard_init();
//    keyboard_translatekeys(TRANSLATE_CURSORKEYS|TRANSLATE_KEYPADENTER|
  //                         TRANSLATE_DIAGONAL);

#if 0
    if (use_joystick) {
	if (joystick_init(0,NULL) < 0) {
	    fprintf(stderr, "nojoy\n");
	}
    }
#endif

#if 0
    nes_image = (unsigned char *)malloc(BUFFER_SIZE);
    if (nes_image == NULL) {
	video_shutdown();
	printf("Error allocating %d bytes for a video buffer...\n", BUFFER_SIZE);
	exit(-1);
    }
#endif
    atexit(video_shutdown);
}
int nomenu=0;
void checkargs(int argc, char *argv[])
{
    extern int nes_psg_quality; /* FIXME: cheap hack, copied from nes_psg.h */
    int x;

    nes_psg_quality = 2;
    
    if (argc == 1) {
	//printusage(argv);
	//exit(0x42);
    }
    
    for (x=1; x<argc; x++) {
	/* Check if it's a filename */
	if (argv[x][0] != '-') {
	    if (filename == NULL) { /* Do we already have a filename? */
		if (!fexists(argv[x])) {
		    fprintf(stderr, "%s not found\n", argv[x]);
		    exit(-1);
		} else {
		    filename = strdup(argv[x]); /* Yay! */
		}
	    }
	} else if (!strcasecmp(argv[x], "--nosound")) {
	    nes_psg_quality = 0;
	} else { /* It's a switch! ----------------------- */
            switch(argv[x][1]) {
	    case 'h': /* Help */
		printusage(argv);
		listmodes();
		exit(0);
		break; /* etc... */
	    }
	}
    }
    if (filename == NULL) {
 // system_flags |= F_GUI;
	printusage(argv);
	//nomenu=1;
	exit(0x42);
	
    }
}
rom_file romfile;
extern unsigned int joy_up, joy_down, joy_left, joy_right,
    joy_a, joy_b, joy_start, joy_select;
    extern unsigned int tjoy_up, tjoy_down, tjoy_left, tjoy_right,
    tjoy_a, tjoy_b, tjoy_start, tjoy_select;
void setc(char z,unsigned int *but);
void setc(char z,unsigned int *but) {
 switch(z) {
    case 'A':
    *but=&joy_a;
    break;
    case 'B':
    *but=&joy_b;
    break;
    case 'U':
    *but=&joy_up;
    break;
    case 'D':
    *but=&joy_down;
    break;
    case 'L':
    *but=&joy_left;
    break;
    case 'R':
    *but=&joy_right;
    break;
    case 'S':
    *but=&joy_start;
    break;
    case 'E':
    *but=&joy_select;
    break;
    }
    }
void setct(char z,unsigned int *but);
void setct(char z,unsigned int *but) {
 switch(z) {
    case 'A':
    *but=&tjoy_a;
    break;
    case 'B':
    *but=&tjoy_b;
    break;
    case 'U':
    *but=&tjoy_up;
    break;
    case 'D':
    *but=&tjoy_down;
    break;
    case 'L':
    *but=&tjoy_left;
    break;
    case 'R':
    *but=&tjoy_right;
    break;
    case 'S':
    *but=&tjoy_start;
    break;
    case 'E':
    *but=&tjoy_select;
    break;
    }
    }

unsigned int *kb[8];
int qui=1;
unsigned char kbc[8];
unsigned int *menu, *sel, *play,
    *rev, *fwd;
    unsigned char menuc, selc, playc,
    revc, fwdc;
    extern int scaling;
extern void set_default(void);
int main(int argc, char *argv[])
{
    int system_type;
    _tiledraw_setup(); 
    checkargs(argc, argv);
       kb[0]=0;
    kb[1]=0;
    kb[2]=0;
    kb[3]=0;
    kb[4]=0;
    kb[5]=0;
    kb[6]=0;
    kb[7]=0;
    menu=0;
    sel=0;
    play=0;
    fwd=0;
    rev=0;
        scaling=2;
    *play=&joy_b;
    *sel=&joy_a;
    *menu=&joy_b;
    *fwd=&joy_b;
    *rev=&joy_b;
    *kb[0]=&tjoy_up;
    *kb[1]=&tjoy_a;
    *kb[2]=&tjoy_right;
    *kb[3]=&tjoy_start;
    *kb[4]=&tjoy_down;
    *kb[5]=&tjoy_select;
    *kb[6]=&tjoy_left;
    *kb[7]=&tjoy_b;

   if(!fexists("/opt/Emulators/iDarcNES/Conf/iDarcNES.cfg")) {

    FILE *fp;
 


 fp=fopen("/opt/Emulators/iDarcNES/Conf/iDarcNES.cfg","w");
 fclose(fp);
 fp=fopen("/opt/Emulators/iDarcNES/Conf/iDarcNES.cfg","w");
    fprintf(fp,"kb0=U\n");
    fprintf(fp,"kb1=A\n");
    fprintf(fp,"kb2=R\n");
    fprintf(fp,"kb3=S\n");
    fprintf(fp,"kb4=D\n");
    fprintf(fp,"kb5=E\n");
    fprintf(fp,"kb6=L\n");
    fprintf(fp,"kb7=B\n");
    fprintf(fp,"select=A\n");
    fprintf(fp,"menu=B\n");
    fprintf(fp,"play=S\n");
    fprintf(fp,"fwd=B\n");
    fprintf(fp,"rev=B\n");
    fprintf(fp,"screen=2\n");
        scaling=2;
    *play=&joy_b;
    *sel=&joy_a;
    *menu=&joy_b;
    *fwd=&joy_b;
    *rev=&joy_b;
    *kb[0]=&tjoy_up;
    *kb[1]=&tjoy_a;
    *kb[2]=&tjoy_right;
    *kb[3]=&tjoy_start;
    *kb[4]=&tjoy_down;
    *kb[5]=&tjoy_select;
    *kb[6]=&tjoy_left;
    *kb[7]=&tjoy_b;
    
    fclose(fp);

            scaling=2;
    *play=&joy_b;
    *sel=&joy_a;
    *menu=&joy_b;
    *fwd=&joy_b;
    *rev=&joy_b;
    *kb[0]=&tjoy_up;
    *kb[1]=&tjoy_a;
    *kb[2]=&tjoy_right;
    *kb[3]=&tjoy_start;
    *kb[4]=&tjoy_down;
    *kb[5]=&tjoy_select;
    *kb[6]=&tjoy_left;
    *kb[7]=&tjoy_b;
    
    }
    FILE *f=fopen("/opt/Emulators/iDarcNES/Conf/iDarcNES.cfg","r");
 
  /*  kb[0]=0;
    kb[1]=0;
    kb[2]=0;
    kb[3]=0;
    kb[4]=0;
    kb[5]=0;
    kb[6]=0;
    kb[70]=0;
    menu=0;
    sel=0;
    play=0;
    fwd=0;
    rev=0;*/
   
    fscanf(f,"kb0=%c\n",&kbc[0]);
   setct(kbc[0],&kb[0]);
 //  kbc[0]=z;
    
    fscanf(f,"kb1=%c\n",&kbc[1]);
   setct(kbc[1],&kb[1]);
   //kbc[1]=z;
    
       fscanf(f,"kb2=%c\n",&kbc[2]);
   setct(kbc[2],&kb[2]);
   //kbc[2]=z;
       
       fscanf(f,"kb3=%c\n",&kbc[3]);
   setct(kbc[3],&kb[3]);
   //kbc[3]=z;
       
       fscanf(f,"kb4=%c\n",&kbc[4]);
   setct(kbc[4],&kb[4]);
   //kbc[4]=z;
       
       fscanf(f,"kb5=%c\n",&kbc[5]);
   setct(kbc[5],&kb[5]);
   //kbc[5]=z;
       
       fscanf(f,"kb6=%c\n",&kbc[6]);
   setct(kbc[6],&kb[6]);
   //kbc[6]=z;
       
       fscanf(f,"kb7=%c\n",&kbc[7]);
   setct(kbc[7],&kb[7]);
   //kbc[7]=z;
       
       fscanf(f,"select=%c\n",&selc);
   setc(selc,&sel);
   //selc=z;
       
       fscanf(f,"menu=%c\n",&menuc);
   setc(menuc,&menu);
       
       fscanf(f,"play=%c\n",&playc);
   setc(playc,&play);
       
       fscanf(f,"fwd=%c\n",&fwdc);
   setc(fwdc,&fwd);
 //  fwdc=z;
       
       fscanf(f,"rev=%c\n",&revc);
   setc(revc,&rev);
  // revc=z;
       
       fscanf(f,"screen=%i\n",&scaling);
   fclose(f);
  
     
    video_init();
    while (qui==1)
    {
    
      if (strstr(filename, ".gz\0")) { /* If file looks gzip compressed... */
	char temp[256];    /* Lets unpack it into the /tmp dir... */

	
	
	
	sprintf(temp, "gunzip -c %s >"TEMP_FILENAME, filename);
	if (system(temp)) {
	    printf("Error gunzipping compressed rom.\n");
	    exit(-1);
        }
	romfile = read_romimage(TEMP_FILENAME);
	unlink(TEMP_FILENAME);
    } else {             /* ...otherwise, business as usual. */
	romfile = read_romimage(filename);
    }
    free(filename);

    if (romfile) {
	system_flags &= ~F_QUIT;
	}
	system_type = guess_system(romfile);
	activate_system(system_type, romfile);
	run_system();
    
    }

    return 0;
   
    
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


/* debug console handling */
void deb_printf(const char *fmt, ...)
{
/*
#if 1
    va_list args;
    FILE *f=fopen("debug.txt","a");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fclose(f);
#endif
*/
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
	timeslice(timeslice_data);
	while (system_flags & F_GUI) {
		video_display_buffer();
	}
    }
    
    if (dn_shutdown) {
	dn_shutdown();
    }
}

/*
 * $Log: ui_svga.c,v $
 * Revision 1.15  2000/09/16 23:45:24  nyef
 * moved video_shutdown in from video_svga.c
 *
 * Revision 1.14  2000/08/22 02:06:51  nyef
 * added dummy implementation of menu_file_open_box()
 *
 * Revision 1.13  2000/07/09 18:33:56  nyef
 * fixed a botched include statement
 *
 * Revision 1.12  2000/07/04 23:19:37  nyef
 * fixed compile problems with the dummy menu implementation
 *
 * Revision 1.11  2000/07/02 02:43:31  nyef
 * added dummy implementation of the per-driver menu interface
 *
 * Revision 1.10  2000/06/09 00:02:42  nyef
 * added --nosound option
 *
 * Revision 1.9  2000/05/31 01:25:06  nyef
 * fixed (hopefully) to work with the new video interface
 *
 * Revision 1.8  1999/11/26 20:08:15  nyef
 * moved sound quality control in from the game system layer
 *
 * Revision 1.7  1999/11/20 16:08:22  nyef
 * fixed to use new romload interface
 *
 * Revision 1.6  1999/07/10 02:58:58  nyef
 * enabled output of debug and status messages
 *
 * Revision 1.5  1999/07/05 01:42:59  nyef
 * added a 320x200x256 mode
 *
 * Revision 1.4  1999/07/02 03:39:16  nyef
 * moved the svgalib initialization to earlier in the startup process
 *
 * Revision 1.3  1999/07/01 02:20:28  nyef
 * moved video_init() in from video_svga.c
 * added some more warning and diagnostic messages
 *
 * Revision 1.2  1999/04/17 20:12:04  nyef
 * changed shutdown() to dn_shutdown().
 *
 * Revision 1.1  1999/01/18 22:08:40  nyef
 * Initial revision
 *
 */
