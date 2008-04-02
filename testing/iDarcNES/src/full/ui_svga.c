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
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

#include "ui.h"
#include "system.h"
#include "video.h"
#include "tool.h"
#include "menu.h"

#define TEMP_FILENAME "/tmp/darc.temp"

char *filename;

void run_system(void);
shutdown_t dn_shutdown;

#define MAX_SVGA_MODE 3
struct {
    int modenumber;
    char name[12];
} modelist[MAX_SVGA_MODE] = {
    {G640x480x256,  "640x480x8"},
#ifdef G320x240x256V
    {G320x240x256V, "320x240x8"},  /* requires v1.4.0 of SVGALib */
#else
    {50,            "320x240x8"},
#endif
    {G320x200x256,  "320x200x8"},
};

extern int svga_mode;

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
    int x;

    printf("\nVideo modes:\n");
    for(x=0; x<MAX_SVGA_MODE; x++) {
	printf("    %d - %s %s\n", x, modelist[x].name,
	       vga_hasmode(modelist[x].modenumber)?"":"(not available)");
    }
    printf("\n");
    printf("Some SVGA chipsets may not be automatically detected by\n"
	   "SVGALib. If this is the case, and you haven't set up the\n"
	   "configuration file (usually /etc/vga/libvga.config), then\n"
	   "you may be limited in the selection of display modes\n"
	   "available.\n");
    printf("\n");
    printf("Some video modes (or SVGA chipsets) may require a newer\n"
	   "version of SVGALib, such as v1.4.0, which is available from\n"
	   "http://www.cs.bgu.ac.il/~zivav/svgalib/\n");
}

void video_shutdown(void)
{
    vga_setmode(TEXT);
    keyboard_close();
}

void video_init()
{
    if (!vga_hasmode(svga_mode)) {
	printf("ui_svga: video mode not available during init. this shouldn't happen.\n");
        exit(-1);
    }
    vga_setmode(svga_mode);
    gl_setcontextvga(svga_mode);
    
/*     middle_x = (vga_getxdim() / 2) - (BUFFER_X/2); */
/*     middle_y = (vga_getydim() / 2) - (BUFFER_Y/2); */
    
    keyboard_init();
    keyboard_translatekeys(TRANSLATE_CURSORKEYS|TRANSLATE_KEYPADENTER|
                           TRANSLATE_DIAGONAL);

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

void checkargs(int argc, char *argv[])
{
    extern int nes_psg_quality; /* FIXME: cheap hack, copied from nes_psg.h */
    int x;

    nes_psg_quality = 2;
    
    if (argc == 1) {
	printusage(argv);
	exit(0x42);
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
	    case 'm':
		x++;
		if (x > argc) {
		    printf("Bad argument\n");
		    exit(-1);
		}
		svga_mode = atoi(argv[x]);
		if ((svga_mode < 0) || (svga_mode >= MAX_SVGA_MODE)) {
		    printf("Video mode undefined. Try \"%s -h\" for a list of available modes\n", argv[0]);
		    exit(-1);
                }
		if (!vga_hasmode(modelist[svga_mode].modenumber)) {
		    printf("This video mode doesn't appear to be supported on your current configuration.\n"
			   "Try \"%s -h\" for a list of available modes\n", argv[0]);
		    exit(-1);
		}
		svga_mode = modelist[svga_mode].modenumber;
		break;
	    case 'h': /* Help */
		printusage(argv);
		listmodes();
		exit(0);
		break; /* etc... */
	    }
	}
    }
    if (filename == NULL) {
	printusage(argv);
	exit(0x42);
    }
}

int main(int argc, char *argv[])
{
    rom_file romfile;
    int system_type;
    
    vga_disabledriverreport();
    vga_init();

    checkargs(argc, argv);
    
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
	system_type = guess_system(romfile);
	video_init();
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
#if 1
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
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
