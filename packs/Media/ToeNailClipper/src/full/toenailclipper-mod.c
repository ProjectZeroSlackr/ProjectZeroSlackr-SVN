// Last updated: March 12, 2008
// ~Keripo

/* The Toenail Clipping Program Source File
Version 2.5 for iPod Linux(Ported by Brandon Holland)
Started programming on Tuesday, June 14, 2005 at 5:00:pm PST
(c) Copyright Paul Taylor, 2005

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

If you make any changes to the code, be sure to send me a copy so I can check it out! 

iPod Linux Port Changes:
->Replaced C++ headers iostream.h + string.h with C headers stdio.h + stdlib.h (Conversion from C++ to C);
->Changed bool functions to int functions (Conversion from C++ to C);
->Removed Save Game Feature (Coming Soon! Need to convert from fstream.h funtions to stdio.h functions);
->Reformatted output to fit iPod LCD screen resolution;
->Reformatted input to be compatible with iPod Click-wheel + buttons; */

#include <stdio.h>
#include <stdlib.h>

int checkdata();
void welcome();
int checkstatus();
void resetstatus();
int restart();
void displaystatus();
int continueclipping();
char whichfoot();
int whichtoe(char foot);
int checktoe(char foot, short int toenumber);
void cliptoe(char foot, short int toenumber);
void gettoe();
void goodbye();
int close();

char leftfoot[] = "!!!!!";
char rightfoot[] = "!!!!!";
short int counter;

int main()
{
  int alldone = 0;
  welcome();
  do
  {
    if (checkdata() == 1)
    { resetstatus(); }
    do
    {
      if (checkstatus() == 1)
      {
        if (restart()) 
        { break; }
      }
      displaystatus();
      alldone = continueclipping();
    } 
    while (alldone == 0);
  resetstatus();
  goodbye();
  } 
  while (close() == 0);
  return 0;
}

int checkdata()
{
  short int badvalue = 0;
  for (counter = 0; counter < 5; counter++)
  {
    if (((leftfoot[counter]) != '!') && ((leftfoot[counter]) != '*'))
    {
      badvalue = 1;
      break;
    }
    if (((rightfoot[counter]) != '!') && ((rightfoot[counter]) != '*'))
    {
      badvalue = 1;
      break;
    }
  }
  if (badvalue == 1)
  {
    printf("corrupted.\n");
    printf("resetting.\n\n");
    return 1;
  }
  return 0;
}

void welcome()
{
  printf("Welcome to TNC!\n\n");
  return;
}

int checkstatus()
{
  if (!(strcmp(leftfoot, "*****")) && !(strcmp(rightfoot, "*****")))
  { return 1; }
  return 0;
}

void resetstatus()
{
  strcpy(leftfoot, "!!!!!");
  strcpy(rightfoot, "!!!!!");
  return;
}

int restart()
{
  char restartchoice;
  displaystatus();
  printf("all clipped!\n\n");
  do
  {
    printf("start over?\n");
    printf("[y/n]: ");
    scanf(" %c", &restartchoice);
  } 
  while ((restartchoice != 'y') && (restartchoice != 'n'));
  if (restartchoice == 'y')
  {
    resetstatus();
    return 0;
  }
  return 1;
}

void displaystatus()
{
  printf("\nstatus:\n\n");
  printf("left  right\n");
  printf("12345 54321\n");
  printf("%s ", leftfoot);
  printf("%s", rightfoot);
  printf("\n\n");
  return;
}

int continueclipping()
{
  int flag = 0;
  char continuechoice;
  do
  {
    printf("clip now?\n");
    printf("[y/n]: ");
    scanf(" %c", &continuechoice);
  } 
  while ((continuechoice != 'y') && (continuechoice != 'n'));
  if (continuechoice == 'y')
  {
    gettoe();
    return 0;
  }
  return 1;
}

char whichfoot()
{
  char foot;
  if (!(strcmp(leftfoot, "*****")))
  { foot = 'f'; }
  else if (!(strcmp(rightfoot, "*****")))
  { foot = 'w'; }
  else
  {
    do
    {
     printf("\nwhich foot?\n");
     printf("[l/r)]: ");
     scanf(" %c", &foot);
    } 
    while ((foot != 'l') && (foot != 'r'));
  }
  return (foot);
}

int whichtoe(char foot)
{
  char footname[5];
  char toe;
  short int whichtoe;
  if (foot == 'l')
  { 
    strcpy(footname, "left"); 
    do
    {
      printf("\nwhich toenail?\n");
      printf("[1,2,3,4,5]: ");
      scanf(" %c", &toe); 
      switch(toe)
      {
        case '1':
        whichtoe = 1;
        break;
        case '2':
        whichtoe = 2;
        break;
        case '3':
        whichtoe = 3;
        break;
        case '4':
        whichtoe = 4;
        break;
        case '5':
        whichtoe = 5;
        break;
        default:
        whichtoe = 0;
        break;
      }
    } 
    while ((whichtoe < 1) || (whichtoe > 5));
  }
  if (foot == 'r')
  { 
    strcpy(footname, "right"); 
    do
    {
      printf("\nwhich toenail?\n");
      printf("[5,4,3,2,1]: ");
      scanf(" %c", &toe);
      switch(toe)
      {
        case '5':
        whichtoe = 1;
        break;
        case '4':
        whichtoe = 2;
        break;
        case '3':
        whichtoe = 3;
        break;
        case '2':
        whichtoe = 4;
        break;
        case '1':
        whichtoe = 5;
        break;
        default:
        whichtoe = 0;
        break;
      }
    } 
    while ((whichtoe < 1) || (whichtoe > 5));
  }
  return (whichtoe);
}

int checktoe(char foot, short int toenumber)
{
  if (foot == 'l')
  {
    if (leftfoot[(toenumber - 1)] == '*')
    {
      printf("already clipped!\n");
      return 1;
    }
  }
  else
  {
    if (rightfoot[(toenumber - 1)] == '*')
    {
      printf("already clipped!\n");
      return 1;
    }
  }
  return 0;
}

void cliptoe(char foot, short int toenumber)
{
  if (foot == 'l')
  { leftfoot[(toenumber - 1)] = '*'; } 
  else
  { rightfoot[(toenumber - 1)] = '*'; }
  printf("*clip, clip*\n");
  return;
}

void gettoe()
{
  char foot;
  short int toenumber;
  foot = whichfoot();
  toenumber = whichtoe(foot);
  printf("\n");
  if (checktoe(foot, toenumber) == 0)
  { cliptoe(foot, toenumber); }
  return;
}

void goodbye()
{
  printf("\nTNC version 2.5.\n");
  printf("(C)Copyrights TNC\n");
  printf("Paul Taylor,2005.\n");
  printf("TNC is GNU GPL'd.\n");
  printf("Thanx to Jesse\n");
  printf("for the TNC idea.\n");
  printf("iPod port done by\n");
  printf("Brandon Holland.\n");
  printf("Modified by Keripo\n\n");
  return;
}

int close()
{
  char close;
  do
  {
    if (checkstatus() == 1)
    { resetstatus(); }
    printf("close now?\n");
    printf("[y/n]: ");
    scanf(" %c", &close);
  } 
  while ((close != 'y') && (close != 'n'));
  if (close == 'y')
  {  
    printf("\nbye-bye!\n");
    return 1;
  }    
  return 0;
}
