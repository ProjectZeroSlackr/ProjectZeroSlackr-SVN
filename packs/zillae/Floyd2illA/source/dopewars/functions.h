#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "dopewars.h"

int can_steal(int drug, int ammount);
int can_hold(int n);
TSheet random_event();
void restart();
void do_pay();
void do_borrow();
void do_deposit();
void do_withdraw();
void do_buy(int drug, int ammount);
void do_sell(int drug, int ammount);
void travel();
int afford(int n);
int sale(int n);

int relative_price[DRUG_COUNT][2];

#endif
