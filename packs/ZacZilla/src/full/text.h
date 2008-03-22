/* TEXT.H
   
   Text messages for the nxSnake game 
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

char welcome[6][50] = {
  "Welcome to NXSnake for iPod",
  " ",
  "Press menu to leave",
  "Play for instructions", 
  "or any other key to start...",
  ""
};

char instructions[11][50] = {
  "  -----  ",
  "  -----  ",
  "   ",
  "   ",
  "Use Fast Forward and Rewind",
  "to direct your snake",
  "Eat the nibbles for points,",
  "avoid hitting the walls.",
  "Menu quits, action pauses",
  ""
};

char nextlevel[4][50] = {
  "Congratulations, you have moved",
  "to the next level.  ",
  "Press any key to start...",
  ""
};

char snakedied[3][50] = {
  "Oops!  Your snake died.",
  "Press any key to try again..."
  ""
};

char gameended[3][50] = {
  "Too bad !! You lost.",
  "Press any key to try again..."
  ""
};

char gamepaused[3][50] = {
  " -- Game Paused --",
  " Press Action ",
  ""
};
