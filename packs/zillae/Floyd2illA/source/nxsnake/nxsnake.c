/* NXSNAKE.C
   
   The main engine and state machine for the nxSnake game
   Copyright 2001, Jordan Crouse (jordanc@censoft.com)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// KERIPO MOD
#include "../_mods.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include "../pz.h"

#define MWINCLUDECOLORS

#include "nano-X.h"
#include "nxsnake.h"

#include "text.h"
#include "levels.h"

//#include "boot_image.xpm"

GR_IMAGE_ID     boot_image_id, banner_image_id;
GR_IMAGE_INFO   boot_image_info, banner_image_info;
int boot_image_w,boot_image_h,banner_image_w,banner_image_h;

/* This matrix indicates where the snake, border and nibbles are */
/* This is how the engine determines where the snake, nibbles and */
/* borders are */

unsigned char playground[YUNITS][XUNITS];

snake_t global_snake;
GR_WINDOW_ID swindow, offscreen, boot_win, head_win;
GR_TIMER_ID nxsnake_timer;

static int game_state = 0;
static int current_level = 0;
static int global_level = 0;



#define SNAKE_START_SPEED 40
static unsigned long game_speed;
static unsigned long start_speed = SNAKE_START_SPEED;
static unsigned long snake_count = 0, anim_image = 0;
static char last_keystroke = 'z';
static int count_wheel = 0, image_loaded = 0;

struct
{
  int active;
  int x;
  int y;
} nibble;

int skipped = 0;
int fin = 0;

#define MOVE_ILLEGAL 0
#define MOVE_LEGAL   1
#define MOVE_NIBBLE  2


void nxsnake_draw_header();

inline void SET_SNAKE_DIRECTION(unsigned short *value, unsigned char dir)
{
  *value &= 0x0FFF;
  *value |= ( (dir & 0xF) << 12);
}

inline void SET_SNAKE_OFFSET(unsigned short *value, unsigned short off)
{
  *value &= 0xF000;
  *value |= (off & 0xFFF);
}

void init_level(int l)
{
  int i, j, k, x_block, y_block, block_size;
  
  global_snake.speed = 1 + l;


  switch(l)
  	{
    /* Level 1 : all is empty */
    case 0 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
            	{
                level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                }
            }
          level[l].startx = XUNITS / 2;
          level[l].starty = YUNITS / 2;
          break;
          
    /* Level 2 : big box */
    case 1 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
                {
                if ((i != 0) && (i != XUNITS-1) && (j != 0) && (j != YUNITS-1))
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                else
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
                }
            }
          level[l].startx = XUNITS / 2;
          level[l].starty = YUNITS / 2;
          break;
          
    /* Level 3 : cross like */      
    case 2 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
                {
                if ((i != XUNITS / 2) && (j != YUNITS / 2))
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                else
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
                }
            }
          level[l].startx = XUNITS / 4;
          level[l].starty = YUNITS / 4;
          break;
          
    /* Level 4 : strange ways */ 
    case 3 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
                {
                if ((i != 0) && (i != XUNITS-1) && (j != 0) && (j != YUNITS-1))
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                else
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
                if (((i==XUNITS/5) || (i==2*XUNITS/5) || (i==3*XUNITS/5) || (i==4*XUNITS/5))
                     && (((j>=YUNITS/5) && (j<=2*YUNITS/5)) || ((j>=3*YUNITS/5) && (j<=4*YUNITS/5))))
                    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
                }
            }
          level[l].startx = XUNITS / 2;
          level[l].starty = YUNITS / 2;
          break;
	
    /* Level 5 : maze */ 
    case 4 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
            	{
                level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                if(
                	((i==XUNITS/5) && (j<3*YUNITS/4)) ||
                    ((i==2*XUNITS/5) && (j>YUNITS/2)) ||
                    ((i==3*XUNITS/5) && (j<3*YUNITS/4)) ||
                    (i==4*XUNITS/5) ||
                    ((j==YUNITS/4) && (i>XUNITS/5) && (i<2*XUNITS/5)) ||
                    ((j==YUNITS/2) && (i>=2*XUNITS/5))
                   )
                   level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
                }
            }
          
          level[l].startx = XUNITS / 10;
          level[l].starty = YUNITS / 2;
          break;
    
    /* Level 6 : 20 random blocks*/
    case 5 :
          for(j=0;j<YUNITS;j++)
            {
            for(i=0;i<XUNITS;i++)
            	{
                level[l].bitmap[i+j*XUNITS] = PLAYGROUND_EMPTY;
                }
            }
          level[l].startx = XUNITS / 2;
          level[l].starty = YUNITS / 2;

	    for(k=0;k<20;k++)
		{
		block_size = 1 + ((int) (rand() % (6 - 1)));
		while(1)
			{
			x_block = 0 + (int) (rand() % (XUNITS - 1));
			y_block = 0 + (int) (rand() % (YUNITS - 1));
 			if((x_block > XUNITS / 2 - 20) &&
			   (x_block < XUNITS / 2 + 20) &&
			   (y_block < YUNITS / 2 + 20) &&
			   (y_block < YUNITS / 2 + 20) &&
			   (x_block < block_size) &&
			   (x_block > XUNITS -1 - block_size) &&
			   (y_block < block_size) &&
			   (y_block > YUNITS -1 - block_size))
				continue;
			else
				break;
			}
		
		for(i=x_block-block_size;i<x_block+block_size;i++)
		  {
		  for(j=y_block-block_size;j<y_block+block_size;j++)
		    level[l].bitmap[i+j*XUNITS] = PLAYGROUND_BORDER;
		  }
		}
	    break;
    }

  global_snake.headx = level[l].startx;
  global_snake.heady = level[l].starty;

  global_snake.tailpointer = 0;

  global_snake.length = 5 + l;
  global_snake.growth = l;

  bzero(global_snake.body, 256 * sizeof(unsigned short));
  
  SET_SNAKE_DIRECTION(&global_snake.body[0], level[l].dir);
  SET_SNAKE_OFFSET(&global_snake.body[0], global_snake.length);
}

