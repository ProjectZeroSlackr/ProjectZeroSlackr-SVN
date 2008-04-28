/*
 * Invaders 0.2.1 for iPodLinux
 * Resistance is futile.  Also is Ohms.
 *
 * Copyright (C) 2005 Filippo Forlani,	version 0.1: original release
 * Copyright (C) 2006 Steve Brown,		version 0.2: SDL / Podzilla2 Port
 *
 * Special thanks to fxb for the starfield code (also appearing in vortex!)
 *
 * Changelog:
 * 0.2.1:
 *  - Explicit include of TTK mods to work on older builds of PZ2.
 *  - Force scroll wheel to be super-sensitive on iPod builds.  Might be overkill.
 *  - Minor bugfixes here and there
 *
 *
 * Todo:
 * - sound effects
 * - modular header widgets, hooray!
 * - retro arcade graphics
 * - many other things!
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "../pz.h"
#include "starfield.h"
#include "starfield.c"

#ifdef DEBUG
#define Dprintf printf
#else
#define Dprintf(...)
#endif

/* This macro is included in the latest TTK, put here for redundancy */
/* Steve Brown's scroll wheel acceleration macro, version 2-poit-oh
 * Amplifies scroll events with a timeout
 * arg1: (variable) event to be amplified
 * arg2: (constant int) slow accel by this factor
 * arg3: (constant int) maximum acceleration factor
 */
#define MIN(x,y)	(((x)<(y))?(x):(y))
	/* TODO: these timers need to be fine-tuned on a real iPod */
#define TTK_ACCEL_L	150 /* events below this delay are accel'ed */
#define TTK_ACCEL_H	250 /* events between the two are decel'ed */
#ifndef TTK_SCROLL_ACCEL
#define TTK_SCROLL_ACCEL(ttk_accel_event, ttk_accel_slow, ttk_accel_max)		\
do { 			\
	static int ttk_accel_throttle = 0;			\
	static int ttk_accel_timer = 0;			\
	static int ttk_accel_now = 0;			\
	ttk_accel_now = ttk_getticks() - ttk_accel_timer;			\
	if(ttk_accel_now > TTK_ACCEL_H || ttk_accel_throttle * ttk_accel_event < 0) {			\
		ttk_accel_throttle = 0; /* user changed direction or timed out; stop accel */			\
	} else if(ttk_accel_now > TTK_ACCEL_L && abs(ttk_accel_throttle) > abs(ttk_accel_event)) {/* slow the accel */			\
		ttk_accel_throttle -= ttk_accel_event;			\
	} else { /* increase accel coefficient up to max */			\
		ttk_accel_throttle += ttk_accel_event; /* count can be negative, meaning backwards direction*/			\
	}			\
	ttk_accel_event *= MIN(abs(ttk_accel_throttle / ttk_accel_slow) + 1, ttk_accel_max);			\
	ttk_accel_timer += ttk_accel_now;			\
} while (0)
#endif


static ttk_surface invaders2_srf;
static ttk_surface score_srf;
static ttk_surface	aliens_gfx;
static ttk_surface	player_gfx;
static ttk_surface	die_gfx[4];

/* GAME VARIABLES AND DEFINITIONS */ 
#define ALIENS_MAX_COLS	(int)16
#define ALIENS_MAX_ROWS	(int)8

#define SC_OFFX		(int)5
#define SC_OFFY		(int)20

#if 1
#define ALIEN_WIDTH	(int)16
#define ALIEN_HEIGHT	(int)11
#else
#define ALIEN_WIDTH	(int)7
#define ALIEN_HEIGHT	(int)8
#endif

#define ALIEN_CELL_WIDTH  (int)(ALIEN_WIDTH+4)
#define ALIEN_CELL_HEIGHT (int)(ALIEN_HEIGHT+4)

#if 0
#define ME_WIDTH	(int)7
#define ME_HEIGHT	(int)4
#else
#define ME_WIDTH	(int)16
#define ME_HEIGHT	(int)16
#endif

#define MYFIRE_HEIGHT	(int)6
#define MYFIRE_WIDTH	(int)2

#define ALIENFIRE_HEIGHT (int)6
#define ALIENFIRE_WIDTH  (int)2

#define GAME_STATUS_DEAD	0
#define GAME_STATUS_PLAY	1
#define GAME_STATUS_RESPAWN	2

int ME_MAX_FIRES;
int ALIEN_MAX_FIRES;

static int aliens_maxx,aliens_minx,aliens_maxy,aliens_miny;
static int me_firing;
static int aliens_firing;
static int aliens_posx, aliens_posy;

