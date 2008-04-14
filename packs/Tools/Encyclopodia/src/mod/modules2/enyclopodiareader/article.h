/***************************************************************************
 *   Copyright (C) 2005 by Robert Bamler   *
 *   Robert.Bamler@gmx.de   *
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
#ifndef IKNOWARTICLE_H
#define IKNOWARTICLE_H

#include<stdio.h>
#include "bz2infile.h"
#include<iostream>
using namespace std;

#define PZ_COMPAT

extern "C" {
#include "pz.h"
}

namespace encyclopodia {

typedef unsigned char uint1;
typedef signed char sint1;
typedef unsigned short uint2;

struct MarkupState;
class Element;


enum ParserErrorState_t {
	erOK=0, erBadPosition, erBadTitle, erExpectedEOA,
	erExpectedHref, erTextLinkNotTerminated, 
	erExpectedAnchorId, erAnchorPointNotTerminated,
	erUnexceptedBlockEnd, erExpectedAuthorList,
	erExpectedBlockEnd, erUnknownBlock, erExpectedControlChar,
	erExpectedLatex, erMathNotTerminated
};

static ParserErrorState_t errorState = erOK;
		
class TextLink;
class Table;

typedef struct ArticlePosition {
	unsigned int blockAddr;
	unsigned int offset;
	ArticlePosition() : blockAddr(0), offset(0) {}
	ArticlePosition(int a, int o) : blockAddr(a), offset(o) {}
} ArticlePosition;

/**
@author Robert Bamler
*/
class Article {
	friend class Table;
	private:
		bool valid;
		string title;
		
		Element *lastElement;
		MarkupState *firstMS, *curMS;
		
		static t_GR_WINDOW_ID bufWin, parentWin;
		static int parentWinWidth, parentWinHeight;
		static t_GR_GC_ID gc;
		
		struct {
			uint2 year;
			uint1 month, dayOfMonth, hour, minute, second;
			sint1 timeZone;
		} lastChange;
		
		void readTimestamp();
		
		struct IntListChunk {
			uint1 count;
			sint1 type;
			uint1 *data;
			IntListChunk *next;
		
			IntListChunk();
			~IntListChunk();
		};
		
		
		IntListChunk *firstAuthorListChunk;
		
		/**
		 * readUsers only reads the int list to char[]s. It doesn't
		 * split the integer junks yet. This is supposed to be faster
		 * and in most cases, the user list is never needed.
		 */
		void readAuthors();
		
		TextLink *firstLink;
		int posY;
		
		/**
		 * Parses an article using the current state of 'stream'. Cannot
		 * be called directely, because Article is a singleton. Use
		 * Article::create() TODO: this comment is wrong
		 */
		Article(TWindow* parentWin, t_GR_SIZE bufWidth, t_GR_SIZE paramParentWinHeight, char *paramTableReplacement);
		
		~Article();
		
		static Article *curInstance;
		void doDrawTable(Table *table, bool needNewBufWin);
		
		Table *inTable;
	
	public:
		inline ParserErrorState_t getErrorState() { return errorState; }
		
		void cleanupCurrentArticle();
		
		inline bool isValid() { return valid; }
		
		/**
		 * Creates a new Article by parsing the input from the current stream.
		 * If an Article was already instanciated before, the function doesn't
		 * touch the instanciated Article and returns 0.
		 */
		static Article *create(t_GR_WINDOW_ID parentWin, t_GR_SIZE bufWidth, t_GR_SIZE paramParentWinHeight, char *paramTableReplacement) {
			if (curInstance)  return 0;
			else  return curInstance = new Article(parentWin, bufWidth, paramParentWinHeight, paramTableReplacement);
		}
		
		inline static void destroyCurInstance() {
			delete curInstance;
			curInstance = 0;
		}
		
		inline static Article *getInstance() { return curInstance; }
		
		void readArticle();
		
		void draw();
		void draw(const int y);
		void scrollUp();
		void scrollDown();
		void scrollLeft();
		void scrollRight();
		
		bool linkHovered();
		void clickHoveredLink(bool (callback)(const char*));
		
		inline bool isInTable() const { return inTable; }
		void leaveTable();
		
		void doReturnFromTable();
		
		void hoverLink();
		void unhoverLink();
		void hoverNextLink();
		void hoverPreviousLink();
		
		inline static void drawTable(Table *table, bool needNewBufWin=true) {
			if (curInstance) curInstance->doDrawTable(table, needNewBufWin);
		}
		
		inline static void returnFromTable() {
			if (curInstance) curInstance->doReturnFromTable();
		}
};

}

#endif
