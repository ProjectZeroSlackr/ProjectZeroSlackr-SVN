/*
 * Copyright (C) 2007 Armon Khosravi, ported to ttk/pz2/ipodlinux (wrote level loading)
 * Copyright (C) 2005, 2006 Ben Basha (Paprica)
 *
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: brickmania.c,v 1.26 2006-08-03 20:40:37 bagder Exp $
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
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "pz.h"

#define false 0
#define true (!false)
#define CYCLETIME 30
#define XOFS ((LCD_WIDTH-220)/2)
#define YOFS ((LCD_HEIGHT-176)/2)

#define PAD_WIDTH 40
#define PAD_HEIGHT 5
#define PAD_POS_Y LCD_HEIGHT - 7
#define BALL 5
#define HALFBALL 3
#define BMPHEIGHT_powerup 6
#define BMPWIDTH_powerup 10

#define MAX_POINTS 200000 /* i dont think it needs to be more */
#define MAX_BALLS 10
#define MENU_LENGTH 4


static TWindow *window;
static TWidget *wid;

static ttk_surface brickmania_ball;
static ttk_surface brickmania_break;
static ttk_surface brickmania_bricks;
static ttk_surface brickmania_gameover;
static ttk_surface brickmania_menu_bg;
static ttk_surface brickmania_menu_items;
static ttk_surface brickmania_pads;
static ttk_surface brickmania_powerups;

static ttk_surface brickm_srf;

enum menu_items {
    BM_START,
    BM_SEL_START,
    BM_RESUME,
    BM_SEL_RESUME,
    BM_NO_RESUME,
    BM_HELP,
    BM_SEL_HELP,
    BM_QUIT,
    BM_SEL_QUIT,
};

short levels_num = 29;

static unsigned char level[8][10];

static short pad_pos_x;
static short x[MAX_BALLS],y[MAX_BALLS];
static short life;
static short start_game,con_game;
static short pad_type;
static int score=0,vscore=0;
static short flip_sides=false;
static short cur_level=1;
static short brick_on_board=0;
static short used_balls=1;
static int highscore;
static short frame=0, when=0, cur=0;
static short maxY=170;
static short maxX=230;
static short xoffset=0;
static short yoffset=0;

static short LCD_WIDTH;
static short LCD_HEIGHT;
static short BMPHEIGHT_menu;
static short BMPWIDTH_menu;
static short BMPXOFS_start;
static short BMPYOFS_start;
static short BMPXOFS_resume;
static short BMPYOFS_resume;
static short BMPXOFS_help;
static short BMPYOFS_help;
static short BMPXOFS_quit;
static short BMPYOFS_quit;
static short HIGHSCORE_XPOS;
static short HIGHSCORE_YPOS;
static short STRINGPOS_finsh;
static short STRINGPOS_congrats;
static short STRINGPOS_navi;
static short STRINGPOS_flipsides;
static short LEFTMARGIN;
static short TOPMARGIN;
static short MENU_BMPHEIGHT;
static short MENU_BMPWIDTH;
static short BRICK_HEIGHT;
static short BRICK_WIDTH;

typedef struct cube {
    short powertop;
    short power;
    char poweruse;
    char used;
    short color;
    short hits;
    short hiteffect;
} cube;
cube brick[80];

typedef struct balls {
    short pos_x;
    short pos_y;
    short y;
    short tempy;
    short x;
    short tempx;
    short glue;
} balls;

balls ball[MAX_BALLS];

typedef struct sfire {
    short top;
    short left;
} sfire;
sfire fire[30];


static int pad_check(int ballxc, int mode, int pon ,int ballnum);
static int load_level (int lev);
static int fire_space();
static int load_score();
static int save_score();
static void int_game (int new_game);
static void game_menu();
static void game_loop();
static void help();


static void draw_brickm (TWidget *wid, ttk_surface srf)
{
    ttk_fillrect (srf, 0, 0, ttk_screen->w, ttk_screen->h, ttk_makecol(DKGREY));

	  switch(frame) {
	  	 case 0: game_menu(); break;
	  	 case 1: game_loop(); break;
	  	 case 2: help(); break;
	  }
	  
	  ttk_blit_image (brickm_srf, srf, (ttk_screen->w/2)-(LCD_WIDTH/2), (ttk_screen->h/2)-(LCD_HEIGHT/2));
}


static int load_level (int lev)
{
	 char filename[25];
	 FILE *fp;
	 
	 snprintf(filename, sizeof(filename), "/opt/Zillae/ZacZilla/Data/levels/%d.lev", lev);
   if((fp = fopen(filename, "r")) == NULL) {
	 	  printf("Could not load level!");
	 	  
	 	  return 0;
	 }
	 
	 fread(&level, sizeof(level), 1, fp);
	 
	 /*for(i=0;i<8;i++) {
	 	  for(j=0;j<10;j++) {
	 	  	printf("%2d ", level[i][j]);
	 	  }
	 	  putchar('\n');
	 }*/
	 
	 fclose(fp);
	 return 1;
}
	 

static int save_score()
{
	 FILE *file;
	 
	 file = fopen("/opt/Zillae/ZacZilla/Data/brickscore.conf", "w");
	 if(file == NULL) {
	 	  pz_error("Couldn't save score!");
	 	  return 0;
	 }
	 
	 fwrite(&highscore, sizeof(highscore), 1, file);
	 
	 fclose(file);
	 return 1;
}


static int load_score()
{
	 FILE *file;
	 
	 file = fopen("/opt/Zillae/ZacZilla/Data/brickscore.conf", "a+");
	 if(file == NULL) {
	 	  pz_error("Couldn't load score!");
	 	  return 0;
	 }
	 
	 fread(&highscore, sizeof(highscore), 1, file);
	 
	 fclose(file);
	 return 1;
}