int advance_snake(void)
{
  unsigned short netoff = 0;
  int pos = 0;

  unsigned char dir;
  unsigned short off;

  short newx;
  short newy;

  int ret;

  if (!global_snake.body[0]) return(MOVE_LEGAL);
  
  dir = GET_SNAKE_DIRECTION(global_snake.body[0]);

  newx = global_snake.headx;
  newy = global_snake.heady;

  /* FIXME:  add speed concept in here, eh.... */

  switch(dir)
    {
    case SNAKE_DIR_LEFT:
      newx = global_snake.headx - 1;
      if (newx < 0) newx = XUNITS - 1;
      break;
	
    case SNAKE_DIR_RIGHT:
      newx = global_snake.headx + 1;
      if (newx >= XUNITS) newx = 0;
      break;

    case SNAKE_DIR_UP:
      newy = global_snake.heady - 1;
      if (newy < 0) newy = YUNITS - 1;
      break;
	  
    case SNAKE_DIR_DOWN:
      newy = global_snake.heady + 1;
      if (newy >= YUNITS) newy = 0;
      break;
    }
  
  /* Now check the new position */
  switch(playground[newy][newx])
    {
    case PLAYGROUND_EMPTY:
    case PLAYGROUND_TAIL:
      ret = MOVE_LEGAL;
      break;
      
    case PLAYGROUND_BORDER:
    case PLAYGROUND_SNAKE:
      return(MOVE_ILLEGAL);  

    case PLAYGROUND_NIBBLE:
      /* Yummy... */
      global_snake.growth += 3;
      global_snake.score++;
      
      game_speed -= 2;

      nibble.active = 0;
      ret = MOVE_NIBBLE;
      break;  
    }

 /* Now advance the head.  That means advancing the absolute x and y */
  /* and adding one to the current offset */
  
  global_snake.headx = newx;
  global_snake.heady = newy;

  /* Finally, handle the tail */

  /* If we are growing, then handle that here (the tail doesn't move) */

  if (global_snake.growth)
    {
      global_snake.growth--;
      global_snake.length++;
      
      off = GET_SNAKE_OFFSET(global_snake.body[0]);
      SET_SNAKE_OFFSET(&global_snake.body[0], off + 1);
      return(ret);
    }

  /* If there is only one segment, then handle that here */
  
  if (!global_snake.tailpointer)
    {
       SET_SNAKE_OFFSET(&global_snake.body[0], global_snake.length);
       return(ret);
    }

  /* Otherwise, we need to move the whole snake */

  off = GET_SNAKE_OFFSET(global_snake.body[0]);
  SET_SNAKE_OFFSET(&global_snake.body[0], off + 1);

  while(global_snake.body[pos])
    {
      off = GET_SNAKE_OFFSET(global_snake.body[pos]);
      
      /* If the size ends here, then clear the rest of the */
      /* snake */

      if (netoff + off >= global_snake.length)
	{
	  if (global_snake.tailpointer > pos + 1)
	    printf("OOPS!  You have a gap between the length and the tail\n");
	  
	  global_snake.body[pos + 1] = 0;
	  global_snake.tailpointer = pos;
		  
	  SET_SNAKE_OFFSET(&global_snake.body[pos], 
			   global_snake.length - netoff);
	  break;
	}
	      
      netoff += off;
      pos++;
    }

  return(ret);
}

