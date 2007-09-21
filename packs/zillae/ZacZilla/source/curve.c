/*
 * Curve 0.4
 *
 * Copyright (C) 2006 Felix Bruns & Terry Stenvold
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <string.h>
#include "pz.h"
extern int curvespeed;
extern int curveplay;
#define PI_180 (3.14159 / 180)
#define SPEED 20

typedef struct _curve_globals {
	
	TWindow * window;
	TWidget * widget;
	

	int paused;
	int timer;
	
	ttk_color bg, fg;
	ttk_surface surface;

	char * msgA, * msgB, * msgC;
	int wb; 
	int wo;
} curve_globals;

static curve_globals cglob;
static ttk_color ccolors[3];
static int curveAx,
	   curveAy,
	   curveBx,
	   curveBy,
	   curveBxx,
	   curveByy,
	   alphaA,
	   alphaB,
	   last[2][4][2],
	   coords[320][240],
	   msgshown,
	   choice,
	   num,
	   wb=0,
	   wo=0,
	   pp=1,
	   spp=1;
static unsigned short curve_small[] = {
	0x6000, // . X X .
	0x6000, // . X X .
	0 // not necessary
};
static const char* player_options[] = { "1 vs. iPod", "1 vs. 1", NULL };
static const char* speed_options[] = { "Normal", "Fast", "Super Human", "Weak", NULL };

int curve_round(double d)
{ 
	return d<0?d-.5:d+.5; 
}

int get_rand_value(int max)
{
	return (int)((float)max * rand() / (RAND_MAX + 1.0));
}

void set_coords(int x, int y)
{
	coords[x+1][y]   = 1;
	coords[x+2][y]   = 1;
	coords[x+1][y+1] = 1;
	coords[x+2][y+1] = 1;
}

int check_coords(int x, int y)
{
	if(coords[x+1][y] == 1){return 1;}
	else if(coords[x+2][y] == 1){return 1;}
	else if(coords[x+1][y+1] == 1){return 1;}
	else if(coords[x+2][y+1] == 1){return 1;}
	else{return 0;}
}

void set_last(int x, int y, int ab)
{
	last[ab][0][0] = x+1;
	last[ab][0][1] = y;

	last[ab][1][0] = x+2;
	last[ab][1][1] = y;

	last[ab][2][0] = x+1;
	last[ab][2][1] = y+1;

	last[ab][3][0] = x+2;
	last[ab][3][1] = y+1;
}

int check_last(int x, int y, int ab)
{
	if(last[ab][0][0] == x+1 && last[ab][0][1] == y){return 1;}
	else if(last[ab][1][0] == x+2 && last[ab][1][1] == y){return 1;}
	else if(last[ab][2][0] == x+1 && last[ab][2][1] == y+1){return 1;}
	else if(last[ab][3][0] == x+2 && last[ab][3][1] == y+1){return 1;}

	else if(last[ab][0][0] == x+2 && last[ab][0][1] == y+1){return 1;}
	else if(last[ab][1][0] == x+1 && last[ab][1][1] == y){return 1;}
	else if(last[ab][2][0] == x+2 && last[ab][2][1] == y){return 1;}
	else if(last[ab][3][0] == x+1 && last[ab][3][1] == y+1){return 1;}

	else if(last[ab][0][0] == x+1 && last[ab][0][1] == y+1){return 1;}
	else if(last[ab][1][0] == x+2 && last[ab][1][1] == y+1){return 1;}
	else if(last[ab][2][0] == x+1 && last[ab][2][1] == y){return 1;}
	else if(last[ab][3][0] == x+2 && last[ab][3][1] == y){return 1;}

	else if(last[ab][0][0] == x+2 && last[ab][0][1] == y){return 1;}
	else if(last[ab][1][0] == x+1 && last[ab][1][1] == y+1){return 1;}
	else if(last[ab][2][0] == x+2 && last[ab][2][1] == y+1){return 1;}
	else if(last[ab][3][0] == x+1 && last[ab][3][1] == y){return 1;}

	else{return 0;}
}

void curve_reset()
{
	int i, j;

	cglob.bg = ttk_ap_get( "window.bg" )->color;
	cglob.fg = ttk_ap_get( "window.fg" )->color;

	if( ttk_screen->bpp == 2 ) {
		if(cglob.bg == ccolors[0]){cglob.msgA = "White lost...";}else{cglob.msgA = "Black lost...";}
		if(cglob.bg == ccolors[1]){cglob.msgB = "White lost...";}else{cglob.msgB = "Grey lost...";}
	} else {
		if(cglob.bg == ccolors[0]){cglob.msgA = "White lost...";}else{cglob.msgA = "Orange lost...";}
		if(cglob.bg == ccolors[1]){cglob.msgB = "White lost...";}else{cglob.msgB = "Blue lost...";}
	}

	ttk_window_set_title (cglob.window, "Curve");

	cglob.paused = 0;
	cglob.timer = 0;

	alphaA = get_rand_value(240);
	alphaB = get_rand_value(240);
	curveAx = get_rand_value(cglob.widget->win->w-20)+10;
	curveAy = get_rand_value(cglob.widget->win->h-20)+10;
	curveBx = get_rand_value(cglob.widget->win->w-20)+10;
	curveBy = get_rand_value(cglob.widget->win->h-20)+10;
	msgshown = 0;

	for(i = 0; i < 4; i++){
		last[0][i][0] = 0;
		last[0][i][1] = 0;
		last[1][i][0] = 0;
		last[1][i][1] = 0;
	}
	for(i = 0; i < 320; i++){
		for(j = 0; j < 240; j++){
			coords[i][j] = 0;
		}
	}
	
	wo=0;
	wb=0;
}

void draw_curve( TWidget *widget, ttk_surface srf )
{
	char buffer[50];
	snprintf(buffer, 50, "Score P1: %02d P2: %02d", cglob.wo, cglob.wb); //puts title into buffer
	cglob.msgC = buffer; 

	ttk_window_set_title (cglob.window, cglob.msgC); //sets title to display score

	cglob.surface = srf;

	if(check_coords(curveAx, curveAy) == 1 && check_last(curveAx, curveAy, 0) != 1 && msgshown != 1){cglob.wb++;
		msgshown = 1;
		ttk_window_set_title (cglob.window, cglob.msgA);
//		choice = pz_dialog( _(cglob.msgA), _("Play again?"), 2, 0, _("Yes"), _("No"));
		if(choice == 1){
			curve_reset();
			pz_close_window (widget->win);
		}
		else{
			curve_reset();
			ttk_fillrect (srf, 0, 0, widget->win->w, widget->win->h, cglob.bg);
		}
	}
	else if(check_coords(curveBx, curveBy) == 1 && check_last(curveBx, curveBy, 1) != 1 && msgshown != 1){cglob.wo++;
		msgshown = 1;
		ttk_window_set_title (cglob.window, cglob.msgB);
//		choice = pz_dialog( _(cglob.msgB), _("Play again?"), 2, 0, _("Yes"), _("No"));
		if(choice == 1){
			curve_reset();
			pz_close_window (widget->win);
		}
		else{
			curve_reset();
			ttk_fillrect (srf, 0, 0, widget->win->w, widget->win->h, cglob.bg);
		}
	}

	set_coords(curveAx, curveAy);
	set_coords(curveBx, curveBy);

	set_last(curveAx, curveAy, 0);
	set_last(curveBx, curveBy, 1);

	if(cglob.bg == ccolors[0]){
		ttk_bitmap (srf, curveAx, curveAy, 4, 2, curve_small, ccolors[2]);
	} else{
		ttk_bitmap (srf, curveAx, curveAy, 4, 2, curve_small, ccolors[0]);
	}
	if(cglob.bg == ccolors[1]){
		ttk_bitmap (srf, curveBx, curveBy, 4, 2, curve_small, ccolors[2]);
	} else{
		ttk_bitmap (srf, curveBx, curveBy, 4, 2, curve_small, ccolors[1]);
	}
}

void ai_walls(int b, int c) //directional wall movement
{
	if (curveBxx > curveBx && curveBy < 5) 		{alphaB-=10;} //top g r
	 else if (curveBxx < curveBx && curveBy < 5) 	{alphaB+=10;} //top g l
	 else if (curveBxx == curveBx && curveBy < 5) 	{alphaB+=10;} //top g s

	if (curveBxx > curveBx && curveBy >= b) 	{alphaB+=10;} //bot g r
	 else if (curveBxx < curveBx && curveBy >= b) 	{alphaB+=10;} //bot g l
	 else if (curveBxx == curveBx && curveBy >= b) {alphaB+=10;} //bot g s

 	if (curveByy > curveBy && curveBx < 5) 		{alphaB+=10;} //left g u
	 else if (curveByy < curveBy && curveBx < 5) 	{alphaB-=10;} //left g d
	 else if (curveByy == curveBy && curveBx < 5) 	{alphaB+=10;} //left g s

	if (curveByy > curveBy && curveBx >= c ) 	{alphaB-=10;} //right g u
	 else if (curveByy < curveBy && curveBx >= c) 	{alphaB+=10;} //right g d
	 else if (curveByy == curveBy && curveBx >= c) {alphaB+=10;} //right g s
}

void ai_rand()
{
	int b, c;
	b =ttk_screen->w;	//screen width
	c =ttk_screen->h; 	//screen height
	b-=5; 			//right of screen
	c-=25; 			//bottom of screen

	if ((curveBx < 10 && curveBy < 10) || (curveBx >= b && curveBy >= c) || (curveBx >= b && curveBy < 5) || (curveBx < 5 && curveBy >= c)){ //corners
		if (curveBx < 10 && curveBy <  10)     {curveBx+=1; curveBy+=1; ai_walls(b,c);}  //top left
		else if (curveBx >= b && curveBy >= c) {curveBx-=1; curveBy-=1; ai_walls(b,c);}  //bottom right
		else if (curveBx >= b && curveBy < 5)  {curveBx-=1; curveBy+=1; ai_walls(b,c);}  //top right
		else if (curveBx < 5 && curveBy >= c)  {curveBx+=1; curveBy-=1; ai_walls(b,c);}  //bottom left
	}
	else if(curveBx >= b || curveBx < 5 || curveBy >= c || curveBy < 5) { //walls
		if (curveBx >= b)        		{curveBx-=1; ai_walls(b,c);} //right 
		if (curveBx < 5)        	   	{curveBx+=1; ai_walls(b,c);} //left 
		if (curveBy >= c)        		{curveBy-=1; ai_walls(b,c);} //bottom 
		if (curveBy < 5)        		{curveBy+=1; ai_walls(b,c);} //top
	}

	num = rand() % 3; //Ai rand

 	 switch(num){
	  case 0 : alphaB+=10; break; //left 
	  case 1 : alphaB-=10; break; //right
	  case 2 : break;} 	     //straight

	curveByy = curveBy; curveBxx = curveBx; //set direction
}

static int curve_down(TWidget *this,int button)
{
		switch( button ) {
		case( TTK_BUTTON_HOLD ):
			cglob.paused = 1;
			break;
		}
		return 0;
}
static int curve_button(TWidget *this,int button,int time)
{
		switch( button ) {
		case( TTK_BUTTON_ACTION ):
			pz_close_window (this->win);
			break;

		case( TTK_BUTTON_MENU ):
			if(pp == 0){
				pz_close_window (this->win);
			}
			break;

		case( TTK_BUTTON_HOLD ):
			cglob.paused = 0;
			break;
		}
		return 0;
	}


static int curve_timer(TWidget *this)
	{
			if(cglob.paused != 1){
 			if(pp != 0){	//config buttons for 2 players
				if(ttk_button_pressed(TTK_BUTTON_PLAY)){alphaA += 10;}
				if(ttk_button_pressed(TTK_BUTTON_NEXT)){alphaA -= 10;}
				if(ttk_button_pressed(TTK_BUTTON_MENU)){alphaB -= 10;}
				if(ttk_button_pressed(TTK_BUTTON_PREVIOUS)){alphaB += 10;}
			}
			else{		//config buttons for 1 player
				if(ttk_button_pressed(TTK_BUTTON_NEXT)){alphaA += 10;}
				if(ttk_button_pressed(TTK_BUTTON_PREVIOUS)){alphaA -= 10;}
				ai_rand();
			}
		
			curveAx = curveAx + curve_round(SPEED * cos(alphaA*PI_180) * .1);
			curveAy = curveAy + curve_round(SPEED * sin(alphaA*PI_180) * .1);
			if(pp != 0){
				curveBx = curveBx + curve_round(SPEED * cos(alphaB*PI_180) * .1);
				curveBy = curveBy + curve_round(SPEED * sin(alphaB*PI_180) * .1);
			}
			else{
				curveBx = curveBx + (SPEED * cos(alphaB*PI_180) * .1);
				curveBy = curveBy + (SPEED * sin(alphaB*PI_180) * .1);
			}

			if(alphaA > 360)
				alphaA -= 360;
			if(alphaB > 360)
				alphaB -= 360;
			if(alphaA < 0)
				alphaA += 360;
			if(alphaB < 0)
				alphaB += 360;

			
			if((curveAx > cglob.widget->win->w || curveAy > cglob.widget->win->h || curveAx < -1 || curveAy < 0) && msgshown != 1){cglob.wb++;;
				msgshown = 1;
				ttk_window_set_title (cglob.window, cglob.msgA);
//				choice = pz_dialog( _(cglob.msgA), _("Play again?"), 2, 0, _("Yes"), _("No"));
				if(choice == 1){
					curve_reset();
					pz_close_window (cglob.widget->win);
					
				}
				else{
					curve_reset();
					ttk_fillrect (cglob.surface, 0, 0, cglob.widget->win->w, cglob.widget->win->h, cglob.bg);
				}
			}
			else if((curveBx > cglob.widget->win->w || curveBy > cglob.widget->win->h || curveBx < -1 || curveBy < 0) && msgshown != 1){cglob.wo++;;
				msgshown = 1;
				ttk_window_set_title (cglob.window, cglob.msgB);
//				choice = pz_dialog( _(cglob.msgB), _("Play again?"), 2, 0, _("Yes"), _("No"));
				if(choice == 1){
					curve_reset();
					pz_close_window (cglob.widget->win);
//					break;
				}
				else{
					curve_reset();
					ttk_fillrect (cglob.surface, 0, 0, cglob.widget->win->w, cglob.widget->win->h, cglob.bg);
				}
			}
		}
		cglob.timer++;
		this->dirty = 1;
		return 0;

	}



TWindow *new_curve_window()
{
	cglob.wo=0; cglob.wb=0;
 
	if( ttk_screen->bpp == 2 ) {
		ccolors[0]  = ttk_makecol( BLACK );
		ccolors[1]  = ttk_makecol( GREY );
		ccolors[2]  = ttk_makecol( WHITE );
		cglob.msgA = "Black lost...";
		cglob.msgB = "Grey lost...";
	} else {
		ccolors[0]  = ttk_makecol( 238, 104,  15 );
		ccolors[1]  = ttk_makecol(  40,  40, 255 );
		ccolors[2]  = ttk_makecol( WHITE );
		cglob.msgA = "Orange lost...";
		cglob.msgB = "Blue lost...";
	}
	cglob.window = ttk_new_window("Curve" );

	cglob.widget = ttk_new_widget(0,0);
	cglob.widget->focusable=1;
	cglob.widget->draw=draw_curve;
	cglob.widget->down=curve_down;
	cglob.widget->button-curve_button;
	cglob.widget->timer=curve_timer;
	ttk_add_widget(cglob.window, cglob.widget);

	cglob.widget->w = cglob.widget->h = 0;

	// old selection of players
	//pp = pz_dialog( _("Player Selection"), _("1 Player or 2 Players"), 2, 0, _("1"), _("2"));

	// new selection of players & speed (menu)
	pp = curveplay;  
	spp = curvespeed; 


	if (spp == 0) {ttk_widget_set_timer(cglob.widget, 50);}
	else if (spp == 1) {ttk_widget_set_timer(cglob.widget, 35);}
	else if (spp == 2) {ttk_widget_set_timer(cglob.widget, 15);}
	else if (spp == 3) {ttk_widget_set_timer(cglob.widget, 60);}

	curve_reset();

	ttk_show_window(cglob.window);
return 0;
}


static int save_curve_config(TWidget *this, int key, int time)
{

	return ttk_menu_button(this, key, time);
}