static void int_game (int new_game)
{
    short i,j;

    pad_pos_x=LCD_WIDTH/2-PAD_WIDTH/2;

    for(i=0;i<MAX_BALLS;i++) {
        ball[i].x=0;
        ball[i].y=0;
        ball[i].tempy=0;
        ball[i].tempx=0;
        ball[i].pos_y=PAD_POS_Y-BALL;
        ball[i].pos_x=pad_pos_x+(PAD_WIDTH/2)-2;
        ball[i].glue=false;
    }

    used_balls=1;
    start_game =1;
    con_game =0;
    pad_type=0;

    flip_sides=false;

    if (new_game==1)
        brick_on_board=0;

    for(i=0;i<=7;i++) {
        for(j=0;j<=9;j++) {
            brick[i*10+j].poweruse=(level[i][j]==0?0:1);
            if (i*10+j<=30)
                fire[i*10+j].top=-8;
            if (new_game==1) {
                brick[i*10+j].power=rand()%25;
                /* +8 make the game with less powerups */

                brick[i*10+j].hits=level[i][j]>=10?
                    level[i][j]/16-1:0;
                brick[i*10+j].hiteffect=0;
                brick[i*10+j].powertop=TOPMARGIN+i*BRICK_HEIGHT+BRICK_HEIGHT;
                brick[i*10+j].used=(level[i][j]==0?0:1);
                brick[i*10+j].color=(level[i][j]>=10?
                                     level[i][j]%16:
                                     level[i][j])-1;
                if (level[i][j]!=0)
                    brick_on_board++;
            }
        }
    }
}


/*void sleep (int secs)
{
    bool done=false;
    char s[20];
    int count=0;

    while (!done) {

        if (vscore<score) {
            vscore++;
            rb->snprintf(s, sizeof(s), "%d", vscore);
            rb->lcd_getstringsize(s, &sw, &w);
            rb->lcd_putsxy(LCD_WIDTH/2-sw/2, 2, s);
            rb->lcd_update_rect(0,0,LCD_WIDTH,w+2);
        } else {
            if (count==0)
                count=*rb->current_tick+HZ*secs;
            if (*rb->current_tick>=count)
                done=true;
        }
        rb->yield();
    }
}*/


static void game_menu()
{
    short sw, w, ttksw = ttk_screen->w;
    char str[10];
    
    //short i;
    
    ttk_blit_image (brickmania_menu_bg, brickm_srf, 0, 0);
    //for(i=0;i<MENU_LENGTH;i++) {
       if (cur==0)
          ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_START, 
                   MENU_BMPWIDTH, MENU_BMPHEIGHT, brickm_srf, BMPXOFS_start, BMPYOFS_start);
       else
          ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_START, 
                   MENU_BMPWIDTH, MENU_BMPHEIGHT, brickm_srf, BMPXOFS_start, BMPYOFS_start);

       if (when==1) {
          if (cur==1)
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_RESUME, 
                      (ttksw>=220)?MENU_BMPWIDTH-16:MENU_BMPWIDTH-13, MENU_BMPHEIGHT-3, brickm_srf, BMPXOFS_resume, BMPYOFS_resume);
          else
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_RESUME, 
                      (ttksw>=220)?MENU_BMPWIDTH-16:MENU_BMPWIDTH-13, MENU_BMPHEIGHT-3, brickm_srf, BMPXOFS_resume, BMPYOFS_resume);

     } else {
          ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_NO_RESUME, 
                   (ttksw>=220)?MENU_BMPWIDTH-16:MENU_BMPWIDTH-13, MENU_BMPHEIGHT-3, brickm_srf, BMPXOFS_resume, BMPYOFS_resume);
       }

       if(ttksw >= 220) {
          if (cur==2)
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_HELP, 
                      MENU_BMPWIDTH-75, MENU_BMPHEIGHT-1, brickm_srf, BMPXOFS_help, BMPYOFS_help);
          else
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_HELP, 
                      MENU_BMPWIDTH-75, MENU_BMPHEIGHT-1, brickm_srf, BMPXOFS_help, BMPYOFS_help);
          
          if (cur==3)
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_QUIT, 
                      MENU_BMPWIDTH-79, MENU_BMPHEIGHT-1, brickm_srf, BMPXOFS_quit, BMPYOFS_quit);
          else
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_QUIT, 
                      MENU_BMPWIDTH-79, MENU_BMPHEIGHT-1, brickm_srf, BMPXOFS_quit, BMPYOFS_quit);
       }
       else {
          if (cur==2)
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_HELP, 
                      MENU_BMPWIDTH-61, MENU_BMPHEIGHT-2, brickm_srf, BMPXOFS_help, BMPYOFS_help);
          else
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_HELP, 
                      MENU_BMPWIDTH-61, MENU_BMPHEIGHT-2, brickm_srf, BMPXOFS_help, BMPYOFS_help);

          if (cur==3)
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_SEL_QUIT, 
                      MENU_BMPWIDTH-64, MENU_BMPHEIGHT-2, brickm_srf, BMPXOFS_quit, BMPYOFS_quit);
          else
             ttk_blit_image_ex (brickmania_menu_items, 0, MENU_BMPHEIGHT * BM_QUIT, 
                      MENU_BMPWIDTH-64, MENU_BMPHEIGHT-2, brickm_srf, BMPXOFS_quit, BMPYOFS_quit);
       }
    //}
        
    ttk_text (brickm_srf, ttk_menufont, HIGHSCORE_XPOS, HIGHSCORE_YPOS, ttk_makecol(WHITE), "High Score");
    snprintf(str, sizeof(str), "%d", highscore);
    sw = ttk_text_width (ttk_menufont, "High Score");
    w = ttk_text_width (ttk_menufont, str);
    ttk_text (brickm_srf, ttk_menufont, HIGHSCORE_XPOS+sw/2-w/2, HIGHSCORE_YPOS+11, ttk_makecol(WHITE), str);

    //rb->lcd_update();
}


