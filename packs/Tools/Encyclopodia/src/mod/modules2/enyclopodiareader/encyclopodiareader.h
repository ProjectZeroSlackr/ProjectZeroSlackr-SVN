#ifndef IKNOWENCYCLOPODIAREADER_H
#define IKNOWENCYCLOPODIAREADER_H

#include "article.h"

namespace encyclopodia {

typedef struct Ebook {
	char *title;
	char *filename;
        struct Ebook *next;
	Ebook();
} Ebook;

struct HistoryEntry {
	ArticlePosition pos;
	HistoryEntry *prev, *next;
	
	HistoryEntry(const ArticlePosition &pPos, HistoryEntry *pPrev, HistoryEntry *pNext)
	  : pos(pPos), prev(pPrev), next(pNext) { }
};


extern void search_ebook(Ebook* book, const char* text);

}

#endif // IKNOWENCYCLOPODIAREADER_H
