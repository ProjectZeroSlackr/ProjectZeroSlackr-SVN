/*
 * Copyright (c) 2005 Kevin Ferrare
 * Copyright (c) 2006 Felix Bruns (fxb) <felixbruns@web.de> (podzilla2/ttk port)
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

#include "pz.h"
#include <string.h>

#define NB_POLYGONS 7
#define NB_POINTS 4
#define MAX_STEP_RANGE 7
#define MIN_STEP_RANGE 3
#define MAX_POLYGONS 40
#define MIN_POLYGONS 1

struct point
{
    int x;
    int y;
};

struct polygon
{
    short vx[NB_POINTS];
    short vy[NB_POINTS];
};

struct polygon_move
{
    short movex[NB_POINTS];
    short movey[NB_POINTS];
};

struct polygon_fifo
{
    int fifo_tail;
    int fifo_head;
    int nb_items;
    struct polygon tab[MAX_POLYGONS];
};

static int r, g, b, rc, gc, bc, sleep_time, aa, paused, backlight;
static int nb_polygons = NB_POLYGONS;
static struct polygon_fifo polygons;
static struct polygon_move move; /* This describes the movement of the leading polygon, the others just follow */
static struct polygon leading_polygon;
static ttk_color back;

/*
 * Compute a new random step to make the point bounce the borders of the screen
 */
static int get_new_step(int step)
{
    if(step > 0){
        return -(MIN_STEP_RANGE + rand() % (MAX_STEP_RANGE - MIN_STEP_RANGE));
    }
    else{
        return (MIN_STEP_RANGE + rand() % (MAX_STEP_RANGE - MIN_STEP_RANGE));
    }
}

/*
 * Generates a random polygon (which fits the screen size though)
 */
static void polygon_init(struct polygon *polygon)
{
    int i;
    for(i = 0; i < NB_POINTS; ++i)
    {
        polygon->vx[i] = (rand() % (ttk_screen->w));
        polygon->vy[i] = (rand() % (ttk_screen->h));
    }
}

/*
 * Draw the given polygon onto the screen
 */
static void polygon_draw(struct polygon *polygon, ttk_surface srf)
{
    int i;
    ttk_color col = ttk_makecol(rc, gc, bc);
    if(aa){
	ttk_aapoly(srf, NB_POINTS, polygon->vx, polygon->vy, col);
    }
    else{
	ttk_poly(srf, NB_POINTS, polygon->vx, polygon->vy, col);
    }
}

/*
 * Polygon moving data stuff
 */
static void polygon_move_init(struct polygon_move *polygon_move)
{
    int i;
    for(i = 0; i < NB_POINTS; ++i)
    {
        polygon_move->movex[i] = get_new_step(-1); 
        /* -1 because we want a positive random step */
        polygon_move->movey[i] = get_new_step(-1);
    }
}

/*
 * Update the given polygon's position according to the given informations in
 * polygon_move (polygon_move may be updated)
 */
static void polygon_update(struct polygon *polygon, struct polygon_move *polygon_move)
{
    int i, x, y, step;
    for(i = 0; i < NB_POINTS; ++i){
        x    = polygon->vx[i];
        step = polygon_move->movex[i];
        x   += step;
        if(x <= 0){
            x = 1;
            polygon_move->movex[i] = get_new_step(step);
        }
        else if(x >= ttk_screen->w){
            x = ttk_screen->w - 1;
            polygon_move->movex[i] = get_new_step(step);
        }
        polygon->vx[i] = x;

        y    = polygon->vy[i];
        step = polygon_move->movey[i];
        y   += step;
        if(y <= 0){
            y = 1;
            polygon_move->movey[i] = get_new_step(step);
        }
        else if(y >= ttk_screen->h){
            y = ttk_screen->h - 1;
            polygon_move->movey[i] = get_new_step(step);
        }
        polygon->vy[i] = y;
    }
}

/*
 * Polygon fifo Stuffs
 */
static void fifo_init(struct polygon_fifo *fifo)
{
    fifo->fifo_tail = 0;
    fifo->fifo_head = 0;
    fifo->nb_items  = 0;
}

static void fifo_push(struct polygon_fifo *fifo, struct polygon *polygon)
{
    if(fifo->nb_items >= MAX_POLYGONS){
        return;
    }
    ++(fifo->nb_items);

    /*
     * Workaround for gcc (which uses memcpy internally) to avoid link error
     * fifo->tab[fifo->fifo_head] = polygon
     */
    memcpy(&(fifo->tab[fifo->fifo_head]), polygon, sizeof(struct polygon));
    ++(fifo->fifo_head);
    if(fifo->fifo_head >= MAX_POLYGONS){
        fifo->fifo_head = 0;
    }
}

