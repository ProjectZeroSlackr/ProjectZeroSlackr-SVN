/*
 * apple2_cards.h
 *
 * interface for apple2 peripheral cards
 */

/* $Id: apple2_cards.h,v 1.2 2000/07/01 15:44:01 nyef Exp $ */

#ifndef APPLE2_CARDS_H
#define APPLE2_CARDS_H

#include "types.h"
#include "event.h"

struct apple2_card;

typedef u8 (*a2_card_read)(struct apple2_card *card, u16 address);
typedef void (*a2_card_write)(struct apple2_card *card, u16 address, u8 data);

struct apple2_card {
    a2_card_read read;
    a2_card_write write;
    u8 *rom;
};

struct apple2_mainboard;

void apple2_register_event(struct apple2_mainboard *apple2, struct event *the_event);

struct ui_menu;

void apple2_set_child_menu(struct ui_menu *menu, int id);

/* card init routines */

struct apple2_card *apple2_disk_init(struct apple2_mainboard *apple2);

#endif /* APPLE2_CARDS_H */

/*
 * $Log: apple2_cards.h,v $
 * Revision 1.2  2000/07/01 15:44:01  nyef
 * added preliminary per-card menu hook
 *
 * Revision 1.1  2000/03/11 01:12:22  nyef
 * Initial revision
 *
 */
