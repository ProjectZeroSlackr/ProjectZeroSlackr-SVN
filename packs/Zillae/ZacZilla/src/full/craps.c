/*
 * Copyright (C) 2006 Armon Khosravi (miz dawg)
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
#include <errno.h>
#include "pz.h"

#define HEADER ttk_screen->wy

enum { BET, BUTTON, ROLL, RESULT, ROLLAGAIN, POINT, RESULT2, PLAYAGAIN } ;


static TWidget *wid;

static int bet=0; 
static int roll, point, menuwin, paused, again;
static int DieOne, DieTwo;
static int imgw, imgh;
char picDieOne[10], picDieTwo[5];

static char *title[] = {
    "Continue?", "Play Again?"
};

static char *question[] = {
    "Risk your earnings?", 
    "Would you like to restart?"
};


static int craps_do_draw (TWidget *wid, ttk_surface srf)
{
	 char str[35];
	 int width, height;
	 int choice;
	 paused=0;
	 height = ttk_text_height(ttk_textfont);
	 
	 if(ttk_screen->bpp == 16) {
	     ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h-HEADER, ttk_makecol(94,38,12));
	     ttk_fillrect (srf, 3, 4, ttk_screen->w-3, ttk_screen->h-HEADER-3, ttk_makecol(12,126,21)); }
	 else {
	 	  ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(WHITE)); }
	 	  
	 
	 switch(menuwin)
	 {
	 	 case BET:
	     width = ttk_text_width(ttk_textfont, "Your Bet: %d");
	     sprintf(str, "Your Bet: %d", bet);
	     ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2), ((ttk_screen->h-HEADER)/2)-(height/2), ttk_ap_getx("window.fg")->color, str);
	     break;
	     
	   case BUTTON:
	   	 again=66;
	   	 DieOne = random() % 6 + 1;
       DieTwo = random() % 6 + 1;
       roll = DieOne + DieTwo;
	   	 width = ttk_text_width(ttk_textfont, "Hit The Center");
	   	 ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2)-5, ((ttk_screen->h-HEADER)/2)-(height/2)-7, ttk_ap_getx("window.fg")->color, "Hit The Center");
	   	 width = ttk_text_width(ttk_textfont, "Button To Roll");
	   	 ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2)-5, (((ttk_screen->h-HEADER)/2)-(height/2))-7+15, ttk_ap_getx("window.fg")->color, "Button To Roll");
	   	 break;
	   	   
	   case ROLL:
	   	 sprintf(str, "You rolled %d", roll);
	   	 ttk_text(srf, ttk_menufont, 5, 10, ttk_ap_getx("window.fg")->color, str);
	   	 if (ttk_screen->bpp != 16) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%dbw.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%dbw.png", DieTwo);*/
           switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1bw.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2bw.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3bw.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4bw.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5bw.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6bw.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1bw.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2bw.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3bw.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4bw.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5bw.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6bw.png");
          break;
          }
           imgw=64, imgh=64;  }
	     else if (ttk_screen->w < 220) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%ds.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%ds.png", DieTwo);*/
                     switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1s.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2s.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3s.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4s.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5s.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6s.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1s.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2s.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3s.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4s.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5s.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6s.png");
          break;
          }
           imgw=64, imgh=64; }
	     else if (ttk_screen->w >= 220) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%d.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%d.png", DieTwo);*/
                     switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6.png");
          break;
          }
           imgw=80, imgh=80; }
	   	 ttk_blit_image (ttk_load_image ( picDieOne), srf, ((ttk_screen->w/2)-imgw)-5, ((ttk_screen->h-HEADER)/2)-(imgh/2));
       ttk_blit_image (ttk_load_image (picDieTwo), srf, (ttk_screen->w/2)+5, ((ttk_screen->h-HEADER)/2)-(imgh/2));
	   	 break;
	   	 	  
	   case RESULT:
	   	switch(roll)
	   	{
	   		  case 7:
	   		  case 11:
	   		  	  again=66;
	   		  	  bet *= 2;
	   		  	  sprintf(str, "You now have $%d", bet);
	   		  	  ttk_text(srf, ttk_menufont, 5, 10, ttk_ap_getx("window.fg")->color, "LUCKY!");
              ttk_text(srf, ttk_menufont, 5, 25, ttk_ap_getx("window.fg")->color, "You win!");
	   		  	  ttk_text(srf, ttk_menufont, 5, 45, ttk_ap_getx("window.fg")->color, str);
	   		  	  again=0;
	   		  	  break;
	   		  case 2:
	   		  case 3:
	   		  case 12:
	   		  	  again=66;
	   		  	  ttk_text(srf, ttk_menufont, 5, 10, ttk_ap_getx("window.fg")->color, "GAMEOVER!");
	   		  	  ttk_text(srf, ttk_menufont, 5, 25, ttk_ap_getx("window.fg")->color, "- Craps! You lose!");
	   		  	  bet = 0;
	   		  	  again=1;
	   		  	  break;
	   		  default:
	   		  	  point=roll;
	   		  	  sprintf(str, "- Your point is now %d.", point);
	   		  	  ttk_text(srf, ttk_menufont, 5, 5, ttk_ap_getx("window.fg")->color, str);
				      paused=3;
              //break;
      }
      break;
      
     case ROLLAGAIN:
     	 DieOne = random() % 6 + 1;
         DieTwo = random() % 6 + 1;
         roll = DieOne + DieTwo;
     	 width = ttk_text_width(ttk_textfont, "Hit The Center Button");
     	 ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2)-5, 10, ttk_ap_getx("window.fg")->color, "Hit The Center Button"); 
     	 width = ttk_text_width(ttk_textfont, "To Roll Again");
	   	 ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2)-5, 25, ttk_ap_getx("window.fg")->color, "To Roll Again");
     	 break;
                 
     case POINT:
       sprintf(str, "You rolled %d, point is %d.", roll, point);
	   	 width = ttk_text_width(ttk_menufont, str);
	   	 ttk_text(srf, ttk_menufont, (ttk_screen->w/2)-(width/2), 10, ttk_ap_getx("window.fg")->color, str);
	 if (ttk_screen->bpp != 16) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%dbw.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%dbw.png", DieTwo);*/
           switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1bw.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2bw.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3bw.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4bw.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5bw.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6bw.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1bw.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2bw.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3bw.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4bw.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5bw.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6bw.png");
          break;
          }
           imgw=64, imgh=64;  }
	     else if (ttk_screen->w < 220) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%ds.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%ds.png", DieTwo);*/
                     switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1s.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2s.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3s.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4s.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5s.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6s.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1s.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2s.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3s.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4s.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5s.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6s.png");
          break;
          }
           imgw=64, imgh=64; }
	     else if (ttk_screen->w >= 220) { 
           /*sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/%d.png", DieOne);
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/%d.png", DieTwo);*/
                     switch(DieOne) {
           case 1: 
           sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/1.png");
          break;
          case 2:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/2.png");
          break;
          case 3:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/3.png");
          break;
          case 4:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/4.png");
          break;
          case 5:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/5.png");
          break;
          case 6:
          sprintf(picDieOne, "/opt/Zillae/ZacZilla/Data/craps/6.png");
          break;
          }
          switch(DieTwo) {
           case 1: 
           sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/1.png");
          break;
          case 2:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/2.png");
          break;
          case 3:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/3.png");
          break;
          case 4:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/4.png");
          break;
          case 5:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/5.png");
          break;
          case 6:
          sprintf(picDieTwo, "/opt/Zillae/ZacZilla/Data/craps/6.png");
          break;
          }
           imgw=80, imgh=80; }
	   	 ttk_blit_image (ttk_load_image (picDieOne), srf, ((ttk_screen->w/2)-imgw)-5, ((ttk_screen->h-HEADER)/2)-(imgh/2));
       ttk_blit_image (ttk_load_image (picDieTwo), srf, (ttk_screen->w/2)+5, ((ttk_screen->h-HEADER)/2)-(imgh/2));
	   	 break;
	   	 
	   case RESULT2:
	   	 if(roll==point)
	   	 {
	   		   bet *= 2;
	   		   roll = 11;
           menuwin = RESULT;
           wid->dirty++;
	   	 }
	   	 else if(roll==7)
	   	 {
			   again=66;
	   		   ttk_text(srf, ttk_menufont, 5, 10, ttk_ap_getx("window.fg")->color, "GAMEOVER!");
	   		   ttk_text(srf, ttk_menufont, 5, 25, ttk_ap_getx("window.fg")->color, "- Seven out, you lose!");
	   		   bet = 0;
	   		   again=1;
	   	 }
	   	 else {
	   	 	   menuwin = ROLLAGAIN;
	   	 	   wid->dirty++; }
	   	 	   
	   	 break;

           case PLAYAGAIN:
           
		       if(again==0)
		             menuwin=BUTTON;
                       else if(again==1)
                             menuwin=BET;

		       wid->dirty++; 
              break;
   }
   return 0;
}