int redirect_snake(GR_EVENT_KEYSTROKE event)
{
  unsigned char newdir = 0;
  int i = 0;

  unsigned char dir = GET_SNAKE_DIRECTION(global_snake.body[0]);
    
  switch(event.ch)
    {      
    case 'l':
      if(dir!=8)
        newdir = dir * 2;
      else
        newdir = SNAKE_DIR_UP;
      break;

    case 'r':
      if(dir!=1)
        newdir = dir / 2;
      else
        newdir = SNAKE_DIR_RIGHT;
      break;
      
    case 'f':
      if(dir!=1)
      	newdir = dir / 2;
      else
        newdir = SNAKE_DIR_RIGHT;
      break;

    case 'w':
      if(dir!=8)
      	newdir = dir * 2;
      else
        newdir = SNAKE_DIR_UP;
      break;

    default:
      return(0);
    }
    

  for(i = global_snake.tailpointer + 1; i > 0; i--)
  global_snake.body[i] = global_snake.body[i - 1];

  global_snake.tailpointer++;

  SET_SNAKE_DIRECTION(&global_snake.body[0], newdir);
  SET_SNAKE_OFFSET(&global_snake.body[0], 0);
  
  /* No bounds checking, that will be done on the next advance */
  return(1);
}

void show_buffer(void)
{
  GR_GC_ID gc = GrNewGC();

  GrCopyArea(swindow, gc, 0, 0, WWIDTH, WHEIGHT, 
			offscreen, 0, 0, MWROP_SRCCOPY);

  GrDestroyGC(gc);
}

void draw_string(char *array, int mode)
{
  char *txtptr = array;
  
  int xpos, ypos;

  int maxwidth = 0, maxheight = 0;

  int i = 0;
  int count = 0;

  GR_GC_ID gc = GrNewGC();
  //GR_FONT_ID font = GrCreateFont((GR_CHAR *) GR_FONT_GUI_VAR, 0, 0);
  
  //GrSetGCFont(gc, font);

  /* Count how many lines we have */
  while(txtptr)
    {
      int tw, th, tb;
      
      if (strlen(txtptr) == 0) break;
      
      GrGetGCTextSize(gc, txtptr, -1, 0,
		      &tw, &th, &tb);

      if (maxwidth < tw) maxwidth = tw;
      if (maxheight < th) maxheight = th;

      txtptr += 50;
      count++;
    }

  txtptr = array;
  maxwidth += 10;
  maxheight += 10;

  /* mode == 1, clear the screen,
     mode == 2, draw a box
  */

  if (mode == 1)
    {
      GrSetGCForeground(gc, BLACK);
      GrFillRect(offscreen, gc, 0, 0, WWIDTH, WHEIGHT);

      GrSetGCForeground(gc, WHITE);
      GrSetGCBackground(gc, BLACK);
    }
  else
    {
      xpos = (PWIDTH / 2) - (maxwidth / 2);
      ypos = (PHEIGHT / 2) - ((count * maxheight) / 2);

      /* Draw a box */
      GrSetGCForeground(gc, BLACK);

      GrFillRect(offscreen, gc, 
		 xpos, 
		 ypos,
		 maxwidth, (count * maxheight));
      
      GrSetGCForeground(gc, WHITE);

      GrRect(offscreen, gc, 
		 xpos, 
		 ypos,
		 maxwidth, (count * maxheight));

      GrSetGCBackground(gc, BLACK);
    }
  
  ypos = (WHEIGHT / 2) - ((count * maxheight) / 2) + 5;

  while(1)
    {
      int tw, th, tb;
      if (strlen(txtptr) == 0) break;

      GrGetGCTextSize(gc, txtptr, -1, 0,
		      &tw, &th, &tb);

      GrText(offscreen, gc, (WWIDTH / 2) - (tw / 2), 
	     ypos + (i  * th), txtptr, -1, 0);
      i++;
      txtptr += 50;
    }
  
  GrDestroyGC(gc);
  //GrDestroyFont(font);
}

