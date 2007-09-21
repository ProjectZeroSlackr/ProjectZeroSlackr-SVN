#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "pz.h"

static void fire(void);
static void them(void);
static int h,w,z,x,y,temp=0,health=100,stat=0;
  static TWidget *wid;
  static int shot1[2];
  static int shot2[2];
  static int shot3[2];
   static int them1[2];
  static int them2[2];
  static int them3[2];
  static int them4[2];
  static int them5[2];
 static  TWindow *win;
static void usvsthem_draw(TWidget *this,ttk_surface srf)
{
  
 char ct[3];
   ttk_fillrect(srf,0,0,w,h,ttk_makecol(WHITE));

  ttk_text(srf,ttk_menufont,x,y,ttk_makecol(BLACK),"US");
 // sprintf(ct,"Health:%i",health);
  ttk_text(srf,ttk_menufont,3,3,ttk_makecol(BLACK),"Health:");
  ttk_text(srf,ttk_menufont,3,15,ttk_makecol(BLACK),health);
  
  if(shot1[0]==1) ttk_text(srf,ttk_menufont,shot1[1],shot1[2],ttk_makecol(BLACK),"DIE");
  if(shot2[0]==1) ttk_text(srf,ttk_menufont,shot2[1],shot2[2],ttk_makecol(BLACK),"DIE");
  if(shot3[0]==1) ttk_text(srf,ttk_menufont,shot3[1],shot3[2],ttk_makecol(BLACK),"DIE");
  if(them1[0]==1) ttk_text(srf,ttk_menufont,them1[1],them1[2],ttk_makecol(BLACK),"THEM");
  if(them2[0]==1) ttk_text(srf,ttk_menufont,them2[1],them2[2],ttk_makecol(BLACK),"THEM");
  if(them3[0]==1) ttk_text(srf,ttk_menufont,them3[1],them3[2],ttk_makecol(BLACK),"THEM");
  if(them4[0]==1) ttk_text(srf,ttk_menufont,them4[1],them4[2],ttk_makecol(BLACK),"THEM");
  if(them5[0]==1) ttk_text(srf,ttk_menufont,them5[1],them5[2],ttk_makecol(BLACK),"THEM");
 
 
}
static int usvsthem_timer(TWidget *this)
{
 
  if (ttk_button_pressed (TTK_BUTTON_MENU)) y-=8;
  if (ttk_button_pressed (TTK_BUTTON_PLAY)) y+=8;
  if (ttk_button_pressed (TTK_BUTTON_PREVIOUS)) x-=8;
  if (ttk_button_pressed (TTK_BUTTON_NEXT)) x+=8;
  if (ttk_button_pressed (TTK_BUTTON_ACTION)) fire();
   if (ttk_button_pressed (TTK_BUTTON_HOLD)) pz_close_window(this->win);
   if(x>=w) x=w;
   if(x<=0) x=0;
   if(y>=h) y=h;
   if(y<=0) y=0;
   if(shot1[0]==1) {
     if(shot1[2]<=0) shot1[0]=0;
     else { 
     shot1[2]-=15;
     temp=shot1[1]-them1[1];
     if((temp<5 && temp> -6)) {
       temp=shot1[2]-them1[2];
       if(temp<5 && temp > -6) 
       them1[0]=0;
     }
     temp=shot1[1]-them2[1];
     if((temp<5 && temp> -6)) {
       temp=shot1[2]-them2[2];
       if(temp<5 && temp > -6) 
       them2[0]=0;
     }
     temp=shot1[1]-them3[1];
     if((temp<5 && temp> -6)) {
       temp=shot1[2]-them3[2];
       if(temp<5 && temp > -6) 
       them3[0]=0;
     }
         temp=shot1[1]-them4[1];
     if((temp<5 && temp> -6)) {
       temp=shot1[2]-them4[2];
       if(temp<5 && temp > -6) 
       them4[0]=0;
     }
         temp=shot1[1]-them5[1];
     if((temp<5 && temp> -6)) {
       temp=shot1[2]-them5[2];
       if(temp<5 && temp > -6) 
       them5[0]=0;
     }
     }
   }
   if(shot2[0]==1) {
     if(shot2[2]<=0) shot2[0]=0;
     else { 
     shot2[2]-=15;
     temp=shot2[1]-them1[1];
     if((temp<5 && temp> -6)) {
       temp=shot2[2]-them1[2];
       if(temp<5 && temp > -6) 
       them1[0]=0;
     }
     temp=shot2[1]-them2[1];
     if((temp<5 && temp> -6)) {
       temp=shot2[2]-them2[2];
       if(temp<5 && temp > -6) 
       them2[0]=0;
     }
     temp=shot2[1]-them3[1];
     if((temp<5 && temp> -6)) {
       temp=shot2[2]-them3[2];
       if(temp<5 && temp > -6) 
       them3[0]=0;
     }
      temp=shot2[1]-them4[1];
     if((temp<5 && temp> -6)) {
       temp=shot2[2]-them4[2];
       if(temp<5 && temp > -6) 
       them4[0]=0;
     }
      temp=shot2[1]-them5[1];
     if((temp<5 && temp> -6)) {
       temp=shot2[2]-them5[2];
       if(temp<5 && temp > -6) 
       them5[0]=0;
     }
     }
   }
   if(shot3[0]==1) {
     if(shot3[2]<=0) shot3[0]=0;
     else {
     shot3[2]-=15;
     temp=shot3[1]-them1[1];
     if((temp<5 && temp> -6)) {
       temp=shot3[2]-them1[2];
       if(temp<5 && temp > -6) 
       them1[0]=0;
     }
     temp=shot3[1]-them2[1];
     if((temp<5 && temp> -6)) {
       temp=shot3[2]-them2[2];
       if(temp<5 && temp > -6) 
       them2[0]=0;
     }
     temp=shot3[1]-them3[1];
     if((temp<5 && temp> -6)) {
       temp=shot3[2]-them3[2];
       if(temp<5 && temp > -6) 
       them3[0]=0;
     }
     temp=shot3[1]-them4[1];
     if((temp<5 && temp> -6)) {
       temp=shot3[2]-them4[2];
       if(temp<5 && temp > -6) 
       them4[0]=0;
     }
        temp=shot3[1]-them5[1];
     if((temp<5 && temp> -6)) {
       temp=shot3[2]-them5[2];
       if(temp<5 && temp > -6) 
       them5[0]=0;
     }
     }
   }
   
   if(them1[0]==1) {
     if(them1[2]>=h || them1[1]>=w || them1[1]<=0 || them1[2]<=0) them1[0]=0;
     else {
     if(them1[1]>x) them1[1]-=2;
     if(them1[1]<x) them1[1]+=2;
      if(them1[2]>y) them1[2]-=4;
     if(them1[2]<y) them1[2]+=4;   
   }  
   }
   if(them2[0]==1) {
   if(them2[2]>=h || them2[1]>=w || them2[1]<=0 || them2[2]<=0) them2[0]=0;
     else {
     if(them2[1]>x) them2[1]-=1;
     if(them2[1]<x) them2[1]+=1;
      if(them2[2]>y) them2[2]-=5;
    if(them2[2]<y) them2[2]+=5;   
   }  
   }
   if(them3[0]==1) {
    if(them3[2]>=h || them3[1]>=w || them3[1]<=0 || them3[2]<=0) them3[0]=0;
     else {
     if(them3[1]>x) them3[1]-=4;
     if(them3[1]<x) them3[1]+=4;
      if(them3[2]>y) them3[2]-=2;
    if(them3[2]<y) them3[2]+=2;   
   }  
   }
      if(them4[0]==1) {
    if(them4[2]>=h || them4[1]>=w || them4[1]<=0 || them4[2]<=0) them4[0]=0;
     else {
     if(them4[1]>x) them4[1]-=5;
     if(them4[1]<x) them4[1]+=5;
      if(them4[2]>y) them4[2]-=1;
    if(them4[2]<y) them4[2]+=1;   
   }  
   }
      if(them5[0]==1) {
    if(them5[2]>=h || them5[1]>=w || them5[1]<=0 || them5[2]<=0) them5[0]=0;
     else {
     if(them5[1]>x) them5[1]-=3;
     if(them5[1]<x) them5[1]+=3;
      if(them5[2]>y) them5[2]-=3;
    if(them5[2]<y) them5[2]+=3;   
   }  
   }
   z++;
   if(z>=30) {
     z=0;
     them();
   }
   if(x-them1[1]<5 && x-them1[1] > -6)  
   if(y-them1[2]<5 && x-them1[2] > -6) { 
   them1[0]=0 ;health-=5;}
   if(x-them2[1]<5 && x-them2[1] > -6) 
   if(y-them2[2]<5 && y-them2[2] > -6) 
   { them2[0]=0; health-=5;}
   if(x-them3[1]<5 && x-them3[1] > -6)
   if(y-them3[2]<5 && y-them3[2] > -6)
    { them3[0]=0; health-=5;}
   if(x-them4[1]<5 && x-them4[1] > -6) 
   if(y-them4[2]<5 && y-them4[2] > -6) 
    {them4[0]=0; health-=5;}
   if(x-them5[1]<5 && x-them5[1] > -6) 
   if(y-them5[2]<5 && y-them5[2] > -6) 
    {them5[0]=0; health-=5;}
   
   
   
if(health<=0) {
new_message_window("Game Over!");
pz_close_window(this->win);
}
  this->dirty++;
  return 0;
}

