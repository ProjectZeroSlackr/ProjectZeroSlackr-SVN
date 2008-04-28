/*
KabooM - A rewrite of the game KabooM! by Activision for the iPod.
Copyright (C) 2005 Daniel Smith

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

General Game Information:

Prepare yourself for a supreme test of reflexes, coordination, and
agility.  You're about to face the world's most unpredictable and
relentless "Mad Bomber."  He hates losing as much as you love winning.
So, to keep him frowning, take a minute to read over these instructions.
Then, grab your buckets and bomb away! 

By turning your IPod Wheel clockwise, you move your 
buckets of water to the right.  Turning the Wheel counterclockwise
moves buckets to the left.
   
Remeber: The objective of the game is to catch all of the bombs, as well as
having fun!.
*/

#include "pz.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static GR_WINDOW_ID kaboom_wid;
static GR_WINDOW_ID kaboom_pixmap;
static GR_GC_ID kaboom_gc;
static GR_TIMER_ID kaboom_refresh_timer;
static GR_IMAGE_ID man_buffer;
static GR_IMAGE_ID bucket_buffer;
static GR_IMAGE_ID bomb_buffer;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} rectangle;

typedef struct {
	int bomb_speed;
	int clust;
} info;

static rectangle bucket[3];

  static int bombdrop; /*the timer thingo for how long to drop the bomb(s), which is defined in new_kaboom window...*/

  static int man_direction; /*direction for man...*/
  static int direction_time; /*the time to go for direction*/
  static int man_location_x; /*location x for man*/
  static int man_location_y;
  static int man_width;
  static int bomb_size;
  static int man_height;
  static int bomb_caught; /*coutner for how many caught bombs*/
  static int lives; /* counter for how many lives which are left*/
  static int buckets_alive; /* counter for how many buckets are alive */
  static info level_info[9]; /*info about the the current level*/
  static int level;
  static int bombs_counted; /*counter for how many bombs have been counted in that level, so that 					the level rotation works according to plan*/ 


struct bomblist{
		int x;
		int y;
	struct bomblist *next;
};

struct bomblist *bomblist_head;

static void bomber_timeout()
{
	bombs_counted = 0;
	direction_time = 5000;
	man_direction = 0;	
	bombdrop = 5000;
}

static void free_bomblist() {
     struct bomblist *cur, *next;
     cur = bomblist_head;
     while (cur) {
         next = cur->next;
         free (cur);
         cur = next;
     }
}

static void bomb_draw() /*function to draw the bomb*/
{
    struct bomblist *cur = bomblist_head;
    struct bomblist *last = 0;
    while (cur) {
        int bucket_hits[3], bucket_hit = 0, offscreen = 0, i;

	GrDrawImageToFit(kaboom_pixmap, kaboom_gc, cur->x, cur->y, bomb_size, bomb_size, 		bomb_buffer);
		
	for (i = 0; i < 3; i++) {
            bucket_hits[i] = (
                              lives &&
                              (bucket[i].x-(bucket[i].width/2) <= cur->x) &&
                              ((bucket[i].x-(bucket[i].width/2) + (bucket[i].width*2)) >= (cur->x + bomb_size)) &&
                              (bucket[i].y <= (cur->y + bomb_size)) &&
                              ((cur->y + bomb_size) <= (bucket[i].y + bucket[i].width))
                              );
        }

        if (cur->y >= screen_info.rows-HEADER_TOPLINE) offscreen = 1;
        bucket_hit = bucket_hits[0] || bucket_hits[1] || bucket_hits[2];

        if (bucket_hit) {
	    bomb_caught++;
            bombs_counted++;
                       
        } else if (offscreen) {
            buckets_alive--;
	    bombs_counted = 0;
	    lives--;
	    if (buckets_alive == 2) {
		bucket[0].x = 0;
		bucket[0].y = 0;
		bucket[0].width = 0;
		bucket[0].height = 0;
	    }

	    if (buckets_alive == 1) {
		bucket[1].x = 0;
		bucket[1].y = 0;
		bucket[1].width = 0;
		bucket[1].height = 0;
	    }
                if (!buckets_alive) {
                    bucket[2].x = 0;
		    bucket[2].y = 0;
		    bucket[2].width = 0;
		    bucket[2].height = 0;
		    new_message_window ("GAME OVER!");			
                    level = 1;
                    bomb_caught = 0;
		    bombs_counted = 0;
                                   }

            } 
        

	if (bucket_hit || offscreen) {
            if (!last) {
                bomblist_head = cur->next;
            } else {
                last->next = cur->next;
            }
            free(cur);
            if (last) cur = last->next;
            else      cur = bomblist_head;
        }
        else {
            cur->y++;
            last = cur;
            cur = cur->next;
        }
    }
    
}



static void bomb_add()

{

	struct bomblist *newBomb = malloc(sizeof(struct bomblist));

	newBomb->x = man_location_x;

	newBomb->y = (HEADER_TOPLINE+(HEADER_TOPLINE/3));

	newBomb->next = bomblist_head;

	bomblist_head = newBomb;

	bomb_draw();
};



void wall_draw() /*function to draw the wall which the robber guy sits on, which includes the colours*/