static void help()
{
    short w,h;
    /* set the maximum x and y in the helpscreen
       dont forget to update, if you change text */
    
    ttk_fillrect (brickm_srf, 0, 0, LCD_WIDTH, LCD_HEIGHT, ttk_makecol(BLACK));
        
    w = ttk_text_width (ttk_menufont, "BrickMania");
    h = ttk_text_height (ttk_menufont);
    maxY = 17*(h+2)+yoffset;
    ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-w/2+xoffset, 1+yoffset, ttk_makecol(WHITE), "BrickMania");
                       
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 1*(h+2)+yoffset, ttk_makecol(245,0,0), "Aim");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 2*(h+2)+yoffset, ttk_makecol(WHITE), "destroy all the bricks by bouncing");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 3*(h+2)+yoffset, ttk_makecol(WHITE), "the ball of them using the paddle.");
        
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 5*(h+2)+yoffset, ttk_makecol(245,0,0), "Controls");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 6*(h+2)+yoffset, ttk_makecol(WHITE), "< & > Move the paddle");
        
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 7*(h+2)+yoffset, ttk_makecol(WHITE), "SELECT  Releases the ball/Fire!");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 8*(h+2)+yoffset, ttk_makecol(WHITE), "STOP  Opens menu/Quit");
        
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 10*(h+2)+yoffset, ttk_makecol(245,0,0), "Specials");
        
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 11*(h+2)+yoffset, ttk_makecol(WHITE), "N Normal:returns paddle to normal");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 12*(h+2)+yoffset, ttk_makecol(WHITE), "D DIE!:loses a life");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 13*(h+2)+yoffset, ttk_makecol(WHITE), "L Life:gains a life/power up");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 14*(h+2)+yoffset, ttk_makecol(WHITE), "F Fire:allows you to shoot bricks");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 15*(h+2)+yoffset, ttk_makecol(WHITE), "G Glue:ball sticks to paddle");
    ttk_text (brickm_srf, ttk_menufont, 1+xoffset, 16*(h+2)+yoffset, ttk_makecol(WHITE), "B Ball:Generates Another Ball");
}


static int pad_check(int ballxc, int mode, int pon ,int ballnum)
{
    /* pon: positive(1) or negative(0) */

    if (mode==0) {
        if (pon == 0)
            return -ballxc;
        else
            return ballxc;
    } else {
        if (ball[ballnum].x > 0)
            return ballxc;
        else
           return ballxc*-1;
    }
}


static int fire_space()
{
    short t;
    for(t=0;t<=30;t++)
        if (fire[t].top+7 < 0)
            return t;

    return 0;
}


