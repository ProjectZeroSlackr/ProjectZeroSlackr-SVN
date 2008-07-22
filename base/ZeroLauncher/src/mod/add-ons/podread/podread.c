/*
 * Last updated: July 21, 2008
 * ~Keripo
 *
 * Modified for ZeroLauncher
 */

/*
 *Copyright (C) 2006 Frederik M.J.V. (freqmod)
 *
 *Based on PodWrite: Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
 *Text wrapping algoritm (the make_wrappings function)
 *based on the TTK textarea widget Copyright (c) 2005 Joshua Oreman
 *
 *This program is free software; you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software Foundation,
 *Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "pz.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* podread State */
static int podread_mode = 0;
static int podread_linecount = 0;
static int podread_screenlines = 0;
static int podread_scroll = 0;
static int podread_autoscroller = 0;
static int podread_curautoscroller = 0;
static char *podread_filename = 0;
static char podread_printstr[100];

static PzWindow *podread_win;
static TWidget  *podread_wid;
static TWidget *podread_menu;
static ttk_menu_item podread_fbx;
static char *podread_buffer;
static int *podread_wrappings;

static PzModule *module;
static const char *cfg_path;

static char *tv_strrep(char *str,char repf,char rept){
	int idx=0;
	while(1){
		if(str[idx]=='\0') break;
		if(str[idx]==repf)str[idx]=rept;
		idx++;
	}
	return str;
}

static int tv_hlp_getline(FILE *fp,char line[], int max){
	int nch = 0;
	int c;
	max = max - 1;                  /* leave room for '\0' */
	
	while((c = getc(fp)) != EOF)
		{
		if(c == '\n')
			break;
	
		if(nch < max)
			{
			line[nch] = c;
			nch = nch + 1;
			}
		}
	
	if(c == EOF && nch == 0)
		return EOF;
	
	line[nch] = '\0';
	return nch;
}

static PzWindow *podread_mh_loadline(ttk_menu_item *item){
	if(podread_filename==NULL){
		pz_error(_("Can not load/save linenumber when no file is opened"));
		return TTK_MENU_DONOTHING;
	}
	char treadstr[256];
	char *f = strdup(podread_filename);
	int rddl;
	FILE *fp;
	sprintf(treadstr,"%s%s", cfg_path, tv_strrep(f, '/', '.'));
	if((fp=fopen(treadstr, "r"))==NULL) {
		pz_error("Could not open file %s for reading linenum, most likly %s is missing or unwriteable", treadstr, cfg_path);
		return TTK_MENU_DONOTHING;
	}
	tv_hlp_getline(fp,treadstr, 256);
	rddl=atoi(treadstr);

	podread_scroll=rddl;
	if (podread_scroll > (podread_linecount - podread_screenlines)) podread_scroll = (podread_linecount - podread_screenlines);
	if (podread_scroll < 0) podread_scroll = 0;
	fclose(fp);
	sync();
	free(f);
	//podread_wid->dirty = 1;
	return TTK_MENU_UPONE;
}
	
static PzWindow *podread_mh_saveline(ttk_menu_item *item){
	if(podread_filename==NULL){
		pz_error(_("Can not load/save linenumber when no file is opened"));
		return TTK_MENU_DONOTHING;
	}
	char treadstr[256];
	char *f = strdup(podread_filename);
	FILE *fp;
	sprintf(treadstr,"%s%s", cfg_path, tv_strrep(f, '/', '.'));
	if((fp=fopen(treadstr, "w"))==NULL) {
		pz_error("Could not open file %s for saving linenum", treadstr);
		return TTK_MENU_DONOTHING;
	}
	fprintf(fp,"%u\n",podread_scroll);
	fclose(fp);
	free(f);
	pz_message(_("Line saved"));	
	return TTK_MENU_DONOTHING;
}



static TWindow *podread_mh_return(ttk_menu_item *item)
{
/*	podread_mode = 0;*/
	pz_close_window(podread_menu->win);
// 	ti_widget_start(podread_wid);
	return TTK_MENU_DONOTHING;
}

static TWindow *podread_mh_win_scroll(ttk_menu_item *item)
{
	podread_mode = 1;
	podread_wid->dirty = 1;
	return TTK_MENU_UPONE;
}

static TWindow *podread_mh_scroll(ttk_menu_item *item)
{
	podread_mode = 0;
	podread_wid->dirty = 1;
	return TTK_MENU_UPONE;
}

