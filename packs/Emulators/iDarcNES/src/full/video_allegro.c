/*
 * video_allegro.c
 *
 * display screen management
 */

/* $Id: video_allegro.c,v 1.11 2000/05/31 01:26:16 nyef Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include "video.h"
#include "tool.h"

int video_active = 0;
int deblevel = 0;

BITMAP *nes_image;

typedef void ((*xlatfunc)(BITMAP *, char *));

unsigned char *vbpfunc8(int line);

unsigned char *vid_pre_xlat;
void init_xlate8(int colors, int *red, int *green, int *blue);

void video_init()
{
    allegro_init();

    set_gfx_mode(GFX_VESA1, 640, 480, 640, 480);
    nes_image = create_bitmap(256, 240);

    install_keyboard();
}

void video_shutdown(void)
{
}

void video_enter_deb(void)
{
    if (video_active && (!deblevel)) {

    }
    deblevel++;
}

void video_leave_deb(void)
{
    deblevel--;
    if (video_active && (!deblevel)) {

    }
}

void video_setsize(int x, int y)
{
    /* FIXME: add real implimentation */
}

void video_display_buffer()
{
    blit(nes_image, screen, 0, 0, 192, 120, 256, 240);
}

unsigned char *video_get_vbp(int line)
{
    return nes_image->line[line];
}

void video_setpal(int colors, int *red, int *green, int *blue)
{
    int i;
    RGB color;
    
    vid_pre_xlat = malloc(colors);
    
    for (i = 0; i < 64; i++) {
	color.r = red[i] >> 2;
	color.g = green[i] >> 2;
	color.b = blue[i] >> 2;
	set_color(i + 64, &color);
	vid_pre_xlat[i] = i + 64;
    }
}

/*
 * $Log: video_allegro.c,v $
 * Revision 1.11  2000/05/31 01:26:16  nyef
 * fixed (hopefully) to work with the new video interface
 *
 * Revision 1.10  1999/02/09 02:28:30  nyef
 * added dummy implimentation of video_setsize()
 *
 * Revision 1.9  1999/02/09 02:10:43  nyef
 * moved video_events() out to ui_allegro.c
 *
 * Revision 1.8  1999/02/09 02:08:49  nyef
 * removed the (dead) vidxlate stuff and the #ifdef GRAPHICS stuff
 *
 * Revision 1.7  1999/01/08 02:35:16  nyef
 * commented out include of nespal.c, which was causing problems.
 *
 * Revision 1.6  1999/01/07 03:20:21  nyef
 * added in hacked up palette interface.
 *
 * Revision 1.5  1998/12/18 04:19:01  nyef
 * changed video driver from autodetect to vesa1.
 *
 * Revision 1.4  1998/10/02 03:29:54  nyef
 * fixed a few small problems that were preventing compilation.
 * nothing major, just stupid things.
 *
 * Revision 1.3  1998/09/02 01:45:01  nyef
 * updated to cover the new buffering scheme and ppu interface.
 *
 * Revision 1.2  1998/07/31 10:46:31  nyef
 * moved includes around a bit to squash EOF redefinition.
 *
 * Revision 1.1  1998/07/31 01:59:21  nyef
 * Initial revision
 *
 */
