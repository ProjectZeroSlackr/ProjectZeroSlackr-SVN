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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "pz.h"
#include "duckhunt.h"

#define MAX_BOUNCE 16

#define TOP 0
#define BOTTOM (mid-duck_h) //116
#define LEFT 0
#define RIGHT (bg_w-duck_w) //220
#define WIDTH bg_w //220
#define HEIGHT bg_h //176
#define HEADER ttk_screen->wy

#define RED 255,0,0
#define YELLOW 255,253,58

static TWindow *window;
static TWidget *wid;

static ttk_surface duckhunt_srf;
static ttk_surface duckhunt_ss, duckhunt_bg, duckhunt_lose_bg, duck[3], 
       dogw[8], dead_duck, dd_fall, dog_duck, dog_laugh[2], s_ducks[2];
       
static int pause=0, ssFrame=0, action=0, start_duck=0, duck_timer=0, mid;
static int ranbounce, duck_frame=0, dogw_frame=0, dogl_frame=0;
static int shots=3, score=0, dd_fally, duck_faway;
static int imgw, imgh, bg_w, bg_h, duck_w, duck_h;
static int round=0, ducks_shot=0, duck_count=0;
static int temp_bcx, temp_bcy;
static int aimerx, aimery;
static int duckx, ducky;
static int highscore=0;

static float dim = .75;

static const int bounce_cords[MAX_BOUNCE][2] = {
	 {3, 2}, {-3, -2}, {1, 3}, {-1, -2},
	 {4, 2}, {-4, -2}, {1, 4}, {-1, -4},
	 {4, 1}, {-4, -1}, {-3, 2}, {-1, 3},
	 {5, 1}, {-5, -1}, {6, 2}, {-6, -2}
};
static int score_opts[3] = {500, 1000, 1500};
static int mini_ducks[10];


static int draw_duckhunt (TWidget *wid, ttk_surface srf)
{
	 ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(BLACK));
	 if (pause == 0)
	    startup_screen();
	 else if (pause == 1) {
	 	  ttk_blit_image (duckhunt_bg, duckhunt_srf, 0, 0);
	 	  dog_walk();
	 }
	 else if (pause == 2) {
	 	  if (duck_timer >= 100) ttk_blit_image (duckhunt_lose_bg, duckhunt_srf, 0, 0);
	 	  else ttk_blit_image (duckhunt_bg, duckhunt_srf, 0, 0);
	 	  duckhunt_bar();
      if (action == 1)
      	 aimer_hold (aimerx, aimery);
      else if (action == 2)
      	 aimer_shoot (aimerx, aimery);
      if (start_duck == 0)
         init_duck();
      else if (start_duck == 1) {
      	 if (duck_timer >= 100)
      	 	  duck_flyaway();
      	 else
      	   move_duck();
      }
      else if (start_duck == 2)
      	 duck_shot();
      else if (start_duck == 3)
      	 dog_holdduck();
      else if (start_duck == 4)
      	 dog_laughing();
      else if (start_duck == 5)
      	 next_round();
      draw_aimer(aimerx, aimery);
   }
   ttk_blit_image (duckhunt_srf, srf, (ttk_screen->w/2)-(bg_w/2), (ttk_screen->h/2)-(bg_h/2));
   return 0;
}

static void startup_screen()
{
	 if (ssFrame == 0) {
	 	  ttk_fillrect (duckhunt_srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(BLACK));
	    ttk_blit_image (duckhunt_ss, duckhunt_srf, (bg_w/2)-(imgw/2), (bg_h/2)-(imgh/2));
	 }
	 else {
	 	  pause=1;
   }
}

