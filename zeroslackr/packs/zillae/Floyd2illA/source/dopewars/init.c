#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "functions.h"
#include "dopewars.h"

void init_drugs() {
  int i;

  for (i=0; i < DRUG_COUNT; i++)
  {
    game.price[i] = (rand() % (relative_price[i][1]-relative_price[i][0])) + relative_price[i][0];
  }
}

void init_defaults() {
  int i;
  
  srand ( time(NULL) );
  for (i=0; i < DRUG_COUNT; i++) // set all drugs to 0
    game.drugs[i] = 0;
  for (i=0; i < DRUG_COUNT; i++) // set old price to 0
    game.old_price[i] = 0;

  game.life = 10;         // 100 hp
  game.days = 1;          // start day
  game.max_days = 30;     // end day
  game.debt = 2000;       // initial loan
  game.coat = 0;          // don't have anything yet
  game.max_coat = 100;    // max you can carry
  game.money = 2000;      // loan money you start the game with
  game.bank = 0;          // initial money in bank account
  game.city = 0;          // initial city
  game.screen = &status;  // initial screen
  game.select=0;          // selected menu item index
  game.ammount=0;         // ammount for sheets
  game.in_transaction=0;  // NEVER CHANGE - important for drawing sheets
  game.dismiss_sheet=0;   // NEVER CHANGE - important for drawing sheets
}