static int aliens_total;
static int aliens_left;
static int aliens_slow;
static int aliens_dir;
static int aliens_rows, aliens_cols;

static int board_left,board_right,board_top,board_bottom;

static int score;
static int high_score = 0;
static int game_status;
static int level;
static int me_lives;

static int gameover_waitcounter;

static unsigned short myfire_bmp[4][6] = { 
	{0x8000, 0x8000, 0x8000, 0x8000},
	{0x0100, 0x3800, 0x5400, 0xFE00},
	{0xA100, 0x1100, 0x2200, 0x3300},
	{0x0000, 0x0000, 0x0000, 0x0000}
};

static unsigned short alienfire_bmp[4][6] = { 
	{0x8000, 0x8000, 0x8000, 0x8000},
	{0x0100, 0x3800, 0x5400, 0xFE00},
	{0xA100, 0x1100, 0x2200, 0x3300},
	{0x0000, 0x0000, 0x0000, 0x0000}
};

typedef struct {
	int posx, posy;
	int dir;
} st_fire;

static int aliens_status[ALIENS_MAX_COLS * ALIENS_MAX_ROWS];		/* >4: alive;  1-4: exploding (animated);  0: dead */
static int me_status;		/* same as above */
static int me_posx,me_posy;

static int CWIDTH, CHEIGHT; /* ratios for rendering text */

st_fire myfire[5];
st_fire alienfire[5];

static void update_score();
static void draw_first();

static int get_rowcol(int x, int y, int* row, int* col)
{
	if (x<aliens_posx || y<aliens_posy ||
		x>aliens_posx + aliens_cols * ALIEN_CELL_WIDTH ||
		y>aliens_posy + aliens_rows * ALIEN_CELL_HEIGHT)
		return 0;
	*col = (x - aliens_posx) / ALIEN_CELL_WIDTH;
	*row = (y - aliens_posy) / ALIEN_CELL_HEIGHT;
	if (*col >= aliens_cols)
		(*col)--;
	if (*row >= aliens_rows)
		(*row)--;
	Dprintf("using row,col %d,%d,%d,%d\n",*row,*col,aliens_posx,aliens_posy);
	return 1;
}

static void invaders2_create_board(int level)
{
	aliens_rows = (invaders2_srf->h - 40) / 20;
	if (aliens_rows > ALIENS_MAX_ROWS) aliens_rows = ALIENS_MAX_ROWS;
	
	aliens_cols = (invaders2_srf->w - 40) / 20;
	if (aliens_cols > ALIENS_MAX_COLS) aliens_cols = ALIENS_MAX_COLS;
	Dprintf("creating aliens %dx%d for a screen %dx%d", aliens_rows, aliens_cols, invaders2_srf->h, invaders2_srf->w);
	
	
	aliens_total = aliens_cols * aliens_rows;
	aliens_left = aliens_total;
	
	int i;
	for(i=0; i < ALIENS_MAX_COLS * ALIENS_MAX_ROWS; i++)
		aliens_status[i] = 5;

	board_left = SC_OFFX;
	board_right = invaders2_srf->w;
	if (aliens_rows * ALIEN_CELL_HEIGHT * 3 < invaders2_srf->h)
		board_top = invaders2_srf->h/2;
	else
		board_top = SC_OFFY;
	board_bottom = invaders2_srf->h - ALIEN_CELL_HEIGHT;

	aliens_maxx = board_right - ALIEN_CELL_WIDTH * aliens_cols - level * 2;
	aliens_minx = board_left;
	aliens_maxy = board_bottom - ALIEN_CELL_HEIGHT * aliens_rows - ME_HEIGHT;
	aliens_miny = board_top;
	
	aliens_posx = board_left;
	aliens_posy = board_top;
	
	me_posx = invaders2_srf->w / 2;
	me_posy = invaders2_srf->h - ME_HEIGHT;
	
	Dprintf("level %d board %d,%d,%d,%d\n", level, board_top, board_left, board_bottom, board_right);
#if 1
	aliens_slow = aliens_left / 4 - level + 4;
#endif
	
	aliens_firing = 0;
	me_firing = 0;
	
	aliens_dir = 2;
	
	me_status = 5;
	
	Module_Starfield_session( level * 50 + 50, 1 );
	
	ME_MAX_FIRES = (level + 1)/2 + 2;
	if (ME_MAX_FIRES > 5)
		ME_MAX_FIRES = 5;
	
	ALIEN_MAX_FIRES = (level + 1) / 2 + 1;
	if (ALIEN_MAX_FIRES > 5)
		ALIEN_MAX_FIRES = 5;
	

}