static void next_round()
{
   ttk_widget_set_timer(wid, 2500);
   switch(ducks_shot) {
   	 case 0:
   	 case 1:
   	 case 2:
   	 case 3:
   	 case 4:
   	 case 5:
   	 	 duckhunt_message("GAME OVER");
   	 	 break;
   	 case 6:
   	 case 7:
   	 case 8:
   	 case 9:
   	 	 duckhunt_message("GOOD");
   	 	 round++;
   	 	 reset_round();
   	 	 break;
   	 case 10:
   	 	 duckhunt_message("PERFECT");
   	 	 score += 10000;
   	 	 round++;
   	 	 reset_round();
   	 	 break;
   }
}

static void duckhunt_bar()
{
	 char str[15], str2[10];
	 int i, width;

	 sprintf(str, "%d", score);
	 sprintf(str2, "R = %d", round+1);
	 if(ttk_screen->w == 220) {
	 	  //draw score
	 	  width = pz_vector_width (str2, 6, 6, 1);
      pz_vector_string_center (duckhunt_srf, str, 220-5-27, 176-5-8, 4, 6, 1, ttk_makecol(WHITE));
      //draw round "R = number"
      ttk_fillrect (duckhunt_srf, 21, HEIGHT-31, 21+width+1, (HEIGHT-31)+7, ttk_makecol(BLACK));
      pz_vector_string (duckhunt_srf, str2, 21, HEIGHT-31, 6, 6, 1, ttk_makecol(184,248,24));
      //draw bullets left
      switch(shots) {
      	case 3:
      		break;
      	case 2:
          ttk_fillrect (duckhunt_srf, 34, HEIGHT-19, 40, HEIGHT-12, ttk_makecol(BLACK));
          break;
        case 1:
        	ttk_fillrect (duckhunt_srf, 27, HEIGHT-19, 40, HEIGHT-12, ttk_makecol(BLACK));
        	break;
        case 0:
        	ttk_fillrect (duckhunt_srf, 20, HEIGHT-19, 40, HEIGHT-12, ttk_makecol(BLACK));
        	break;
      }
      //draw mini ducks
      for(i=0;i<10;i++)
        ttk_blit_image (s_ducks[mini_ducks[i]], duckhunt_srf, 81+(i*7), HEIGHT-18);
   }
   else  if(ttk_screen->w<220) {
   	  //draw score
   	  width = pz_vector_width (str2, 5, 5, 1);
   	  pz_vector_string_center (duckhunt_srf, str, bg_w-24, bg_h-9, 3, 5, 1, ttk_makecol(WHITE));
   	  //draw round "R = number"
   	  ttk_fillrect (duckhunt_srf, 16, HEIGHT-23, 16+width+1, (HEIGHT-23)+6, ttk_makecol(BLACK));
   	  pz_vector_string (duckhunt_srf, str2, 16, HEIGHT-23, 5, 5, 1, ttk_makecol(184,248,24));
   	  //draw bullets left
   	  switch(shots) {
      	case 3:
      		break;
      	case 2:
          ttk_fillrect (duckhunt_srf, 26, HEIGHT-14, 30, HEIGHT-9, ttk_makecol(BLACK));
          break;
        case 1:
        	ttk_fillrect (duckhunt_srf, 20, HEIGHT-14, 30, HEIGHT-9, ttk_makecol(BLACK));
        	break;
        case 0:
        	ttk_fillrect (duckhunt_srf, 15, HEIGHT-14, 30, HEIGHT-9, ttk_makecol(BLACK));
        	break;
      }
      //draw mini ducks
      for(i=0;i<10;i++)
        ttk_blit_image (s_ducks[mini_ducks[i]], duckhunt_srf, 61+(i*5), HEIGHT-14);
   }
   else {
   	  //draw score
	 	  width = pz_vector_width (str2, 9, 9, 1);
      pz_vector_string_center (duckhunt_srf, str, 320-7-40, 240-7-19, 6, 9, 1, ttk_makecol(WHITE));
      //draw round "R = number"
      ttk_fillrect (duckhunt_srf, 30, HEIGHT-45, 30+width+1, (HEIGHT-45)+10, ttk_makecol(BLACK));
      pz_vector_string (duckhunt_srf, str2, 30, HEIGHT-45, 9, 9, 1, ttk_makecol(184,248,24));
      //draw bullets left
      switch(shots) {
      	case 3:
      		break;
      	case 2:
          ttk_fillrect (duckhunt_srf, 50, HEIGHT-28, 58, HEIGHT-17, ttk_makecol(BLACK));
          break;
        case 1:
        	ttk_fillrect (duckhunt_srf, 39, HEIGHT-28, 58, HEIGHT-17, ttk_makecol(BLACK));
        	break;
        case 0:
        	ttk_fillrect (duckhunt_srf, 29, HEIGHT-28, 58, HEIGHT-17, ttk_makecol(BLACK));
        	break;
      }
      //draw mini ducks
      for(i=0;i<10;i++)
        ttk_blit_image (s_ducks[mini_ducks[i]], duckhunt_srf, 118+(i*10), HEIGHT-26);
   }
   
}