void draw_screen(int full)
{
  int bstart, bend;
  int x = 0, y = 0;

  GR_GC_ID gc = GrNewGC();

  for(y = 0; y < YUNITS; y++)
    {
       bstart = -1;
       bend = 0;
  
    for(x = 0; x < XUNITS; x++)
      {
	if (playground[y][x] == PLAYGROUND_EMPTY)
	  {
	    if (bstart == -1) bend = bstart = x;
	    else bend++;
	    continue;
	  }

	/* Draw the background block */
	
	if (bstart != -1)
	  {
	    GrSetGCForeground(gc, WHITE);
	    GrFillRect(offscreen, gc, (bstart * XUNITSIZE), (y * YUNITSIZE), 
		       (bend - bstart + 1) * XUNITSIZE, YUNITSIZE);
	
	    skipped += (bend - bstart);
	    
	    bstart = -1;
	    bend = 0;
	  }

	if (!full && playground[y][x] == PLAYGROUND_BORDER)
	  continue;
	
	switch(playground[y][x])
	  {
	  case PLAYGROUND_BORDER:
	    GrSetGCForeground(gc, GRAY);
	    break;
	    
	  case PLAYGROUND_SNAKE:
        GrSetGCForeground(gc, BLACK);
	    break;
            
	  case PLAYGROUND_TAIL:
	    GrSetGCForeground(gc, BLACK);
	    break;
	    
	  case PLAYGROUND_NIBBLE:
	    GrSetGCForeground(gc, GRAY);
	    break;
	    
	  }
	
	GrFillRect(offscreen, gc, (x * XUNITSIZE), 
		   (y * YUNITSIZE), XUNITSIZE, YUNITSIZE);

#ifdef NOTUSED
	if (playground[y][x] == PLAYGROUND_NIBBLE && nibble.active)
	  {
	    GR_POINT points[4];
	    
	    points[0].x = (x * XUNITSIZE) + 1;
	    points[0].y = points[2].y = (y * YUNITSIZE) + 1 + ((YUNITSIZE -2) / 2);
	    points[1].x = points[3].x = (x * YUNITSIZE) + 1 + ((XUNITSIZE -2) / 2);
	    points[1].y = (y * YUNITSIZE) + 1;
	    points[2].x = points[0].x + (XUNITSIZE - 2);
	    points[3].y = points[1].y + (YUNITSIZE - 2);
	  
	    GrFillPoly(offscreen, gc, 4, points);
	  }
#endif

	if (playground[y][x] == PLAYGROUND_NIBBLE && nibble.active)
	  {
	    int xpos = (x * XUNITSIZE) + (XUNITSIZE / 2);
	    int ypos = (y * YUNITSIZE) + (YUNITSIZE / 2);

	    GrSetGCForeground(gc, BLACK);

	    GrFillEllipse(offscreen, gc, xpos, ypos,
			  (XUNITSIZE / 2) - 1, (YUNITSIZE / 2) - 1);
	  }

      }

    /* If we have background clear up to the edge, handle that here */

    if (bstart != -1)
      {
	GrSetGCForeground(gc, WHITE);
	GrFillRect(offscreen, gc, (bstart * XUNITSIZE), (y * YUNITSIZE), 
		   (bend - bstart + 1) * XUNITSIZE, YUNITSIZE);
	
	bend = bstart = 0;
      }
    
    }
  
  GrDestroyGC(gc);
}

