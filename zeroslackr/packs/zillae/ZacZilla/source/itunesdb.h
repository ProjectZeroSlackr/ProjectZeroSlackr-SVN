/*
 * Copyright (C) 2004 Jens Taprogge <jens.taprogge@post.rwth-aachen.de>
 *
 * See COPYING for details.
 */

#ifndef _ITUNESDB_H
#define _ITUNESDB_H

#include <stdint.h>
#include <sys/time.h>

struct itdb_track {
	unsigned int 		 db_offset;
	uint32_t		 ipodid;
	unsigned int		 filetype;
	unsigned int 		 filesize;
	time_t			 lastmod;
	time_t			 lastplayed;
	unsigned short int 	 rating;
	unsigned int		 length;
	unsigned short int 	 year;
	unsigned short int	 trackno;
	unsigned short int 	 notracks;
	unsigned short int	 cdno;
	unsigned short int 	 nocds;
	short int 		 voladj;
	unsigned short int 	 playcount;
	
	char			*title;
	char			*path;
	char			*album;
	char			*artist;
	char			*genre;
	char			*fdesc;
	char			*comment;
	char			*composer;
};


struct itdb_plist {
	unsigned int 		 db_offset;
	uint32_t		 type;
	unsigned int		 notracks;
	uint32_t		*ipodids;

	char			*title;
};


enum {
	ITDB_PARSE_TITLE = 1 << 0,
	ITDB_PARSE_PATH = 1 << 1,
	ITDB_PARSE_ALBUM = 1 << 2,
	ITDB_PARSE_ARTIST = 1 << 3,
	ITDB_PARSE_GENRE = 1 << 4,
	ITDB_PARSE_COMPOSER = 1 << 5,
	ITDB_PARSE_TRACK_ANY = ITDB_PARSE_TITLE | ITDB_PARSE_PATH |
		ITDB_PARSE_ALBUM | ITDB_PARSE_ARTIST | ITDB_PARSE_GENRE |
		ITDB_PARSE_COMPOSER,
		
	ITDB_PARSE_PLAYLIST_NORMAL = 1 << 10,
	ITDB_PARSE_PLAYLIST_MASTER = 1 << 11,
	ITDB_PARSE_PLAYLIST_ANY = ITDB_PARSE_PLAYLIST_NORMAL |
		ITDB_PARSE_PLAYLIST_MASTER,
	
	ITDB_PARSE_DETAILS = 1 << 15,
	
	ITDB_PARSE_UNKNOWN = 0,
	
	ITDB_PARSE_ALL = ~0
};


enum {
	ITDB_PLISTTYPE_MASTER = 1
};


struct itdb_parsecont;


struct itdb_parsecont *itdb_new_parsecont();
void itdb_delete_parsecont(struct itdb_parsecont *pc);

void itdb_set_cachesize(struct itdb_parsecont *pc, size_t size);

void itdb_sel_parseentries(struct itdb_parsecont *pc, unsigned int entries);

int itdb_parse_file(struct itdb_parsecont *pc, char *filename);

struct itdb_track *itdb_track_dup(struct itdb_track *t);

void itdb_track_free(struct itdb_track *t);


/* callback definitions
 *
 * The library does posses control over the 'track' structure only until the
 * the callback returns or a new library call is issued. Do NOT
 * itdb_track_free() the track structure.
 *
 * itdb_track_dup() provides a convenient way to obtain a private copy of the
 * track structure.
 *
 * userdata is the same pointer that has been provided to the itdb_add_*cb()
 * call.
 */

typedef int (*itdb_track_callback) (struct itdb_track *track, void *userdata);

typedef int (*itdb_plist_callback) (struct itdb_plist *plist, void *userdata);

typedef int (*itdb_notracks_callback) (unsigned int, void *userdata);

typedef int (*itdb_noplists_callback) (unsigned int, void *userdata);


/* 
 * Functions to provide callbacks to the library. 
 *
 * If a certain callback should be deactivated, NULL can be passed as the
 * callback function.
 */

void itdb_add_track_cb(struct itdb_parsecont *pc, itdb_track_callback track_cb,
		void *userdata);

void itdb_add_plist_cb(struct itdb_parsecont *pc, itdb_plist_callback plist_cb,
		void *userdata);

void itdb_add_notracks_cb(struct itdb_parsecont *pc, 
		itdb_notracks_callback notracks_cb,
		void *userdata);

void itdb_add_noplists_cb(struct itdb_parsecont *pc, 
		itdb_noplists_callback noplists_cb,
		void *userdata);


/*
 * Functions to update single entries of specific records.
 *
 * Do not call these while the db is parsed (using e.g. itdb_parse_file()).
 */

int itdb_set_playcount(struct itdb_parsecont *pc, unsigned int db_offset, 
		unsigned short int pcount); 

int itdb_set_lastplayed(struct itdb_parsecont *pc, unsigned int db_offset, 
		time_t lp); 

int itdb_set_rating(struct itdb_parsecont *pc, unsigned int db_offset, 
		unsigned short int rating); 

int itdb_upd_play(struct itdb_parsecont *pc, unsigned int db_offset);


/*
 * The returned track structure is only to considered private until the next
 * library call is issued. Do NOT itdb_track_free() the structure.
 */

struct itdb_track *itdb_read_track(struct itdb_parsecont *pc, unsigned int
		db_offset);

#endif

