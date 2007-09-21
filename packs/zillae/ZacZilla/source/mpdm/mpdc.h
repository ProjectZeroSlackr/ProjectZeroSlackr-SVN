#ifndef _MPDC_H_
#define _MPDC_H_

#include "../pz.h"
#include "libmpdclient.h"

extern mpd_Connection *mpdz;

/* mpdc.c */
void mpdc_change_volume(mpd_Connection *con_fd, int volume);
void mpdc_next();
void mpdc_prev();
void mpdc_playpause();
void mpd_destroy_connection(mpd_Connection *con_fd, char *err);
mpd_Connection *mpd_init_connection(void);
int mpdc_status(mpd_Connection *con_fs);
int mpdc_init();
void mpdc_destroy();
int mpdc_tickle();

/* player.c */
TWindow *mpd_currently_playing(void);

/* menu.c */
TWindow *new_album_menu(void);
TWindow *new_artist_menu(void);
TWindow *new_queue_menu(void);
TWindow *new_song_menu(void);
TWindow *new_playlist_menu(void);
TWindow *new_genre_menu(void);

TWidget *populate_albums(char *search);
TWidget *populate_songs(char *search);

#endif /* _MPDC_H_ */
