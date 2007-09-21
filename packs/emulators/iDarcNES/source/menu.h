/*
 * menu.h
 *
 * driver-specific menu support
 */

#ifndef MENU_H
#define MENU_H

/* $Id: menu.h,v 1.4 2000/08/21 01:13:07 nyef Exp $ */

typedef void (*ui_menu_callback)(void *);
struct ui_menu {
    char *name;
    int flags; /* MF_xxx constants */
    ui_menu_callback callback;
    void *callback_data;
    struct ui_menu *child_menu;
    void *ui_data;
};

#define MF_NONE  0x0000
#define MF_CHILD 0x0001

void menu_init(struct ui_menu *);
void menu_rename_item(struct ui_menu *);
void menu_enable_item(struct ui_menu *);
void menu_disable_item(struct ui_menu *);

typedef void (*ui_open_callback)(void *, char *);
void menu_file_open_box(ui_open_callback callback, void *data, char *filter);


#endif /* MENU_H */

/*
 * $Log: menu.h,v $
 * Revision 1.4  2000/08/21 01:13:07  nyef
 * added preliminary support for file open boxes
 *
 * Revision 1.3  2000/07/01 15:42:59  nyef
 * added support for child menus
 *
 * Revision 1.2  2000/06/29 01:25:12  nyef
 * renamed the menu functions more appropriately
 *
 * Revision 1.1  2000/06/29 01:05:17  nyef
 * Initial revision
 *
 */
