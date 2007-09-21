/* TEXT.H
   
   4 levels for the nxSnake game
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

#include "nxsnake.h"
#define LEVELCOUNT 6

struct
{
  int startx;
  int starty;
  int dir;
  char bitmap[XUNITS * YUNITS];
} level[LEVELCOUNT] = {
{ 20, 20,  8, {} },
{ 7,   3,  8, {} },
{ 10, 10,  8, {} },
{ 10, 10,  8, {} },
{ 10, 10,  4, {} },
{ 10,  5,  8, {} }
};