static struct polygon *fifo_pop(struct polygon_fifo *fifo)
{
    int index;
    if(fifo->nb_items == 0){
        return(NULL);
    }
    --(fifo->nb_items);
    index=fifo->fifo_tail;
    ++(fifo->fifo_tail);
    if(fifo->fifo_tail >= MAX_POLYGONS){
        fifo->fifo_tail = 0;
    }
    return(&(fifo->tab[index]));
}

/*
 * Drawing stuff
 */
static void polygons_draw(struct polygon_fifo *polygons, ttk_surface srf)
{
    int i, j;
    for(i = 0, j = polygons->fifo_tail; i < polygons->nb_items; ++i, ++j){
        if(j >= MAX_POLYGONS){
            j = 0;
        }
        polygon_draw(&(polygons->tab[j]), srf);
    }
}

/*
 * Color stuff
 */
static void new_color()
{
    r = rand() % 255;
    g = rand() % 255;
    b = rand() % 255;
}

static void change_color()
{
    if(rc < r)
        ++rc;
    else if(rc > r)
        --rc;
    if(gc < g)
        ++gc;
    else if(gc > g)
        --gc;
    if(bc < b)
        ++bc;
    else if(bc > b)
        --bc;
    if(rc == r && gc == g && bc == b)
        new_color();
}

static void mystify_draw(TWidget *wid, ttk_surface srf)
{
    ttk_fillrect(srf, 0, 0, ttk_screen->w, ttk_screen->h, back);
    polygons_draw(&polygons, srf);
}

static int mystify_scroll(TWidget *wid, int dir)
{
#ifdef IPOD
    TTK_SCROLLMOD(dir, 4);
#endif
    if(dir > 0){
        sleep_time++;
    }
    else{
        if(sleep_time >= 0)
            sleep_time--;
    }
    ttk_widget_set_timer(wid, sleep_time);
    wid->dirty++;
    return TTK_EV_CLICK;
}

static int mystify_down(TWidget *wid, int button)
{
    switch(button){
        case TTK_BUTTON_MENU:
            ttk_hide_window(wid->win);
            break;
        case TTK_BUTTON_NEXT:
            if(nb_polygons < MAX_POLYGONS)
                ++nb_polygons;
            wid->dirty++;
            break;
        case TTK_BUTTON_PREVIOUS:
            if(nb_polygons > MIN_POLYGONS)
                --nb_polygons;
            wid->dirty++;
            break;
        case TTK_BUTTON_ACTION:
            aa = !aa;
            break;
        case TTK_BUTTON_HOLD:
            paused = 1;
            break;
        case TTK_BUTTON_PLAY:
        default:
            return TTK_EV_UNUSED;
            break;
    }
    return TTK_EV_CLICK;
}

static int mystify_button(TWidget *wid, int button, int ms)
{
    switch(button){
        case TTK_BUTTON_HOLD:
            paused = 0;
            break;
        default:
            return TTK_EV_UNUSED;
            break;
    }
    return TTK_EV_CLICK;
}

static int mystify_timer(TWidget *wid)
{
    if(paused){
        return 0;
    }
    
    if(polygons.nb_items > nb_polygons){
        /* We have too many polygons, we must drop some of them */
        fifo_pop(&polygons);
    }
    if(nb_polygons == polygons.nb_items){
        /* We have the good number of polygons, we can safely drop 
        the last one to add the new one later */
        fifo_pop(&polygons);
    }
    fifo_push(&polygons, &leading_polygon);
    
    /*
    * Then we update the leading polygon for the next round acording to
    * current move (the move may be altered in case of sreen border 
    * collision)
    */
    polygon_update(&leading_polygon, &move);
    change_color();
    
    wid->dirty++;
    
    return 0;
}



static void mystify_init(){
    

    back       = ttk_makecol(BLACK);
    new_color();
    rc         = r;
    gc         = g;
    bc         = b;
    aa         = 1;
    paused     = 0;
    sleep_time = 3;
    
    
    fifo_init(&polygons);
    polygon_move_init(&move);
    polygon_init(&leading_polygon);
}

TWindow * mystify_new_window()
{
    TWindow *win;
    TWidget  *wid;
    
    win            = ttk_new_window();
    win->title="Mystify";
    wid            = ttk_new_widget(0,0);
    wid->w         = ttk_screen->w;
    wid->h         = ttk_screen->h - ttk_screen->wy;
    wid->draw      = mystify_draw;
    wid->down      = mystify_down;
    wid->button    = mystify_button;
    wid->scroll    = mystify_scroll;
    wid->timer     = mystify_timer;
    
    wid->focusable = 1;
    
    mystify_init();
    
    ttk_widget_set_timer(wid, sleep_time);
    ttk_add_widget(win, wid);
    ttk_window_hide_header(win);
    
    ttk_show_window(win);
}