static void game_loop()
{
	 short j,i,k,bricky,brickx;
   char s[30];
   short num_count=10;
   short sw;
   //short sec_count=0, end;
   
   if (life >= 0) {
      ttk_fillrect (brickm_srf, 0, 0, LCD_WIDTH, LCD_HEIGHT, ttk_makecol(BLACK));
      if (flip_sides) {
         //if (*rb->current_tick>=sec_count) {
            //sec_count=*rb->current_tick+HZ;
            if (num_count!=0)
               num_count--;
            else
               flip_sides=false;
         //}
         snprintf(s, sizeof(s), "%d", num_count);
         sw = ttk_text_width (ttk_menufont, s);
         ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-2, STRINGPOS_flipsides, ttk_makecol(WHITE), s);
      }

      snprintf(s, sizeof(s), "Life: %d", life);
      ttk_text (brickm_srf, ttk_menufont, 2, 2, ttk_makecol(WHITE), s);

      snprintf(s, sizeof(s), "Level %d", cur_level);
      sw = ttk_text_width (ttk_menufont, s);
      ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH-sw-2, 2, ttk_makecol(WHITE), s);

      if (vscore<score) vscore++;
      snprintf(s, sizeof(s), "%d", vscore);
      sw = ttk_text_width (ttk_menufont, s);
      ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-sw/2, 2, ttk_makecol(WHITE), s);

      /* continue game */
      if (con_game== 1 && start_game!=1) {
         snprintf(s, sizeof(s), "Press SELECT To Continue");
         sw = ttk_text_width (ttk_menufont, s);
         ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-sw/2, STRINGPOS_navi, ttk_makecol(WHITE), s);
         //sec_count=*rb->current_tick+HZ;
      }

      /* draw the ball */
      for(i=0;i<used_balls;i++)
         ttk_blit_image (brickmania_ball, brickm_srf, ball[i].pos_x, ball[i].pos_y);

      if (brick_on_board==0)
         brick_on_board--;

      /* if the pad is fire */
      for(i=0;i<=30;i++) {
         if (fire[i].top+7>0) {
            if (con_game!=1)
               fire[i].top-=4;
            ttk_line (brickm_srf, fire[i].left, fire[i].top, fire[i].left, fire[i].top+7, ttk_makecol(WHITE));
         }
      }

      /* the bricks */
      for (i=0;i<=7;i++) {
         for (j=0;j<=9;j++) {
            if (brick[i*10+j].power<7) {
               if (brick[i*10+j].poweruse==2) {
                  if (con_game!=1)
                     brick[i*10+j].powertop+=2;
                  ttk_blit_image_ex (brickmania_powerups, 0, BMPHEIGHT_powerup*brick[i*10+j].power,
                       BMPWIDTH_powerup, BMPHEIGHT_powerup, brickm_srf, LEFTMARGIN+j*BRICK_WIDTH+
                       (BRICK_WIDTH/2-BMPWIDTH_powerup/2), brick[i*10+j].powertop);
               }
            }

            if ((pad_pos_x<LEFTMARGIN+j*BRICK_WIDTH+5 && pad_pos_x+PAD_WIDTH>LEFTMARGIN+j*BRICK_WIDTH+5) &&
            brick[i*10+j].powertop+6>=PAD_POS_Y && brick[i*10+j].poweruse==2) {
               switch(brick[i*10+j].power) {
                  case 0:
                     life++;
                     score+=50;
                     break;
                  case 1:
                     life--;
                     if (life>=0) {
                        int_game(0);
                        ttk_delay(2000);
                        
                     }
                     break;
                  case 2:
                     score+=34;
                     pad_type=1;
                     break;
                  case 3:
                     score+=47;
                     pad_type=2;
                     for(k=0;k<used_balls;k++)
                        ball[k].glue=false;
                     break;
                  case 4:
                     score+=23;
                     pad_type=0;
                     for(k=0;k<used_balls;k++)
                        ball[k].glue=false;
                     flip_sides=false;
                     break;
                  case 5:
                     score+=23;
                     //sec_count=*rb->current_tick+HZ;
                     num_count=10;
                     flip_sides=!flip_sides;
                     break;
                  case 6:
                     score+=23;
                     used_balls++;
                     ball[used_balls-1].x= rand()%1 == 0 ? -1 : 1;
                     ball[used_balls-1].y= -4;
                     break;
               }
               brick[i*10+j].poweruse=1;
            }

            if (brick[i*10+j].powertop>PAD_POS_Y)
               brick[i*10+j].poweruse=1;
                    
            brickx=LEFTMARGIN+j*BRICK_WIDTH;
            bricky=TOPMARGIN+i*BRICK_HEIGHT;
            if (pad_type==2) {
               for (k=0;k<=30;k++) {
                  if (fire[k].top+7>0) {
                     if (brick[i*10+j].used==1 && (fire[k].left+1 >= brickx &&
                     fire[k].left+1 <= brickx+BRICK_WIDTH) && (bricky+BRICK_HEIGHT>fire[k].top)) {
                        score+=13;
                        fire[k].top=-16;
                        if (brick[i*10+j].hits > 0) {
                           brick[i*10+j].hits--;
                           brick[i*10+j].hiteffect++;
                           score+=3;
                        }
                        else {
                           brick[i*10+j].used=0;
                           if (brick[i*10+j].power!=10)
                              brick[i*10+j].poweruse=2;
                           brick_on_board--;
                        }
                     }
                  }
               }
            }
                    
            if (brick[i*10+j].used==1) {
               ttk_blit_image_ex (brickmania_bricks, 0, BRICK_HEIGHT*brick[i*10+j].color,
                BRICK_WIDTH, BRICK_HEIGHT, brickm_srf, LEFTMARGIN+j*BRICK_WIDTH, TOPMARGIN+i*BRICK_HEIGHT);
               if (brick[i*10+j].hiteffect>0)
                  ttk_blit_image_ex (brickmania_break, 0, BRICK_HEIGHT*brick[i*10+j].hiteffect,
                   BRICK_WIDTH, BRICK_HEIGHT, brickm_srf, LEFTMARGIN+j*BRICK_WIDTH, TOPMARGIN+i*BRICK_HEIGHT);
            }

            for(k=0;k<used_balls;k++) {
               if (ball[k].pos_y <100) {
                  if (brick[i*10+j].used==1) {
                     if ((ball[k].pos_x+ball[k].x+HALFBALL >= brickx && ball[k].pos_x+ball[k].x+HALFBALL <=
                     brickx+BRICK_WIDTH) && ((bricky-4<ball[k].pos_y+BALL && bricky>ball[k].pos_y+BALL) ||
                     (bricky+4>ball[k].pos_y+BALL+BALL && bricky<ball[k].pos_y+BALL+BALL)) && (ball[k].y >0)) {
                        ball[k].tempy=bricky-ball[k].pos_y-BALL;
                     }
                     else if ((ball[k].pos_x+ball[k].x+HALFBALL >= brickx && ball[k].pos_x+ball[k].x+HALFBALL <=
                     brickx+BRICK_WIDTH) && ((bricky+BRICK_HEIGHT+4>ball[k].pos_y && bricky+BRICK_HEIGHT<ball[k].pos_y) ||
                     (bricky+BRICK_HEIGHT-4<ball[k].pos_y-BALL && bricky+BRICK_HEIGHT>ball[k].pos_y-BALL)) && (ball[k].y <0)) {
                        ball[k].tempy=-(ball[k].pos_y-(bricky+BRICK_HEIGHT));
                     }

                     if ((ball[k].pos_y+HALFBALL >= bricky && ball[k].pos_y+HALFBALL <= bricky+BRICK_HEIGHT) &&
                     ((brickx-4<ball[k].pos_x+BALL && brickx>ball[k].pos_x+BALL) || (brickx+4>ball[k].pos_x+BALL+BALL &&
                     brickx<ball[k].pos_x+BALL+BALL)) && (ball[k].x >0)) {
                        ball[k].tempx=brickx-ball[k].pos_x-BALL;
                     }
                     else if ((ball[k].pos_y+ball[k].y+HALFBALL >= bricky && ball[k].pos_y+ball[k].y+HALFBALL <=
                     bricky+BRICK_HEIGHT) && ((brickx+BRICK_WIDTH+4>ball[k].pos_x && brickx+BRICK_WIDTH<ball[k].pos_x) ||
                     (brickx+BRICK_WIDTH-4<ball[k].pos_x-BALL && brickx+BRICK_WIDTH>ball[k].pos_x- BALL)) && (ball[k].x <0)) {
                        ball[k].tempx=-(ball[k].pos_x-(brickx+BRICK_WIDTH));
                     }

                     if ((ball[k].pos_x+HALFBALL >= brickx && ball[k].pos_x+HALFBALL <= brickx+BRICK_WIDTH) &&
                     ((bricky+BRICK_HEIGHT==ball[k].pos_y) || (bricky+BRICK_HEIGHT-6<=ball[k].pos_y &&
                     bricky+BRICK_HEIGHT>ball[k].pos_y)) && (ball[k].y <0)) { /* bottom line */
                        if (brick[i*10+j].hits > 0) {
                           brick[i*10+j].hits--;
                           brick[i*10+j].hiteffect++;
                           score+=2;
                        }
                        else {
                           brick[i*10+j].used=0;
                           if (brick[i*10+j].power!=10)
                              brick[i*10+j].poweruse=2;
                        }

                        ball[k].y = ball[k].y*-1;
                     }
                     else if ((ball[k].pos_x+HALFBALL >= brickx && ball[k].pos_x+HALFBALL <= brickx+BRICK_WIDTH) &&
                     ((bricky==ball[k].pos_y+BALL) || (bricky+6>=ball[k].pos_y+BALL &&
                     bricky<ball[k].pos_y+BALL)) && (ball[k].y >0)) { /* top line */
                        if (brick[i*10+j].hits > 0) {
                           brick[i*10+j].hits--;
                           brick[i*10+j].hiteffect++;
                           score+=2;
                        }
                        else {
                           brick[i*10+j].used=0;
                           if (brick[i*10+j].power!=10)
                              brick[i*10+j].poweruse=2;
                        }

                        ball[k].y = ball[k].y*-1;
                     }

                     if ((ball[k].pos_y+HALFBALL >= bricky && ball[k].pos_y+HALFBALL <= bricky+BRICK_HEIGHT) &&
                     ((brickx==ball[k].pos_x+BALL) || (brickx+6>=ball[k].pos_x+BALL &&
                     brickx<ball[k].pos_x+BALL)) && (ball[k].x > 0)) { /* left line */
                        if (brick[i*10+j].hits > 0) {
                           brick[i*10+j].hits--;
                           brick[i*10+j].hiteffect++;
                           score+=2;
                        }
                        else {
                           brick[i*10+j].used=0;
                           if (brick[i*10+j].power!=10)
                              brick[i*10+j].poweruse=2;
                        }
                        ball[k].x = ball[k].x*-1;

                     }
                     else if ((ball[k].pos_y+HALFBALL >= bricky && ball[k].pos_y+HALFBALL <= bricky+BRICK_HEIGHT) &&
                     ((brickx+BRICK_WIDTH==ball[k].pos_x) || (brickx+BRICK_WIDTH-6<=ball[k].pos_x &&
                     brickx+BRICK_WIDTH>ball[k].pos_x)) && (ball[k].x < 0)) { /* Right line */
                        if (brick[i*10+j].hits > 0) {
                           brick[i*10+j].hits--;
                           brick[i*10+j].hiteffect++;
                           score+=2;
                        }
                        else {
                           brick[i*10+j].used=0;
                           if (brick[i*10+j].power!=10)
                              brick[i*10+j].poweruse=2;
                        }

                        ball[k].x = ball[k].x*-1;
                     }

                     if (brick[i*10+j].used==0) {
                        brick_on_board--;
                        score+=8;
                     }
                  }
               }
            } /* for k */
         } /* for j */
      } /* for i */

      /* draw the pad */
      ttk_blit_image_ex (brickmania_pads, 0, pad_type*PAD_HEIGHT, PAD_WIDTH, PAD_HEIGHT, 
       brickm_srf, pad_pos_x, PAD_POS_Y);

      for(k=0;k<used_balls;k++) {
         if ((ball[k].pos_x >= pad_pos_x && ball[k].pos_x <= pad_pos_x+PAD_WIDTH) && (PAD_POS_Y-4<ball[k].pos_y+BALL &&
         PAD_POS_Y>ball[k].pos_y+BALL) && (ball[k].y >0))
            ball[k].tempy=PAD_POS_Y-ball[k].pos_y-BALL;
         else if ((4>ball[k].pos_y && 0<ball[k].pos_y) && (ball[k].y <0))
            ball[k].tempy=-ball[k].pos_y;
         if ((LCD_WIDTH-4<ball[k].pos_x+BALL && LCD_WIDTH>ball[k].pos_x+BALL) && (ball[k].x >0))
            ball[k].tempx=LCD_WIDTH-ball[k].pos_x-BALL;
         else if ((4>ball[k].pos_x && 0<ball[k].pos_x) && (ball[k].x <0))
            ball[k].tempx=-ball[k].pos_x;

         /* top line */
         if (ball[k].pos_y<= 0)
            ball[k].y = ball[k].y*-1;
                /* bottom line */
         else if (ball[k].pos_y+BALL >= LCD_HEIGHT) {
            if (used_balls>1) {
               used_balls--;
               ball[k].pos_x = ball[used_balls].pos_x;
               ball[k].pos_y = ball[used_balls].pos_y;
               ball[k].y = ball[used_balls].y;
               ball[k].tempy = ball[used_balls].tempy;
               ball[k].x = ball[used_balls].x;
               ball[k].tempx = ball[used_balls].tempx;
               ball[k].glue = ball[used_balls].glue;

               ball[used_balls].x=0;
               ball[used_balls].y=0;
               ball[used_balls].tempy=0;
               ball[used_balls].tempx=0;
               ball[used_balls].pos_y=PAD_POS_Y-BALL;
               ball[used_balls].pos_x=pad_pos_x+(PAD_WIDTH/2)-2;

               k--;
               continue;
            } 
            else {
               life--;
               if (life>=0) {
                  int_game(0);
                  ttk_delay(2000);
               }
            }
         }

         /* left line ,right line */
         if ((ball[k].pos_x <= 0) || (ball[k].pos_x+BALL >= LCD_WIDTH)) {
            ball[k].x = ball[k].x*-1;
            ball[k].pos_x = ball[k].pos_x <= 0 ? 0 : LCD_WIDTH-BALL;
         }

         if ((ball[k].pos_y+BALL >= PAD_POS_Y && (ball[k].pos_x >= pad_pos_x &&
         ball[k].pos_x <= pad_pos_x+PAD_WIDTH)) && start_game != 1 && !ball[k].glue) {
            if ((ball[k].pos_x+HALFBALL >= pad_pos_x && ball[k].pos_x+HALFBALL <= pad_pos_x+(PAD_WIDTH/2/4)) ||
            (ball[k].pos_x +HALFBALL>= pad_pos_x+(PAD_WIDTH-(PAD_WIDTH/2/4)) &&
            ball[k].pos_x+HALFBALL <= pad_pos_x+PAD_WIDTH)) {      
               ball[k].y = -2;
               if (ball[k].pos_x != 0 && ball[k].pos_x+BALL!=LCD_WIDTH)
                  ball[k].x = pad_check(6,0,ball[k].pos_x+2<=pad_pos_x+(PAD_WIDTH/2)?0:1,k);
            }
            else if ((ball[k].pos_x+HALFBALL >= pad_pos_x+(PAD_WIDTH/2/4) && ball[k].pos_x+HALFBALL <=
            pad_pos_x+2*(PAD_WIDTH/2/4)) || (ball[k].pos_x+HALFBALL >= pad_pos_x+(PAD_WIDTH-2*(PAD_WIDTH/2/4)) &&
            ball[k].pos_x+HALFBALL <= pad_pos_x+(PAD_WIDTH-(PAD_WIDTH/2/4)) )) {
               ball[k].y = -3;
               if (ball[k].pos_x != 0 && ball[k].pos_x+BALL!=LCD_WIDTH)
                  ball[k].x = pad_check(4,0,ball[k].pos_x+2<= pad_pos_x+(PAD_WIDTH/2)? 0:1,k);

            }
            else if ((ball[k].pos_x+HALFBALL >= pad_pos_x+2*(PAD_WIDTH/2/4) && ball[k].pos_x+HALFBALL <=
            pad_pos_x+3*(PAD_WIDTH/2/4)) || (ball[k].pos_x+2 >= pad_pos_x+(PAD_WIDTH-3*(PAD_WIDTH/2/4)) &&
            ball[k].pos_x+2 <= pad_pos_x+ ((PAD_WIDTH/2)-2*(PAD_WIDTH/2/4)) )) {
               ball[k].y = -4;
               if (ball[k].pos_x != 0 && ball[k].pos_x+BALL!=LCD_WIDTH)
                  ball[k].x = pad_check(3,0,ball[k].pos_x+2<= pad_pos_x+(PAD_WIDTH/2)? 0:1,k);

            }
            else if ((ball[k].pos_x+HALFBALL >= pad_pos_x+3*(PAD_WIDTH/2/4) && ball[k].pos_x+HALFBALL <=
            pad_pos_x+4*(PAD_WIDTH/2/4)-2) || (ball[k].pos_x+2 >= pad_pos_x+(PAD_WIDTH/2+2) &&
            ball[k].pos_x+2 <= pad_pos_x+(PAD_WIDTH-3*(PAD_WIDTH/2/4)) )) {
               ball[k].y = -4;
               if (ball[k].pos_x != 0 && ball[k].pos_x+BALL!=LCD_WIDTH)
                  ball[k].x = pad_check(2,1,0,k);
            }
            else {
               ball[k].y = -4;
            }
         }

         if (!ball[k].glue) {
            ball[k].pos_x+=ball[k].tempx!=0?ball[k].tempx:ball[k].x;
            ball[k].pos_y+=ball[k].tempy!=0?ball[k].tempy:ball[k].y;
            ball[k].tempy=0;
            ball[k].tempx=0;
         }

         if (ball[k].pos_y+5 >= PAD_POS_Y && (pad_type==1 && !ball[k].glue) && (ball[k].pos_x >= pad_pos_x &&
         ball[k].pos_x <= pad_pos_x+PAD_WIDTH)) {
            ball[k].y=0;
            ball[k].pos_y=PAD_POS_Y-BALL;
            ball[k].glue=true;
         }
      } /* for k */
            
      //rb->lcd_update();

      if (brick_on_board < 0) {
         if (cur_level<levels_num) {
            cur_level++;
            load_level(cur_level);
            score+=100;
            int_game(1);
            ttk_delay(2000);
         }
         else {
         	  sw = ttk_text_width (ttk_menufont, "Congratulations!");
         	  ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-sw/2, STRINGPOS_congrats, ttk_makecol(WHITE), "Congratulations!");
         	  sw = ttk_text_width (ttk_menufont, "You have finished the game!");
         	  ttk_text (brickm_srf, ttk_menufont, LCD_WIDTH/2-sw/2, STRINGPOS_finsh, ttk_makecol(WHITE), "You have finished the game!");
            vscore=score;
            //rb->lcd_update();
            if (score>highscore) {
               ttk_delay(2000);
               highscore=score;
               save_score();
               new_message_window("New High Score");
            }
            else {
               ttk_delay(3000);
            }
            
            when=0;
            frame=0;
            wid->dirty++;
         }
      }
   }
   else {
      ttk_blit_image (brickmania_gameover, brickm_srf, LCD_WIDTH/2-55,LCD_HEIGHT-87);
      //rb->lcd_update();
      if (score>highscore) {
         ttk_delay(2000);
         highscore=score;
         save_score();
         new_message_window("New High Score");
      } 
      else {
         ttk_delay(3000);
      }

      for(k=0;k<used_balls;k++) {
         ball[k].x=0;
         ball[k].y=0;
      }

      when=0;
      frame=0;
      wid->dirty++;
   }
   /*if (end > *rb->current_tick)
      rb->sleep(end-*rb->current_tick);
   else
      rb->yield();*/
}