static TWindow *podread_mh_autoscroll(ttk_menu_item *item)
{
	podread_mode = 2;
	podread_wid->dirty = 1;
	return TTK_MENU_UPONE;
}

static TWindow *podread_mh_music(ttk_menu_item *item)
{
	podread_mode = 3;
	podread_wid->dirty = 1;
	return TTK_MENU_UPONE;
}

static void podread_free(){
	if (podread_filename) {
		free(podread_filename);
		podread_filename = 0;
	}
	if (podread_buffer) {
		free(podread_buffer);
		podread_buffer = 0;
	}
	if (podread_wrappings) {
		free(podread_wrappings);
		podread_wrappings = 0;
	}
	pz_close_window(podread_menu->win);
	podread_autoscroller=0;
}

static TWindow *podread_mh_quit(ttk_menu_item *item)
{
	podread_free();
	pz_close_window(podread_win);
	return TTK_MENU_DONOTHING;
}

static ttk_menu_item *podread_new_menu_item(const char *name, TWindow *(*func)(ttk_menu_item *), int flags)
{
	ttk_menu_item *mi = (ttk_menu_item *)calloc(1, sizeof(ttk_menu_item));
	if (mi) {
		mi->name = name;
		mi->makesub = func;
		mi->flags = flags;
	}
	return mi;
}

static PzWindow *new_podread_menu_window()
{
	PzWindow *w;
	podread_menu = ttk_new_menu_widget(0, ttk_menufont, ttk_screen->w, ttk_screen->h-ttk_screen->wy);
	if (!podread_menu) return 0;
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Return to Podread"), podread_mh_return, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Scroll"), podread_mh_scroll, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Scroll window"), podread_mh_win_scroll, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Autoscroll"), podread_mh_autoscroll, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Music control"), podread_mh_music, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Load linenumber"), podread_mh_loadline, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Save linenumber"), podread_mh_saveline, 0));
	ttk_menu_append(podread_menu, podread_new_menu_item(N_("Quit Podread"), podread_mh_quit, 0));
	w = pz_new_menu_window(podread_menu);
	if (w) w->title = _("PodRead");
	return w;
}

/* podread Event Handling */

static void podread_draw (PzWidget *wid, ttk_surface srf) 
{
	ttk_fillrect(srf, wid->x, wid->y, wid->x+wid->w, wid->y+wid->h, ttk_ap_getx("window.bg")->color);
	int i=0;
	int tfh=ttk_text_height(ttk_textfont);
	char tmpchar;

	int h = wid->h-15;
	if (podread_linecount > podread_screenlines) {
	ttk_ap_fillrect (srf, ttk_ap_get ("scroll.bg"), wid->x + wid->w - 10,
			 wid->y + ttk_ap_getx ("header.line") -> spacing,
			 wid->x + wid->w, wid->y + h);
	ttk_ap_rect (srf, ttk_ap_get ("scroll.box"), wid->x + wid->w - 10,
		     wid->y + ttk_ap_getx ("header.line") -> spacing,
		     wid->x + wid->w, wid->y + h);
	ttk_ap_fillrect (srf, ttk_ap_get ("scroll.bar"), wid->x + wid->w - 10,
			 wid->y + ttk_ap_getx ("header.line") -> spacing + ((podread_scroll) *(h-2) / podread_linecount),
			 wid->x + wid->w,
			 wid->y - ttk_ap_getx ("header.line") -> spacing + ((podread_scroll + podread_screenlines) *(h-2) / podread_linecount) );	
    	}
	if(podread_wrappings!=0&&podread_buffer!=0){
		for(i=0;i<podread_screenlines;i++){
			if((i+podread_scroll)>(podread_linecount-1)||(i+podread_scroll)<0)break;
			tmpchar=podread_buffer[podread_wrappings[i+podread_scroll]];
			podread_buffer[podread_wrappings[i+podread_scroll]]=0;
			
			if((i+podread_scroll-1)<0){
				ttk_text_lat1(srf, ttk_textfont, wid->x+1, wid->y+(tfh*i), ttk_ap_getx("window.fg")->color,podread_buffer);
			}else {
				ttk_text_lat1(srf, ttk_textfont, wid->x+1, wid->y+(tfh*i), ttk_ap_getx("window.fg")->color,&podread_buffer[podread_wrappings[i+podread_scroll-1]]);
			}
			podread_buffer[podread_wrappings[i+podread_scroll]]=tmpchar;
		}
	}
	if(podread_mode==0)sprintf(podread_printstr,"Mode: Scroll Line:%i",podread_scroll);
	else if(podread_mode==1)sprintf(podread_printstr,"Mode: Scroll screen:%i (%i)",podread_scroll/podread_screenlines,podread_scroll);
	else if(podread_mode==2)sprintf(podread_printstr,"Mode: Autoscr: %i",podread_autoscroller);
	else if(podread_mode==3)sprintf(podread_printstr,"Music control");
	ttk_text(srf, ttk_textfont, wid->x+5, (wid->y+wid->h)-tfh, ttk_ap_getx("window.fg")->color,podread_printstr);
}

