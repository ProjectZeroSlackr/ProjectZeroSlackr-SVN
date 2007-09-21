/********************************************************************************************
*
* plasma.c
*
* Copyright (C) ???? Christian ???????, original plasma for Rockbox (http://www.rockbox.org)
* Copyright (C) 2006 Felix Bruns, ported to iPodlinux/podzilla2/ttk
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "pz.h"

static TWindow *window;
static TWidget *widget;

static unsigned short *framebuffer;

static unsigned char wave_array[256];  /* Pre calculated wave array */

static short colours[256]; /* Smooth transition of shades */
static int redfactor = 1, greenfactor = 2, bluefactor = 3;
static int redphase = 0, greenphase = 50, bluephase = 100;

static unsigned char sp1, sp2, sp3, sp4; /* Speed of plasma */
static int plasma_frequency;
static int plasma_wav_amp;

static unsigned char p1, p2, p3, p4, t1, t2, t3, t4, z;
static int timep = 0;
static int t_disp = 0;

struct timeval tv;
static int show_fps = 0, start_fps, frame = 0;

#define MSG_DISP_TIME 30

/* Precalculated sine * 16384 (fixed point 18.14) */
static const short sin_table[91] =
{
    0,   285,   571,   857,  1142,  1427,  1712,  1996,  2280,  2563,
    2845,  3126,  3406,  3685,  3963,  4240,  4516,  4790,  5062,  5334,
    5603,  5871,  6137,  6401,  6663,  6924,  7182,  7438,  7691,  7943,
    8191,  8438,  8682,  8923,  9161,  9397,  9630,  9860, 10086, 10310,
    10531, 10748, 10963, 11173, 11381, 11585, 11785, 11982, 12175, 12365,
    12550, 12732, 12910, 13084, 13254, 13420, 13582, 13740, 13894, 14043,
    14188, 14329, 14466, 14598, 14725, 14848, 14967, 15081, 15190, 15295,
    15395, 15491, 15582, 15668, 15749, 15825, 15897, 15964, 16025, 16082,
    16135, 16182, 16224, 16261, 16294, 16321, 16344, 16361, 16374, 16381,
    16384
};

static short plasma_sin(int val)
{
    /* value should be between 0 and 360 degree for correct lookup*/
    val%=360;
    if(val<0)
        val+=360;

    /* Speed improvement through successive lookup */
    if (val < 181)
    {
        if (val < 91)
        {
            /* phase 0-90 degree */
            return (short)sin_table[val];
        }
        else
        {
            /* phase 91-180 degree */
            return (short)sin_table[180-val];
        }
    }
    else
    {
        if (val < 271)
        {
            /* phase 181-270 degree */
            return -(short)sin_table[val-180];
        }
        else
        {
            /* phase 270-359 degree */
            return -(short)sin_table[360-val];
        }
    }
    return 0;
}

/*
 * Main wave function so we don't have to re-calc the sine 
 * curve every time. Mess around amp and freq to make slighlty
 * weirder looking plasmas!
 */
static void wave_table_generate(void)
{
    int i;
    for (i=0;i<256;++i)
    {
        wave_array[i] = (unsigned char)((plasma_wav_amp
                      * (plasma_sin((i * 360 * plasma_frequency) / 256))) / 16384);
    }
}

/* Make a smooth colour cycle. */
void shades_generate(int timep)
{
    int i;
    unsigned red, green, blue;
    unsigned r = timep * redfactor + redphase;
    unsigned g = timep * greenfactor + greenphase;
    unsigned b = timep * bluefactor + bluephase;

    for(i=0; i < 256; ++i)
    {
        r &= 0xFF; g &= 0xFF; b &= 0xFF;

        red = 2 * r;
        if (red > 255)
            red = 510 - red;
        green = 2 * g;
        if (green > 255)
            green = 510 - green;
        blue = 2 * b;
        if (blue > 255)
            blue= 510 - blue;

        colours[i] = ttk_makecol(red, green, blue);

        r++; g++; b++;
    }
}

