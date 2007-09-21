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
  "Welcome to NXSnake for Floydzilla",
  " ",
  "Press MENU to Quit",
  "PLAY for Instructions", 
  "any other key to Start!",
  ""
};

char instructions[10][50] = {
  "  -----  ",
  "  -----  ",
  "   ",
  "   ",
  "-Use Fast Forward and Rewind",
  "to direct your snake",
  "-Eat the nibbles for points,",
  "-Avoid hitting the walls.",
  "Menu: Quits - Action: Pauses",
  ""
};

char nextlevel[5][50] = {
  "Congratulations!",
  "You have moved to",
  "the Next Level.",
  "Press any key to Start...",
  ""
};

char snakedied[3][50] = {
  "Oops! Your Snake died.",
  "Press any key to try again..."
  ""
};

char gameended[3][50] = {
  "Too bad... Game Over.",
  "Press any key to try again..."
  ""
};

char gamepaused[3][50] = {
  " -- Game Paused --",
  " Press Action ",
  ""
};