static void ChangeSelection( int side ) 
{
  switch(menuwin) {
	case BET:
		if(side == 1) if (bet < 100) bet+=5;
		if(side == 0) if (bet > 0) bet-=5;
		wid->dirty++;
		break;
	case BUTTON:
		break;
	case ROLL:
		break;
	case RESULT:
		break;
  case ROLLAGAIN:
  	break;
  case POINT:
    break;
  case RESULT2:
  	break;
	}

}

static void ConfirmSelection()
{
  switch(menuwin) {
	case BET:
		menuwin = BUTTON;
		break;
	case BUTTON:
		menuwin=ROLL;
		break;
	case ROLL:
			menuwin=RESULT;
		break;
	case RESULT:
    if(paused==3) menuwin=ROLLAGAIN;
    if(again!=66) menuwin=PLAYAGAIN;
		break;
	case ROLLAGAIN:
		menuwin=POINT;
		break;
  case POINT:
  	menuwin=RESULT2;
    break;
  case RESULT2:
  	if(again!=0) menuwin=PLAYAGAIN;;
  	break;
	}

	wid->dirty++;

}
	 	  
static int craps_scroll(TWidget *this,int dir)
{
     TTK_SCROLLMOD( dir, 5);
      if( dir > 0){ChangeSelection( 1 );}
      else {ChangeSelection( 0 );}
       if(menuwin==BET){wid->dirty++;}
       return TTK_EV_CLICK;
     }

   static int craps_down(TWidget *this,int button)
   {
       switch (button) {
   
        case TTK_BUTTON_ACTION:
   ConfirmSelection();
   wid->dirty++;
        break;
        
      case TTK_BUTTON_MENU:
      	  bet=0;
          pz_close_window(this->win);
        break;}
   
   return 0;
}

TWindow *new_craps_window()
{
    TWindow *ret;

    ret = ttk_new_window ("Craps");
    
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=craps_do_draw;
    wid->scroll=craps_scroll;
    wid->down=craps_down;
    ttk_add_widget(ret, wid);
    
    srandom((unsigned)time(NULL));
    menuwin = BET;

    ttk_widget_set_timer(wid, 150);
    ttk_show_window (ret);
}