static void starfield_draw() {
	ttk_fillrect(invaders2_srf, 0, 0, invaders2_srf->w, invaders2_srf->h, 
		ttk_makecol(BLACK));
	Module_Starfield_draw(invaders2_srf);
}

static void draw_alien(int col, int row, int ibmp)
{
	int ij = col+ aliens_cols*row;
	if(aliens_status[ij] == 5)
		ttk_blit_image(aliens_gfx, invaders2_srf, aliens_posx + col * ALIEN_CELL_WIDTH,
			aliens_posy + row * ALIEN_CELL_HEIGHT);
	else if(aliens_status[ij] > 0) { /* animate death */
		ttk_blit_image(die_gfx[--aliens_status[ij]], invaders2_srf, 
			aliens_posx + col * ALIEN_CELL_WIDTH, aliens_posy + row * ALIEN_CELL_HEIGHT);
	}
}

static void kill_me() {
	if (me_status < 5) return; /* don't kill me if i'm dead already! */
	
	me_status--; /* start die_gfx[] animation */
	me_lives--;
	if (me_lives < 1) {
		game_status = GAME_STATUS_DEAD;
		gameover_waitcounter = 100;
	} else { /* short penalty for dying */
		game_status = GAME_STATUS_RESPAWN;
		gameover_waitcounter = 30;
	}
	
}

static void aliens_draw(int ibmp)
{
	int i, j;
	for(j = 0; j < aliens_rows; j++){
		for(i = 0; i < aliens_cols; i++) {
			draw_alien(i, j, ibmp);
		}
	}
}

static void aliens_update_position()
{
	
	aliens_posx += aliens_dir;
	
	if(aliens_posx < aliens_minx || aliens_posx > aliens_maxx) {
		Dprintf("switch\n");
		aliens_dir -= 2* aliens_dir; /* reverse direction */
		aliens_posx += aliens_dir;
		aliens_posy += 2;
		if(aliens_posy + aliens_rows * ALIEN_CELL_HEIGHT 
				>= board_bottom + ME_HEIGHT) /* aliens crash into bottom */
			kill_me();
	}
}

static void me_draw()
{
	if (me_status == 5) {
		ttk_blit_image(player_gfx, invaders2_srf, me_posx - ME_WIDTH/2, me_posy);
	} else if (me_status > 0) {
		ttk_blit_image(die_gfx[--me_status], invaders2_srf, me_posx - ME_WIDTH/2, me_posy);
	}
}

static void fire_draw()
{
	int i;
	for(i = 0; i < me_firing; i++) {
		ttk_bitmap(invaders2_srf, myfire[i].posx,
				myfire[i].posy, MYFIRE_WIDTH, MYFIRE_HEIGHT,
				myfire_bmp[0], ttk_makecol(255, 128, 0));
		ttk_bitmap(invaders2_srf, myfire[i].posx+1,
				myfire[i].posy, MYFIRE_WIDTH, MYFIRE_HEIGHT,
				myfire_bmp[0], ttk_makecol(255, 128, 0));
	}
	
	for(i = 0; i < aliens_firing; i++) {
		Dprintf("alien posx,y %d,%d,%d\n", i, alienfire[i].posx,
			alienfire[i].posy);
		ttk_bitmap(invaders2_srf, alienfire[i].posx, alienfire[i].posy,
			ALIENFIRE_WIDTH, ALIENFIRE_HEIGHT, alienfire_bmp[0], ttk_makecol(0, 255, 0));
		ttk_bitmap(invaders2_srf, alienfire[i].posx+1, alienfire[i].posy,
			ALIENFIRE_WIDTH, ALIENFIRE_HEIGHT, alienfire_bmp[0], ttk_makecol(0, 255, 0));
	}
}

static void myfire_delete(int i)
{
	int j;
	for(j = me_firing - 1; j > i; j--)
	{
		myfire[j-1].posx = myfire[j].posx;
		myfire[j-1].posy = myfire[j].posy;
		myfire[j-1].dir = myfire[j].dir;
	}
	me_firing--;
}

static void alienfire_delete(int i)
{
	int j;
	for(j = aliens_firing - 1; j > i; j--) {
		alienfire[j-1].posx = alienfire[j].posx;
		alienfire[j-1].posy = alienfire[j].posy;
		alienfire[j-1].dir = alienfire[j].dir;
	}
	aliens_firing--;
}