void init_screens()
{
  char str[50];

  strcpy(buy.title, "Buy");
  strcpy(buy.menuitems[0].caption, "Cocaine");
  sprintf(str, "$%d", game.price[0]);
  strcpy(buy.menuitems[0].value, str);
  buy.menuitems[0].sheet = &buy_sheet;
  strcpy(buy.menuitems[1].caption, "Weed");
  sprintf(str, "$%d", game.price[1]);
  strcpy(buy.menuitems[1].value, str);
  buy.menuitems[1].sheet = &buy_sheet;
  strcpy(buy.menuitems[2].caption, "Ecstasy");
  sprintf(str, "$%d", game.price[2]);
  strcpy(buy.menuitems[2].value, str);
  buy.menuitems[2].sheet = &buy_sheet;
  strcpy(buy.menuitems[3].caption, "Acid");
  sprintf(str, "$%d", game.price[3]);
  strcpy(buy.menuitems[3].value, str);
  buy.menuitems[3].sheet = &buy_sheet;
  strcpy(buy.menuitems[4].caption, "Crack");
  sprintf(str, "$%d", game.price[4]);
  strcpy(buy.menuitems[4].value, str);
  buy.menuitems[4].sheet = &buy_sheet;
  strcpy(buy.menuitems[5].caption, "Heroin");
  sprintf(str, "$%d", game.price[5]);
  strcpy(buy.menuitems[5].value, str);
  buy.menuitems[5].sheet = &buy_sheet;
  buy.count = DRUG_COUNT;
  buy.prev = &status;
  buy.next = &sell;

  strcpy(sell.title, "Sell");
  strcpy(sell.menuitems[0].caption, buy.menuitems[0].caption);
  sprintf(str, "( %d )", game.drugs[0]);
  strcpy(sell.menuitems[0].value, str);
  sell.menuitems[0].sheet = &sell_sheet;
  strcpy(sell.menuitems[1].caption, buy.menuitems[1].caption);
  sprintf(str, "( %d )", game.drugs[1]);
  strcpy(sell.menuitems[1].value, str);
  sell.menuitems[1].sheet = &sell_sheet;
  strcpy(sell.menuitems[2].caption, buy.menuitems[2].caption);
  sprintf(str, "( %d )", game.drugs[2]);
  strcpy(sell.menuitems[2].value, str);
  sell.menuitems[2].sheet = &sell_sheet;
  strcpy(sell.menuitems[3].caption, buy.menuitems[3].caption);
  sprintf(str, "( %d )", game.drugs[3]);
  strcpy(sell.menuitems[3].value, str);
  sell.menuitems[3].sheet = &sell_sheet;
  strcpy(sell.menuitems[4].caption, buy.menuitems[4].caption);
  sprintf(str, "( %d )", game.drugs[4]);
  strcpy(sell.menuitems[4].value, str);
  sell.menuitems[4].sheet = &sell_sheet;
  strcpy(sell.menuitems[5].caption, buy.menuitems[5].caption);
  sprintf(str, "( %d )", game.drugs[5]);
  strcpy(sell.menuitems[5].value, str);
  sell.menuitems[5].sheet = &sell_sheet;
  sell.count = DRUG_COUNT;
  sell.prev = &buy;
  sell.next = &jet;
  
  strcpy(jet.title, "Jet");
  strcpy(jet.menuitems[0].caption, "Orlando");
  strcpy(jet.menuitems[0].value, "");
  jet.menuitems[0].sheet = &travel_sheet;
  strcpy(jet.menuitems[1].caption, "Las Vegas");
  strcpy(jet.menuitems[1].value, "");
  jet.menuitems[1].sheet = &travel_sheet;
  strcpy(jet.menuitems[2].caption, "Cupertino");
  strcpy(jet.menuitems[2].value, "");
  jet.menuitems[2].sheet = &travel_sheet;
  strcpy(jet.menuitems[3].caption, "Miami");
  strcpy(jet.menuitems[3].value, "");
  jet.menuitems[3].sheet = &travel_sheet;
  strcpy(jet.menuitems[4].caption, "Los Angeles");
  strcpy(jet.menuitems[4].value, "");
  jet.menuitems[4].sheet = &travel_sheet;
  strcpy(jet.menuitems[5].caption, "New York");
  strcpy(jet.menuitems[5].value, "");
  jet.menuitems[5].sheet = &travel_sheet;
  jet.count = CITY_COUNT;
  jet.prev = &sell;
  jet.next = &bank;

  strcpy(bank.title, "Bank");
  strcpy(bank.menuitems[0].caption, "Money:");
  sprintf(str, "$%d", game.money);
  strcpy(bank.menuitems[0].value, str);
  bank.menuitems[0].sheet = NULL;
  strcpy(bank.menuitems[1].caption, "Money in Bank:");
  sprintf(str, "$%d", game.bank);
  strcpy(bank.menuitems[1].value, str);
  bank.menuitems[1].sheet = NULL;
  strcpy(bank.menuitems[2].caption, "");
  strcpy(bank.menuitems[2].value, "");
  bank.menuitems[2].sheet = NULL;
  strcpy(bank.menuitems[3].caption, "");
  strcpy(bank.menuitems[3].value, "");
  bank.menuitems[3].sheet = NULL;
  strcpy(bank.menuitems[4].caption, "Deposit");
  strcpy(bank.menuitems[4].value, "");
  bank.menuitems[4].sheet = &deposit_sheet;
  strcpy(bank.menuitems[5].caption, "Withdraw");
  strcpy(bank.menuitems[5].value, "");
  bank.menuitems[5].sheet = &withdraw_sheet;
  bank.count = 6;
  bank.prev = &jet;
  bank.next = &bookie;
  
  strcpy(bookie.title, "Bookie");
  strcpy(bookie.menuitems[0].caption, "Debt:");
  sprintf(str, "$%d", game.debt);
  strcpy(bookie.menuitems[0].value, str);
  bookie.menuitems[0].sheet = NULL;
  strcpy(bookie.menuitems[1].caption, "");
  strcpy(bookie.menuitems[1].value, "");
  bookie.menuitems[1].sheet = NULL;
  strcpy(bookie.menuitems[2].caption, "");
  strcpy(bookie.menuitems[2].value, "");
  bookie.menuitems[2].sheet = NULL;
  strcpy(bookie.menuitems[3].caption, "");
  strcpy(bookie.menuitems[3].value, "");
  bookie.menuitems[3].sheet = NULL;
  strcpy(bookie.menuitems[4].caption, "Pay");
  strcpy(bookie.menuitems[4].value, "");
  bookie.menuitems[4].sheet = &pay_sheet;
  strcpy(bookie.menuitems[5].caption, "Borrow");
  strcpy(bookie.menuitems[5].value, "");
  bookie.menuitems[5].sheet = &borrow_sheet;
  bookie.count = 6;
  bookie.prev = &bank;
  bookie.next = NULL;

  // last because of the City
  strcpy(status.title, "Status");
  strcpy(status.menuitems[0].caption, "City:");
  strcpy(status.menuitems[0].value, jet.menuitems[game.city].caption);
  status.menuitems[0].sheet = NULL;
  strcpy(status.menuitems[1].caption, "Day:");
  sprintf(str, "%d/30", game.days);
  strcpy(status.menuitems[1].value, str);
  status.menuitems[1].sheet = NULL;
  strcpy(status.menuitems[2].caption, "Life:");
  sprintf(str, "%d/10", game.life);
  strcpy(status.menuitems[2].value, str);
  status.menuitems[2].sheet = NULL;
  strcpy(status.menuitems[3].caption, "Money:");
  sprintf(str, "$%d", game.money);
  strcpy(status.menuitems[3].value, str);
  status.menuitems[3].sheet = NULL;
  strcpy(status.menuitems[4].caption, "Debt:");
  sprintf(str, "$%d", game.debt);
  strcpy(status.menuitems[4].value, str);
  status.menuitems[4].sheet = NULL;
  strcpy(status.menuitems[5].caption, "Coat:");
  sprintf(str, "%d/%d", game.coat, game.max_coat);
  strcpy(status.menuitems[5].value, str);
  status.menuitems[5].sheet = NULL;
  status.count = 6;
  status.prev = NULL;
  status.next = &buy;
}