static void fire()
{
  if(shot1[0]==0) {
    shot1[1]=x;
    shot1[2]=y+5;
    shot1[0]=1;
  }
  else if(shot2[0]==0) {
  shot2[1]=x;
    shot2[2]=y+5;
    shot2[0]=1;
  }
  else if(shot3[0]==0) {
  shot3[1]=x;
    shot3[2]=y+5;
    shot3[0]=1;
  }
}

static void them()
{
  if(them1[0]==0) {
    them1[1]=rand()%w;
    them1[2]=5;
    them1[0]=1;
  }
  else if(them2[0]==0) {
  them2[1]=rand()%w;
    them2[2]=5;
    them2[0]=1;
  }
  else if(them3[0]==0) {
  them3[1]=rand()%w;
    them3[2]=5;
    them3[0]=1;
 }
 else if(them4[0]==0) {
  them4[1]=rand()%w;
    them4[2]=5;
    them4[0]=1;
 }
 else if(them5[0]==0) {
  them5[1]=rand()%w;
    them5[2]=5;
    them5[0]=1;
 }
}

TWindow *new_usvsthem_window()
{

  win=ttk_new_window("Us Vs Them");
  wid=ttk_new_widget(0,0);
  ttk_window_hide_header(win);
  wid->focusable=1;
  wid->draw=usvsthem_draw;
  wid->timer=usvsthem_timer;
  ttk_add_widget(win,wid);
  h=ttk_screen->h;
  w=ttk_screen->w;
  x=w/2;
  y=h/4*3;
  ttk_widget_set_timer(wid,100);
  shot1[0]=0;
shot2[0]=0;
shot3[0]=0;
them1[0]=0;
them2[0]=0;
them3[0]=0;
health=100;
srand(time(NULL));
  ttk_show_window(win);
  return 0;
}