static void duckhunt_message (char *message)
{
	 int length;
	 
	 length = pz_vector_width (message, 6, 8, 1);
	 ttk_rect (duckhunt_srf, ((WIDTH/2)-(length/2))-6, ((HEIGHT/3)-(8/2))-6, ((WIDTH/2)+(length/2))+6, ((HEIGHT/3)+(8/2))+6, ttk_makecol(WHITE));
	 ttk_fillrect (duckhunt_srf, ((WIDTH/2)-(length/2))-5, ((HEIGHT/3)-(8/2))-5, ((WIDTH/2)+(length/2))+5, ((HEIGHT/3)+(8/2))+5, ttk_makecol(BLACK));
   pz_vector_string (duckhunt_srf, message, (WIDTH/2)-(length/2), (HEIGHT/3)-(8/2), 6, 8, 1, ttk_makecol(WHITE));
}

static void dog_walk()
{
	 char str[20];
	 
	 ttk_widget_set_timer(wid, 700);
	 sprintf(str, "ROUND %d", round+1);
	 duckhunt_message(str);
	 duckhunt_bar();
	 
	 if (dogw_frame < 5) {
	 	  if(ttk_screen->w == 220)
	      ttk_blit_image (dogw[dogw_frame], duckhunt_srf, dogw_frame*20, 103);
	    else if(ttk_screen->w<220)
	    	ttk_blit_image (dogw[dogw_frame], duckhunt_srf, dogw_frame*15, 77);
	    	else
	    	ttk_blit_image (dogw[dogw_frame], duckhunt_srf, dogw_frame*29, 150);
	    dogw_frame++;
	 }
	 else if (dogw_frame >= 5 && dogw_frame < 8) {
	 	  if (dogw_frame == 5) {
	 	  	 if(ttk_screen->w == 220)
	 	       ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 80, 97);
	 	     else if(ttk_screen->w < 220) 
	 	     	 ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 60, 73);
	 	     else
	 	     ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 116, 141);
	 	  }
	 	  else {
	 	  	 if(ttk_screen->w == 220)
	 	  	   ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 80, 75);
	 	  	 else if(ttk_screen->w<220)
	 	  	 	 ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 60, 56);
	 	  	 	 else
	 	  	 	 ttk_blit_image (dogw[dogw_frame], duckhunt_srf, 116, 109);
	 	  }
	 	  dogw_frame++;
	 }
	 else if (dogw_frame == 8) {
	 	  pause=2;
	 	  dogw_frame=0;
	 	  draw_aimer (aimerx, aimery);
	 	  ttk_widget_set_timer(wid, 50);
	 	  wid->dirty++;
	 }  
}