static void brickm_start()
{
	 
	 if(ttk_screen->w >= 220) {
	 	  LCD_WIDTH = 220;
	 	  LCD_HEIGHT = 176;
	    BRICK_HEIGHT = 8;
	    BRICK_WIDTH = 21;
	    LEFTMARGIN = 5;
	    TOPMARGIN = 30;
	    MENU_BMPHEIGHT = 20;
	    MENU_BMPWIDTH = 112;
	    BMPHEIGHT_menu = 176;
	    BMPWIDTH_menu = 220;
	    BMPXOFS_start = (55+XOFS);
	    BMPYOFS_start = (78+YOFS);
	    BMPXOFS_resume = (62+XOFS);
	    BMPYOFS_resume = (100+YOFS);
	    BMPXOFS_help = (92+XOFS);
	    BMPYOFS_help = (118+YOFS);
	    BMPXOFS_quit = (93+XOFS);
	    BMPYOFS_quit = (138+YOFS);
	    HIGHSCORE_XPOS = (7+XOFS);
	    HIGHSCORE_YPOS = (56+YOFS);
	    STRINGPOS_finsh = 140;
	    STRINGPOS_congrats = 157;
	    STRINGPOS_navi = 150;
	    STRINGPOS_flipsides = 150;
	    
	    brickm_srf = ttk_new_surface (LCD_WIDTH, LCD_HEIGHT, ttk_screen->bpp);
	    brickmania_powerups = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_powerups.gif");
	    brickmania_ball = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_ball.gif");
	    brickmania_break = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_break.gif");
	    brickmania_bricks = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_bricks.gif");
	    brickmania_gameover = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_gameover.gif");
	    brickmania_menu_bg = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_menu_bg.gif");
	    brickmania_menu_items = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_menu_items.gif");
	    brickmania_pads = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_pads.gif");
	 }
	 else {
	 	  LCD_WIDTH = ttk_screen->w;
	 	  LCD_HEIGHT = ttk_screen->h;
	    BRICK_HEIGHT = 7;
	    BRICK_WIDTH = 17;
	    LEFTMARGIN = 3;
	    TOPMARGIN = 21;
	    MENU_BMPHEIGHT = 16;
	    MENU_BMPWIDTH = 89;
	    BMPHEIGHT_menu = 132;
	    BMPWIDTH_menu = 176;
	    BMPXOFS_start = 44;
	    BMPYOFS_start = 58;
	    BMPXOFS_resume = 50;
	    BMPYOFS_resume = 75;
	    BMPXOFS_help = 74;
	    BMPYOFS_help = 89;
	    BMPXOFS_quit = 75;
	    BMPYOFS_quit = 104;
	    HIGHSCORE_XPOS = 7;
	    HIGHSCORE_YPOS = 36;
	    STRINGPOS_finsh = 110;
	    STRINGPOS_congrats = 110;
	    STRINGPOS_navi = 100;
	    STRINGPOS_flipsides = 100;
	    
	    brickm_srf = ttk_new_surface (LCD_WIDTH, LCD_HEIGHT, ttk_screen->bpp);
	    brickmania_powerups = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_powerups1.gif");
	    brickmania_ball = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_ball.gif");
	    brickmania_break = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_break1.gif");
	    brickmania_bricks = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_bricks1.gif");
	    brickmania_gameover = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_gameover1.gif");
	    brickmania_menu_bg = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_menu_bg1.gif");
	    brickmania_menu_items = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_menu_items1.gif");
	    brickmania_pads = ttk_load_image("/opt/Zillae/ZacZilla/Data/images/brickmania_pads1.gif");
	 }
	  
	 load_score();
	 load_level(cur_level);
}


