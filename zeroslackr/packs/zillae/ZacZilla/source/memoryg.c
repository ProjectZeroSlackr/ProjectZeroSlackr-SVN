/*
 * Copyright (C) 2005 Filippo Forlani
 * 
 * Simple Memory Game for Ipod Linux
 *
 * Todo:
 * - bitmaps instead characters
 * - many other things!
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
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "pz.h"
#include "ipod.h"

static GR_WINDOW_ID memoryg_wid;
static GR_GC_ID memoryg_gc;
static GR_TIMER_ID memoryg_timer_id;

////////////MEMROYGAME///////////////////////
//game memory definition
//GAME STATUS
#define GAME_MAIN 0
#define GAME_PLAY 1
#define GAME_NEXT_WALL 2

//MAX BOARD
#define MAX_CARDS_ROWS 6
#define MAX_CARDS_COLS 7
#define MAX_CARDS_MAX MAX_CARDS_COLS*MAX_CARDS_ROWS

//Cards dimension (using bitmap should change)
#define CARD_WIDTH 11
#define CARD_HEIGHT 13

// data message for select new card on board
#define CARD_NEXT 1
#define CARD_PREVIOUS -1

//current game, cards on board, current card and cards opened (0, 1 or 2)
static int cards_cols,cards_rows,cards_max;
static int card_current=0;
static int cards_opened=0;

static int first_card,second_card;//which card is opened on board

static int counter;//a counter to display the 2 cards opened for a while
static int score;  //score
static int incscore;//increment of score (function of num. of cards on board)
static int alldone; //n.of couples left on board, when 0 all board is done.
static int game_status;//status of game: gameplay, next_wall...

int memoryg_high_score;//high score... not static, will be also in memory?

//structure of a card, position, status, and type (will be changed for bitmap)
typedef struct{
  int posx,posy;
  int status;//-1 inactive; 0 back; 1 open;
	char c; 
} card;

static card cards[MAX_CARDS_ROWS*MAX_CARDS_COLS];

////////////// MEMORY GAME FUNCTIONS //////////////////

static void card_draw(card* p) //draw a card in its position with its status
{

  switch ( p->status )
  {
  case (-1): //don't draw it!
          break;

  case (0):  //draw back
          GrRect( memoryg_wid, memoryg_gc,
                  p->posx, p->posy - CARD_HEIGHT,
                  CARD_WIDTH+1, CARD_HEIGHT+1);
    GrLine(memoryg_wid, memoryg_gc,
           p->posx, p->posy, p->posx + CARD_WIDTH, p->posy - CARD_HEIGHT);
    break;

  case (1):
          GrText(memoryg_wid, memoryg_gc,
                 p->posx + 2, p->posy - 2, &(p->c), -1, GR_TFASCII);
    GrRect(memoryg_wid, memoryg_gc,
           p->posx, p->posy - CARD_HEIGHT,
           CARD_WIDTH+1, CARD_HEIGHT+1);
    break;
  }
}

static void refresh_card(card* cc) //refresh the card on display
{
  GrSetGCForeground(memoryg_gc, GR_RGB(255,255,255));
  GrSetGCBackground(memoryg_gc, GR_RGB(255,255,255));
  GrFillRect(memoryg_wid,memoryg_gc,
             cc->posx-2,cc->posy-2-CARD_HEIGHT,CARD_WIDTH+4,CARD_HEIGHT+4);
  GrSetGCForeground(memoryg_gc, GR_RGB(0,0,0));
  card_draw(cc);

}

static void card_drawsel() //display the selection mark on card_current
{
  GrRect(memoryg_wid,memoryg_gc,
         cards[card_current].posx-1,cards[card_current].posy-1-CARD_HEIGHT,
         CARD_WIDTH+3,CARD_HEIGHT+3);
}

static void card_undrawsel() //undisplay the selection mark
{
  GrSetGCForeground(memoryg_gc, GR_RGB(255,255,255));
  card_drawsel();
  GrSetGCForeground(memoryg_gc, GR_RGB(0,0,0));
}

static void cards_swap(card* c1,card* c2) //swap card type during shuffle
{
  char cswp;
  cswp=c1->c;
  c1->c=c2->c;
  c2->c=cswp;
}

static void cards_shuffle() //shuffle cards
{
  int i;
	int total=cards_rows * cards_cols;
	
  for(i=0;i <1000; i++)
		cards_swap( &(cards[rand() % total]), &(cards[rand() % total]) );
}

static void cards_create() //this create the new board, based on cards_rows and cols
{
  int i,half,ihalf;
  int offx,offy;
	
	cards_max = cards_rows*cards_cols;
	half = cards_max/2;
	
	offx=screen_info.cols/2 - (CARD_WIDTH+5)*cards_cols/2;
  offy=screen_info.rows/2 - (CARD_HEIGHT+3)*cards_rows/2 -3;	
  
	for(i=0; i<half; i++)
  {
    ihalf=i+half;
    cards[i].posx = (i % cards_cols)*(CARD_WIDTH+5)+offx;
    cards[i].posy = (i / cards_cols)*(CARD_HEIGHT+3)+offy;
    cards[i].c = i+'A';
    cards[i].status = 0;
    cards[ihalf].posx = (ihalf % cards_cols)*(CARD_WIDTH+5)+offx;
    cards[ihalf].posy = (ihalf / cards_cols)*(CARD_HEIGHT+3)+offy;
    cards[ihalf].c = i+'A';
    cards[ihalf].status = 0;
  }
	
  card_current=0;
  cards_opened=0;
  counter=0;

	incscore=cards_rows*cards_cols;
	
	//number of couples to be made
  alldone=half;
}

//////////////////////////////////////////

static void memoryg_DrawScene()
{
  //nothing to do now for this function
}

static void memoryg_Game_Loop()
{
  memoryg_DrawScene();
}

static void memoryg_do_draw()
{
  int i;
  pz_draw_header("Ipod Memory");

  for(i=0;i<cards_rows*cards_cols;i++)
    card_draw(&(cards[i]));

  memoryg_Game_Loop();
}

static void update_score()
{
  char c[24];
  sprintf(c,"Score: %+.4d",score);

  GrLine(memoryg_wid,memoryg_gc,
         0,screen_info.rows-HEADER_TOPLINE-CARD_HEIGHT-1,
         screen_info.cols,screen_info.rows-HEADER_TOPLINE-CARD_HEIGHT-1);

  GrText(memoryg_wid,memoryg_gc,
         0,screen_info.rows-HEADER_TOPLINE-2,c,-1,GR_TFASCII);

  if(score>memoryg_high_score)memoryg_high_score=score;
  sprintf(c,"H.Score %+.4d",memoryg_high_score);
  GrText(memoryg_wid,memoryg_gc,
         80,screen_info.rows-HEADER_TOPLINE-2,
         c,-1,GR_TFASCII);

}

static void cards_select(int v)
{
  do
  {
    card_current+=v;
		
    if(card_current >= cards_max) card_current = 0;
    if(card_current < 0) card_current = cards_max-1;
  }
  while (cards[card_current].status!=0);
}

static int memoryg_handle_event(GR_EVENT *event)
{
	// first see if there are some jobs started: counter to view couple cards
	// in this case, maybe all couples are found
  if (counter>0)
  {
    counter--;
    if((counter==0) || (event->type==GR_EVENT_TYPE_KEY_DOWN))
    {
      counter=0;
			
      refresh_card(&cards[second_card]);
      refresh_card(&cards[first_card]);
			
      if(alldone==0) //see if the board is finished
      {
        GrText(memoryg_wid, memoryg_gc,
							 40, screen_info.rows/2 -14,
               "You Won!!!", -1, GR_TFASCII);
							 
        GrText(memoryg_wid, memoryg_gc,
               10, screen_info.rows/2,
               "Push Button for Next Level", -1, GR_TFASCII);
							 
        game_status = GAME_NEXT_WALL;
				
        if(score>=memoryg_high_score)
        {
          GrText(memoryg_wid,memoryg_gc,
									20, screen_info.rows/2 +14,
                 "NEW HIGH SCORE!", -1, GR_TFASCII);
          update_score();
        }
      }
			
      else card_drawsel();
    }
    return 0;
  }

  switch(event->type)
  {
  case ( GR_EVENT_TYPE_TIMER ):
          memoryg_Game_Loop();
					//still not used, later for some animation...
    break;

  case( GR_EVENT_TYPE_KEY_DOWN ):
          switch(event->keystroke.ch)
      {
      case '\r': //push button
        if(game_status==GAME_NEXT_WALL)  //did I finished the board?
        { 
					//then recreate new table
					//update rows and cols
					cards_cols =
					 (cards_cols >= MAX_CARDS_COLS) ? MAX_CARDS_COLS : cards_cols +1;
					cards_rows =
					 (cards_rows >= MAX_CARDS_ROWS) ? MAX_CARDS_ROWS : cards_rows +1;
					 
					cards_create();
          cards_shuffle();
					
          GrSetGCForeground(memoryg_gc, GR_RGB(255,255,255));
					
          GrFillRect(memoryg_wid, memoryg_gc,
                     0, 0, screen_info.cols, screen_info.rows);
										 
          GrSetGCForeground(memoryg_gc, GR_RGB(0,0,0));
					
          memoryg_do_draw();
          card_drawsel();
          
					update_score();
          game_status=GAME_PLAY;
					
          break;
        }
				// if not continue with push func during gameplay
        cards[card_current].status = 1;
        cards_opened++;
				//now test if is the first card or second
        if(cards_opened==1)
        {
          first_card = card_current;
          cards_select(CARD_NEXT);
          
					refresh_card(&cards[first_card]);
        }

        if(cards_opened==2)
        { 
          cards[card_current].status=1;
					
          refresh_card(&cards[card_current]);
          
					second_card=card_current;
          counter=20;
          //test if a couple is made
					if(cards[second_card].c==cards[first_card].c)
          {
						//if yes, put out cards from board
            cards[second_card].status=-1;
            cards[first_card].status=-1;
						
            score+=incscore;
            alldone--;
          }
          else
          {  
						//if not put cards back
            cards[second_card].status = 0;
            cards[first_card].status = 0;
            score--;
          }
					
          cards_opened = 0;
          update_score();
					
          if(alldone!=0)cards_select(CARD_NEXT);
        }
				
        card_drawsel();
				
        break;
				
      case 'd':
        break;
				
      case 'w':
        break;
				
      case 'f':
        break;
				
      case 'l':
        //if left then go back a card
				card_undrawsel();
        cards_select( CARD_PREVIOUS);
        card_drawsel();
				
        break;
      case 'r':
			  //if right then go forward a card
        card_undrawsel();
        cards_select( CARD_NEXT);
        card_drawsel();
        break;	
      case 'm':
				//if Menu Button then destroy all dynamically created
        pz_close_window(memoryg_wid);
        GrDestroyTimer (memoryg_timer_id);
        GrDestroyGC(memoryg_gc);
        break;
				
      default:
        break;
				
      }
			
    break;
  }
	
  return 1;
}

void new_memoryg_window()
{

  memoryg_gc=pz_get_gc(1);
  
	GrSetGCBackground(memoryg_gc, GR_RGB(255,255,255));
  GrSetGCForeground(memoryg_gc, GR_RGB(0,0,0));

  memoryg_wid=pz_new_window(0,
                            HEADER_TOPLINE+1,
                            screen_info.cols,
                            screen_info.rows-HEADER_TOPLINE-1,
                            memoryg_do_draw,
                            memoryg_handle_event);

  cards_rows=3;
	cards_cols=4;
	score=0;
	
	cards_create();
  srandom(time(NULL));
	cards_shuffle();

  GrSelectEvents(memoryg_wid,
										GR_EVENT_MASK_TIMER|
										GR_EVENT_MASK_EXPOSURE|
										GR_EVENT_MASK_KEY_UP|
										GR_EVENT_MASK_KEY_DOWN);

  memoryg_timer_id=GrCreateTimer(memoryg_wid,100);

  GrMapWindow(memoryg_wid);
  
	card_drawsel();
  update_score();
	
  game_status=GAME_PLAY;
}
