/*
 * Copyright (C) 2005 Jonathan Bettencourt (jonrelay)
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

/*
 * This file is all about Predictive Text. :)
 *
 * This can use either a text file and an array of structs for the
 * dictionary, or a SQLite database. If you want to use a SQLite
 * database you have to #define PTEXT_DB somewhere in the Makefile.
 */

// KERIPO MOD
#include "../_mods.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pz.h"


#ifndef PTEXT_DB

typedef struct text_ptext_dict_ {
	char * numberstring;
	char * word;
} text_ptext_dict;
// KERIPO MOD
//const char * text_ptext_file = "/usr/share/dict/ptextwords";
const char * text_ptext_file = PTEXT_WORDS;
static text_ptext_dict * text_ptext_dictionary;
static int text_ptext_dict_size = 0;
static int text_ptext_dict_real_size = 0;

#else

#include "../sqlite/sqlite3.h"
// KERIPO MOD
//const char * text_ptext_file = "/etc/ptext.db";
const char * text_ptext_file = PTEXT_DB;
static sqlite3 * text_ptext_db = NULL;

#endif


int text_ptext_add_word(char * ns, char * w)
{
#ifndef PTEXT_DB
	if (text_ptext_dict_size >= text_ptext_dict_real_size) {
		if (( text_ptext_dictionary = (text_ptext_dict *)realloc(text_ptext_dictionary, sizeof(text_ptext_dict) * (text_ptext_dict_real_size+512)) ) == NULL ) { return 1; }
		text_ptext_dict_real_size += 512;
	}
	text_ptext_dictionary[text_ptext_dict_size].numberstring = ns;
	text_ptext_dictionary[text_ptext_dict_size].word = w;
	text_ptext_dict_size++;
	return 0;
#else
	char q[255];
	snprintf(q, 255, "INSERT INTO ptext VALUES(\'%s\', \'%s\');", ns, w);
	sqlite3_exec(text_ptext_db, q, NULL, NULL, NULL);
	return 0;
#endif
}

int text_ptext_inited(void)
{
#ifndef PTEXT_DB
	return (text_ptext_dict_size != 0);
#else
	return (text_ptext_db != NULL);
#endif
}

int text_ptext_init(void)
{
#ifndef PTEXT_DB
	text_ptext_dict_real_size = 512;
	text_ptext_dict_size = 0;
	if ( (text_ptext_dictionary = (text_ptext_dict *)malloc(text_ptext_dict_real_size * sizeof(text_ptext_dict))) == NULL ) {
		pz_error(_("malloc failed; can't load ptext dictionary"));
		return 1;
	} else {
		
		FILE * fp;
		char currline[128];
		char * sof;
		char * fpos;
		char * currfield;
		char * word;
		char * numstr;
		if ( (fp = fopen(text_ptext_file, "r")) == NULL ) {
			return 1;
		}
		clearerr(fp);
		while ( (fgets(currline, 120, fp) != NULL) && !feof(fp) ) {
			if (currline[strlen(currline)-1] == 10) { currline[strlen(currline)-1] = 0; }
			if ((currline[0] != '#') && (currline[0] != 0)) {
				fpos = currline;
				sof = fpos;
				while (((*fpos) != ',') && ((*fpos) != 0)) { fpos++; }
				if (( currfield = (char *)malloc((fpos - sof + 1) * sizeof(char)) ) == NULL) { return 1; }
				memcpy(currfield, sof, fpos-sof);
				currfield[fpos-sof] = 0;
				numstr = strdup(currfield);
				free(currfield);
				fpos++;
				sof = fpos;
				while (((*fpos) != ',') && ((*fpos) != 0)) { fpos++; }
				if (( currfield = (char *)malloc((fpos - sof + 1) * sizeof(char)) ) == NULL) { return 1; }
				memcpy(currfield, sof, fpos-sof);
				currfield[fpos-sof] = 0;
				word = strdup(currfield);
				free(currfield);
				fpos++;
				sof = fpos;
				if ( text_ptext_add_word(numstr, word) ) { free(numstr); free(word); return 1; }
				free(numstr);
				free(word);
			}
		}
		fclose(fp);
		return 0;
	}
#else
	if (text_ptext_db != NULL) {
		sqlite3_close(text_ptext_db);
		text_ptext_db = NULL;
	}
	return ((  sqlite3_open(text_ptext_file, &text_ptext_db)  ) == SQLITE_OK);
#endif
}

void text_ptext_free(void)
{
#ifdef PTEXT_DB
	if (text_ptext_db != NULL) {
		sqlite3_close(text_ptext_db);
		text_ptext_db = NULL;
	}
#endif
}

