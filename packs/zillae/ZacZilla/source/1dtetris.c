/*
 * Copyright (C) 2006 Alexander Papst
 *
 * Idea from http://www.tetris1d.org
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

#include "pz.h"
#include <time.h>

#define HWH (ttk_screen->h-ttk_screen->wy) //Gets the screen height without the header



static TWindow *window;

static TWidget *wid;
//For drawing
static int w;
static int pf_h, pf_w, pf_x, sq_size; //values for playfield frame
static int nf_w, nf_h, nf_x, nf_y; //values for "next" frame
static int sf_x, sf_w; //values for "score" frame
//For the "game"
static int unsigned score;
static int brick_length, brick_depth, next_length;

/* draws a block */
void draw_block (ttk_surface srf, int x, int y) {
	ttk_fillrect (srf, x+2, y, x+sq_size+1, y+sq_size-1, ttk_makecol(WHITE));
}

/* draws a brick */
void draw_brick (ttk_surface srf, int x, int y, int length, int depth) {
	static int i;
	for (i=depth; i<length+depth; i++) {
		draw_block(srf,x,y+sq_size*i);
	}
}

/* draws everything else */
void draw_1dtetris (TWidget *wid, ttk_surface srf)
{
	char buffer[8];
	if (score == 0) snprintf(buffer, 8, "%d", score); else	snprintf(buffer, 8, "%d00", score);

	// draw playfield
	ttk_fillrect (srf, 0, 0, w, HWH, ttk_makecol(BLACK));
	ttk_rect (srf, pf_x, 6, pf_x+pf_w, 6+pf_h, ttk_makecol(WHITE));
	ttk_line (srf, pf_x+1, 6, pf_x+pf_w-2, 6, ttk_makecol(BLACK));

	// draw next frame
	ttk_rect (srf, nf_x, nf_y, nf_x+nf_w, nf_y+nf_h, ttk_makecol(WHITE));
	ttk_text (srf, ttk_textfont, nf_x+1, nf_y-ttk_text_height(ttk_textfont), ttk_makecol(WHITE), "next");

	// draw score frame
	ttk_rect (srf, sf_x, nf_y, sf_x+sf_w, nf_y+nf_w, ttk_makecol(WHITE));
	ttk_text (srf, ttk_textfont, sf_x+1, nf_y-ttk_text_height(ttk_textfont), ttk_makecol(WHITE), "score");
	ttk_text (srf, ttk_textfont, sf_x+sf_w-ttk_text_width(ttk_textfont, buffer)-3, nf_y + (nf_w/2 - ttk_text_height(ttk_textfont)/2), ttk_makecol(WHITE), buffer);

	// draw moving brick
	draw_brick(srf, pf_x, 7-(sq_size*brick_length), brick_length, brick_depth);
	// draw next brick
	draw_brick(srf, nf_x+sq_size/2, nf_y+sq_size/2, next_length, 0);

	// hide bad programming skills for users eyes :)
	ttk_fillrect (srf, pf_x, 0, pf_x+pf_w, 6, ttk_makecol(BLACK));
}

void gameloop() {
	brick_depth++;
	if (brick_depth == 11) {
		score = score + (brick_length-1)*2;
		brick_depth = 0;
		brick_length = next_length;
		next_length = rand()%3+2; // Create random next piece
	}
}
static int button_1dtetris(TWidget *this,int button,int time)
{
  if(button==TTK_BUTTON_ACTION) { 
    ttk_widget_set_timer(wid,500);
    return 0;
    }
    else return TTK_EV_UNUSED;
    }

	static int down_1dtetris(TWidget *this,int button)
	{
int ret=0;
			switch (button) {
				case TTK_BUTTON_ACTION:  //center / action button
					ttk_widget_set_timer (wid, 50); // makes the brick move faster
					break;

				case TTK_BUTTON_MENU:    //menu button
				score=0;
					ttk_hide_window(window); //quit to menu
					break;

				default:
					ret = TTK_EV_UNUSED;  //do nothing if not buttons pressed
			}
			return ret;
			}

static int timer_1dtetris(TWidget *this)
{
			gameloop();
			wid->dirty = 1;	//redraw everything
return 0;
}

TWindow *new_1dtetris_window()
{
    srand(time(NULL));

    w =ttk_screen->w; //gets screen width

	// calulate a lot of suff that everything looks good on each ipod
    pf_w = sq_size = ((HWH-10)/10);
    pf_h = pf_w*10+2;
    pf_x = w/2 - pf_w/2;
    pf_w += 3;

    nf_w = sq_size+sq_size+3;
    nf_h = 5*sq_size;
    nf_x = pf_x/2 - nf_w/2;
    nf_y = HWH/2 - nf_h/2;

    if (ttk_text_width(ttk_textfont, "00000000")+6>nf_h) sf_w = ttk_text_width(ttk_textfont, "00000000")+6; else sf_w = nf_h;
    sf_x = pf_x/2 - sf_w/2 + pf_x + pf_w;

    brick_length = rand()%3+2;
    brick_depth = 0;
    next_length = rand()%3+2; // Create random next piece

    score = 0;

    gameloop(); //calculate 2 bricks before gameloop starts

    window = ttk_new_window ();
    window->title="1D Tetris";
    wid=ttk_new_widget(0,0);
    wid->w = ttk_screen->w;
    wid->h = window->h;
    wid->focusable=1;
    wid->draw=draw_1dtetris;
    wid->down=down_1dtetris;
    wid->timer=timer_1dtetris;
    wid->button=button_1dtetris;
    ttk_window_show_header(window);
    ttk_add_widget(window, wid);

    ttk_widget_set_timer (wid, 500); //start the timer
ttk_show_window (window);
return 0;
}