void draw_nibble(void)
{
  /* If there is no nibble assigned, then pick an new spot */
  if (!nibble.active)
    {
      while(1)
	{
	  int x= 0 + (int) (rand() % (XUNITS - 1));
	  int y= 0 + (int) (rand() % (YUNITS - 1));
	  
	  if (playground[y][x] != PLAYGROUND_EMPTY) continue;
      if (playground[y+1][x] != PLAYGROUND_EMPTY) continue;
      if (playground[y-1][x] != PLAYGROUND_EMPTY) continue;
      if (playground[y][x+1] != PLAYGROUND_EMPTY) continue;
      if (playground[y][x-1] != PLAYGROUND_EMPTY) continue;

	  if (y > 1 && playground[y-1][x] != PLAYGROUND_EMPTY)
	    continue;

	  if (y < (YUNITS - 2) && playground[y + 1][x] != PLAYGROUND_EMPTY)
	    continue;

	  if (x > 1 && playground[y][x - 1] != PLAYGROUND_EMPTY)
	    continue;

	  if (x < (XUNITS - 2) && playground[y][x + 1] != PLAYGROUND_EMPTY)
	    continue;

	  /* For now, make sure that the nibble doesn't */
	  /* show up near the border */
	  
	  
	  nibble.x = x;
	  nibble.y = y;
	  nibble.active = 1;
	  break;
	}
    }
  
  playground[nibble.y][nibble.x] = PLAYGROUND_NIBBLE;
  playground[nibble.y][nibble.x+1] = PLAYGROUND_NIBBLE;
  playground[nibble.y][nibble.x-1] = PLAYGROUND_NIBBLE;
  playground[nibble.y+1][nibble.x] = PLAYGROUND_NIBBLE;
  playground[nibble.y-1][nibble.x] = PLAYGROUND_NIBBLE;

}

void draw_border(void)
{
#ifdef NOTUSED
  int y  = 0;

  /* FIXME:  This should be more dynamic, eh? */
  /* For now, just a square box */
  
  memset(&playground[0][0], PLAYGROUND_BORDER, XUNITS);
  memset(&playground[YUNITS - 1][0], PLAYGROUND_BORDER, XUNITS);

  for(y = 0; y < YUNITS; y++)
    {
      playground[y][0] = PLAYGROUND_BORDER;
      playground[y][XUNITS - 1] = PLAYGROUND_BORDER;
    }
#endif

  memcpy(playground, level[current_level].bitmap, XUNITS * YUNITS);
}

/* This just draws the snake into the matrix */
/* draw_playground() actually puts it on the screen */

void draw_snake(void)
{
  int i = 0;

  int sx = global_snake.headx;
  int sy = global_snake.heady;
    
  int ex = 0;
  int ey = 0;

  int pos = 0;

  while(global_snake.body[pos])
    {
      unsigned char dir = GET_SNAKE_DIRECTION(global_snake.body[pos]);
      unsigned short off = GET_SNAKE_OFFSET(global_snake.body[pos]);

      switch(dir)
	{
	case SNAKE_DIR_RIGHT:

	  if (sx - off < 0)
	    {
	      int remainder;

	      /* Split the line */
	      memset(&playground[sy][0], PLAYGROUND_SNAKE, sx);

	      remainder = off - sx - 1;
	      
	      memset(&playground[sy][XUNITS - remainder], PLAYGROUND_SNAKE, remainder);
	      
	      ex = XUNITS - 1 - remainder;
	    }
	  else
	    {
	      /* We can just memset the line, because it goes horizontal */
	      memset(&playground[sy][sx - off], PLAYGROUND_SNAKE, off);
	      ex = sx - off;
	    }

	  ey = sy;

	  break;
	
	case SNAKE_DIR_LEFT:

	  if (sx + off > (XUNITS - 1))
	    {
	      int remainder;

	      /* Split the line */
	      memset(&playground[sy][sx], PLAYGROUND_SNAKE, XUNITS - sx);

	      remainder = off - ((XUNITS - 1) - sx) - 1;

	      memset(&playground[sy][0], PLAYGROUND_SNAKE, remainder);
	      
	      ex = remainder;
	    }
	  else
	    {
	      /* We can just memset the line, because it goes horizontal */
	      memset(&playground[sy][sx], PLAYGROUND_SNAKE, off);
	      ex = sx + off;
	    }

	  ey = sy;
	  break;

	case SNAKE_DIR_DOWN:

	  ex = sx;

	  if (sy - off < 0)
	    {
	      int remainder;
	      remainder = off - sy - 1;

	      ey = YUNITS - 1 - remainder;
          
	      for(i = 0; i <= sy; i++)
		playground[i][sx] = PLAYGROUND_SNAKE;
	      
	      for(i = ey; i <= YUNITS - 1; i++)
		playground[i][sx] = PLAYGROUND_SNAKE;
	    }
	  else
	    {
	      ey = sy - off;
	      
	      for(i = ey; i <= sy; i++)
		playground[i][sx] = PLAYGROUND_SNAKE;
	    }

	  break;
	  
	case SNAKE_DIR_UP:
	  ex = sx;

	  if (sy + off > (YUNITS - 1))
	    {
	      int remainder;

	      for(i = sy; i <= YUNITS - 1; i++)
		playground[i][sx] = PLAYGROUND_SNAKE;

	      remainder = off - ((YUNITS - 1) - sy) - 1;
	      
	      for(i = 0; i <= remainder; i++)
		playground[i][sx] = PLAYGROUND_SNAKE;
	      
	      ey = remainder;
	    }
	  else
	    {
	       ey = sy + off;

	       for(i = sy; i <= ey; i++)
		 playground[i][sx] = PLAYGROUND_SNAKE;

	    }

	  break;
	}
      
      sx = ex;
      sy = ey;
      pos++;
    }
}

