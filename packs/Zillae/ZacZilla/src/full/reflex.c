#include <stdio.h>

#include "pz.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


static TWindow *window;
static TWidget *wid;
static int w, h, bpp;
static int sys;

static int timeer(TWidget *this);
static int i; //random number
static int n; //for variable
static int t;
static int b; //timer stop
static int one;
static int x=1; //clear screen
static char now[50],  welcome[100],  points[10], yourscore[100], ready[50]; 
static int score=0;
static int flag;

void draw_reflex (TWidget *wid, ttk_surface srf)
{
	h=ttk_screen->h;
	w=ttk_screen->w;
	sys = (wid->x + wid->w)/2;
	
	switch(x)
	{
		case 1:
		{
			sprintf(welcome,"Welcome to Reflex Tester, press play to start.");
			ttk_text (srf,ttk_textfont, (sys)-(ttk_text_width(ttk_textfont,welcome)/2), 50, ttk_makecol(255,0,0),welcome);
			break;
		}
		case 2:
		{
			ttk_delay(i);
			ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(255,0,0));
			sprintf(now,"PRESS ACTION NOW!!!");
			ttk_widget_set_timer(wid, 1);
			ttk_text (srf,ttk_textfont, (sys)-(ttk_text_width(ttk_textfont,now)/2), 60, ttk_makecol(0,0,0),now);
			break;
		}
		case 3:
		{
			sprintf(yourscore,"You took %i milliseconds to respond", score);
			ttk_text (srf,ttk_textfont, (sys)-(ttk_text_width(ttk_textfont,yourscore)/2), 50, ttk_makecol(255,0,0),yourscore);
			break;
		}
	}
}


static int reflex_down(TWidget *this,int button)
{
			switch(button)
			{
				case TTK_BUTTON_PLAY:
					x=2;
					this->dirty++;
					break;
				case TTK_BUTTON_ACTION:
					x=3;
					b=1;
					this->dirty++;
					break;
				case TTK_BUTTON_MENU:
					pz_close_window (this->win);
					break;
			}
			return 0;
	}
static int timeer(TWidget *this)
{
			int ret=0;
			switch(b)
			{
				case 0:
				{
					score++;
					ret=TTK_EV_CLICK;
				}
				case 1:
				{
					//printf("action pressed\n");
				}
			}
			return ret;
	}



TWindow *new_reflex_window()
{
    t=10;
	n=0;
	b=0;
	x=1;
	score=0;
	one=1;
	 w =ttk_screen->w; //gets screen width
    h =ttk_screen->h; //gets screen hieght
    bpp =ttk_screen->bpp; //gets colour depth
	srand(time(0));
	i = rand() % 10000;
	
	window = ttk_new_window ("Reflex");
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_reflex;
    wid->down=reflex_down;
    wid->timer=timeer;
    ttk_add_widget(window, wid);
    ttk_show_window (window);
    return 0;
}