{

GrLine(kaboom_pixmap, kaboom_gc, 0, (HEADER_TOPLINE+(HEADER_TOPLINE/3)), screen_info.cols, (HEADER_TOPLINE+(HEADER_TOPLINE/3)));

if (screen_info.bpp == 16) {

GrSetGCForeground(kaboom_gc, GR_RGB(0,255,0));

GrFillRect( kaboom_pixmap, kaboom_gc, 0, (HEADER_TOPLINE+(HEADER_TOPLINE/3)), screen_info.cols, screen_info.rows);

GrSetGCForeground(kaboom_gc, GR_RGB(0,0,0));

}

}


void bucket_draw() /*functioon to draw the bucket*/

{

if (buckets_alive == 3) {

GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[0].x, bucket[0].y, bucket[0].width, bucket[0].height, bucket_buffer);

GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[1].x, bucket[1].y, bucket[1].width, bucket[1].height, bucket_buffer);

GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[2].x, bucket[2].y, bucket[2].width, bucket[2].height, bucket_buffer);

}

if (buckets_alive == 2) {

GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[1].x, bucket[1].y, bucket[1].width, bucket[1].height, bucket_buffer);


GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[2].x, bucket[2].y, bucket[2].width, bucket[2].height, bucket_buffer);

}

if (buckets_alive == 1)
GrDrawImageToFit(kaboom_pixmap, kaboom_gc, bucket[2].x, bucket[2].y, bucket[2].width, bucket[2].height, bucket_buffer);

}


static void man_draw() /*function to draw the man who drops the bombs*/
{
man_location_x += man_direction;
if (man_location_x <= 0)
	man_direction *= -1;
if (man_location_x >= screen_info.cols - man_width){
	man_location_x = screen_info.cols - man_width;
	man_direction *= -1;
}


GrDrawImageToFit(kaboom_pixmap, kaboom_gc, man_location_x, man_location_y, man_width, man_height, man_buffer);

}


static void kaboom_do_draw()
{
	char text[30];
	wall_draw(); /* calls the drawing of the wall (which is really a line for now)*/
      bomb_draw(); /* call to draw the bomb */
	bucket_draw(); /*call to draw the bucket */
	man_draw(); /*call to draw the man who drops bombs */

	 /* Draw the counter */
    sprintf(text,"%d", bomb_caught);
    GrText(kaboom_pixmap,
           kaboom_gc,
           1, HEADER_TOPLINE,  /* x, y */
           text,
           -1,
           GR_TFASCII);
	GrCopyArea (kaboom_wid, kaboom_gc, 0, 0, screen_info.cols, screen_info.rows, kaboom_pixmap, 0, 0, MWROP_SRCCOPY);
	GrSetGCForeground(kaboom_gc, GR_RGB(255,255,255));
	GrFillRect( kaboom_pixmap, kaboom_gc, 0, 0, screen_info.cols, screen_info.rows);
	GrSetGCForeground(kaboom_gc, GR_RGB(0,0,0));

}

static void kaboom_loop()
{
if (bombs_counted == level_info[level].clust) {
	level++;
	bomber_timeout();
	}
    kaboom_do_draw();	
}

static int kaboom_handle_event(GR_EVENT *event)
{
    int ret = 0;
    static int paused = 0;

    switch (event->type) {
    case GR_EVENT_TYPE_TIMER:
    
    bombdrop -= 50;
    if (bombdrop == 0){
	bomb_add();
	bombdrop = level_info[level].bomb_speed;
	}

    direction_time -= 50;
if (direction_time <= 0){
    direction_time = rand()%800 + 200;
    man_direction =  rand()%11 - 5;
    }
    kaboom_do_draw();
    

	if (!paused)
            kaboom_loop();
	break;

    case GR_EVENT_TYPE_KEY_DOWN:
        switch (event->keystroke.ch) {

        case IPOD_WHEEL_COUNTERCLOCKWISE: /* Wheel left */
			if (bucket[2].x <= 0)
				bucket[0].x = 0, bucket[1].x = 0, bucket[2].x = 0;
			else bucket[0].x -= 5, bucket[1].x -= 5, bucket[2].x -= 5;
			kaboom_do_draw();
		    	break;

        case IPOD_WHEEL_CLOCKWISE: /* Wheel right */
			if (bucket[2].x >=((screen_info.cols)-bucket[2].width))	
			bucket[0].x = bucket[1].x = bucket[2].x = ((screen_info.cols)-bucket[2].width);
			else bucket[0].x += 5, bucket[1].x += 5, bucket[2].x += 5;
			kaboom_do_draw();
			break;

        case IPOD_SWITCH_HOLD: /* hold switch on */
            paused = 1; /* pause the loop */
            break;
        case IPOD_BUTTON_MENU: /* Menu button */
	    pz_close_window(kaboom_wid);
	    GrDestroyWindow (kaboom_pixmap);
            GrDestroyTimer(kaboom_refresh_timer);
            GrDestroyGC(kaboom_gc);
	    break;
        case IPOD_BUTTON_ACTION: /* Action button */
        case IPOD_BUTTON_PLAY: /* Play/pause button */
        case IPOD_BUTTON_REWIND: /* Rewind button */
        case IPOD_BUTTON_FORWARD: /* Fast forward button */
        default:
            ret |= KEY_UNUSED; /* allow a default key action */
            break;
        }
        break;
    case GR_EVENT_TYPE_KEY_UP:
        switch (event->keystroke.ch) {
        case IPOD_SWITCH_HOLD: /* un-held */
            paused = 0;
            break;
        default:
            ret |= KEY_UNUSED;
            break;
        }
    default:
        ret |= EVENT_UNUSED; /* we didn't ask for this event, go away */
    }
    return ret;
}