static void fire_update()
{
	int i = 0;
	int rowhit, colhit;
	while(i < me_firing) {
		myfire[i].posy += myfire[i].dir;
		/* test if hit something */
		if(get_rowcol(myfire[i].posx, myfire[i].posy,
					&rowhit, &colhit)) {
			if(aliens_status[colhit + rowhit * aliens_cols] >= 5) {
				/* when hit, fire out, destroy it */
				myfire_delete(i);
				//draw_alien(colhit,rowhit,itest);
				Dprintf("hit %d,%d,%d\n", rowhit,colhit,aliens_left);
				Dprintf(" %d,%d\n",aliens_maxx-aliens_minx,
						cell_maxx - cell_minx);
				aliens_status[colhit + rowhit * aliens_cols]--; /* start die_gfx[] */
				score += colhit * 2;
				update_score();
				aliens_left--;
				Dprintf("%d left\n",aliens_left);
				if(aliens_left <= 0) {
					level++;
					invaders2_create_board(level);
					return;
				}
				continue;
			}
		}
		/* test if out board */
		if(myfire[i].posy < board_top) {
			myfire_delete(i);
			i--;
		}
		i++;
	}
	
	i = 0;
	while(i < aliens_firing) {
		alienfire[i].posy += alienfire[i].dir;
		Dprintf("alienfire pos %d\n", alienfire[i].posy);
		/* test if hit me */
		if(alienfire[i].posy < (me_posy + ME_HEIGHT) &&
			alienfire[i].posy >= me_posy &&
			alienfire[i].posx >= me_posx &&
			alienfire[i].posx <= me_posx + ME_WIDTH) {
			/* End of game */
			kill_me();
			return;
		}
		/* test if out board */
		if(alienfire[i].posy >= me_posy + ME_HEIGHT) {
			alienfire_delete(i);
			i--;
		}
		i++;
	}
}

static void draw_first()
{
	ttk_line(invaders2_srf, 0, invaders2_srf->h-2-4,
		invaders2_srf->w-1, invaders2_srf->h-2-4, ttk_makecol(WHITE));
	update_score();
	me_lives=3;
	me_status = 5;
	score = 0;
}

static int invaders2_Game_Loop(struct TWidget *this)
{
	if (game_status == GAME_STATUS_DEAD) {
		if (gameover_waitcounter > 0) {  /* timer handles player dead or respawning */ 
			gameover_waitcounter--;	
		} else {
			level = 0;
			invaders2_create_board(level);
			draw_first();
			game_status = GAME_STATUS_PLAY;
			me_status = 5;
		}
	} else if (game_status == GAME_STATUS_RESPAWN) {/* respawn */
		if (gameover_waitcounter > 0) {  /* wait for penalty */ 
			gameover_waitcounter--;
		} else { /* then put player back in play */
			game_status = GAME_STATUS_PLAY;
			me_status = 5;
		}
	} //else { /* GAME_STATUS_PLAY */
	aliens_update_position();
	aliens_slow = aliens_left / 4 - level + 4;
	
	/* evaluate if fire or not */
	if((rand() % 10) == 0 && aliens_firing < ALIEN_MAX_FIRES && aliens_left > 0) {
		/* find cols active and set relative row to start */
		int cols[ALIENS_MAX_COLS] = {-1};
		int i, j, hitcol;
		aliens_firing++;
		
		for(i = 0; i < aliens_cols; i++){
			cols[i] = -1;
			for(j = aliens_rows - 1; j >= 0; j--) {
				if(aliens_status[i + j * aliens_cols] > 0) {
					cols[i] = j;
					break;
				}
			}
			Dprintf("[%d:%d] ",i,cols[i]);
		}
		do { /* possible to hang here ? */
			hitcol = rand() % aliens_cols;
		} while (cols[hitcol] < 0); /* if no aliens in col, choose another */
		Dprintf("\n%d is %d\n", hitcol, cols[hitcol]);
		alienfire[aliens_firing - 1].posx = aliens_posx + hitcol *
			ALIEN_CELL_WIDTH + ALIEN_CELL_WIDTH / 2;
		alienfire[aliens_firing - 1].posy = aliens_posy + cols[hitcol] *
			ALIEN_CELL_HEIGHT + ALIEN_CELL_HEIGHT;
		alienfire[aliens_firing-1].dir = 1;
	}
	this->dirty++;
	
	return 0;
}

static void invaders2_do_draw(TWidget* wid, ttk_surface srf)
{
	invaders2_srf = srf;
	starfield_draw();
	aliens_draw(0);
	
	me_draw();
	
	fire_draw();

	fire_update();
	
	update_score();
	ttk_blit_image(score_srf, invaders2_srf, 0, 0); /* copy score buffer onto screen */
	wid->dirty++;
}