void draw_score(void)
{
  //int tw, th, tb;

  char text[100];
  
  sprintf(text, "Lives:%d Lev:%d Score:%d",
  global_snake.lives, global_level + 1, global_snake.score);
  
  if(image_loaded == 1)
  	nxsnake_draw_header();
  else
  	pz_draw_header(text);
  
}

void do_frame(int full)
{

  /* Clear it out */
  memset(playground, PLAYGROUND_EMPTY, XUNITS * YUNITS);

  /* Fill the matrix */
  draw_border();
  draw_snake();

  draw_nibble();

  /* Only draw the score if we need to */
  /* draw_score(); */

  /* Draw it to the offscreen buffer */
  draw_screen(full);

  /* And finally, show it on the screen */
  show_buffer();
}
  
void start_level(int l)
{
  init_level(l);   /* Initalize the snake */
  if ((image_loaded == 1) && (game_state == SNAKE_START))
      pz_close_window(boot_win);
      
  game_state = SNAKE_PLAYING;
  if(global_snake.score < 60)
  	game_speed = start_speed + 10;
  nibble.active = 0;
  nibble.x = 0;
  nibble.y = 0;

  draw_score();
}

void start_game(void)
{
  current_level = 0;
  global_level = 0;

  global_snake.lives = 3;
  global_snake.score = 0;

  game_speed = start_speed;

  start_level(current_level);
  draw_score();
  do_frame(1);
}
  
void end_game(void)
{
  game_state = SNAKE_DONE;
  pz_draw_header("Snake");
  /* draw_score(); */
  draw_string((char *) gameended, 1);
  show_buffer();
}

void do_snake_advance(void)
{
  switch(advance_snake())
    {
    case MOVE_LEGAL:
      do_frame(0);
      break;
      
    case MOVE_NIBBLE:
      if ((global_snake.score % LEVEL_SCORE) == 0)
		{
	  	current_level++;
        global_level++;
	  	game_state = SNAKE_NEXTLEVEL;
        if(global_snake.score > 60)
    		global_snake.lives ++;
	  	draw_string((char *) nextlevel, 2);
	  	show_buffer();
		}
      else
		{
        /* As it's not just a point : */
        nibble.active = 0;
        do_frame(0);
	  	draw_score();
		}
      
      break;
      
    case MOVE_ILLEGAL:
      global_snake.lives--;
      //global_snake.score = 0;
      if (!global_snake.lives)
		end_game();
      else
		{
	  	game_state = SNAKE_NEXTLEVEL;
	  	draw_string((char *) snakedied, 2);
	  	show_buffer();
		}
      
      break;
    }
}
  
