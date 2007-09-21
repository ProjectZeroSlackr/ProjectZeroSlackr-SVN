#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "../pz.h"
#include "init.h"
#include "dopewars.h"

// for randomizing prices: {low, high}
int relative_price[DRUG_COUNT][2] = {
  {10000, 20000},
  {300, 800},
  {60, 150},
  {1000, 3000},
  {6000, 8000},
  {5000, 7000},
};

int can_steal(int drug, int ammount) {
  if (ammount > game.drugs[drug]) {
    ammount = game.drugs[drug];
  }
  
  return (ammount);
}

int can_hold(int n) {
  if (n > (game.max_coat - game.coat)) {
    n = game.max_coat - game.coat;
  }
  
  return (n);
}

TSheet random_event(int n) {
  TSheet event_sheet;
  char str[100];
  int r=0, s=0, t[DRUG_COUNT];
  
  strcpy(event_sheet.title, "");
 
  switch (n) {
    case 0: // cops (1-3)
      r=((rand() % 2)+1);
      sprintf(str, "The cops are after you!\nYou get shot %d times.\nYou escape.", r);
      game.life -= r;
      
      break;
    case 1: // found (1-10)
      r=can_hold( (rand() % 9) +1);
      s=(rand() % DRUG_COUNT);

      sprintf(str, "You find %d units of\n%s inside a brown\npaper bag.", r, buy.menuitems[s].caption);
      game.coat += r;
      game.drugs[s] += r;
//      game.old_price[s] += game.price[s];
      
      break;
    case 2: // stolen (1-10)
      // only gets a random value between the drugs we have
      // first we get how many drugs have non-zero values, put them onto an array and choose a random from that
      for (s=0; s < DRUG_COUNT; s++) if (game.drugs[s]) t[r++] = s;
      if (! r) return (random_event(rand() % EVENT_COUNT));
      s=(rand() % r);
      r=can_steal(t[s], ((rand() % 9) + 1));

      sprintf(str, "A junkie steals %d units\nof %s from you.", r, buy.menuitems[t[s]].caption);
      game.coat -= r;
      game.drugs[t[s]] -= r;
      
      break;
    case 3: // steal (1-500)
      r=can_hold( (rand() % 499) + 1);
      sprintf(str, "You steal a tourist's\nwallet and find $%d.", r);
      game.money += r;
      
      break;
    case 4: // supply down (15%)
      r = (rand() % DRUG_COUNT);
      sprintf(str, "The supply of %s\nis short.\nPrices are down.", buy.menuitems[r].caption);
      game.price[r] = (game.price[r] * .15);
      
      break;
    case 5: // supply up (50%)
      r = (rand() % DRUG_COUNT);
      sprintf(str, "The supply of %s\nis high.\nPrices are up.", buy.menuitems[r].caption);
      game.price[r] += (game.price[r] * .15);
      
      break;
  }

  strcpy(event_sheet.text, str);
  strcpy(event_sheet.button, "Ok");
  event_sheet.ammount = -1;

  return (event_sheet);
}

void restart() {
  // let's restart the game
  init_defaults();
  init_drugs();
  init_screens();
  init_sheets();
}

void do_pay() {
  game.money -= game.ammount;
  game.debt -= game.ammount;
}

void do_borrow() {
  game.money += game.ammount;
  game.debt += game.ammount;
}

void do_deposit() {
  game.money -= game.ammount;
  game.bank += game.ammount;
}

void do_withdraw() {
  game.money += game.ammount;
  game.bank -= game.ammount;
}

void do_buy(int drug, int ammount) {
  game.old_price[drug] += game.price[drug]*ammount;
  game.money -= game.price[drug]*ammount;
  game.coat += ammount;
  game.drugs[drug] += ammount;
  
  init_screens();
}

void do_sell(int drug, int ammount) {
  int avg;
  
  avg = (game.old_price[drug] / game.drugs[drug]);
  game.old_price[drug] -= avg*ammount;
  game.money += game.price[drug]*ammount;
  game.coat -= ammount;
  game.drugs[drug] -= ammount;
  
  init_screens();
}

void travel() {
  game.days++;                      // increment a day
  game.debt += game.debt * .05;     // 5 percent interest
  game.city = game.select;          // set new city
  game.screen = &status;            // go back to status screen
  if (game.life < 10) game.life++;  // increment your life if you're injured

  init_drugs();
  init_sheets();
}

int afford(int n) {
  int r;

  r = game.money / game.price[n];
  r = can_hold(r);
  
  return (r);
}

int sale(int n) {
  return (game.ammount * game.price[n]);
}