static void draw_plasma (TWidget *this, ttk_surface surface)
{
    int x, y;

    shades_generate(timep++); /* dynamically */
    t1 = p1;
    t2 = p2;
    for(y = 0; y < ttk_screen->h; y++)
    {
        t3 = p3;
        t4 = p4;
        for(x = 0; x < ttk_screen->w; x++)
        {
            z = wave_array[t1] + wave_array[t2] + wave_array[t3] + wave_array[t4];
            framebuffer[y*ttk_screen->w + x] = colours[z];
            t3 += 1;
            t4 += 2;
        }
        t1 += 2;
        t2 += 1;
    }

    p1 += sp1;
    p2 -= sp2;
    p3 += sp3;
    p4 -= sp4;

    //memcpy (ttk_screen->srf->pixels, framebuffer, ttk_screen->w * ttk_screen->h * ttk_screen->srf->format->BytesPerPixel);
    memcpy (surface->pixels, framebuffer, ttk_screen->w * ttk_screen->h * ttk_screen->srf->format->BytesPerPixel);

    char str_buffer[40];
    if (t_disp > 0)
    {
        t_disp--;
        snprintf(str_buffer, sizeof(str_buffer), "Wave-Amp:%d \nFrequency:%d", plasma_wav_amp, plasma_frequency);
        ttk_text (surface, ttk_textfont, 4, ttk_screen->h - ttk_text_height(ttk_textfont) - 3, ttk_makecol(BLACK), str_buffer);
        ttk_text (surface, ttk_textfont, 3, ttk_screen->h - ttk_text_height(ttk_textfont) - 4, ttk_makecol(WHITE), str_buffer);
    }

    if(frame == 0)
    {
        gettimeofday(&tv, NULL);
        start_fps = tv.tv_sec;
    }

    if (show_fps > 0)
    {
        gettimeofday(&tv, NULL);
        snprintf(str_buffer, sizeof(str_buffer), "FPS: %d", frame/(tv.tv_sec-start_fps+1));
        ttk_text (surface, ttk_menufont, 3, 3, ttk_makecol(WHITE), str_buffer);
    }
    frame++;
}

static int down_plasma (TWidget *this, int button) 
{
    switch (button) {
    case TTK_BUTTON_HOLD:
        //ttk_window_hide_header (window);
        ttk_window_show_header (window);
        return 0;
        break;
    }
    return TTK_EV_UNUSED;
}

static int button_plasma (TWidget *this, int button, int time) 
{
    switch (button) {
    case TTK_BUTTON_HOLD:
        //ttk_window_show_header (window);
        ttk_window_hide_header (window);
        return 0;
        break;

    case TTK_BUTTON_PREVIOUS:
        plasma_wav_amp -= 10;
        wave_table_generate();
        t_disp=MSG_DISP_TIME;
        this->dirty++;
        return 0;
        break;

    case TTK_BUTTON_NEXT:
        plasma_wav_amp += 10;
        wave_table_generate();
        t_disp=MSG_DISP_TIME;
        this->dirty++;
        return 0;
        break;

    case TTK_BUTTON_MENU:
        frame = 0;
        pz_close_window (window);
        return 0;
        break;

    case TTK_BUTTON_PLAY:
        if(show_fps > 0)
            show_fps--;
        else
            show_fps++;
        this->dirty++;
        return 0;
        break;

    case TTK_BUTTON_ACTION:
        redfactor   = rand()%4;
        greenfactor = rand()%4;
        bluefactor  = rand()%4;
        redphase    = rand()%256;
        greenphase  = rand()%256;
        bluephase   = rand()%256;
        this->dirty++;
        return 0;
        break;
    }
    return TTK_EV_UNUSED;
}

static int scroll_plasma (TWidget *this, int dir) 
{
#ifdef IPOD
    TTK_SCROLLMOD(dir, 3);
#endif
    if(dir > 0) {
        plasma_frequency++;
        wave_table_generate();
        t_disp=MSG_DISP_TIME;
    } else {
        plasma_frequency--;
        wave_table_generate();
        t_disp=MSG_DISP_TIME;
    }
    this->dirty++;
    return 0;
}

static int timer_plasma (TWidget *this) 
{
    this->dirty++;
    return 0;
}



TWindow *new_plasma_window() 
{
    window = ttk_new_window ();
    window->title="Plasma";
widget = ttk_new_widget (0, 0);
    widget->w = ttk_screen->w - ttk_screen->wx;
    widget->h = ttk_screen->h - ttk_screen->wy;
    widget->focusable = 1;
    widget->dirty = 1;
    widget->draw    =    draw_plasma;
    widget->button  =  button_plasma;
    widget->down    =    down_plasma;
    widget->scroll  =  scroll_plasma;
    widget->timer   =   timer_plasma;

    ttk_widget_set_timer(widget, 10);

    plasma_frequency = 1;
    plasma_wav_amp   = 90;
    framebuffer = malloc (ttk_screen->w * ttk_screen->h * ttk_screen->srf->format->BytesPerPixel);

    /*Generate the neccesary pre calced stuff*/
    wave_table_generate();

    sp1 = 4;
    sp2 = 2;
    sp3 = 4;
    sp4 = 2;
    p1 = p2 = p3 = p4 = 0;
    ttk_window_hide_header (window);

    ttk_add_widget (window, widget);

    ttk_show_window (window);
}