static int nxsnake_handle_event(GR_EVENT *event)
{
  switch(event->type)
    {
    case GR_EVENT_TYPE_KEY_DOWN:

      /* Allow m to quit no matter where we are*/
      
      if (event->keystroke.ch == 'm')
	    {
	      //GrClose();
          fin = 1;
          if(image_loaded == 1) {
          	pz_close_window(head_win);
            if (game_state == SNAKE_START) {
          	  pz_close_window(boot_win);
            }
          }
      	  pz_close_window(offscreen);
          pz_close_window(swindow);
          GrDestroyTimer(nxsnake_timer);

          break;
	      //exit(0);
	    }
      
      switch(game_state)
	{
	case SNAKE_START:
        if (event->keystroke.ch == 'd') 
            {
			game_state = SNAKE_START;
            draw_string((char *) instructions, 1);
            show_buffer();
            break;
            }
	case SNAKE_INSTRUCTIONS:
	case SNAKE_DONE:
	  
	  switch(event->keystroke.ch)
	    {

	    default:
	      start_game();
	      break;
	    }

	  break;

	case SNAKE_PAUSED:
	  if ((event->keystroke.ch == '\r') ||
          (event->keystroke.ch == 'd'))
	    {
	      draw_score();
	      do_frame(1);
	      game_state = SNAKE_PLAYING;
	    }
	  
	  break;

	case SNAKE_NEXTLEVEL:

	  if (current_level >= LEVELCOUNT)
	    current_level = 0;
	  
	  start_level(current_level);
	  draw_score();
	  do_frame(1);   /* and show the first frame */

	  break;

	case SNAKE_PLAYING:
      if (event->keystroke.ch == 'd') 
            {
			game_state = SNAKE_PAUSED;
            draw_string((char *) instructions, 1);
            show_buffer();
            break;
            }
    
	  if (event->keystroke.ch == '\r')
	    	{
			game_state = SNAKE_PAUSED;
          	draw_string((char *) gamepaused, 2);
	  	  	show_buffer();
	      	break;
	    	}
	  
      if ((event->keystroke.ch == 'w') || (event->keystroke.ch == 'f'))
      		{
      		if (redirect_snake(event->keystroke)) 
	    		do_snake_advance();
        	}
        
      if ((event->keystroke.ch == 'l') || (event->keystroke.ch == 'r'))
      	{
        if(last_keystroke == event->keystroke.ch)
      		{
        	if (count_wheel > 7)
            	{
                if (redirect_snake(event->keystroke)) 
	    			do_snake_advance();
                count_wheel = 0;
                }
        	else
 				count_wheel ++;
        	}
      	else
      		count_wheel = 0;
        }
      
        
	  last_keystroke = event->keystroke.ch;
      
	  break;
	}

      break;

    case GR_EVENT_TYPE_EXPOSURE:
      show_buffer();
      break;
    }
 return(1);
}
    
void handle_idle()
{
  switch(game_state)
    {
    case SNAKE_START:
    case SNAKE_INSTRUCTIONS:
    	break;
    case SNAKE_DONE:
    case SNAKE_NEXTLEVEL:
    case SNAKE_PAUSED:
      /* nothing to do here */
      break;
      
    case SNAKE_PLAYING:
      snake_count = snake_count + 5;
      if((snake_count >= game_speed) || (game_speed < 5))
      	{  	      
      	do_snake_advance();
        snake_count = 0;
        }
      break;
    }
}

void do_draw()
{
	printf("Do draw\n");
	pz_draw_header("Snake");
}

void nxsnake_draw_header()
{
	char text[3];
    GR_GC_ID gc = GrNewGC();
	GrDrawImageToFit(head_win, gc, 0, 0, banner_image_w, banner_image_h, banner_image_id); 
    GrSetGCUseBackground (gc,0);
    GrSetGCForeground(gc, BLACK);
    sprintf(text, "%d",global_snake.lives);
    GrText(head_win, gc, 32, 14, text, -1, 0);
    
    sprintf(text, "%d",global_level + 1);
    GrText(head_win, gc, 76, 14, text, -1, 0);
    
    sprintf(text, "%d",global_snake.score);
    GrText(head_win, gc, 119, 14, text, -1, 0);
      
    GrDestroyGC(gc);
}

void draw_boot_image()
{
	GR_GC_ID gc = GrNewGC();
	GrDrawImageToFit(boot_win, gc, 0, 0, boot_image_w, boot_image_h, boot_image_id); 
    GrDestroyGC(gc);
}

