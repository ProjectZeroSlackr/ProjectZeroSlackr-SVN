#ifndef __DOPEWARS_H__
#define __DOPEWARS_H__

#define DRUG_COUNT  6
#define CITY_COUNT  6
#define EVENT_COUNT 6

typedef struct {
  char          title[50];
  char          text[100];
  char          button[50];
  unsigned int  ammount;   // set to -1 if you do not want the ammount stuff
} TSheet;

typedef struct {
  char     caption[50];
  char     value[50];
  TSheet  *sheet;    // set to NULL if you want a disabled menu item
} TMenu;

typedef struct tscreen {
  char     title[50];
  TMenu    menuitems[10];
  int      count;
  struct tscreen *prev;
  struct tscreen *next;
} TScreen;

typedef struct {
  unsigned int  days;
  unsigned int  max_days;
  unsigned int  life;
  int           money;
  int           bank;
  int           debt;
  unsigned int  coat;
  unsigned int  max_coat;
  unsigned int  drugs[DRUG_COUNT];
  unsigned int  old_price[DRUG_COUNT];
  unsigned int  count_old_price[DRUG_COUNT];
  unsigned int  price[DRUG_COUNT];
  unsigned int  city;
  TScreen      *screen;
  unsigned int  in_transaction;
  unsigned int  select;
  unsigned int  ammount;
  unsigned int  dismiss_sheet;
} TGame;

TGame game;
TScreen status, buy, sell, jet, bank, bookie;
TSheet buy_sheet, sell_sheet, deposit_sheet, withdraw_sheet, pay_sheet, borrow_sheet, travel_sheet;

#endif