char * text_ptext_letters_to_numbers(char * s)
{
	char * t = strdup(s);
	int i;
	for (i=0; i<strlen(t); i++) {
		switch (t[i]) {
		case '0':
			/*
			t[i] = '0';
			break;
			*/
		case '1':
			t[i] = '1';
			break;
		case '2':
		case 'a':		case 'b':		case 'c':
		case 'A':		case 'B':		case 'C':
			t[i] = '2';
			break;
		case '3':
		case 'd':		case 'e':		case 'f':
		case 'D':		case 'E':		case 'F':
			t[i] = '3';
			break;
		case '4':
		case 'g':		case 'h':		case 'i':
		case 'G':		case 'H':		case 'I':
			t[i] = '4';
			break;
		case '5':
		case 'j':		case 'k':		case 'l':
		case 'J':		case 'K':		case 'L':
			t[i] = '5';
			break;
		case '6':
		case 'm':		case 'n':		case 'o':
		case 'M':		case 'N':		case 'O':
			t[i] = '6';
			break;
		case '7':
		case 'p':		case 'q':		case 'r':		case 's':
		case 'P':		case 'Q':		case 'R':		case 'S':
			t[i] = '7';
			break;
		case '8':
		case 't':		case 'u':		case 'v':
		case 'T':		case 'U':		case 'V':
			t[i] = '8';
			break;
		case '9':
		case 'w':		case 'x':		case 'y':		case 'z':
		case 'W':		case 'X':		case 'Y':		case 'Z':
			t[i] = '9';
			break;
		}
	}
	return t;
}

char * text_ptext_look_up(char * s)
{
#ifndef PTEXT_DB
	int i;
	for (i=0; i<text_ptext_dict_size; i++) {
		if (!strcmp(s,text_ptext_dictionary[i].numberstring)) {
			return text_ptext_dictionary[i].word;
		}
	}
	return s;
#else
	char q[255];
	char ** r;
	int nr,nc;
	snprintf(q, 255, "SELECT word FROM ptext WHERE number=\'%s\';", s);
	sqlite3_get_table(text_ptext_db, q, &r, &nr, &nc, NULL);
	if (nr < 1) {
		strncpy(q, s, 255);
	} else {
		strncpy(q, r[nc], 255);
	}
	sqlite3_free_table(r);
	return strdup(q);
#endif
}

char * text_ptext_reverse_look_up(char * s)
{
#ifndef PTEXT_DB
	int i;
	for (i=0; i<text_ptext_dict_size; i++) {
		if (!strcmp(s,text_ptext_dictionary[i].word)) {
			return text_ptext_dictionary[i].numberstring;
		}
	}
	return text_ptext_letters_to_numbers(s);
#else
	char q[255];
	char ** r;
	int nr,nc;
	snprintf(q, 255, "SELECT number FROM ptext WHERE word=\'%s\';", s);
	sqlite3_get_table(text_ptext_db, q, &r, &nr, &nc, NULL);
	if (nr < 1) {
		strncpy(q, text_ptext_letters_to_numbers(s), 255);
	} else {
		strncpy(q, r[nc], 255);
	}
	sqlite3_free_table(r);
	return strdup(q);
#endif
}

int text_ptext_alphanum(char ch)
{
	return ( ((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) );
}

int text_ptext_predict(char * buf, int pos, int method)
{
	int mypos = pos;
	int myend;
	int ldiff;
	char localcopy[256];
	char * localcpy;
	char t;
	if (!text_ptext_inited()) { return 0; }	
	while ((mypos > 0) && text_ptext_alphanum(buf[mypos-1])) {
		mypos--;
	}
	myend = mypos;
	while (text_ptext_alphanum(buf[myend])) {
		myend++;
	}
	if (myend > mypos) {
		memcpy(localcopy, buf+mypos, myend-mypos);
		localcopy[myend-mypos] = 0;
		switch (method) {
		case 1:
			if ((localcopy[0] >= '0') && (localcopy[0] <= '9')) {
				localcpy = text_ptext_letters_to_numbers(localcopy);
				strcpy(localcopy, localcpy);
			} else {
				t = localcopy[myend-mypos-1];
				if (t == '0') { t = '1'; }
				if ((t >= '0') && (t <= '9')) {
					localcopy[myend-mypos-1] = 0;
					localcpy = text_ptext_reverse_look_up(localcopy);
					strcpy(localcopy, localcpy);
					localcopy[strlen(localcopy)+1] = 0;
					localcopy[strlen(localcopy)] = t;
				} else {
					localcpy = text_ptext_letters_to_numbers(localcopy);
					strcpy(localcopy, localcpy);
				}
			}
			localcpy = text_ptext_look_up(localcopy);
			break;
		default:
			localcpy = text_ptext_letters_to_numbers(localcopy);
			localcpy = text_ptext_look_up(localcpy);
			break;
		}
		ldiff = (myend-mypos)-strlen(localcpy);
		if (ldiff < 0) { return 0; }
		memcpy(buf+mypos, localcpy, strlen(localcpy));
		if (ldiff) {
			while (buf[myend-1]) {
				buf[myend-ldiff] = buf[myend];
				myend++;
			}
		}
		return ldiff;
	}
	return 0;
}

