/* NXSNAKE.H
   
   Constants for the nxSnake game 
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

#include "ipod.h"

#ifndef NXSNAKE_H
#define NXSNAKE_H

#define WM_PROPS   (GR_WM_PROPS_NOAUTOMOVE |\
                    GR_WM_PROPS_BORDER |\
		    		GR_WM_PROPS_CAPTION |\
                    GR_WM_PROPS_CLOSEBOX)

#define LEVEL_SCORE 10 /* Advance levels every 10 nibbles */

  // Size initialization :
  /* This is where we define the size of the whole window */

#define WWIDTH  160    /* 138 for iPod mini*/
#define WHEIGHT 128	   /* 110 for iPod mini*/

  /* This is the size of the playground */
  /* Minus room for the score and such  */

#define PWIDTH (WWIDTH)
#define PHEIGHT (WHEIGHT - HEADER_TOPLINE)

  /* This is the number of units the screen should be */

#define XUNITS (PWIDTH / 2)
#define YUNITS (PHEIGHT / 2)

  /* This is the size of the border, the snake, and the nibbles */
#define XUNITSIZE (PWIDTH / XUNITS)
#define YUNITSIZE (PHEIGHT / YUNITS)


/* Game states */

#define SNAKE_START        0
#define SNAKE_INSTRUCTIONS 1
#define SNAKE_PLAYING      2
#define SNAKE_PAUSED       3
#define SNAKE_NEXTLEVEL    4
#define SNAKE_DONE         5

#define SNAKE_DIR_UP    0x01
#define SNAKE_DIR_DOWN  0x04
#define SNAKE_DIR_LEFT  0x02
#define SNAKE_DIR_RIGHT 0x08

#define PLAYGROUND_EMPTY  0x01
#define PLAYGROUND_BORDER 0x02
#define PLAYGROUND_SNAKE  0x03
#define PLAYGROUND_TAIL   0x04
#define PLAYGROUND_NIBBLE 0x05

#define GET_SNAKE_DIRECTION(val)      ((val & (0xF << 12)) >> 12)
#define GET_SNAKE_OFFSET(val)         (val & 0xFFF)

typedef struct
{
  unsigned short score;
  unsigned char lives;

  unsigned char  speed;  /* The speed of the snake */

  unsigned char headx;  /* The absolute position of the head */
  unsigned char heady;
  
  unsigned char tailpointer;

  unsigned short length;
  unsigned short growth;
  
  /* Allow up to 256 girations */
  unsigned short body[256];
} snake_t;

#endif
