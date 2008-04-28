/*
 *
 * capture.c
 *
 * Copyright (C) 2006 Felix Bruns <felixbruns@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "pz.h"


TWindow  *window;
TWidget  *slider;
TWidget  *number;
TWidget  *capture;

static int timeout_value, timeout_start, saved = 0;
struct timeval tv;
static char buffer[25];

static const char* format_options[] = { "PPM", "BMP", NULL };

static int write_word(FILE *fp, unsigned short w){
    putc(w, fp);
    return (putc(w >> 8, fp));
}

static int write_dword(FILE *fp, unsigned int dw){
    putc(dw, fp);
    putc(dw >> 8, fp);
    putc(dw >> 16, fp);
    return (putc(dw >> 24, fp));
}

static int write_long(FILE *fp, int  l){
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
}

static void number_draw (TWidget *this, ttk_surface srf)
{
    ttk_fillrect(srf, 0, 0, ttk_screen->w * 3 / 5, 15, ttk_ap_getx("window.bg")->color);
    sprintf(buffer, "Timeout: %d seconds", timeout_value);
    ttk_text (srf, ttk_textfont, 2, 0, ttk_ap_getx("window.fg")->color, buffer);
}

static void slider_set_setting (int setting, int value) 
{
    timeout_value = value; //set the global timeout_value variable to the current value
    number->dirty++; //redraw the display of the current value
    
}

static TWindow *new_settings_slider_window (char *title, int setting, int slider_min, int slider_max)
{
  
    return window;
}

static TWindow *capture_set_timeout()
{
    return new_settings_slider_window (_("Set Timeout"), 1, 5, 30);
}

static TWindow *new_capture_start_window (char *title)
{
    TWindow *win = ttk_new_window(); //create a new window

    TWidget *cap = ttk_new_widget (0, 0); //create a new widget to draw the current value of the slider
    ttk_widget_set_timer(cap, 1);
    ttk_add_widget (win, cap); //add the widget to the window

    ttk_window_set_title (win, title); //set the window title
    win->data = 0x12345678; //set this flag to free and recreate the window everytime
    ttk_set_popup (win); //popup the window

    gettimeofday(&tv, NULL);
    timeout_start = tv.tv_sec;

    return TTK_MENU_UPALL;
}

TWindow *capture_start()
{
    saved = 0;
    return new_capture_start_window (_("Screen Capture"));
}

static int capture_save(char *filename) //filename without extension
{
    FILE * fp;
    int w, h, i, j, k, r, g, b;
    ttk_color c;

    
        sprintf(buffer, "%s.%s", filename, "bmp");
        fp = fopen(buffer, "wb");
        if (!fp) {
            //pz_error(_("Could not save screenshot."));
            return 1;
        }
        ttk_surface_get_dimen(ttk_screen->srf, &w, &h);

        //Bitmap File Header
        putc(66, fp);
        putc(77, fp);
        write_dword(fp, 54+(w*h*24)/8);
        write_dword(fp, 0);
        write_dword(fp, 54);

        //Bitmap Info Header
        write_dword(fp, 40);
        write_long(fp, w);
        write_long(fp, -h);
        write_word(fp, 1);
        write_word(fp, 24);
        write_dword(fp, 0);
        write_dword(fp, 0);
        write_long(fp, 0);
        write_long(fp, 0);
        write_dword(fp, 0);
        write_dword(fp, 0);

        //Bitmap Data
        for(i=0; i<h; i++){
            for(j=0; j<w; j++){
                c = ttk_getpixel(ttk_screen->srf, j, i);
                ttk_unmakecol(c, &r, &g, &b);
                putc(b, fp);
                putc(g, fp);
                putc(r, fp);
            }
            if(w % 4 != 0){
                for(k=0; k<w%4; k++){
                    putc(0, fp);
                }
            }
        }
    

    if (fclose(fp) == 0) {
        saved = 1;
       new_message_window(_("Screen captured!!!"));
    }

    return 0;
}

static int capture_timer(TWidget *this)
{
    gettimeofday(&tv, NULL);
    int diff = tv.tv_sec - timeout_start;
    //printf("%d == %d\n", diff, timeout_value);
    if(diff == timeout_value && saved == 0)
    {
#ifdef IPOD
		// KERIPO MOD
        //sprintf(buffer, "/hp/%d", (int)tv.tv_sec);
		sprintf(buffer, "/opt/Zillae/ZacZilla/Data/Capture/%d", (int)tv.tv_sec);
        if(capture_save(buffer) == 1)
        {
            sprintf(buffer, "/%d", (int)tv.tv_sec);
            capture_save(buffer);
        }
#else
        sprintf(buffer, "./%d", (int)tv.tv_sec);
        capture_save(buffer);
#endif
        timeout_start = 0;
    }
    if(diff <= timeout_value)
    {
        this->data = (void *)(timeout_value-diff);
        this->dirty++;
    }
    else
    {
        this->data = (void *)(0);
    }
    return 0;
}

static void capture_draw(TWidget *this, ttk_surface srf)
{
    char last[25];
    if(this->data){
        sprintf(buffer, "%02d", (int)this->data);
        ttk_text (srf, ttk_textfont, this->x+4, this->y+4, ttk_ap_getx("header.bg")->color, last);
        ttk_text (srf, ttk_textfont, this->x+4, this->y+4, ttk_ap_getx("header.fg")->color, buffer);
        sprintf(last, "%s", buffer);
    }
}

static TWidget *capture_widget()
{
    capture = ttk_new_widget(0, 0);
    capture->w = TTK_ICON_WIDTH;
    capture->h = TTK_ICON_HEIGHT;
    capture->timer = capture_timer;
    capture->draw  = capture_draw;
    capture->data  = NULL;

    ttk_widget_set_timer(capture, 500);

    return capture;
}

static int save_capture_config(TWidget *this, int key, int time)
{
    if(key == TTK_BUTTON_MENU){
        
    }
    return ttk_menu_button(this, key, time);
}

