#ifndef __TTK_MENU_DATA__H__
#define __TTK_MENU_DATA__H__

typedef struct _menu_data 
{
    ttk_menu_item **menu;
    int allocation;
    ttk_menu_item *mlist;
    int *vixi, *xivi, vitems;
    ttk_surface *itemsrf, *itemsrfI;
    ttk_font font;
    int items, itemheight, top, sel, visible, scroll, spos, sheight;
    int ds;
    int closeable;
    int epoch;
    int free_everything;
    int i18nable;
    int drawn;
} menu_data;

#endif
