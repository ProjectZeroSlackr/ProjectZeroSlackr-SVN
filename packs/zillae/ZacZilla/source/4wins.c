/*
 * Copyright (C) 2006 Armon Khosravi
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
#include <time.h>
#include <limits.h>
#include <string.h>
#include "pz.h"

#define ERROR_MESSAGE 1;
#define MOVE_SPACE 3
#define ZEILEN_HOEHE 17
#define MENU_SCHRIFT_HOEHE 11
#define UEBER_SCHRIFT_HOEHE 19
#define MAX_ZEILEN 6
#define PLATZ_OBEN 26
#define SCROLL_BAR_BREITE 3
#define MINDEST_HOEHE_SCROLL_BAR 5
#define MESSAGE_BOX_RAND 3
#define RAND_AUSSEN_MESSAGE_WINDOW 2
#define ZEILEN_ABSTAND 12
#define HEADER ttk_screen->wy


static TWindow *window;
static TWidget *wid;

int option=0, pause=0;
int move=2;
int menu_font;
int ueberschrift_font;
int platz_vor_ueberschrift;
int platz_ueber_ueberschrift;
int position;
int ende;
int stones[7][6];
int player;
int zuge;
int singleplayer;

static ttk_surface image_stone[3];
static ttk_surface image_pfeil;
static ttk_surface image_turn[3];

struct row_points
{
	int good;
	int bad;
	};
	
struct message
{
	int enabled;
	char *message;
	int type;
	int left, top, width, height, text_top;
	} messages;

void set_fonts(ttk_surface srf);
//void error_event(PzEvent *e);
int points_for_point(int x, int y, int player, int points[4]);
void connect4_computer_play(ttk_surface srf);
void error_window(char *message);
void close_message(ttk_surface srf);
void error_message(ttk_surface srf, char *message);
void *get_mem(int bytes);
void move_left(void);
void move_right(void);
void execute_stone(ttk_surface srf);
void connect4_new_game(void);
int search_for_win(int player);
int search_stones(int player, int i, int ii, int richtung);
void set_menu_font(void);
void paint(ttk_surface srf);
void paint_stone(ttk_surface srf, int i, int ii);
void paint_message(ttk_surface srf);
void paint_head(ttk_surface srf);
void paint_right(ttk_surface srf);


void connect4_do_draw (TWidget *wid, ttk_surface srf)
{
    ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(WHITE));

	  switch(option)
	  {
	  	case 0:
        srand(time(0));
	      set_fonts(srf);
        singleplayer=1;
        ende=0;
	      connect4_new_game();
	      paint(srf);
	      break;
	      
	    case 1:
	    	paint(srf);
	    	option=0;
	    	break;
	    	
	    case 2:
	    	execute_stone(srf);
	    	if(pause==0) {
	    	  paint(srf);
	    	  paint_head(srf); 
	    	}
	    	pause=0;
	    	option=0;
	    	break;
	    	
	    	
	    case 3:
	    	close_message(srf);
	    	option=0;
	    	break;
	    	
	    case 4:
	    	paint_message(srf);
	    	option=0;
	    	break;
	    	
	    case 5:
	    	paint(srf);
	    	paint_head(srf);
	    	option=0;
	    	break;
	  }
}


void connect4_computer_play(ttk_surface srf)
{
	int i;
	int row;
	struct row_points rows[]={{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, INT_MAX}};
	int best=7, count_best=1;
	int me_ap[5]= {0, 43, 9331, 2015539, 347734933};
	int me_bp[5]= {0, 1, 259, 55987, 12093235};
	int otherp[5]= {0, 7, 1555, 335923, 20454996};
	int *me_a, *me_b, *other;
	me_a=me_ap;
	me_b=me_bp;
	other=otherp;
	me_a++;
	me_b++;
	other++;
	for (row=0;row<7;row++)
	{
		i=-1;
		while (stones[row][i+1]==0 && i!=5)
		{
			i++;
			}
		if (i!=-1)
		{

			rows[row].good=points_for_point(row, i, player, me_a);
			rows[row].good+=points_for_point(row, i, player ^ (1^2), other);
			if (i!=0)
			{
				stones[row][i]=player;
				rows[row].bad=points_for_point(row, i-1, player, me_b);
				rows[row].bad+=points_for_point(row, i-1, player ^ (1^2), other);
				stones[row][i]=0;
				}
			if (rows[row].good>=rows[row].bad)
			{
				if (rows[row].good>rows[best].good)
				{
					best= row;
					count_best=1;
					}
				else if (rows[row].good==rows[best].good)
				{
					if (rows[row].bad<rows[best].bad)
					{
						best= row;
						count_best=1;
						}
					else if (rows[row].bad==rows[best].bad)
					{
						count_best++;
						}
					}
				}
			else if (rows[best].good<rows[best].bad)
			{
				if (rows[row].bad<rows[best].bad)
				{
					best= row;
					count_best=1;
					}
				else if (rows[row].bad==rows[best].bad)
				{
					if (rows[row].good>rows[best].good)
					{
						best= row;
						count_best=1;
						}
					else if (rows[row].good==rows[best].good)
					{
						count_best++;
						}
					}
				}
			}
		}
	if (count_best!=1)
	{
		count_best=rand()/(RAND_MAX+1.0)*count_best-0.5;
		i=-1;
		for (row=0;i!=count_best;row++)
		{
			if ((rows[row].good==rows[best].good) && (rows[row].bad==rows[best].bad))
			{
				i++;
				}
			}
		best= row-1;
		}
	i=-1;
	while (stones[best][i+1]==0 && i!=5)
	{
		i++;
		}
	stones[best][i]=player;
	paint_stone(srf, best, i);
	zuge--;
	player= player ^ (1^2);
	}

int points_for_point(int x, int y, int player, int points[4])
{
	int point=0;
	//int before=0;
	point= points[search_stones( player,x-3, y, 0)];
	point+= points[search_stones( player,x-2, y, 0)];
	point+= points[search_stones( player,x-1, y, 0)];
	point+= points[search_stones( player,x, y, 0)];
	point+= points[search_stones( player,x-3, y-3, 1)];
	point+= points[search_stones( player,x-2, y-2, 1)];
	point+= points[search_stones( player,x-1, y-1, 1)];
	point+= points[search_stones( player,x, y, 1)];
	point+= points[search_stones( player,x, y-3, 2)];
	point+= points[search_stones( player,x, y-2, 2)];
	point+= points[search_stones( player,x, y-1, 2)];
	point+= points[search_stones( player,x, y, 2)];
	point+= points[search_stones( player,x+3, y-3, 3)];
	point+= points[search_stones( player,x+2, y-2, 3)];
	point+= points[search_stones( player,x+1, y-1, 3)];
	point+= points[search_stones( player,x, y, 3)];
	return point;
}

void paint_message(ttk_surface srf)
{
	int rand, x=ZEILEN_ABSTAND, i=-1;
	char *text;
	text= (char *)get_mem(sizeof(char)*(strlen(messages.message+1)));
	*text= '\0';
	rand= MESSAGE_BOX_RAND;
	ttk_fillrect (srf, messages.left+1, messages.top+1, messages.width-2, messages.height-2, ttk_makecol(WHITE));
	ttk_rect (srf, messages.left-1, messages.top-1, messages.width+2, messages.height+2, ttk_makecol(WHITE));
	ttk_rect (srf, messages.left, messages.top, messages.width, messages.height, ttk_makecol(BLACK));
	while (messages.message[++i]!='\0')
	{
		if (messages.message[i]=='\n')
		{
			ttk_text (srf, ttk_menufont, messages.left+ rand, messages.top+MESSAGE_BOX_RAND+x-1, ttk_ap_getx("window.fg")->color, text);
			x+= ZEILEN_ABSTAND;
			*text= '\0';;
			}
		else
		{
			strncat(text, &messages.message[i], 1);
			}
		}
	ttk_text (srf, ttk_menufont, messages.left+ rand, messages.top+MESSAGE_BOX_RAND+x-1, ttk_ap_getx("window.fg")->color, text);
	}


void set_fonts(ttk_surface srf)
{
	if(ttk_screen->w == 176 || ttk_screen->w == 160) 
	{
		image_stone[0]=ttk_load_image("Data/0s.png");
	  if (!(image_stone[1]=ttk_load_image("Data/1s.png")))
	  {
		  error_message(srf, "Load failed");
		}
	  image_stone[2]=ttk_load_image("Data/2s.png");
	  image_pfeil=ttk_load_image("Data/pfeil_s.png");
	  image_turn[1]=ttk_load_image("Data/turn_1.png");
	  image_turn[2]=ttk_load_image("Data/turn_2.png");
	}
	  
	else if(ttk_screen->w == 138)
	{
		image_stone[0]=ttk_load_image("Data/0m.png");
	  if (!(image_stone[1]=ttk_load_image("Data/1m.png")))
	  {
		  error_message(srf, "Load failed");
		}
	  image_stone[2]=ttk_load_image("Data/2m.png");
	  image_pfeil=ttk_load_image("Data/pfeil_m.png");
	  image_turn[1]=ttk_load_image("Data/turn_1.png");
	  image_turn[2]=ttk_load_image("Data/turn_2.png");
	}
	
	else
	{
		image_stone[0]=ttk_load_image("Data/0.png");
	  if (!(image_stone[1]=ttk_load_image("Data/1.png")))
	  {
		  error_message(srf, "Load failed");
		}
	  image_stone[2]=ttk_load_image("Data/2.png");
	  image_pfeil=ttk_load_image("Data/pfeil.png");
	  image_turn[1]=ttk_load_image("Data/turn_1.png");
	  image_turn[2]=ttk_load_image("Data/turn_2.png");
	}
}


void paint_head(ttk_surface srf)
{
	ttk_fillrect (srf, 0, 0, 140, 8, ttk_makecol(WHITE));
	if(ttk_screen->w == 176 || ttk_screen->w == 160) { //ipod is nano
	  ttk_blit_image (image_pfeil, srf, 16*position, 0); }
	else if(ttk_screen->w == 138) {
		ttk_blit_image (image_pfeil, srf, 12*position, 0); }
	else {
	  ttk_blit_image (image_pfeil, srf, 20*position, 0); }
}


void paint_right(ttk_surface srf)
{
	ttk_fillrect (srf, 140, 0, 20, 128, ttk_makecol(WHITE));
  if(ttk_screen->w == 176 || ttk_screen->w == 160) {
    ttk_blit_image (image_turn[player], srf, 113, 20); }
  else if(ttk_screen->w == 138) {
  	ttk_blit_image (image_turn[player], srf, 85, 20); }
  else {
	  ttk_blit_image (image_turn[player], srf, 141, 20); }
}

void paint(ttk_surface srf)
{
	int i, ii;
	paint_head(srf);
	paint_right(srf);
	for (i=0;i<7;i++)
	{
		for (ii=0;ii<6;ii++)
		{
			paint_stone(srf, i, ii);
			}
		}
	}

void paint_stone(ttk_surface srf, int i, int ii)
{
	if(ttk_screen->w == 176 || ttk_screen->w == 160) { //ipod is nano
		ttk_line (srf, 0, 104, 111, 104, ttk_makecol(BLACK));
	  ttk_blit_image (image_stone[stones[i][ii]], srf, i*16, 8+ii*16); }
	else if(ttk_screen->w == 138) {
		ttk_line (srf, 0, 80, 83, 80, ttk_makecol(BLACK));
	  ttk_blit_image (image_stone[stones[i][ii]], srf, i*12, 8+ii*12); }
	else {
		ttk_line (srf, 0, 128, 139, 128, ttk_makecol(BLACK));
	  ttk_blit_image (image_stone[stones[i][ii]], srf, i*20, 8+ii*20); }
}

void close_message(ttk_surface srf)
{
	messages.enabled=0;
	paint(srf);
}

void error_message(ttk_surface srf, char *message)
{
	int width, height;
	
	ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(WHITE));
	width = ttk_text_width (ttk_menufont, message);
	height = ttk_text_height (ttk_menufont);
	ttk_rect (srf, ((ttk_screen->w/2)-(width/2))-5, (((ttk_screen->h-HEADER)/2)-(height/2))-5, ((ttk_screen->w/2)+(width/2))+5, (((ttk_screen->h-HEADER)/2)+(height/2))+5, ttk_makecol(BLACK));
	ttk_text (srf, ttk_menufont, (ttk_screen->w/2)-(width/2), ((ttk_screen->h-HEADER)/2)-(height/2), ttk_ap_getx("window.fg")->color, message);
}

void execute_stone(ttk_surface srf)
{
	int i;
	char message[24];
	if (ende == 0)
	{
		strcpy(message, " Player 0 win");
		i=-1;
		while (stones[position][i+1]==0 && i!=5)
		{
			i++;
			}
		if (i!=-1)
		{
			stones[position][i]=player;
			paint_stone(srf, position, i);
			zuge--;
			if (search_for_win(player))
			{
				ende=1;
				message[8]= player+'0';
				error_message(srf, message);
				pause=1;
				//ttk_window_set_title(window, message);
				}
			else if (zuge==0)
				{
					ende=1;
					error_message(srf, "Draw");
					pause=1;
					//ttk_window_set_title(window, "Draw");
					}
			else
			{
				player= player ^ (1^2);
				paint_right(srf);
				if (singleplayer == 1)
				{
					connect4_computer_play(srf);
					paint_right(srf);
					if (search_for_win(player^ (1^2)))
					{
						ende=1;
						message[8]= player ^( (1^2)+'0');
						error_message(srf, message);
						pause=1;
						//ttk_window_set_title(window, message);
						return;
						}
					}
				if (zuge==0)
				{
					ende=1;
					error_message(srf, "Draw");
					pause=1;
					//ttk_window_set_title(window, "Draw");
					}
				}
			}
		}
}

void connect4_new_game(void)
{
	int i;
	int ii;
	position=0;
	player=1;
	zuge=42;
	ende=0;
	ttk_window_set_title(window, "Connect 4");
	for (i=0; i<7; i++)
	{
		for (ii=0; ii<6; ii++)
		{
			stones[i][ii]=0;
			}
		}
	}

int search_for_win(int player)
{
	int i;
	int ii;
	//int res;
	for (i=0; i<7; i++)
	{
		for (ii=0; ii<6; ii++)
		{
			if ((search_stones(player, i, ii, 0)==4) || (search_stones(player, i, ii, 1)==4) || (search_stones(player, i, ii, 2)==4) || (search_stones(player, i, ii, 3)==4))
			{
				return 1;
				}
			}
		}
	return 0;
	}

int search_stones(int player, int i, int ii,int richtung)
{
	int a, b, c, res=0;
	switch (richtung)
	{
		case 0:
		case 1:
		case 7:
			a=1;
			break;
		case 3:
		case 4:
		case 5:
			a=-1;
			break;
		case 2:
		case 6:
			a=0;
		}
	switch (richtung)
	{
		case 1:
		case 2:
		case 3:
			b=1;
			break;
		case 5:
		case 6:
		case 7:
			b=-1;
			break;
		case 0:
		case 4:
			b=0;
		}
	if (((i+a*3<0) || (i+a*3>6) || (ii+b*3<0) ||(ii+b*3>5)) || ((i<0) || (i>6) || (ii<0) ||(ii>5)))
	{
		return -1;
		}
	else
	{
		for (c=0; c<4; c++)
		{
			if (stones[i][ii]==player)
			{
				res++;
				}
			else
			 if (stones[i][ii]!=0)
			{
				return -1;
				break;
				}
			i+=a;
			ii+=b;
			}
		return res;
		}
}

void *get_mem(int bytes)
{
	return (void *) malloc(bytes);
}

void move_left(void)
{
	if (move>=MOVE_SPACE)
	{
		move=0;
		if (position!=0)
		{
			position--;
			//paint_head();
			option=5;
			wid->dirty++;
			}
		}
	else
	{
		move++;
		}
	}

void move_right(void)
{
	if (move==0)
	{
		move=MOVE_SPACE;
		if (position!=6)
		{
			position++;
			//paint_head();
			option=5;
			wid->dirty++;
			}
		}
	else
	{
		move--;
		}
	}



/*if(messages.enabled==1)
		 {
       //close_message();
       option=3;
       wid->dirty++;
     }*/

  static int scroll_connect4(TWidget *this,int dir)
  {
     TTK_SCROLLMOD( dir, 10);
      if( dir > 0){
      	move_right();
      	}
      else {
      	move_left();
      }
    return TTK_EV_CLICK;
    }

 static int down_connect4(TWidget *this,int button)
 {
      switch (button) {
      case TTK_BUTTON_ACTION:
      	 if (ende)
         {
           singleplayer=1;
					 connect4_new_game();
    			 //paint();
    			 option=1;
    			 wid->dirty++;
           ende=0;
         }
         else
         {
				   //execute_stone();
				   option=2;
				   wid->dirty++;
				 }
         break;

      case TTK_BUTTON_MENU:
      	 option=0;
      	 pz_close_window(this->win);
         break;

      case TTK_BUTTON_PLAY:
         break;
         
    
     }
     return 0;
   }

/*		if(messages.enabled==1)
		{
			//paint_message();
			option=4;
			wid->dirty++;
		}*/


TWindow *new_connect4_window()
{
	  window = ttk_new_window ();
	  window->title="Connect 4";
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=connect4_do_draw;
    wid->scroll=scroll_connect4;
    wid->down=down_connect4;
    ttk_add_widget(window, wid);
    ttk_show_window (window);
}