static void dog_holdduck()
{
	 ttk_widget_set_timer(wid, 2500);
	 if(ttk_screen->w == 220)
	   ttk_blit_image (dog_duck, duckhunt_srf, (bg_w/2)-22, mid-42);
	 else if(ttk_screen->w<220)
	 	 ttk_blit_image (dog_duck, duckhunt_srf, (bg_w/2)-17, mid-32);
	 	 else
	 	ttk_blit_image (dog_duck, duckhunt_srf, (bg_w/2)-32, mid-61); 
	 duck_count++;
	 ducks_shot++;
	 if (duck_count == 10)
	 	  start_duck=5;
	 else
	    start_duck=0;
	 shots=3;
}

static void dog_laughing()
{
	 ttk_widget_set_timer(wid, 300);
	 if(ttk_screen->w == 220)
	   ttk_blit_image (dog_laugh[dogl_frame], duckhunt_srf, (bg_w/2)-15, mid-40);
	 else if(ttk_screen->w<220)
	 	 ttk_blit_image (dog_laugh[dogl_frame], duckhunt_srf, (bg_w/2)-11, mid-30);
	 else 
	  ttk_blit_image (dog_laugh[dogl_frame], duckhunt_srf, (bg_w/2)-22, mid-50);
	 dogl_frame++;
	 if (dogl_frame == 2) dogl_frame=0;
	 if (duck_timer >= 10) {
	 	  duck_count++;
	 	  if (duck_count == 10)
	 	  	 start_duck=5;
	 	  else
	 	     start_duck=0;
	 	  duck_timer=0;
	 	  shots=3;
	 }
}

static void duck_flyaway()
{
	 ttk_widget_set_timer(wid, 50);
	 duckhunt_message ("FLY AWAY");
	 ttk_blit_image (duck[duck_frame], duckhunt_srf, duckx, duck_faway);
	 duck_faway -= 5;
   duck_frame++;
	 if (duck_frame == 3) duck_frame=0;
	 if (duck_faway <= -30) {
	 	  duck_timer=0;
	 	  start_duck=4; //DOG LAUGHS AT YOU!
	 }
}

static void duck_shot()
{
	 ttk_widget_set_timer(wid, 50);
	 dd_fally += 4;
	 ttk_blit_image (dd_fall, duckhunt_srf, duckx+5, dd_fally);
	 if(ttk_screen->w > 220) {
	   if (dd_fally >= (mid-45))
	 	    start_duck=3;  //DOG HOLDS DEAD DUCK !!!11ONE+SHIFT!!1
	 }
	 if(ttk_screen->w == 220) {
	   if (dd_fally >= (mid-32))
	 	    start_duck=3;  //DOG HOLDS DEAD DUCK !!!11ONE+SHIFT!!1
	 }
	 else {
	 	 if (dd_fally < (mid-24))
	 	    start_duck=3;  //DOG HOLDS DEAD DUCK !!!11ONE+SHIFT!!1
	 } 
}

static void move_duck()
{
	 ttk_blit_image (duck[duck_frame], duckhunt_srf, duckx, ducky);
	 duckx += temp_bcx;
	 ducky += temp_bcy;
	 if (duckx <= LEFT)
	 	 	temp_bcx = temp_bcx + (temp_bcx*(-2));
	 else if (duckx >= RIGHT)
	 	 	temp_bcx = -temp_bcx;

	 if (ducky <= TOP)
	 	 	temp_bcy = temp_bcy + (temp_bcy*(-2));
	 else if (ducky >= BOTTOM)
	 	 	temp_bcy = -temp_bcy;
	 duck_frame++;
	 if (duck_frame == 3) duck_frame=0;
	 duck_faway = ducky;
}
	 

static void init_duck()
{
	 ttk_widget_set_timer(wid, 50);
	 duckx = random() % 120;//221;
	 ducky = random() % 55;//177;
	 ranbounce = random() % MAX_BOUNCE;
	 temp_bcx = bounce_cords[ranbounce][0] + round;
	 temp_bcy = bounce_cords[ranbounce][1] + round;
	 start_duck = 1;
}
	 
