/***************************************************************************
 *   Copyright (C) 2006 by Robert Bamler                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define PZ_COMPAT
#include <pz.h>

// Kill compatibility version of pz_new_window
#undef pz_new_window
#undef pz_do_window

#include "article.h"
#include "search.h"
#include "encyclopodiareader.h"
#include "bz2infile.h"

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#ifdef IPOD
        static const char *LIBPATH = "/opt/Tools/Encyclopodia/Library/";
#else
        //PLACE YOUR FILE-PATHS HERE
        static const char *LIBPATH = "/home/jsha/src/ipod/ep-files/library/";
#endif

namespace encyclopodia {

static PzModule* encyclopodiareader_module = NULL;

static Ebook *firstEbook = 0;

static HistoryEntry *historyBegin = 0;
static HistoryEntry *historyCur = 0;

extern Bz2infile* bzip_stream;

static char *aboutpagename = 0;

static PzWindow *article_window = NULL;

bool displayArticle(const char *title);
TWindow* openEbookAndCreateSearchWindow(ttk_menu_item* item);
void prepareRenderer(const char *filepath);
static int epodia_handle_event(PzEvent *event);
void historyBack();
void historyForward();
void readLibrary();

Ebook::Ebook()
	: title(NULL),
	filename(NULL),
	next(NULL) {
}

void check_alloc(void* ptr) {
	if (!ptr) {
		pz_message("malloc failed");
		exit(1);
	}
}

void search_ebook(Ebook* book, const char* text) {
	displayArticle(text);
}

void add_ebook_menu_item(TWidget* menu_widget, Ebook* book)
{
	ttk_menu_item *item;
	item = new ttk_menu_item;
	check_alloc(item);
	item->name = book->title;
	item->data = (void*)(book);
	item->makesub = openEbookAndCreateSearchWindow;
	item->flags = TTK_MENU_ICON_SUB;
	ttk_menu_insert(menu_widget, item, 0);
}

PzWindow* new_encyclopodiareader_window()
{
	readLibrary();

	TWidget *menu_widget = ttk_new_menu_widget(0, ttk_menufont,
			ttk_screen->w, ttk_screen->h);

	TWindow *ret = NULL;
	// For users with only one ebook, don't bother presenting a menu of
	// available ebooks.
	if (!firstEbook) {
		pz_error("No ebooks found!");
		ret = NULL;
	} else if (firstEbook->next == NULL) {
		ttk_menu_item item;
		item.data = firstEbook;
		ret = openEbookAndCreateSearchWindow(&item);
	} else {
		Ebook* current_ebook = firstEbook;
		while (current_ebook != NULL) {
			add_ebook_menu_item(menu_widget, current_ebook);
			current_ebook = current_ebook->next;
		}

		ret = pz_new_menu_window(menu_widget);
	}

	return ret;
}

void read_line(FILE *file, string *out)
{
	const char stopper = '\n';
        char c = fgetc(file);
	while (c != stopper && !feof(file)) {
		*out += c;
		c = fgetc(file);
	}
}

void readLibrary()
{
	DIR *dir = opendir(LIBPATH);
	if(!dir) {
		//not implemented: error handling
		return;
	}
	
	char olddir[256];
	if (!getcwd(olddir, 256)) {
		//not implemented: error handling
		return;
	}
	chdir(LIBPATH);			// required for stat and fopen
	
	Ebook *lastEbook = 0;
	int numEbooks = 0;
	
	struct stat statbuf;
	struct dirent *entry;
	while (( entry = readdir(dir) ))
	{
		// ignore directories and non *.epodia files
		int len;
		stat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)==0 && (len = strlen(entry->d_name)) > 7) {
			bool valid = true;
			int i;
			for (i=1; i<=7; ++i) {
				if (tolower(entry->d_name[len-i]) != ".epodia"[7-i]) {
					valid = false;
					break;
				}
			}
			
			if (valid) {
				++numEbooks;
				if (lastEbook) {
					lastEbook->next = new Ebook;
					check_alloc(lastEbook->next);
					lastEbook = lastEbook->next;
				} else {
					lastEbook = new Ebook;
					firstEbook = lastEbook;
				}
				
				lastEbook->filename = new char[strlen(entry->d_name) + 1];
				check_alloc(lastEbook->filename);
				strcpy(lastEbook->filename, entry->d_name);
				
				// Now open the file and read the ebook's title
				FILE *file = fopen(entry->d_name, "r");

				char c = 0;
				while (!feof(file)) {
					bool titleLine = true;
					int i;
					for (i=0; i<6; ++i) {
						if (feof(file)  ||  (c=fgetc(file)) != "title="[i]) {
							titleLine = false;
							break;
						}
					}
					
					if (titleLine) {
						string tmp;
						read_line(file, &tmp);
						lastEbook->title = new char[tmp.size() + 1];
						strcpy(lastEbook->title, tmp.c_str());

						break;
					}
				}
				
				if (!lastEbook->title) {
					lastEbook->title = new char[strlen(entry->d_name)-7+1];
					strncpy(lastEbook->title, entry->d_name, strlen(entry->d_name)-7);
					lastEbook->title[strlen(entry->d_name)-7] = 0;
				}
				
				fclose(file);
			}
		}
	}
	
	closedir(dir);
	chdir(olddir);
}

void draw_cur_article(PzWidget* widget, ttk_surface srf) {
	if (!Article::getInstance()) {
		pz_message("Can't draw: article is null");
		return;
	} else if (Article::getInstance()->getErrorState() != erOK) {
		pz_message("Error reading article");
		Article::destroyCurInstance();
		return;
	}

	Article::getInstance()->draw();
}

void prepareRenderer(const char *filepath)
{
	FILE *file = fopen(filepath, "rb");
	if (!file) {
		pz_error("Could not open %s", filepath);
		return;
	}
	
#ifndef DEBUG
	while (!feof(file)) {
		if (feof(file)) {
			//not implemented: error handling
			return;
		}
		
		char c = getc(file);
		if (c == '\n') {
			break;
		} else if (c == 'a') {
			//search for term "aboutpage="
			char *cc="boutpage=";
			while (*cc) if (getc(file) != *cc++) break;
			
			if (*cc == 0) {
				// string "aboutpage=" found
				string tmp;
				read_line(file, &tmp);
				aboutpagename = new char[tmp.size() + 1];
				strcpy(aboutpagename, tmp.c_str());
				ungetc('\n', file);
			}
		}
		
		while (!feof(file)  &&  getc(file) != '\n');
	}
#endif	
	bzip_stream->open(file);

	article_window = pz_do_window(_("Article"), PZ_WINDOW_NORMAL,
			draw_cur_article, epodia_handle_event, 100);
	
	Article* article = encyclopodia::Article::create(
		article_window, screen_info.cols, screen_info.rows,
		_("show table")
	);

	if (!article) {
		pz_message("Failed to create article");
		return;
	}
	if (!Article::getInstance()) {
		pz_message("Article::getInstance is null.  Should not happen");
		return;
	}
}

TWindow* openEbookAndCreateSearchWindow(ttk_menu_item* item) {
	Ebook* book = (Ebook*)item->data;
	TWindow* window =  create_search_window(item);
	
	pz_draw_header(book->title);

	// Prepare the article window, but don't show it until displayArticle()
	char fullpath[strlen(LIBPATH) + strlen(book->filename) - 1];
	strcpy(fullpath, LIBPATH);
	strcat(fullpath, book->filename);
	prepareRenderer(fullpath);

	return window;
}

ArticlePosition findArticle(const unsigned char *searchterm, unsigned int length, bool subtoc=true)
{
	if (subtoc) {
		if (bzip_stream->cur() != 'S' || bzip_stream->next() != 'U' || bzip_stream->next() != 'B') {
			return ArticlePosition();
		}
		bzip_stream->next();
	}
	if (bzip_stream->cur() != 'T' || bzip_stream->next() != 'O' || bzip_stream->next() != 'C') {
		return ArticlePosition();
	}
	
	unsigned char addrLength = bzip_stream->next();
	unsigned char offsetLength = bzip_stream->next();
	unsigned char wordLengthLength = bzip_stream->next();
	
	if (bzip_stream->next() != 0xFF) {
		return ArticlePosition();
	}
	
	switch (bzip_stream->next()) {
		case 0x01: {	// List contains addresses of blocks where more detailed lists start
			unsigned char c;
			unsigned int curlength = 0;
			unsigned int i;
			unsigned char lastaddr[addrLength];
			for (i=0; i<addrLength; ++i)
				lastaddr[i] = 0;
			
			while ((c = bzip_stream->next()) != 0xFF) {
				if (c == 0xFE) {
					curlength = bzip_stream->nextUint(wordLengthLength);
					if (curlength >= length) break;
				} else {
					for (i=0; i<curlength; ++i)
						bzip_stream->next();
					
					lastaddr[0] = bzip_stream->cur();
					for (i=1; i<addrLength; ++i)
						lastaddr[i] = bzip_stream->next();
				}
			}
			
			if (curlength == length) {
				/* reached first item with correct length. Search last item
				 * that is lexicographically smaller than searchterm */
				
				while ((c = bzip_stream->next()) != 0xFF && c != 0xFE) {
					const unsigned char *cc = searchterm;
					i = length;
					if (c == *searchterm) {
						while (--i && bzip_stream->next() == *(++cc));
					}
					
					if (*cc == bzip_stream->cur()) {
						// the strings are equal
						for (i=0; i<addrLength; ++i)
							lastaddr[i] = bzip_stream->next();
						break;
					} else if (bzip_stream->cur() < *cc) {
						while (--i) bzip_stream->next();
						for (i=0; i<addrLength; ++i)
							lastaddr[i] = bzip_stream->next();
					} else {
						break;
					}
				}
			}
			
			unsigned int addr = 0;
			for (i=0; i<addrLength; ++i) {
				addr <<= 8;
				addr += lastaddr[i];
			}
			
			if (addr == 0) {
				return ArticlePosition();
			}
			bzip_stream->setBlockBegin(-addr, SEEK_END);
			return findArticle(searchterm, length);
		}
		
		case 0x02: {	// List contains addresses and offsets of articles
			unsigned char c;
			unsigned int curlength = 0;
			unsigned int i;
			unsigned char lastaddr[addrLength];
			int offset = -1;
			for (i=0; i<addrLength; ++i)
				lastaddr[i] = 0;
			
			while ((c = bzip_stream->next()) != 0xFF) {
				if (c == 0xFE) {
					curlength = bzip_stream->nextUint(wordLengthLength);
					if (curlength >= length) break;
				} else if (c == 0xFD) {
					for (i=0; i<addrLength; ++i)
						lastaddr[i] = bzip_stream->next();
				} else {
					for (i=1; i<curlength+offsetLength; ++i)
						bzip_stream->next();
				}
			}
			
			if (bzip_stream->cur() == 0xFF) {
				return ArticlePosition();
			}
			
			if (curlength == length) {
				/* reached first item with correct length. Search last item
				 * that is lexicographically smaller than searchterm */
				
				while ((c = bzip_stream->next()) != 0xFF && c != 0xFE) {
					if (c == 0xFD) {
						for (i=0; i<addrLength; ++i)
							lastaddr[i] = bzip_stream->next();
					} else {
						const unsigned char *cc = searchterm;
						i = length;
						if (c == *searchterm) {
							while (--i && bzip_stream->next() == *(++cc));
						}
						
						if (*cc == bzip_stream->cur()) {
							// the strings are equal
							offset = bzip_stream->nextUint(offsetLength);
							break;
						} else if (bzip_stream->cur() > *cc) {
							break;
						}
						
						i += offsetLength;
						while (--i) 
							c = bzip_stream->next();
					}
				}
			}
			
			int addr = 0;
			for (i=0; i<addrLength; ++i) {
				addr <<= 8;
				addr += lastaddr[i];
			}
			
			if (addr == 0 || offset == -1) {
				return ArticlePosition();
			}
			return ArticlePosition(addr, offset);
		}
		
		default: return ArticlePosition();
	}
}