static int scroll_brickm(TWidget *this,int dir)
{

int ret = TTK_EV_CLICK;
short k;


     TTK_SCROLLMOD( dir, 10);
      if( dir > 0){
      	 switch(frame) {
      	 	  case 0:
      	 	  	 /*if (cur==MENU_LENGTH-1) cur = 0;
               else cur++;
               if (when==0 && cur==1) {cur=2;}*/
               cur++;
               if (cur==4) cur=0;
               else if (when==0 && cur==1) cur=2;
               wid->dirty++;
               break;
            case 1:
            	 if(flip_sides==false) {
            	 	  if (pad_pos_x+8+PAD_WIDTH > LCD_WIDTH) {
                     for(k=0;k<used_balls;k++)
                        if (start_game==1 || ball[k].glue)
                           ball[k].pos_x+=LCD_WIDTH-pad_pos_x-PAD_WIDTH;
                     pad_pos_x+=LCD_WIDTH-pad_pos_x-PAD_WIDTH;
                  }
                  else {
                     for(k=0;k<used_balls;k++)
                        if ((start_game==1 || ball[k].glue))
                           ball[k].pos_x+=8;
                     pad_pos_x+=8;
                  }
               }
               wid->dirty++;
            	 break;
            case 2:
            	 if(maxY > LCD_HEIGHT) yoffset-=3;
            	 wid->dirty++;
            	 break;
         }
      	 
      }
      else {
      	 switch(frame) {
      	 	  case 0:
      	 	  	 /*if (cur==0) cur = MENU_LENGTH-1;
               else cur--;
               if (when==0 && cur==1) {cur = 0;}*/
               cur--;
               if (cur==-1) cur=3;
               else if(when==0 && cur==1) cur=0;
               wid->dirty++;
               break;
            case 1:
            	 if(flip_sides==false) {
            	 	  if (pad_pos_x-8 < 0) {
                     for(k=0;k<used_balls;k++)
                        if (start_game==1 || ball[k].glue)
                           ball[k].pos_x-=pad_pos_x;
                     pad_pos_x-=pad_pos_x;
                  }
                  else {
                     for(k=0;k<used_balls;k++)
                        if (start_game==1 || ball[k].glue)
                           ball[k].pos_x-=8;
                     pad_pos_x-=8;
                  }
               }
               wid->dirty++;
            	 break;
            case 2:
            	 if(yoffset < 0) yoffset+=3;
            	 wid->dirty++;
            	 break;
         }

      }
      return ret;
}
   static int down_brickm(TWidget *this,int button)
   {
      int ret=0;
      int k;
      switch (button) {
      case TTK_BUTTON_ACTION:
      	 switch(frame) {
      	 	  case 0:
      	 	  	 if (cur==0) { score=0; vscore=0; cur_level=1; life=2; int_game(1); when=1; frame=1; } 
               else if (cur==1 && when==1) {
               	  for(k=0;k<used_balls;k++)
                     if (ball[k].x!=0 && ball[k].y !=0)
                        con_game=1;
                  for(k=0;k<used_balls;k++) {
                     if (ball[k].x!=0)
                        x[k]=ball[k].x;
                     ball[k].x=0;
                     if (ball[k].y!=0)
                        y[k]=ball[k].y;
                     ball[k].y=0;
                  }
               	  frame=1; 
               	  wid->dirty++;
               } 
               else if (cur==2) { frame=2; } 
               else if (cur==3) { ttk_hide_window(wid->win); }
               break;
            case 1:
            	 if (start_game==1 && con_game!=1 && pad_type!=1) {
                  for(k=0;k<used_balls;k++) {
                     ball[k].y=-4;
                     ball[k].x=pad_pos_x+(PAD_WIDTH/2)-2>=LCD_WIDTH/2?2:-2;
                  }
                  start_game =0;
               }
               else if (pad_type==1) {
                  for(k=0;k<used_balls;k++) {
                     if (ball[k].glue)
                        ball[k].glue=false;
                     else if (start_game==1) {
                        ball[k].x = x[k];
                        ball[k].y = y[k];
                     }
                  }
                  if (start_game!=1 && con_game==1) {
                     start_game =0;
                     con_game=0;
                  }
               } 
               else if (pad_type==2 && con_game!=1) {
                  int tfire;
                  tfire=fire_space();
                  fire[tfire].top=PAD_POS_Y-7;
                  fire[tfire].left=pad_pos_x+1;
                  tfire=fire_space();
                  fire[tfire].top=PAD_POS_Y-7;
                  fire[tfire].left=pad_pos_x+PAD_WIDTH-1;
               } 
               else if (con_game==1 && start_game!=1) {
                  for(k=0;k<used_balls;k++) {
                     ball[k].x=x[k];
                     ball[k].y=y[k];
                  }
                  con_game=0;
               }
            	 break;
            case 2:
            	 break;
         }
         wid->dirty++;
         break;

      case TTK_BUTTON_MENU:
      	 switch(frame) {
      	    case 0: break;
      	    case 1: when=1; frame=0; break;
      	    case 2: frame=0; break;
      	 }
      	 if(frame != 0) cur=0;
      	 wid->dirty++;
         break;

      case TTK_BUTTON_PLAY:
         break;

      case TTK_BUTTON_NEXT:
      	 switch(frame) {
      	 	  case 0: break;
      	 	  case 1: break;
      	 	  case 2:
      	 	 	   if(xoffset+maxX > LCD_WIDTH) xoffset-=2;
      	 	 	   break;
      	 }
      	 wid->dirty++;
         break;

      case TTK_BUTTON_PREVIOUS:
      	 switch(frame) {
      	 	  case 0: break;
      	 	  case 1: break;
      	 	  case 2:
      	 	 	   if( xoffset<0) xoffset+=2;
      	 	 	   break;
      	 }
      	 wid->dirty++;
         break;

      case TTK_BUTTON_HOLD:   
         break;
         
      default:
         ret = TTK_EV_UNUSED;
      }
      return ret;
}
static void timer_brickm(TWidget *this)
{
   	  if(frame==1) wid->dirty++;
      

   


}

TWindow *new_brickm_window()
{
	  brickm_start();
	  
    window = ttk_new_window ();
    window->title="BrickMania";
    ttk_window_hide_header(window);
    wid=ttk_new_widget(0,0);
    wid->w=window->w;
    wid->h=window->h;
    wid->focusable=1;
    wid->scroll=scroll_brickm;
    wid->down=down_brickm;
    wid->timer=timer_brickm;
    wid->draw=draw_brickm;
    ttk_add_widget(window, wid);
    ttk_widget_set_timer(wid, 10);
    ttk_show_window (window);
    return 0;
}