static void draw_aimer (int x, int y)
{
	 ttk_ellipse (duckhunt_srf, x, y, 5, 5, ttk_makecol(RED));
	 ttk_line (duckhunt_srf, x-5, y, x+5, y, ttk_makecol(RED));
	 ttk_line (duckhunt_srf, x, y-5, x, y+5, ttk_makecol(RED));
}

static void aimer_hold (int x, int y)
{
	 ttk_fillellipse (duckhunt_srf, x, y, 5, 5, ttk_makecol(YELLOW));
}

static void aimer_shoot (int x, int y)
{
	 ttk_fillellipse (duckhunt_srf, x, y, 5, 5, ttk_makecol(WHITE));
	 if ((aimerx > duckx && aimerx < duckx+28) && (aimery > ducky && aimery < ducky+30)) {
	 	  //BRAP YOU POPPED THAT PUNKASS DUCK!
	 	 ttk_widget_set_timer(wid, 1500);
	 	  score += score_opts[shots-1];
	 	  mini_ducks[duck_count]=1;
	 	  start_duck=2;
	 	  duck_timer=0;
	 	  dd_fally=ducky;
	 	  ttk_blit_image (duckhunt_bg, duckhunt_srf, 0, 0);
	 	  duckhunt_bar();
	 	  ttk_blit_image (dead_duck, duckhunt_srf, duckx, ducky);
	 }
	 if (shots > 0) shots--;
	 action=0;
}

static void move_up()
{
	 if(aimery >= 8) {
     aimery -= 7;
   }
}

static void move_down()
{
	 if(aimery <= (HEIGHT-7)) {
     aimery += 7;
   }
}

static void move_left()
{
	 if(aimerx >= 7) {
     aimerx -= 7;
   }
}

static void move_right()
{
	 if(aimerx <= (WIDTH-7)) {
     aimerx += 7;
   }
}

static void hit_action()
{
   if (pause == 0) {
      ssFrame++;
      wid->dirty++;
   }
}

static void reset_round()
{
	 int i;
	 aimerx = WIDTH/2;
	 aimery = HEIGHT/2;
	 shots = 3;
	 ducks_shot = 0;
	 duck_count = 0;
	 duck_timer = 0;
	 action = 0;
	 start_duck = 0;
	 pause = 1;
	 for(i=0;i<10;i++)
	   mini_ducks[i] = 0;
}