void displayArticle(const ArticlePosition &pos)
{
	bzip_stream->setBlockBegin(-pos.blockAddr, SEEK_END);
	bzip_stream->seekForwardTo(pos.offset);

	Article::getInstance()->readArticle();

	if (!Article::getInstance() || !Article::getInstance()->getErrorState() == erOK) {
		cerr << "Error occured while reading Article.\n";
		char *buf = new char[30];
		sprintf(buf, "er:%d %c%c%c%c\n", Article::getInstance()->getErrorState(), bzip_stream->cur(), bzip_stream->next(), bzip_stream->next(), bzip_stream->next());
		cerr << buf;
		cerr.flush();
		delete[] buf;
	}

	pz_show_window(article_window);
}

bool displayArticle(const char *title)
{
	pz_draw_header("Searching...");
	unsigned char searchterm[strlen(title) * 2 + 1];
	unsigned int length;
	{
		unsigned char *bufpos = searchterm;
		for (unsigned char *c = (unsigned char*) title; *c; ++c) {
			if ((*c & 0x80) == 0x00) *bufpos = tolower(*c);
			else if ((*c & 0xFC) == 0xC0) {		// *c == 110000??
				unsigned char tmp = ((*c & 0x03) << 6)  |  (*(c+1) & 0x3F);
				if ((tmp>=0xC0 && tmp<=0xD6) || (tmp>=0xD9 && tmp<=0xDE))
					tmp += 0x20;	// convert to lower case
				*bufpos++ = 0xC0 | (0x03 & (tmp>>6));
				*bufpos = 0x80 | (0x3F & tmp);
				++c;
				if (*c == 0) break;
			} else if (*c >= 0xFC) {
				//This should actually never be reached with propper utf8 text
				*bufpos++ = 0xFC;
				*bufpos = (0x0F & *c);
			} else {
				*bufpos = *c;
			}
			++bufpos;
		}
		*bufpos = 0;
		length = bufpos - searchterm;
	}
	
	bzip_stream->goToFirstBlock();
	ArticlePosition pos = findArticle(searchterm, length, false);
	
	if (pos.blockAddr == 0) {
		pz_message("Failed to find article");
		Article::destroyCurInstance();
		return false;
	}
	
	if (historyCur) {
		HistoryEntry *i = historyCur->next;
		while (i) {
			HistoryEntry *ii = i->next;
			delete i;
			i = ii;
		}
		historyCur = (historyCur->next = new HistoryEntry(pos, historyCur, 0));
	} else {
		historyCur = (historyBegin = new HistoryEntry(pos, 0, 0));
	}
	
	displayArticle(pos);
	
	return true;
}