void init_sheets()
{
  char str[50];

  sprintf(str, "Buy %s", buy.menuitems[game.select].caption);
  strcpy(buy_sheet.title, str);
  sprintf(str, "You can afford %d.", afford(game.select));
  strcpy(buy_sheet.text, str);
  strcpy(buy_sheet.button, "Buy");
  buy_sheet.ammount = afford(game.select);

  sprintf(str, "Sell %s", buy.menuitems[game.select].caption);
  strcpy(sell_sheet.title, str);
  sprintf(str, "Total Sale: $%d.", sale(game.select));
  strcpy(sell_sheet.text, str);
  strcpy(sell_sheet.button, "Sell");
  sell_sheet.ammount = game.drugs[game.select];
  
  strcpy(deposit_sheet.title, "Deposit");
  sprintf(str, "You have $%d.", game.money);
  strcpy(deposit_sheet.text, str);
  strcpy(deposit_sheet.button, "Ok");
  deposit_sheet.ammount = game.money;

  strcpy(withdraw_sheet.title, "Withdraw");
  sprintf(str, "You have $%d.", game.money);
  strcpy(withdraw_sheet.text, str);
  strcpy(withdraw_sheet.button, "Ok");
  withdraw_sheet.ammount = game.bank;

  strcpy(pay_sheet.title, "Bookie");
  sprintf(str, "You have $%d.", game.money);
  strcpy(pay_sheet.text, str);
  strcpy(pay_sheet.button, "Pay");
  if (game.debt > game.money) {
    pay_sheet.ammount = game.money;
  } else {
    pay_sheet.ammount = game.debt;
  }
  
  strcpy(borrow_sheet.title, "Bookie");
  sprintf(str, "You have $%d.", game.money);
  strcpy(borrow_sheet.text, str);
  strcpy(borrow_sheet.button, "Borrow");
  borrow_sheet.ammount = game.money * .10;
  
  strcpy(travel_sheet.title, "");
  strcpy(travel_sheet.text, "");
  strcpy(travel_sheet.button, "");
}