static void duckhunt_setup()
{
	 int i; char file[20];
	 reset_duckhunt();
	/* for(i=0;i<8;i++) { sprintf (file, "Data/duckhunt/dog%d.gif", i+1); dogw[i] = ttk_load_image (file); }
	 for(i=0;i<3;i++) { sprintf (file, "Data/duckhunt/duck%d.gif", i+1); duck[i] = ttk_load_image (file); }
	*///new_message_window("BIBI");
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog1.gif"); 
	dogw[0] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog2.gif"); 
	dogw[1] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog3.gif"); 
	dogw[2] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog4.gif"); 
	dogw[3] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog5.gif"); 
	dogw[4] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog6.gif"); 
	dogw[5] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog7.gif"); 
	dogw[6] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/dog8.gif"); 
	dogw[7] = ttk_load_image (file);
	
	
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck1.gif"); 
	duck[0] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck2.gif"); 
	duck[1] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck3.gif"); 
	duck[2] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck4.gif"); 
	duck[3] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck5.gif"); 
	duck[4] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck6.gif"); 
	duck[5] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck7.gif"); 
	duck[6] = ttk_load_image (file);
	sprintf (file, "/opt/Zillae/ZacZilla/Data/duckhunt/duck8.gif"); 
	duck[7] = ttk_load_image (file);
	 duckhunt_lose_bg = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/duckhunt_lose_bg.gif");
	 dog_laugh[0] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/dog_laugh1.gif");
	 dog_laugh[1] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/dog_laugh2.gif");
	 duckhunt_ss = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/duckhunt_ss.gif");
	 duckhunt_bg = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/duckhunt_bg.gif");
	 dead_duck = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/dead_duck.gif");
	 dog_duck = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/dog_duck.gif");
	 dd_fall = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/dd_fall.gif");
	// new_message_window("HI");
	 s_ducks[0] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/s_duckw.gif");
	 s_ducks[1] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/duckhunt/s_duckr.gif");
	//  new_message_window("HIHI");
	 if(ttk_screen->w <= 220) {
	    duckhunt_lose_bg = ttk_scale_surface(duckhunt_lose_bg,dim);
	    dog_laugh[0] = ttk_scale_surface(dog_laugh[0],dim);
	    dog_laugh[1] = ttk_scale_surface(dog_laugh[1],dim);
	    duckhunt_ss = ttk_scale_surface(duckhunt_ss,dim);
	    duckhunt_bg = ttk_scale_surface(duckhunt_bg,dim);
	    dead_duck = ttk_scale_surface(dead_duck,dim);
	    dog_duck = ttk_scale_surface(dog_duck,dim);
	    dd_fall = ttk_scale_surface(dd_fall,dim);
	    s_ducks[0] = ttk_scale_surface(s_ducks[0],dim);
	    s_ducks[1] = ttk_scale_surface(s_ducks[1],dim);
	    for(i=0;i<3;i++) duck[i] = ttk_scale_surface(duck[i],dim);
	    for(i=0;i<8;i++) dogw[i] = ttk_scale_surface(dogw[i],dim);
	    mid = 87;
	 }
	 // new_message_window("BIHI");
	 if(ttk_screen->w >= 220) {
	 mid = 116;
	 dim=1.45454545;
	 duckhunt_lose_bg = ttk_scale_surface(duckhunt_lose_bg,dim);
	    dog_laugh[0] = ttk_scale_surface(dog_laugh[0],dim);
	    dog_laugh[1] = ttk_scale_surface(dog_laugh[1],dim);
	    duckhunt_ss = ttk_scale_surface(duckhunt_ss,dim);
	    duckhunt_bg = ttk_scale_surface(duckhunt_bg,dim);
	    dead_duck = ttk_scale_surface(dead_duck,dim);
	    dog_duck = ttk_scale_surface(dog_duck,dim);
	    dd_fall = ttk_scale_surface(dd_fall,dim);
	    s_ducks[0] = ttk_scale_surface(s_ducks[0],dim);
	    s_ducks[1] = ttk_scale_surface(s_ducks[1],dim);
	    for(i=0;i<3;i++) duck[i] = ttk_scale_surface(duck[i],dim);
	    for(i=0;i<8;i++) dogw[i] = ttk_scale_surface(dogw[i],dim);
	    }
	 
	  
	 ttk_surface_get_dimen (duckhunt_ss, &imgw, &imgh);
	 ttk_surface_get_dimen (duckhunt_bg, &bg_w, &bg_h);
	 ttk_surface_get_dimen (duck[0], &duck_w, &duck_h);
	 srandom((unsigned)time(NULL));
	 aimerx = WIDTH/2;
	 aimery = HEIGHT/2;
	// new_message_window("BIBIHI");
	 duckhunt_srf = ttk_new_surface (bg_w, bg_h, ttk_screen->bpp);
}

static void reset_duckhunt()
{
	 int i;
	 ssFrame = 0;
	 action = 0;
	 pause = 0;
	 score = 0;
	 dogl_frame = 0;
	 duck_frame = 0;
	 duck_timer = 0;
	 dogw_frame = 0;
	 start_duck = 0;
	 ducks_shot = 0;
	 duck_count = 0;
	 round = 0;
	 shots = 3;
	 for(i=0;i<10;i++)
	   mini_ducks[i] = 0;
	 
	 if(ttk_screen->w < 220) {
	    ttk_free_surface(duckhunt_lose_bg);
	    ttk_free_surface(dog_laugh[0]);
	    ttk_free_surface(dog_laugh[1]);
	    ttk_free_surface(duckhunt_ss);
	    ttk_free_surface(duckhunt_bg);
	    ttk_free_surface(dead_duck);
	    ttk_free_surface(dog_duck);
	    ttk_free_surface(dd_fall);
	    for(i=0;i<3;i++)
	      ttk_free_surface(duck[i]);
	    for(i=0;i<8;i++)
	      ttk_free_surface(dogw[i]);
	 }
	 ;
}

