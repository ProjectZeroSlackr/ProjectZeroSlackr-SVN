/*
 * Copyright (C) 2004 Nuno Sousa
 * Some menu code based on menu.c by Bernard Leach
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
#include <math.h>
#include <string.h>

#include "../pz.h"
#include "../ipod.h"

#include "init.h"
#include "functions.h"
#include "dopewars.h"

static GR_WINDOW_ID idw_wid;
static GR_GC_ID idw_gc;
static GR_SCREEN_INFO screen_info;



static void idw_draw_sheet(TSheet *sheet) {
  GR_SIZE width, height, base;
  char str[50], *ap;
  int i;
	
	if (sheet == NULL) {
    sheet = game.screen->menuitems[game.select].sheet;
  }
	
	// draw the title
  pz_draw_header (sheet->title);

  GrGetGCTextSize(idw_gc, sheet->button, -1, GR_TFASCII, &width, &height, &base);
  height+=4;  
  
  // (left, top, right, bottom)
  GrSetGCForeground(idw_gc, BLACK);
  GrLine(idw_wid, idw_gc, 20, 0, 20, screen_info.rows-44);
  GrLine(idw_wid, idw_gc, 20, screen_info.rows-44, screen_info.cols-20, screen_info.rows-44);
  GrLine(idw_wid, idw_gc, screen_info.cols-20, 0, screen_info.cols-20, screen_info.rows-44);
  
	GrSetGCForeground(idw_gc, WHITE);
  GrFillRect(idw_wid, idw_gc, 21, 0, screen_info.cols-41, screen_info.rows-44);
	GrSetGCForeground(idw_gc, BLACK);
  GrSetGCUseBackground(idw_gc, GR_FALSE);
  GrSetGCMode(idw_gc, GR_MODE_SET);
  
  ap = strtok(sheet->text, "\n");
  for (i=0; ap != NULL; i++) {
    GrText(idw_wid, idw_gc, 30, 1 + ((i + 1) * height - 4), ap, -1, GR_TFASCII);
    ap = strtok(NULL, "\n");
  }
  
	if (sheet->ammount != -1)
	{
    // print ammount
    sprintf(str, "Ammount: %d", game.ammount);
    GrText(idw_wid, idw_gc, 30, 50, str, -1, GR_TFASCII);
  } else {
    // the next time the user presses the middle button the sheet will disappear
    game.dismiss_sheet = 1;
  }

  // print the single menu option
  GrSetGCForeground(idw_gc, BLACK);
  GrFillRect(idw_wid, idw_gc, 21, 1 + 4 * height-4, screen_info.cols-41, height-1);
  GrSetGCForeground(idw_gc, WHITE);
  GrSetGCUseBackground(idw_gc, GR_TRUE);

  GrText(idw_wid, idw_gc, (screen_info.cols/2)-width+4, (5 * height - 8), sheet->button, -1, GR_TFASCII);

  // we're in a transaction
  game.in_transaction = 1;  
}

static void idw_draw_screen() {
  int i, dif=0, max=0;
  GR_SIZE width, height, base;
  	
	// This white line is printed because the sheet leaves two pixels
  GrSetGCForeground(idw_gc, WHITE);
  GrLine(idw_wid, idw_gc, 0, 0, screen_info.cols, 0);
  GrSetGCForeground(idw_gc, BLACK);
	
	// calculate the biggest space you can leave for the right-side column
	for (i=0; i < game.screen->count; i++)
	{
	  GrGetGCTextSize(idw_gc, game.screen->menuitems[i].value, -1, GR_TFASCII, &width, &height, &base);
    width+=5;
    
    if (width > max) max = width;
	}

  GrGetGCTextSize(idw_gc, "M", -1, GR_TFASCII, &width, &height, &base);
	height += 4;

  
  // print all the menus
  for (i=0; i < game.screen->count; i++)
  {
    if ((game.select == i) && (game.screen->menuitems[i].sheet)) {
			GrSetGCForeground(idw_gc, BLACK);
			GrFillRect(idw_wid, idw_gc, 0, 1 + i * height, screen_info.cols, height);
			GrSetGCForeground(idw_gc, WHITE);
			GrSetGCUseBackground(idw_gc, GR_TRUE);
		} else
		{
			GrSetGCUseBackground(idw_gc, GR_FALSE);
			GrSetGCForeground(idw_gc, WHITE);
			GrFillRect(idw_wid, idw_gc, 0, 1 + i * height, screen_info.cols, height);
			GrSetGCForeground(idw_gc, BLACK);
		}
		
		// highlite drug's name if you can make a positive sale
		if ((game.screen == &sell) && (game.drugs[i])) {
      dif = (game.price[i]*game.drugs[i] - game.old_price[i]);
    
      if (dif > 0) {
        GrText(idw_wid, idw_gc, 9, 1 + ((i + 1) * height - 4), game.screen->menuitems[i].caption, -1, GR_TFASCII);
      }
    }
    
    GrText(idw_wid, idw_gc, 8, 1 + ((i + 1) * height - 4), game.screen->menuitems[i].caption, -1, GR_TFASCII);
    
    // right-side column
    GrText(idw_wid, idw_gc, screen_info.cols - max, 1 + ((i + 1) * height - 4), game.screen->menuitems[i].value, -1, GR_TFASCII);
  }
}

static void idw_do_draw()
{
  char header[50];
	
	snprintf(header, sizeof(header), "DopeWars - %s", game.screen->title);
  pz_draw_header (header);
  
  // validate the selected menu
  if (!game.screen->menuitems[game.select].sheet) 
  {
    for (; (!game.screen->menuitems[game.select].sheet) && (game.select < game.screen->count); game.select++);
  }
  
  idw_draw_screen();
}

void check_over()
{
  char str[50];
  TSheet gameover_sheet;

  if (game.days == game.max_days)
  {
    // game over
    strcpy(gameover_sheet.title, "Game Over!");
    if (game.debt) {
      strcpy(str, "You didn't pay off your\ndebt!\nYou lose.");
    } else {
      sprintf(str, "You finished with $%d.", game.money + game.bank - game.debt);
    }
    strcpy(gameover_sheet.text, str);
    strcpy(gameover_sheet.button, "Again!");
    gameover_sheet.ammount = -1;

    idw_draw_sheet(&gameover_sheet);
  }

  if (! game.life)
  {
    // game over
    strcpy(gameover_sheet.title, "Game Over!");
    sprintf(str, "You died.\nYou finished with $%d.", game.money + game.bank - game.debt);
    strcpy(gameover_sheet.text, str);
    strcpy(gameover_sheet.button, "Again!");
    gameover_sheet.ammount = -1;

    idw_draw_sheet(&gameover_sheet);
  }
}

static int idw_do_keystroke (GR_EVENT *event)
{
  TSheet event_sheet;
  int n;

  if ( game.dismiss_sheet ) {
    if (event->keystroke.ch == '\r') {
      if (game.days == game.max_days) restart();
      
      game.dismiss_sheet = 0;
      game.in_transaction = 0;
      idw_do_draw();
    }
    
    return 1;
  }

  switch (event->keystroke.ch) {
    case '\r': /* Wheel button */
      if (! game.screen->menuitems[game.select].sheet) break;
      if ( (game.screen == &buy) && (!afford(game.select)) ) break;
      if ( (game.screen == &sell) && (!game.drugs[game.select]) ) break;
      if ( (game.screen->menuitems[game.select].sheet == &borrow_sheet) && (!game.debt) ) break;
      if ( (game.screen->menuitems[game.select].sheet == &withdraw_sheet) && (!game.bank) ) break;

      init_sheets();
      if (! game.in_transaction ) {
        // set initial ammount value to be shown on the sheet
        game.ammount = game.screen->menuitems[game.select].sheet->ammount;
        init_sheets();
      }
      init_screens();
     
      if ( game.screen == &jet ) {
        if (game.city == game.select) break;
        
        travel();

        // let's get a random event (high-low)+low
        n = (rand() % EVENT_COUNT);

        // it has a 50-50 chance of occurring
        if (! (rand() % 2)) {
          event_sheet = random_event(n);
          idw_draw_sheet(&event_sheet);
          init_screens();
        } else {
          init_screens();
          idw_do_draw();
        }
        
        check_over();
        break;
      }
            
      if ( game.in_transaction ) {
        // we're going to buy/sell
        if (game.screen == &buy) do_buy(game.select, game.ammount);
        if (game.screen == &sell) do_sell(game.select, game.ammount);
        if (game.screen->menuitems[game.select].sheet == &deposit_sheet) do_deposit();
        if (game.screen->menuitems[game.select].sheet == &withdraw_sheet) do_withdraw();
        if (game.screen->menuitems[game.select].sheet == &pay_sheet) do_pay();
        if (game.screen->menuitems[game.select].sheet == &borrow_sheet) do_borrow();
        
        // transaction finished
        game.in_transaction = 0;
        init_screens();
        idw_do_draw();
        break;
      }
      
      idw_draw_sheet(NULL);
      break;
      
    case 'd': /* Play/pause button */
      break;
    case 'w': /* Rewind button */
      if (game.in_transaction) break;
      if (!game.screen->prev) break;
      
      game.screen = game.screen->prev;
      game.select=0;
      idw_do_draw();
      break;
      
    case 'f': /* Fast forward button */
      if (game.in_transaction) break;
      if (!game.screen->next) break;
      
      game.screen = game.screen->next;
      game.select=0;
      idw_do_draw();
      break;
    case 'l': /* Wheel left */
      if (game.in_transaction) {
        if (game.ammount) game.ammount--;
        init_sheets();
        idw_draw_sheet(NULL);
        break;
      }
      if (game.select <= 0) break;
      
      game.select--;
      idw_do_draw();
      break;
      
    case 'r': /* Wheel right */
      if (game.in_transaction) {
        if (game.ammount < game.screen->menuitems[game.select].sheet->ammount) game.ammount++;
        init_sheets();
        idw_draw_sheet(NULL);
        break;
      }
      if (game.select >= game.screen->count-1) break;
      
      game.select++;
      idw_do_draw();
      break;
      
    case 'm': /* Menu button */
      //if (game.in_transaction) {
        // cancel transaction
     //   game.in_transaction = 0;
    //    game.ammount = 0;
    //    idw_do_draw();
    //    break;
   //   }
  //    pz_close_window (idw_wid);
    //  GrDestroyGC(idw_gc);
   //   break;
   // default:
   			GrDestroyGC( idw_gc );
			pz_close_window( idw_wid );
      break;
  }
  
  return 1;
}

void new_idw_window()
{
  idw_gc = GrNewGC();
	GrGetScreenInfo(&screen_info);

	init_defaults();
  init_drugs();
	init_screens();
	init_sheets();

	idw_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), idw_do_draw, idw_do_keystroke);

	GrSelectEvents(idw_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);
	GrMapWindow(idw_wid);
}
