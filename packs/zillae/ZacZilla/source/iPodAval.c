/*
 * Avalanche for iPod
 * By Joseph Constan (saintjimmy5294)
 * Made March, 2007
 * Uh, this program is free and stuff.
 */
/* 
 * Also, this game is not working for some reason,
 * Get error "Could not load module Avalanche: 
 * out of memory" when pz2 is starting up,
 * so feel free to try and fix it.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "pz.h"

#define RED 255, 0, 0
#define HEADER ttk_screen->wy
#define screenheight (ttk_screen->h - ttk_screen->wy)


static TWindow *window;
static TWidget *wid;
static int spike_count;
static int ball_count;

typedef struct ball_struct {
	int y;
	int x;
} ball_pos;

typedef struct player_struct {
	int x;
	int y;
} player_pos;

typedef struct spike_struct {
	int y;
	int x;
} spike_pos;

static spike_pos spike;
static player_pos player;
static ball_pos ball;
static int timer[2], yspeed;
static int spike_gone, ball_gone, mil;
static int score, highscore, game_status;
static ttk_surface player_image;
static ttk_surface spike_image;
static ttk_surface ball_image;

static void draw_player (ttk_surface srf);
static void draw_ball (ttk_surface srf);
static void draw_spike (ttk_surface srf);
static void new_spike();
static void new_ball ();
static void game_over (ttk_surface srf);
static void avalanche_timer();
static void reset_avalanche();
static int check_forspike();
static int check_forball();

static void draw_avalanche (TWidget *wid, ttk_surface srf)
{
	 ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(WHITE));
	 if((spike_gone = check_forspike()) == 1)
	 	  new_spike();
	 if((ball_gone = check_forball()) == 1)
	 	  new_ball();
	 draw_spike(srf);
	 draw_player(srf);
     draw_ball (srf);

}

static void draw_player (ttk_surface srf)
{
	 ttk_blit_image (player_image, srf, player.x, player.y);
   if((player.y >= (spike.y-38) && player.y <= (spike.y-6)) && ((player.x <= (spike.x+26) && (player.x >= (spike.x-26)))))
	 	 game_over(srf);
	 spike.y += yspeed+1;
   if((player.y >= (ball.y-29) && player.y <= (ball.y+3)) && ((player.x <= (ball.x+22) && (player.x >= (ball.x-22)))))
	 	 score += 1;
	 ball.y += yspeed;
	if(yspeed > 20)
		 yspeed = 20;
}

static void draw_spike (ttk_surface srf)
{
	ttk_blit_image (spike_image, srf, spike.x, spike.y);
}

static void draw_ball (ttk_surface srf)
{
	ttk_blit_image (ball_image, srf, ball.x, ball.y);
}

static void new_spike ()
{
	 spike.y = screenheight-screenheight;
	 spike.x = rand() % (ttk_screen->w-30);
}

static void new_ball ()
{
	 ball.y = screenheight-screenheight;
	 ball.x = rand() % (ttk_screen->w-30);
}

static int check_forspike()
{
	 if(timer[1] = 10)
	 	 return 1;
	 else
		 return 0;
}

static int check_forball()
{
	 if(timer[1] = 15)
	 	 return 1;
	 else
		 return 0;
}

static void game_over (ttk_surface srf)
{
	 int width, height;
	 char str[11];

	 ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(BLACK));
	 height = ttk_text_height (ttk_menufont);
	 pz_vector_string_center (srf, "Final Score", ttk_screen->w/2, 10, 8, 10, 1, ttk_makecol(RED));
	 sprintf(str, "%d", score);
	 pz_vector_string_center (srf, str, ttk_screen->w/2, 25, 8, 10, 1, ttk_makecol(RED));
	 
	 width = ttk_text_width (ttk_menufont, "High Score:");
	 ttk_text (srf, ttk_menufont, (ttk_screen->w/2)-(width/2), screenheight/2, ttk_makecol(WHITE), "High Score:");
	 sprintf(str, "%d", 0);
	 width = ttk_text_width (ttk_menufont, str);
	 ttk_text (srf, ttk_menufont, (ttk_screen->w/2)-(width/2), ((screenheight/2)+height)+5, ttk_makecol(WHITE), str);
	 game_status = 1;
}

static int timer_avalanche(TWidget *this)
{
	 timer[0]++;
   timer[1]++;
   if(mil < 20) {
     if(timer[0]==20) {
   	   mil--;
   	   ttk_widget_set_timer(this, mil);
   	   timer[0]=0;
   	 }
   }
   this->dirty++;
}

static void reset_avalanche()
{
	 player.x = (ttk_screen->w/2)-7;
	 player.y = (ttk_screen->h)+23;
	 timer[0] = 0;
	 timer[1] = 0;
	 spike.y = -8;
	 ball.y = -8;
	 spike_count=1;
	 ball_count=1;
	 score = 0;
	 yspeed = 5;
	 mil = 50;
	 game_status = 0;
}

static int scroll_avalanche (TWidget *this,int dir)
{

     TTK_SCROLLMOD( dir, 1);
      if( dir > 0){
      	if(player.x <= (ttk_screen->w-25))
      		player.x+=10;
      }
      else {
      	if(player.x >= 5)
      		player.x-=10;
      }
      return TTK_EV_CLICK;
    }
static int down_avalanche(TWidget *this,int button)
{
  
      switch (button) {
      case TTK_BUTTON_ACTION:
      	 if(game_status == 1) {
      	 	 reset_avalanche();
      	 	 this->dirty++;
      	 }
         break;

      case TTK_BUTTON_MENU:
      	 
      	 pz_close_window(this->win);
         break;
         
  
      }
      return 0;
   }



TWindow *new_avalanche_window()
{
	  reset_avalanche();
	  srandom((unsigned)time(NULL));

	
    window = ttk_new_window ("avalanche");
    wid = ttk_new_widget(0,0);
    wid->focusable=1;
    wid->draw=draw_avalanche;
    wid->timer=timer_avalanche;
    wid->scroll=scroll_avalanche;
    wid->down=down_avalanche;
    
    ttk_add_widget(window, wid);
    ttk_widget_set_timer(wid, mil);
    	player_image = ttk_load_image("Data/Alal/player.PNG");
//	ttk_surface_get_dimen (player_image, &imgw, &imgh);
		
	spike_image = ttk_load_image ("Data/Alal/spike.png");
	//ttk_surface_get_dimen (spike_image, &imgw, &imgh);
	
	ball_image = ttk_load_image ("Data/Alal/ball.png");
//	ttk_surface_get_dimen (ball_image, &imgw, &imgh);
	ttk_show_window(window);
    return 0;
}


/*static void init_avalanche()
{
    module = pz_register_module ("avalanche", NULL);
    pz_menu_add_action ("/Extras/Games/avalanche", new_avalanche_window);

	player_image = ttk_load_image(pz_module_get_datapath(module, "player.PNG"));
	ttk_surface_get_dimen (player_image, &imgw, &imgh);
		
	spike_image = ttk_load_image (pz_module_get_datapath (module, "spike.png"));
	ttk_surface_get_dimen (spike_image, &imgw, &imgh);
	
	ball_image = ttk_load_image (pz_module_get_datapath (module, "ball.png"));
	ttk_surface_get_dimen (ball_image, &imgw, &imgh);

}

PZ_MOD_INIT (init_avalanche)*/