static int draw_duckhunt_highscore (TWidget *wid, ttk_surface srf) 
{
	int length;
	char str[30];
	
	sprintf (str, "High Score = %d", readHighScore("/opt/Zillae/ZacZilla/Data/duckhunt/hs"));
	length = ttk_text_width (ttk_menufont, str);
	ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(WHITE));
  ttk_text (srf, ttk_menufont, (ttk_screen->w/2)-(length/2), (ttk_screen->h-HEADER)/2, ttk_makecol(BLACK), str);
  return 0;
}

static int event_duckhunt_highscore (TWidget *this,int button) 
{
    if ((button == TTK_BUTTON_MENU) || (button == TTK_BUTTON_HOLD)) { pz_close_window(this->win); }
    if (button == TTK_BUTTON_NEXT) { writeHighScore("/opt/Zillae/ZacZilla/Data/duckhunt/hs",0); this->dirty = 1;}
    return 0;
}

      
static int down_duckhunt(TWidget *this,int button)
{
      switch (button) { 
      case TTK_BUTTON_ACTION:
      	 hit_action();
         break;
         
      case TTK_BUTTON_HOLD:
      	 if ( score > readHighScore("/opt/Zillae/ZacZilla/Data/duckhunt/hs") )
      	  writeHighScore("/opt/Zillae/ZacZilla/Data/duckhunt/hs",score); 
      	 pz_close_window(this->win);
         break;
         
     
      }
      return 0;
   }

static int timer_duckhunt(TWidget *this)
{
   	  if (pause == 2) {
   	     if (ttk_button_pressed (TTK_BUTTON_MENU)) move_up();
   	     if (ttk_button_pressed (TTK_BUTTON_PLAY)) move_down();
   	     if (ttk_button_pressed (TTK_BUTTON_PREVIOUS)) move_left();
   	     if (ttk_button_pressed (TTK_BUTTON_NEXT)) move_right();
   	     if (shots > 0 && start_duck == 1) {
   	        if (ttk_button_pressed (TTK_BUTTON_ACTION)) action = 1;
   	     }
   	     if (start_duck == 1) duck_timer++;
   	     else if (start_duck == 4) duck_timer++;
      	 wid->dirty++;
   	  }
   	  else if (pause == 1) wid->dirty++;
   return 0;
 }
      
static int button_duckhunt(TWidget *this,int button,int time)
{
		 switch (button) {
     case TTK_BUTTON_ACTION:
     	  if (shots > 0) {
     	     if (pause == 2 && action == 1) {
     	  	    action=2;
     	  	    wid->dirty++;
     	     }
     	  }
     	  break;
    
     }
     return 0;
   }
     	  	 


TWindow *new_duckhunt_window()
{
	  duckhunt_setup();
	  
	  
    window = ttk_new_window ("DuckHunt");
    ttk_window_hide_header(window);
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_duckhunt;
    wid->timer=timer_duckhunt;
    wid->down=down_duckhunt;
    wid->button=button_duckhunt;
    ttk_add_widget(window, wid);
    ttk_widget_set_timer(wid, 50);
    ttk_show_window (window);
    return 0;
}

TWindow *new_duckhuntHS_window()
{
   TWindow *duckhunths;
	  TWidget *wid;
    duckhunths = ttk_new_window ("DuckHunt Highscores");
    wid=ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_duckhunt_highscore;
    wid->down=event_duckhunt_highscore;
    ttk_add_widget (duckhunths, wid);
   ttk_show_window (duckhunths);
   return 0;
}