void new_kaboom_window (void)
{
//bombdrop = 1500; /*timer for how long to drop the bomb, which is defined in new_kaboom window....*/
  bucket[0].x = bucket[1].x = bucket[2].x = ((screen_info.cols)/2); /* location x for buckets*/
  bucket[0].y = (((screen_info.rows-HEADER_TOPLINE)*95)/100);   /* location y for bucket0*/
  bucket[1].y = (((screen_info.rows-HEADER_TOPLINE)*85)/100);   /* location y for bucket1*/  
  bucket[2].y = (((screen_info.rows-HEADER_TOPLINE)*75)/100);   /* location y for bucket0*/
  bomblist_head = 0;
  man_width = screen_info.cols/16;
  man_height =  (HEADER_TOPLINE+(HEADER_TOPLINE/3));
  man_location_x = (screen_info.cols/2); /*location x for man*/
  direction_time = rand()%300;
  man_direction =  rand()%11 - 5;
  man_location_y = 1;
  bomb_caught = 0; /*counter for how many caught bombs*/
  bucket[0].width = bucket[1].width = bucket[2].width = screen_info.cols/10; /*bucket width*/
  bucket[0].height = bucket[1].height = bucket[2].height = screen_info.cols/30; /*bucket height*/
  man_buffer = GrLoadImageFromFile("/opt/Zillae/ZacZilla/Data/man.bmp", 0);
  bucket_buffer = GrLoadImageFromFile("/opt/Zillae/ZacZilla/Data/bucket.bmp", 0);
  bomb_buffer = GrLoadImageFromFile("/opt/Zillae/ZacZilla/Data/bomb.bmp", 0);
  bombdrop = 1000;
  bomb_size = ((man_width * 75)/100);
  buckets_alive = 3;
  lives = 3;
  level = 1;
  bombs_counted = 0;
  /****************************************************************
  *****************************************************************
  *****************************************************************
  *****************************************************************
  ***********************THE NEXT PART DEFINES THE*****************
  **************************AMOUNT OF BOMBS PER********************
  ********************************CLUSTER**************************
  *****************************************************************
  *****************************************************************
  *****************************************************************
  ****************************************************************/
	level_info[0].clust = 0;
	level_info[1].clust = 10;
	level_info[2].clust = 20;
 	level_info[3].clust = 30;
	level_info[4].clust = 40;
	level_info[5].clust = 50;
	level_info[6].clust = 75;
	level_info[7].clust = 100;
	level_info[8].clust = 150;

/* ALL OF THIS STUFF BELOW DEFINES THE SPEED AT WHICH THE MAN RELEASES BOMBS. */

	level_info[0].bomb_speed = 0;
	level_info[1].bomb_speed = 1000;
	level_info[2].bomb_speed = 900;
 	level_info[3].bomb_speed = 800;
	level_info[4].bomb_speed = 700;
	level_info[5].bomb_speed = 600;
	level_info[6].bomb_speed = 500;
	level_info[7].bomb_speed = 400;
	level_info[8].bomb_speed = 300;
  



    kaboom_gc = pz_get_gc(1);   /* Get a copy of the graphics context */

	/*Set foreground color */

    GrSetGCUseBackground(kaboom_gc, GR_FALSE);

    GrSetGCForeground(kaboom_gc, GR_RGB(0,0,0));



    /* Define the pixmap to use */

	kaboom_pixmap = GrNewPixmap (screen_info.cols, screen_info.rows-HEADER_TOPLINE, NULL);

	free_bomblist(); /*free the bomblist*/

	/* Open the window: */

    kaboom_wid = pz_new_window (0, HEADER_TOPLINE + 1, /* (x, y) */

                                  screen_info.cols,

                                  screen_info.rows - (HEADER_TOPLINE + 1), /* Height of screen - header  */

                                  kaboom_do_draw, /* function pointer, called on an EXPOSURE event */

                                  kaboom_handle_event); /* function pointer, called on all other selected events */



    /* Select the types of events you need for your window: */

    GrSelectEvents (kaboom_wid, GR_EVENT_MASK_EXPOSURE |

                      GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP

					  | GR_EVENT_MASK_TIMER );



    /* Map the window to the screen: */

    GrMapWindow (kaboom_wid);



	pz_draw_header("KabooM!"); /*draw the header, kaboom!*/

	kaboom_do_draw();



    /* Create the timer used for timing the bomb drop: */

    kaboom_refresh_timer = GrCreateTimer (kaboom_wid, 50); /* Timer interval, millisecs */

}