static int podread_widget_scroll(TWidget *wid, int dir)
{
	TTK_SCROLLMOD (dir,4);
	switch (podread_mode) {
	case 0:
		podread_scroll += dir;
		if (podread_scroll < 0) podread_scroll = 0;
		if (podread_scroll > (podread_linecount - podread_screenlines)) podread_scroll = (podread_linecount - podread_screenlines);
		wid->dirty = 1;
		break;
	case 1:
		podread_scroll += dir*podread_screenlines;
		if (podread_scroll < 0) podread_scroll = 0;
		if (podread_scroll > (podread_linecount - podread_screenlines)) podread_scroll = (podread_linecount - podread_screenlines);
		wid->dirty = 1;
		break;
	case 2:
		if (dir<0) {
			podread_autoscroller-=1;
			if(podread_autoscroller<-40)
				podread_autoscroller=-40;
			wid->dirty = 1;
		} else if (dir>0) {
			podread_autoscroller+=1;
			if(podread_autoscroller>40)
				podread_autoscroller=40;
			wid->dirty = 1;
		} else {
			return TTK_EV_UNUSED;
		}
		wid->dirty = 1;
		break;	

	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

static int podread_widget_button(TWidget *wid, int btn, int t)
{
	if(btn == PZ_BUTTON_MENU){
		PzWindow *w = new_podread_menu_window();
		if (w) pz_show_window(w);
		wid->dirty = 1;
		return TTK_EV_CLICK;
	}
	if (btn == PZ_BUTTON_ACTION) {
		podread_mode+=1;
		if(podread_mode>3)podread_mode=0;
		wid->dirty = 1;
	}
	switch (podread_mode) {
	case 0:
	case 1:
		if (btn == PZ_BUTTON_PREVIOUS) {
			podread_scroll = 0;
			wid->dirty = 1;
		} else if (btn == PZ_BUTTON_NEXT) {
			podread_scroll = (podread_linecount - podread_screenlines);
			wid->dirty = 1;
		} else {
			return TTK_EV_UNUSED;
		}
		break;
	case 2:
		if (btn == PZ_BUTTON_NEXT) {
			podread_autoscroller = 40;
			wid->dirty = 1;
		}else if (btn == PZ_BUTTON_PREVIOUS) {
			podread_autoscroller = -40;
			wid->dirty = 1;

		}else if (btn == PZ_BUTTON_PLAY) {
			podread_autoscroller = 0;
			wid->dirty = 1;
		}else {
			return TTK_EV_UNUSED;
		}
	default:
		return TTK_EV_UNUSED;
		break;
	}
	return TTK_EV_CLICK;
}

static int podread_autoscroll(struct TWidget *wid){
	podread_curautoscroller++;
 	int dir=0;
 	if(podread_autoscroller>0){
		if(podread_curautoscroller>=40){
			podread_curautoscroller=0;
			dir=1;
		}else{
			podread_curautoscroller+=podread_autoscroller;
		}
	} else if(podread_autoscroller<0){
		if(podread_curautoscroller<=-40){
			podread_curautoscroller=0;
			dir=-1;
		}else{
			podread_curautoscroller+=podread_autoscroller;
		}
 	}
	if(dir!=0){
		TTK_SCROLLMOD (dir,4);
		podread_scroll += dir;
		if (podread_scroll > (podread_linecount - podread_screenlines)) podread_scroll = (podread_linecount - podread_screenlines);
		if (podread_scroll < 0) podread_scroll = 0;
		wid->dirty = 1;
	}
	return 0;
	
}

static int *make_wrappings(char *buf,int bufsize,int width,int height,ttk_font fnt,struct TWidget *widg){//remember to free the result
    char podread_printstr[100];
    int wid=width-17;
	podread_mode=-1;
	podread_linecount=0;
    // *wrapat[] stores pointers to the char we wrap *after* on each line.
    // (If it's a space, tab, or nl, it's eaten.)
    int *wrapat;
    wrapat=malloc((bufsize/20)*sizeof(int *));
    // Number of lines. Used in computing the size of our surface.
    int lines;
    int lastreallock=(bufsize/20);
    // Current EOL (pointer in wrapat[])
    int *end;
    // Current xpos
    int xpos;
    //current char pos
    int cpos=0;
 wrapit:
    lines = 0;
    end = wrapat;
    xpos = 0;
    cpos=0;

    // The text width functions should be very inexpensive, so we can just
    // do them per-char and be simple.

    while (cpos<bufsize){
	char s[2] = { '?', 0 };
	s[0] = buf[cpos];

	if (lines>=lastreallock&&lines>(bufsize/20)) {
		//printf("Realloc for lines%i\n",lines);
		if((wrapat = realloc(wrapat, sizeof(int *) *(lines+50000)))==NULL) {
				pz_error(_("Could not allocate space for more lines."));
				return 0;
			}
		lastreallock=lines+50000;
	}
	
        switch (buf[cpos]) {
        case '\n':
	    *end++ = cpos+1;
            lines++;
            xpos = 0;
	    cpos++;
            continue;
        case '\t':
            xpos = (xpos + 15) & 15;
            break;
        default:
	    xpos += ttk_text_width_lat1 (fnt, &s[0]);
            break;
        }

        if (xpos > wid) {
	    int qcpos=cpos;
            // Backtrack to find a char to break on
            while (((qcpos>0) && (!lines || qcpos >= end[-1])) && ((buf[qcpos] != ' ') && (buf[qcpos] != '\t') && (buf[qcpos] != '-'))){
		qcpos--;
	    }
            if (qcpos<=0 || (lines && qcpos <= end[-1])) {
                // Couldn't find one in the past line, this is a really big word. Break it up.

		*end++ = cpos - 1;
                cpos--; // reconsider the chopped character for the next line
            } else {
                *end++ = qcpos+1;
		cpos=qcpos+1;
            }
            lines++;
            xpos = 0;
        }
	if (((lines *20) > height) && (wid == (width - 2))) {
	    wid -= 11; // scrollbar
	    goto wrapit;
	}
	if(widg!=0){
		if((bufsize/100)==0) podread_scroll=0;
		else podread_scroll=cpos/(bufsize/100);
		if(podread_linecount!=podread_scroll){
			sprintf(podread_printstr,"Wrapping file %i%%",podread_scroll);//,cpos,bufsize);
			ttk_fillrect(ttk_screen->srf, widg->x+(widg->w/2)-(ttk_text_width(ttk_textfont,podread_printstr)/2), widg->y+(widg->h/2)-(ttk_text_height(ttk_textfont)/2), widg->x+(widg->w/2)+(ttk_text_width(ttk_textfont,podread_printstr)/2), widg->y+(widg->h/2)+(ttk_text_height(ttk_textfont)/2), ttk_ap_getx("window.bg")->color);
			ttk_text(ttk_screen->srf, 
				ttk_textfont, 
				widg->x+(widg->w/2)-(ttk_text_width(ttk_textfont,podread_printstr)/2), 
				widg->y+(widg->h/2)-(ttk_text_height(ttk_textfont)/2), 
				ttk_ap_getx("window.fg")->color,podread_printstr);
			ttk_gfx_update (ttk_screen->srf); 
		}
		podread_linecount=podread_scroll;
	}
	cpos++;
    }
    podread_linecount=lines;
    if((wrapat = realloc(wrapat, sizeof(int *) *(lines)))==NULL) {
	pz_error(_("Could not free unneeded line space."));
	return 0;
    }
    return wrapat;
}

static int podread_event (PzEvent *ev) 
{
    switch (ev->type) {
	case PZ_EVENT_BUTTON_UP:  /* button released, don't care which one */
		/* Close the window without caching. (Next time the user selects this menu item,
		* new_mymodule_window() will be called again.) If you want the exact same window
		* to pop up next time, without any new init code, you should use pz_hide_window().
		*
		* ev->wid is the widget the event was sent to, and ev->wid->win is the window that
		* widget is in.
		*/
		return podread_widget_button(ev->wid,ev->arg,ev->time);
		//pz_close_window (ev->wid->win);
        break;
	case PZ_EVENT_DESTROY:
        /* Our window is about to be freed - free anything we allocated ourselves. */
		podread_free();
        break;
 
	case PZ_EVENT_SCROLL:
		return podread_widget_scroll(ev->wid,ev->arg);
		//work with ev->arg = the distance (+ or -) that the user scrolled
	break;
	case PZ_EVENT_TIMER:
		return podread_autoscroll(ev->wid);
	break;
    }
    /* Normal event return - handled, no clicky noise */
    return 0;
}

PzWindow *new_podread_window_with_file(const char *fn)
{
	PzWindow *ret;
	TWidget *wid;
	
	/* set podread's state */
	podread_mode = 0;
	podread_linecount = 0;
	podread_screenlines = 0;
	podread_scroll = 0;
	podread_filename = 0;
	podread_autoscroller = 0;
	podread_curautoscroller = 0;
	podread_printstr[0]=0;
	/* create the window, widget, and buffer */
	podread_win = ret = pz_new_window(_("PodRead"), PZ_WINDOW_NORMAL);
	podread_wid=wid= pz_add_widget (ret, podread_draw, podread_event);
	pz_widget_set_timer (wid, 50);
	ret = pz_finish_window(ret);
	
	podread_screenlines=((int)(wid->h/ttk_text_height(ttk_textfont))-1);
	
	podread_filename = strdup(fn);
	FILE *f;
	char *buf;
	int fl;
	
	f = fopen(fn, "rb");
	if (!f) return 0;
	fseek(f, 0, SEEK_END);
	fl = ftell(f);
	if (fl<0) {
		fclose(f);
		
		return 0;
	}
	fseek(f, 0, SEEK_SET);
	buf = (char *)malloc(fl+1);
	if (!buf) {
		fclose(f);
		pz_error(_("Text buffer memory allocation error"));
		return 0;
	}
	fl = fread(buf, 1, fl, f);
	buf[fl]=0;
	fclose(f);
	podread_mode=-2;
	podread_buffer=buf;
	podread_mode=-1;
	podread_wrappings=make_wrappings(buf,fl,wid->w,wid->h,ttk_textfont,wid);//remember to free the result	
	if(podread_wrappings==0){
		podread_free();
		pz_close_window(podread_win);
//		pz_error(_("Text wrapping memory allocation error"));
		return 0 ;
	}	
	podread_mode=0;
	podread_scroll=0;
	return ret;
}

// Blatant rip from PZ0's textview.c
static int check_is_ascii_file(const char *filename)
{
	FILE *fp;
	unsigned char buf[20], *ptr;
	long file_len;
	struct stat ftype; 

	stat(filename, &ftype); 
	if(S_ISBLK(ftype.st_mode)||S_ISCHR(ftype.st_mode))
		return 0;
	if((fp=fopen(filename, "r"))==NULL) {
		perror(filename);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	rewind(fp);
	fread(buf, file_len<20?file_len:20, 1, fp);
	fclose(fp);
	
	for(ptr=buf;ptr-buf<(file_len<20?file_len:20);ptr++)
		if(*ptr<7||*ptr>127)
			return 0;
	return 1;
}

#include "../../launch/browser-ext.h"
static int check_is_text_file(const char* file)
{
	return (
		check_file_ext(file, ".txt") ||
		check_file_ext(file, ".log") ||
		check_file_ext(file, ".nfo") ||
		check_file_ext(file, ".ini") ||
		check_file_ext(file, ".cfg") ||
		check_file_ext(file, ".conf")
		);
}

static PzWindow *podread_open_handler(ttk_menu_item *item)
{
	return new_podread_window_with_file(item->data);
}

static void cleanup()
{
	pz_browser_remove_handler(check_is_text_file);
}

static void podread_mod_init()
{
	module = pz_register_module("podread", cleanup);
	cfg_path = pz_module_get_cfgpath(module, "");
	
	podread_fbx.name = N_("Open with PodRead");
	podread_fbx.makesub = podread_open_handler;
	pz_browser_add_action (check_is_ascii_file, &podread_fbx);
	pz_browser_set_handler(check_is_text_file, new_podread_window_with_file);
}

PZ_MOD_INIT(podread_mod_init)