int load_image()
{
	// KERIPO MOD
	//if (!(boot_image_id = GrLoadImageFromFile("/nxsnake/boot_image.gif", 0))) {
	if (!(boot_image_id = GrLoadImageFromFile(NXSNAKE_BOOT_IMG, 0))) {
                fprintf(stderr, "Can't load boot image file\n");
                return 0;
        }
    /*if (!(boot_image_id = GrLoadImageFromBuffer(boot_image, 20480, 1))) {
                fprintf(stderr, "Can't load boot image file\n");
                return 0;
        }*/

	// KERIPO MOD
    //if (!(banner_image_id = GrLoadImageFromFile("/nxsnake/banner_image.gif", 0))) {		
    if (!(banner_image_id = GrLoadImageFromFile(NXSNAKE_BANNER_IMG, 0))) {
                fprintf(stderr, "Can't load header image file\n");
                return 0;
        }
    
    GrGetImageInfo(boot_image_id, &boot_image_info);
	boot_image_w = boot_image_info.width;
    boot_image_h = boot_image_info.height;
    GrGetImageInfo(banner_image_id, &banner_image_info);
	banner_image_w = banner_image_info.width;
    banner_image_h = banner_image_info.height;
    return 1;
    
}

int new_snake_window(int argc, char **argv)
{
  GR_SCREEN_INFO si;
  
  fin = 0;
  snake_count = 0;
  anim_image = 0;
  game_speed = start_speed;

  GrGetScreenInfo(&si); /* Get screen info */
      
  srand(time(0));
  
  game_state = SNAKE_START;

  /* Make the window */
 
  /*swindow = GrNewWindowEx(WM_PROPS, "nxsnake", GR_ROOT_WINDOW_ID, 
		       10, 10, WWIDTH, WHEIGHT, BLACK);*/
  
  swindow = pz_new_window (0,HEADER_TOPLINE+1,si.cols,si.rows-HEADER_TOPLINE-1,
                         do_draw,nxsnake_handle_event);
  
  GrSelectEvents(swindow, GR_EVENT_MASK_EXPOSURE | 
		 		GR_EVENT_MASK_KEY_DOWN|GR_EVENT_MASK_TIMER);

  nxsnake_timer = GrCreateTimer( swindow, 5 );

  offscreen = pz_new_window (0,HEADER_TOPLINE+1,si.cols,si.rows-HEADER_TOPLINE-1, do_draw,nxsnake_handle_event);
  GrSelectEvents(offscreen, GR_EVENT_MASK_KEY_DOWN|GR_EVENT_MASK_TIMER);

  GrMapWindow(offscreen);
  GrMapWindow(swindow);

  image_loaded = 0;
  if(load_image())
  	{
    image_loaded = 1;
 	head_win = pz_new_window (0,0,si.cols,HEADER_TOPLINE,
  			nxsnake_draw_header,nxsnake_handle_event);
    boot_win = pz_new_window (0,0,si.cols,si.rows,
                    do_draw,nxsnake_handle_event);
    
    GrSelectEvents(head_win, GR_EVENT_MASK_KEY_DOWN|GR_EVENT_MASK_TIMER);
    GrSelectEvents(boot_win, GR_EVENT_MASK_KEY_DOWN|GR_EVENT_MASK_TIMER);

    GrMapWindow(boot_win);
    GrMapWindow(head_win);
    draw_boot_image();
    }
  else   /* Draw the instructions into the buffer */
  	{
    draw_string( (char *) welcome, 1);
    do_draw();
    }

  while(fin != 1)
    {
      GR_EVENT event;

      /* We start at 130ms, but it goes down every */
      /* time a nibble is eaten */

      /* If they get this far, then they rock! */
      if (game_speed < 5) game_speed = 5;

      GrGetNextEventTimeout(&event, game_speed);
      
      switch(event.type)
        {
        case GR_EVENT_TYPE_EXPOSURE:
          break;
        case GR_EVENT_TYPE_KEY_DOWN:
          nxsnake_handle_event(&event);
          break;

        case GR_EVENT_TYPE_TIMER:
          handle_idle();
          break;
          
        }
    
    }
    
  return(1);
}