static int epodia_handle_event(PzEvent *event)
{
	int ret = 0;

	switch (event->type) {
		case PZ_EVENT_SCROLL:
			TTK_SCROLLMOD(event->arg, 1);
			if (event->arg < 0) {
				if (Article::getInstance()->linkHovered()) {
					Article::getInstance()->hoverPreviousLink();
				} else {
					Article::getInstance()->scrollUp();
				}
			} else if (event->arg > 0) {
				if (Article::getInstance()->linkHovered()) {
					Article::getInstance()->hoverNextLink();
				} else {
					Article::getInstance()->scrollDown();
				}
			}
			break;

		case PZ_EVENT_BUTTON_DOWN:
			switch (event->arg) {
				case IPOD_SWITCH_HOLD:
					break;
				case IPOD_BUTTON_MENU:
					if (Article::getInstance()->linkHovered()) {
						Article::getInstance()->unhoverLink();
					} else if (Article::getInstance()->isInTable()) {
						Article::getInstance()->leaveTable();
					} else {
						pz_close_window(article_window);
						Article::destroyCurInstance();
					}
					break;
			//	case IPOD_BUTTON_ACTION:
				case '\n':
					if (Article::getInstance()->linkHovered()) {
						Article::getInstance()->clickHoveredLink(encyclopodia::displayArticle);
					} else {
						Article::getInstance()->hoverLink();
					}
					break;
				case IPOD_BUTTON_REWIND:
					if (Article::getInstance()->isInTable())
						Article::getInstance()->scrollLeft();
					else
						encyclopodia::historyBack();
					break;
				case IPOD_BUTTON_FORWARD:
					if (Article::getInstance()->isInTable())
						Article::getInstance()->scrollRight();
					else
						encyclopodia::historyForward();
					break;
				default:
					ret |= KEY_UNUSED; // allow a default key action
					break;
			}
			break;
		
		case GR_EVENT_TYPE_KEY_UP:
			ret |= KEY_UNUSED;
			break;
		default:
			ret |= EVENT_UNUSED;
	}
	if (event->type != PZ_EVENT_TIMER)
		event->wid->dirty = 1;
	return ret;
}

void historyBack()
{
	if (historyCur && historyCur->prev) {
		historyCur = historyCur->prev;
		displayArticle(historyCur->pos);
	}
}

void historyForward()
{
	if (historyCur && historyCur->next) {
		historyCur = historyCur->next;
		displayArticle(historyCur->pos);
	}
}


void init_encyclopodiareader()
{
	bzip_stream = new Bz2infile();

	encyclopodiareader_module = pz_register_module ("encyclopodiareader", NULL);
	pz_menu_add_action_group("/Encyclopodia", "#Reference", new_encyclopodiareader_window);
}

} // end namespace encyclopodia


// Link the following init functions without C++ mangling, so the podzilla
// loader can find them.
extern "C" {

PZ_MOD_INIT(encyclopodia::init_encyclopodiareader);

} // end extern "C" linkage