static void update_score()
{
	ttk_fillrect(score_srf, 0, 0, score_srf->w, score_srf->h, ttk_makecol(WHITE));
	
	char s[24];
	sprintf(s, "%.5d", score);
	pz_vector_string(score_srf, s, 2, 2, CWIDTH, CHEIGHT, 0, ttk_makecol(BLACK));
	if(score > high_score)
		high_score = score;
	sprintf(s, _("Hi:%.5d"), high_score);
	int right = pz_vector_width(s, CWIDTH, CHEIGHT, 0);
	pz_vector_string(score_srf, s, invaders2_srf->w - right - 2, 2, CWIDTH, CHEIGHT, 0, ttk_makecol(BLACK));
	
	/* draw my lives on top */
	int i;
	for (i = 0; i < me_lives; i++) {
		int x = (score_srf->w / 2) - (me_lives * (player_gfx->w + 2) / 2) + i * (player_gfx->w + 2);
		if(!(gameover_waitcounter % 2)) /* blinky */
			ttk_blit_image(player_gfx, score_srf, x, 0);
	}
	
	if (game_status == GAME_STATUS_DEAD) {
		char game_over[] = "GAME OVER";
		int xcenter = score_srf->w / 2;
		pz_vector_string_center(score_srf, game_over, xcenter, 2 + CHEIGHT/2, CWIDTH, CHEIGHT, 0, ttk_makecol(BLACK));
	}
	
}

	
static int invaders2_scroll(Twidget *this,int dir)
{
	if(game_status == GAME_STATUS_PLAY) { /* if in play */
		
		
			//TTK_SCROLLMOD(ev->arg, 5); /* might be too sensitive */
			TTK_SCROLL_ACCEL(dir, 2, 20);
			me_posx += ev->arg; //if this is negative, subtract
			if (me_posx < board_left) me_posx = board_left;
			if (me_posx > board_right) me_posx = board_right;
			this->dirty++;
			ret |= TTK_EV_CLICK;
			}
			return ret;
			
}
static void invaders2_down(TWidget *this,int button)
{
		if(game_status == GAME_STATUS_PLAY) { /* if in play */
			switch(button) {
			case TTK_BUTTON_ACTION: /* push button */
				if(me_firing < ME_MAX_FIRES) {
					me_firing++;
					myfire[me_firing - 1].posx = me_posx;
					myfire[me_firing - 1].posy = me_posy -4;
					myfire[me_firing - 1].dir = -2;
				}
				
				break;
			case TTK_BUTTON_MENU:
				/* if Menu Button destroy all dynamically
				 * created */
				ttk_hide_window(this->win);
				break;
			
			
			}
		}
		else {
		switch(button) {
	case TTK_BUTTON_ACTION: /* push button */
		if(gameover_waitcounter <= 0) {
			score = 0;
			level = 0;
			score = 0;
			invaders_create_board(level);
			game_status=GAME_STATUS_PLAY;
			draw_first();
		}
		
		break;
	case TTK_BUTTON_MENU:
		/* if Menu Button then destroy all
		 * dynamically created */
		ttk_hide_window(this->win);
		break;
	}

}
}
		
	


TWindow *new_invaders2_window()
{
	TWindow *ret;
	TWidget *wid;
	ret = ttk_new_window(_("Invaders2"));
	wid = ttk_new_widget(0,0);
	wid->focusable=1;
	wid->draw=invaders2_do_draw;
	wid->down=invaders2_down;
	wid->scroll=invaders2_scroll;
	ttk_add_widget(ret, wid);
	
	ttk_widget_set_timer(wid, 50); /* ms */
	wid->timer = invaders2_Game_Loop;

	invaders2_srf = ret->srf;
	
	Module_Starfield_init();
	
	/* load graphics */
	aliens_gfx = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/alien.png");
	player_gfx = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/player.png");
	die_gfx[0] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/die0.png");
	die_gfx[1] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/die1.png");
	die_gfx[2] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/die2.png");
	die_gfx[3] = ttk_load_image ("/opt/Zillae/ZacZilla/Data/invaders2/die3.png");

	
	if (!aliens_gfx || !player_gfx || !die_gfx[0] 
		|| !die_gfx[1] || !die_gfx[2] || !die_gfx[3]) {
		pz_error (_("Error loading Invaders graphics: "), strerror (errno));
		return;
	}
	
	CWIDTH = ret->w  * 3 / 100 ;
	CHEIGHT = ret->h * 7 / 100 ;
	/* set up scoreboard */
	score_srf = ttk_new_surface (ret->w, CHEIGHT + 4, 16);
	
	score = 0;
	level = 0;


	game_status = GAME_STATUS_PLAY;
	invaders2_create_board(level);

	draw_first();

	ttk_show_window(ret);
}


