#include <stdio.h>
#include "pz.h"
int ouch=0,oucht=25;
 TWidget *wid;
  TWindow *win;
static int ouch_draw(/*TWidget *wid,*/ ttk_surface srf)
{
  if(ouch==0) {
  ttk_fillrect(srf,0,0,wid->w,wid->h,ttk_makecol(BLACK));
  }
  else
  {
  ttk_fillrect(srf,0,0,wid->w,wid->h,ttk_makecol(WHITE));
}
return 0;
}
static int ouch_timer(TWidget *this){
  if(ouch=0) ouch=1;
  else ouch=0;
return 0;
}
static int ouch_down(TWidget *this,int button)
{
  switch(button) {
  case TTK_BUTTON_MENU:
  pz_close_window(this->win);
  break;
  case TTK_BUTTON_PREVIOUS:
  oucht++;
  break;
  case TTK_BUTTON_NEXT:
  oucht--;
  break;
  }
}
TWindow *new_ouch_window()
{
 
  win=ttk_new_window("Ouch!");
  ttk_window_hide_header(win);
  wid=ttk_new_widget(0,0);
  wid->w=win->w;
  wid->h=win->h;
  wid->focusable=1;
  wid->draw=ouch_draw;
  wid->timer=ouch_timer;
  wid->down=ouch_down;
  ttk_widget_set_timer(wid,oucht);
  ttk_add_widget(win,wid);
  ttk_show_window(win);
  return 0;
}

