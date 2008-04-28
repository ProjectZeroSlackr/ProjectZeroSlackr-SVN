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
#include "article.h"

namespace encyclopodia {

extern void check_alloc(void*);

Article *Article::curInstance = 0;

GR_GC_ID Article::gc = 0;
GR_WINDOW_ID Article::parentWin = 0;
GR_WINDOW_ID Article::bufWin = 0;
int Article::parentWinWidth = 0;
int Article::parentWinHeight = 0;

typedef const unsigned char SpecialChar;
typedef const unsigned char CStype;		// control sequence type
typedef const unsigned char Blocktype;

SpecialChar scBlockBegin = 0xFF;
SpecialChar scBlockEnd   = 0xFE;
SpecialChar scString     = 0xFD;
SpecialChar scEscapeChar = 0xFC;

CStype csDummy = 0;
CStype csChangeFontFamily = 16;
CStype csChangeFontSize = 17;
CStype csLineBreak = 18;
CStype csParagraphBreak = 19;
CStype csHline = 20;
CStype csBeginItalic = 21;
CStype csEndItalic = 22;
CStype csBeginBold = 23;
CStype csEndBold = 24;
CStype csBeginUnderlined = 25;
CStype csEndUnderlined = 26;
CStype csTableRow = 27;
CStype csTableCell = 28;
CStype csTableHeaderCell = 29;

Blocktype blockArticle = 128;
Blocktype blockIntList = 129;
Blocktype blockList = 130;
Blocktype blockListItem = 131;
Blocktype blockTextLink = 132;
Blocktype blockAnchorPoint = 133;
Blocktype blockHeader = 134;
Blocktype blockTable = 135;
Blocktype blockMath = 136;
Blocktype blockAlternativeStuff = 137;

Bz2infile* bzip_stream = NULL;

inline uint1 readUint1() { return bzip_stream->next(); }
inline sint1 readSint1() { return bzip_stream->next(); }
inline uint2 readUint2() { return (bzip_stream->next() << 8) | bzip_stream->next(); }

int drawRegionRight, bufWinWidth, bufWinX, bufWinY, bufWinTextBeginY, bufWinHeight;
TextLink *hoveredLink = 0;

const uint1 DEFAULTFONTSIZE = 14;

/**
 * Holds all the information needed for Block::draw() to draw
 * itself using the font and other settings that have been set
 * by previous blocks.
 */

struct MarkupState;
void setFontFromMS(GR_GC_ID &gc, MarkupState &ms);

struct MarkupState {
	public:
		int x;
		long y;
		uint1 fontSize;
		uint1 fontFamily;
		uint1 fontStyle;
		uint1 spaceWidth;
		int indent;
		
		static const uint1 fsBold       = 0x01;
		static const uint1 fsItalic     = 0x02;
		static const uint1 fsUnderlined = 0x04;
		
		Element *nextElement;
		MarkupState *prev, *next;
		
		/* inHoveredLink is currently static, because we never
		 * change it more than once while drawing (only in TextLink).
		 * You should, however, acces it via ms.inHoveredLink rather
		 * than MarkupState::inHoveredLink, because it could become
		 * non-static one day. */
		static bool inHoveredLink;
		
		/* tableindent is currently static, but you should accesss it
		 * via ms.tableindent rather than MarkupState::tableindent,
		 * because it might become non-static one day. */
		static int tableindent;
		
		MarkupState()
		  : x(0), y(0), fontSize(DEFAULTFONTSIZE), fontFamily(0), fontStyle(0),
		    spaceWidth(0), indent(0), prev(0), next(0) {
			inHoveredLink = false;
			tableindent = 0;
		}
		
		void writeSettingsToGC(GR_GC_ID &gc)
		{
			GrSetGCUseBackground(gc, GR_FALSE);
			GrSetGCForeground(gc, BLACK);
			setFontFromMS(gc, *this);
		}
};
bool MarkupState::inHoveredLink = false;
int MarkupState::tableindent = 0;




/**
 * Abstract base class for all control sequences and blocks.
 */
class Element {
	public:
		/**
		 * All classes derived from Block must implement a draw method that has three jobs:
		 * 1. It draws the represented block on the given window using the given GC.
		 * 3. It modifies the GC, if needed. For example, ChanageFontFamily-class would
		 *    change the font family of the GC.
		 * 2. It modifies the MarkupState, if needed. MarkupStates are used to store the
		 *    complete state of the parser at periodical positions in the article, so that
		 *    it is easy to draw the article from that position on.
		 */
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const =0;
		
		/**
		 * This virtual function is used by Table::calculateCellSizes() to calculate the sizes of the
		 * table cells before it actually draws the table to a window; The function
		 * 1. modifies ms in the same way that would do the draw function;
		 * 2. increases curCellWidth if it would need more horizontal space to draw the element;
		 */
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const {
			draw(0, gc, ms); //This *must* be overwritten if draw() accesses the window
		};
		
		virtual ~Element() { };
		
		Element *next;
};

class Text : public Element {
	friend class Article;
	private:
		class Chunk {
			public:

			Chunk(const char *string, uint1 length);
			
			~Chunk();
			
			void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms, bool reallyDraw, uint1 &curwidth) const;
			
			/* lastx, lasty, lastwidth, and lastheight are set after
			* a chunk was drawn that did not end in a space. When the
			* next chunk is drawn, the draw function will check whether
			* ms is still at the same position as before and whether
			* this chunk doesn't start with a space. If both is true, it
			* considers wrapping around both the last word and the new
			* word if neccessary.
			* When a chunk is drawn that does end in a space, lastx
			* will be set to -1 so that the next chunk won't wrap
			* around words from this chunk.
			* Note that this might bring problems when we start drawing
			* to a pixmap. That's why the Article::draw function
			* only stores MarkupStates in the list if it is sure that
			* there's no wordbreaking to be done.
			*/
			static int lastx;
			static long lasty;
			static int lastwidth;
			static int lastheight;
			
			void appendToString(string *out);

			static Text::Chunk* createFromBzipStream();

			Chunk *next;

			unsigned char getSize() { return size; }

			private:
			
			unsigned char size;
			unsigned char *str;

			// Private default constructor
			Chunk();
			void initFromBzipStream();
		};
		
		Chunk *firstChunk;
	
	public:
		Text();
		~Text();
		
		Text(const char *str, uint1 length) {
			firstChunk = new Chunk(str, length);
			firstChunk->next = 0;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const;
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const;
		
		operator string();
};

int Text::Chunk::lastx = -1;
long Text::Chunk::lasty = -1;
int Text::Chunk::lastwidth = -1;
int Text::Chunk::lastheight = -1;

Text::operator string()
{
	string ret;

	for (Chunk *i=firstChunk; i; i=i->next)
		i->appendToString(&ret);
	
	return ret;
}

void Text::Chunk::appendToString(string *out)
{
	out->append((const char*)str, (size_t)size);
}

Text::Chunk* Text::Chunk::createFromBzipStream() {
	Text::Chunk *chunk = new Chunk();
	if (!chunk) {
		pz_error("Couldn't new a chunk");
		exit(1);
	}
	chunk->initFromBzipStream();
	return chunk;
}

void Text::Chunk::initFromBzipStream()
{
	size = bzip_stream->next();
	str = new unsigned char[size];
	check_alloc(str);
	unsigned char *c = str;
	unsigned char tmp;
	
	bzip_stream->next();
	unsigned char i=0;
	while (i<size) {
		tmp = bzip_stream->cur();
		tmp = (tmp == scEscapeChar ?
		         (0xF0 | bzip_stream->next())
		       : (tmp)
		     );
		
		/* simply read over characters we cannot display (not doing this
		   would cause encyclopodia to crash in certain situations). */
		if (tmp < 33  &&  tmp != ' ') {
			--size;
			if (i!=size) bzip_stream->next();
		} else if (tmp > 0xC3) {
			--size;
			while (i!=size && (bzip_stream->next() & 0xC0)==0x80)
				--size;
		} else {
			*c = tmp;
			++c;
			++i;
			if (i!=size) bzip_stream->next();
		}
	}
}


Text::Text()
{
	firstChunk = Chunk::createFromBzipStream();
	Chunk *lastChunk = firstChunk;
	
	while (bzip_stream->next() == scString) {
		lastChunk = (lastChunk->next = Chunk::createFromBzipStream());
	}
	
	lastChunk->next = 0;
}

// Private default constructor
Text::Chunk::Chunk()
 : next(NULL), size(0), str(NULL)
{
}

Text::Chunk::Chunk(const char *string, uint1 length)
  : size(length)
{
	str = new unsigned char[length];
	check_alloc(str);
	for (uint1 i=0; i<length; ++i)
		str[i] = string[i];
}

Text::Chunk::~Chunk()
{
	delete[] str;
}

Text::~Text()
{
	Chunk *curChunk;
	while (firstChunk) {
		curChunk = firstChunk->next;
		delete firstChunk;
		firstChunk = curChunk;
	}
}


class ListItemBegin : public Element {
	private:
		uint1 depth;
		uint2 number;
		void doDraw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms, bool reallyDraw=false) const;
	
	public:
		ListItemBegin();
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const;
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const;
};


class ListItemEnd : public Element {
	public:
		ListItemEnd() { };
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const;
};

class TextLink : public Element {
	protected:
		Text *text, *href;
	
	public:
		TextLink *nextLink;
		mutable int beginY;
		
		TextLink();
		~TextLink();
		
		TextLink(Text *text)
		  : text(text), href(0), nextLink(0), beginY(-1) { }
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const;
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const;
		
		virtual void clickLink(bool (callback)(const char*)) {
			if (href) callback(string(*href).c_str());
			else callback(string(*text).c_str());;
		}
};

TextLink::~TextLink()
{
	if (text) delete text;
	if (href) delete href;
}


class ChangeFontFamily : public Element {
	private:
		uint2 newFamily;
		
	public:
		ChangeFontFamily() {
			newFamily = readUint2();
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.fontFamily = newFamily;
			setFontFromMS(gc, ms);
		}
};

class ChangeFontSize : public Element {
	private:
		uint1 newSize;
		
	public:
		static const unsigned char INTERNALFONTSIZES[];
		
		ChangeFontSize() {
			newSize = INTERNALFONTSIZES[readUint1()];
		}
		
		ChangeFontSize(uint1 paramNewSize)
		  : newSize(INTERNALFONTSIZES[paramNewSize]) { }
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.fontSize = newSize;
			setFontFromMS(gc, ms);
		}
};
const unsigned char ChangeFontSize::INTERNALFONTSIZES[] = {
// nominal:     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
/* internal: */ 0,  8,  8,  8,  8,  8, 10, 11, 12, 13, 14, 15, 15, 16, 16, 17, 17
};

class BeginFontStyle : public Element {
	private:
		uint1 mask;
		
	public:
		BeginFontStyle(const uint1 fontStyle) {
			mask = fontStyle;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.fontStyle |= mask;
			setFontFromMS(gc, ms);
		}
};

class EndFontStyle : public Element {
	private:
		uint1 mask;
		
	public:
		EndFontStyle(const uint1 fontStyle) {
			mask = !fontStyle;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.fontStyle &= mask;
			setFontFromMS(gc, ms);
		}
};

class LineBreak : public Element {
	public:
		LineBreak() { }
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.y += ms.fontSize;
			ms.x = ms.indent;
		}
};

class ParagraphBreak : public Element {
	public:
		ParagraphBreak() { }
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			ms.y += ms.fontSize + 8;
			ms.x = ms.indent;
		}
};


class Hline : public Element {
	public:
		Hline() {
			// Don't do nothing.
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			GrLine(win, gc, 0, ms.y, drawRegionRight, ms.y);
			ms.y += 1;
		}
		
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const {
			ms.y += 1;
		}
};


class AnchorPoint : public Element {
	private:
		Text *id;
	
	public:
		AnchorPoint() {
			if (bzip_stream->next() != scString) {
				errorState = erExpectedAnchorId;
				id = 0;
				return;
			}
			
			id = new Text();
			if (bzip_stream->cur() != scBlockEnd || bzip_stream->next() != blockAnchorPoint)
				errorState = erAnchorPointNotTerminated;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			/* Don't do nothing  */
		}
		
		~AnchorPoint() {
			if (id) delete id;
		}
};


class BeginHeader : public Element {
	friend class EndHeader;
	private:
		uint1 order;
		static BeginHeader *lastHeaderRead;
		
		mutable uint1 fontFamilyBefore, fontSizeBefore, fontStyleBefore;
		
		static const uint1 MAXHEADERORDER=3;
		static const uint1 headerFontSizes[MAXHEADERORDER+1];
		static const uint1 headerFontStyles[MAXHEADERORDER+1];
	
	public:
		BeginHeader() {
			order = readUint1();
			if (order > MAXHEADERORDER)  order = MAXHEADERORDER;
			lastHeaderRead = this;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			fontFamilyBefore = ms.fontFamily;
			fontSizeBefore = ms.fontSize;
			fontStyleBefore = ms.fontStyle;
			
			ms.fontFamily = 0;
			ms.fontSize = headerFontSizes[order];
			ms.fontStyle = headerFontStyles[order];
			
			setFontFromMS(gc, ms);
		}
};

BeginHeader *BeginHeader::lastHeaderRead = 0;
const uint1 BeginHeader::headerFontSizes[] = {
	ChangeFontSize::INTERNALFONTSIZES[14],
	ChangeFontSize::INTERNALFONTSIZES[14],
	ChangeFontSize::INTERNALFONTSIZES[13],
	ChangeFontSize::INTERNALFONTSIZES[12]
};
const uint1 BeginHeader::headerFontStyles[] = {
	MarkupState::fsBold | MarkupState::fsUnderlined,
	MarkupState::fsBold,
	MarkupState::fsBold,
	MarkupState::fsBold
};


class EndHeader : public Element {
	private:
		BeginHeader *beginner;
	
	public:
		EndHeader() {
			beginner = BeginHeader::lastHeaderRead;
			BeginHeader::lastHeaderRead = 0;
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState & ms) const {
			if (beginner) {
				ms.fontFamily = beginner->fontFamilyBefore;
				ms.fontSize = beginner->fontSizeBefore;
				ms.fontStyle = beginner->fontStyleBefore;
				
				setFontFromMS(gc, ms);
			}
		}
};

class Math : public Element {
	private:
		Text *content;
	
	public:
		Math() {
			if (bzip_stream->next() != scString) {
				errorState = erExpectedLatex;
				content = 0;
			} else {
				content = new Text();
				
				if (bzip_stream->cur() != scBlockEnd  ||  bzip_stream->next() != blockMath) {
					errorState = erMathNotTerminated;
					return;
				}
			}
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState & ms) const {
			if (content) content->draw(win, gc, ms);
			//TODO: LaTeX rendering
		}
		
		~Math() {
			if (content) delete content;
		}
};

class Table : public TextLink {
	private:
		static Table *curReadTable;
		
		class Cell : public Element {
			public:
				uint1 colspan, rowspan;
				bool isHeader;
				Cell *nextCell;	//Note that the member "next" points to the first *child* element
				Cell(bool paramIsHeader)
				  : isHeader(paramIsHeader), nextCell(0) {
					next = 0;
					rowspan = readUint1();
					colspan = readUint1();
				}
				virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const { }
				
				~Cell()
				{
					while (next) {
						Element *nextNext = next->next;
						delete next;
						next = nextNext;
					}
					// destructor "~Element()" for "this" is called implicitly
				}
		};
		
		struct Row {
			Cell *firstCell;
			uint2 height;
			Row *nextRow;
			Row() : firstCell(0), height(0) { }
			
			~Row()
			{
				while (firstCell) {
					Cell *nextCell = firstCell->nextCell;
					delete firstCell;
					firstCell = nextCell;
				}
			}
		};
		
		bool calculateSizeOfCell(const int &rowid, const int &colid, Element *el, GR_GC_ID &gc, unsigned char &cellWidth, unsigned short &cellHeight, const unsigned short &rowHeight);
		
		Row *firstRow;
		Row *lastRow;
		Cell *lastCell;
		
		uint1 borderwidth;
		uint1 *columnwidths;
		uint2 rowCount, colCount;
		
		static const Text replacementBeforeLink;
		static const Text replacementAfterLink;
		static const Text replacementBetweenLinkAndDescription;
		static const unsigned char cellpadding = 2;
		static Text *linkText;
		Table *parentTable;
		TextLink *firstLinkChild;
		Element *firstHeaderElement;
		
	public:
		int posX, posY;
		
		Table()
		  : TextLink(linkText), columnwidths(0), rowCount(1), colCount(0), firstHeaderElement(0), posX(0), posY(0) {
			borderwidth = readUint1();
			
			/* On the small ipod screen, tables without
			   any border appear extremely messy. */
			if (borderwidth == 0) borderwidth = 1;
			
			parentTable = curReadTable;
			curReadTable = this;
			lastCell = 0;
			firstRow = new Row();
			lastRow = firstRow;
			next = 0;
		}
		
		~Table()
		{
			text = 0;		// Otherwise, text will be destroyed by ~TextLink()
			
			while (firstRow) {
				Row *nextRow = firstRow->nextRow;
				delete firstRow;
				firstRow = nextRow;
			}
			
			while (firstHeaderElement) {
				Element *nextHE = firstHeaderElement->next;
				delete firstHeaderElement;
				firstHeaderElement = nextHE;
			}
			
			delete[] columnwidths;
		}
		
		void leaveTable()
		{
			MarkupState::tableindent = 0;
			if (parentTable) {
				Article::drawTable(parentTable, false);
			} else {
				Article::returnFromTable();
			}
		}
		
		Table *doEndHeaderLine() {
			firstHeaderElement = next;
			next = 0;
			return this;
		}
		static Table *endHeaderLine() {
			return curReadTable->doEndHeaderLine();
		}
		
		void doNewRow() {
			if (lastCell) {
				//Should actually always be reached
				lastCell->nextCell = 0;
				lastRow = (lastRow->nextRow = new Row());
				++rowCount;
				lastCell = 0;
			}
		}
		static void newRow() {
			curReadTable->doNewRow();
		}
		
		Element *doNewCell(bool isHeader) {
			if (lastCell)  lastCell = (lastCell->nextCell = new Cell(isHeader));
			else  lastCell = (lastRow->firstCell = new Cell(isHeader));
			
			if (rowCount == 1)
				colCount += lastCell->colspan;
			
			return lastCell;
		}
		static Element *newCell(bool isHeader) {
			return curReadTable->doNewCell(isHeader);
		}
		
		Table *doEndTable() {
			curReadTable = parentTable;
			lastRow->nextRow = 0;
			if (lastCell == 0) {
				// This happens if a table mistakenly ends with a "new row"-block
				Row *row = firstRow;
				while (row  &&  row->nextRow != lastRow) row = row->nextRow;
				if (!row) {
					delete firstRow;
					firstRow = 0;
					lastRow = 0;
				} else {
					delete row->nextRow;
					lastRow = row;
					lastRow->nextRow = 0;
				}
			}
			firstLinkChild = nextLink;
			nextLink = 0;
			return this;
		}
		static Table *endTable() {
			return curReadTable->doEndTable();
		}
		
		virtual void draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const {
			if (ms.x != ms.indent) {
				ms.y += ms.fontSize + 8;
				ms.x = ms.indent;
			}
			replacementBeforeLink.draw(win, gc, ms);
			TextLink::draw(win, gc, ms);
			
			if (firstHeaderElement) {
				replacementBetweenLinkAndDescription.draw(win, gc, ms);
				for (
					Element *curElement = firstHeaderElement;
					curElement;
					curElement = curElement->next
				) {
					curElement->draw(win, gc, ms);
				}
			}
			
			replacementAfterLink.draw(win, gc, ms);
			
			ms.y += ms.fontSize + 8;
			ms.x = ms.indent;
		}
		
		virtual void calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const {
			// calculates the size of the replacement.
			if (ms.x != ms.indent) {
				ms.y += ms.fontSize + 8;
				ms.x = ms.indent;
			}
			replacementBeforeLink.calculateSize(gc, ms, curCellWidth);
			TextLink::calculateSize(gc, ms, curCellWidth);
			
			if (firstHeaderElement) {
				replacementBetweenLinkAndDescription.calculateSize(gc, ms, curCellWidth);
				for (
					Element *curElement = firstHeaderElement;
					curElement;
					curElement = curElement->next
				) {
					curElement->calculateSize(gc, ms, curCellWidth);
				}
			}
			
			replacementAfterLink.calculateSize(gc, ms, curCellWidth);
			
			ms.y += ms.fontSize + 8;
			ms.x = ms.indent;
		}
		
		void calculateCellSizes();

		virtual void clickLink(bool (callback)(const char*)) {
			if (!columnwidths) calculateCellSizes();
			Article::drawTable(this);
		}
		
		void drawTable();
		
		void scrollUp() {
			if (posY > 15) posY -= 15;
			else posY = 0;
			drawTable();
		}
		void scrollDown() {
			posY += 15;
			drawTable();
		}
		void scrollLeft() {
			if (posX > 15) posX -= 15;
			else posX = 0;
			drawTable();
		}
		void scrollRight() {
			posX += 15;
			drawTable();
		}
		
		TextLink *getFirstLink() {
			return firstLinkChild;
		}
		
		static void setLinkText(const char *text) {
			if (linkText) delete linkText;
			linkText = new Text(text, strlen(text));
		}
};

Table *Table::curReadTable = 0;
const Text Table::replacementBeforeLink = Text("[", 1);
const Text Table::replacementAfterLink = Text("]", 1);
const Text Table::replacementBetweenLinkAndDescription = Text(": ", 2);
Text *Table::linkText = 0;


void Article::cleanupCurrentArticle()
{
	valid = false;
	
	if (firstMS) {
		Element *firstElement = firstMS->nextElement;
		Element *curElement;
		while (firstElement) {
			curElement = firstElement->next;
			delete firstElement;
			firstElement = curElement;
		}
	}
	lastElement = 0;
	
	while (firstMS) {
		curMS = firstMS->next;
		delete firstMS;
		firstMS = curMS;
	}
	
	IntListChunk *curAuthorListChunk;
	while (firstAuthorListChunk) {
		curAuthorListChunk = firstAuthorListChunk->next;
		delete firstAuthorListChunk;
		firstAuthorListChunk = curAuthorListChunk;
	}
	
	hoveredLink = 0;
	bufWinY = 0;
	bufWinX = 0;
	bufWinTextBeginY = 0;
	firstLink = 0;
	posY = 0;
	
	if (inTable) {
		bufWinWidth = parentWinWidth;
		drawRegionRight = parentWinWidth;
		bufWinHeight = parentWinHeight * 2;
		bufWin = GrNewPixmap(bufWinWidth, bufWinHeight, 0);
		inTable = 0;
	}
}

void Article::readArticle()
{
	cleanupCurrentArticle();
	
	TextLink *lastLink = firstLink;
	hoveredLink = 0;
	
	firstMS = new MarkupState();
	curMS = firstMS;
	curMS->inHoveredLink = false;
	
	/* Set bufWinY to a value that will cause draw() to
	 * recalculate the buffer when called for the first time */
	bufWinY = -bufWinHeight;
	posY = 0;
	
	if (bzip_stream->cur() != scBlockBegin || bzip_stream->next() != blockArticle || bzip_stream->next() != scString) {
		errorState = erBadPosition;
		return;
	}

	/* We read the article title to "firstMS->nextElement" although
	 * we don't want it to be stored in firstMS->nextElement. However,
	 * this makes adding new Elements in the loop much easier,
	 * because we don't always have to check whether the Element
	 * we want to add is the first Element (adding the first
	 * element to a linked list requires different commands than
	 * adding any other element). After the loop, we set
	 * firstMS->nextElement to firstMS->nextElement->next.
	 */
	firstMS->nextElement = new Text();
	lastElement = firstMS->nextElement;
	
	if (bzip_stream->cur() != scBlockBegin || bzip_stream->next() != csDummy) {
		errorState = erBadTitle;
		return;
	}
	
	readTimestamp();
	readAuthors();				// only read into char[], don't parse yet
	
	if (errorState != erOK)  return;
	
	bzip_stream->next();
	
	bool arrivedAtEnd = false;
	while (errorState == erOK) {
		switch (bzip_stream->cur()) {
		case scBlockBegin: {
			switch (bzip_stream->next()) {
			case csChangeFontFamily:
				lastElement = (lastElement->next = new ChangeFontFamily());
				break;
			
			case csChangeFontSize:
				lastElement = (lastElement->next = new ChangeFontSize());
				break;
			
			case csLineBreak:
				lastElement = (lastElement->next = new LineBreak());
				break;
			
			case csParagraphBreak:
				lastElement = (lastElement->next = new ParagraphBreak());
				break;
			
			case csHline:
				lastElement = (lastElement->next = new Hline());
				break;
			
			case csBeginItalic:
				lastElement = (lastElement->next = new BeginFontStyle(
					MarkupState::fsItalic
				));
				break;
			
			case csEndItalic:
				lastElement = (lastElement->next = new EndFontStyle(
					MarkupState::fsItalic
				));
				break;
			
			case csBeginBold:
				lastElement = (lastElement->next = new BeginFontStyle(
					MarkupState::fsBold
				));
				break;
			
			case csEndBold:
				lastElement = (lastElement->next = new EndFontStyle(
					MarkupState::fsBold
				));
				break;
			
			case csBeginUnderlined:
				lastElement = (lastElement->next = new BeginFontStyle(
					MarkupState::fsUnderlined
				));
				break;
			
			case csEndUnderlined:
				lastElement = (lastElement->next = new EndFontStyle(
					MarkupState::fsUnderlined
				));
				break;
			
			case csTableRow:
				lastElement->next = 0;
				Table::newRow();
				break;
			
			case csTableCell:
				lastElement->next = 0;
				lastElement = Table::newCell(false);
				break;
			
			case csTableHeaderCell:
				lastElement->next = 0;
				lastElement = Table::newCell(true);
				break;
			
			case blockList:
				// we can savely ignore this
				break;
			
			case blockListItem:
				lastElement = (lastElement->next = new ListItemBegin());
				break;
			
			case blockTextLink:
				if (lastLink) {
					lastLink = (lastLink->nextLink = new TextLink());
				}
				else {
					lastLink = new TextLink();
					firstLink = lastLink;
				}
				lastElement = (lastElement->next = lastLink);
				break;
			
			case blockAnchorPoint:
				lastElement = (lastElement->next = new AnchorPoint());
				//not implemented: make list of all anchor points
				break;
			
			case blockHeader:
				lastElement = (lastElement->next = new BeginHeader());
				break;
			
			case blockTable: {
				if (lastLink) {
					lastLink = (lastLink->nextLink = new Table());
				}
				else {
					lastLink = new Table();
					firstLink = lastLink;
				}
				lastElement = (lastElement->next = lastLink);
				break;
			}
			
			case blockMath:
				lastElement = (lastElement->next = new Math());
				break;
			
			case blockAlternativeStuff:
				//ignore. do things when reading end tag
				break;
			
			default:
				errorState = erUnknownBlock;
			}
			bzip_stream->next();
			break;
		}
		case scBlockEnd: {
			switch (bzip_stream->next()) {
			case blockList:
				// we can savely ignore this
				bzip_stream->next();
				break;
			
			case blockListItem:
				lastElement = (lastElement->next = new ListItemEnd());
				bzip_stream->next();
				break;
			
			case blockArticle:
				arrivedAtEnd = true;
				break;
			
			case blockHeader:
				lastElement = (lastElement->next = new EndHeader());
				bzip_stream->next();
				break;
			
			case blockTable:
				lastElement->next = 0;
				lastLink->nextLink = 0;	//for nested tables
				lastLink = Table::endTable();
				lastElement = lastLink;
				bzip_stream->next();
				break;
			
			case blockAlternativeStuff:
				lastElement->next = 0;
				lastElement = Table::endHeaderLine();
				bzip_stream->next();
				break;
			
			default:
				errorState = erUnexceptedBlockEnd;
			}
			break;
		}
		case scString: {
			lastElement = (lastElement->next = new Text());
			break;
		}
		default: {
			errorState = erExpectedControlChar;
		}
		}
		
//		if (/*|| bzip_stream->errorState() (not implemented: error handling)*/) {
//			break;
//		}
		
		if (arrivedAtEnd)  break;
	}
	
	lastElement->next = 0;
	
	if (lastElement == firstMS->nextElement)  lastElement = 0;
	firstMS->nextElement = firstMS->nextElement->next;	// See comment above reading firstMS->nextElement
	
	valid = true;
}

Article::Article(GR_WINDOW_ID paramParentWin, GR_SIZE paramParentWinWidth, GR_SIZE paramParentWinHeight, char *paramTableReplacement)
  : valid(false), firstMS(0), firstAuthorListChunk(0), firstLink(0), inTable(0)
{
	Table::setLinkText(paramTableReplacement);
	
	parentWinWidth = paramParentWinWidth;
	parentWinHeight = paramParentWinHeight;
	gc = pz_get_gc(1);
	
	parentWin = paramParentWin;
	errorState = erOK;
	
	bufWinWidth = parentWinWidth;
	drawRegionRight = parentWinWidth;
	bufWinHeight = parentWinHeight * 2;
	bufWin = GrNewPixmap(bufWinWidth, bufWinHeight, 0);
}


Article::~Article()
{
	if (curInstance)
		curInstance->cleanupCurrentArticle();
}


void Article::readTimestamp()
{
	lastChange.year = (bzip_stream->next() << 8) | bzip_stream->next();
	lastChange.month = bzip_stream->next();
	lastChange.dayOfMonth = bzip_stream->next();
	lastChange.hour = bzip_stream->next();
	lastChange.minute = bzip_stream->next();
	lastChange.second = bzip_stream->next();
	lastChange.timeZone = (sint1) bzip_stream->next();
}


Article::IntListChunk::IntListChunk()
{
	count = readUint1();
	type = readSint1();
	uint2 numbytes = (type < 0 ? -type : type) * count;
	
	data = new uint1[numbytes];
	
	uint1 *datapos = data;
	uint1 *dataend = data + numbytes;
	
	while (datapos != dataend) {
		*datapos = bzip_stream->next();
		++datapos;
	}
	
	if (bzip_stream->next() != scBlockEnd || bzip_stream->next() != blockIntList) {
		errorState = erExpectedBlockEnd;
	}
}

Article::IntListChunk::~IntListChunk()
{
	delete[] data;
}

void Article::readAuthors()
{
	/* DELAYED: view Authors vor each article (as a workaround, all authors of an
	   ebook are listed in the "about this ebook" page till now, to comply with GFDL) */
	if (bzip_stream->next() != scBlockBegin || bzip_stream->next() != blockIntList) {
		errorState = erExpectedAuthorList;
		return;
	}
	
	firstAuthorListChunk = new IntListChunk();
	IntListChunk *lastAuthorListChunk = firstAuthorListChunk;
	
	while (bzip_stream->next() == scBlockBegin && bzip_stream->next() == blockIntList) {
		lastAuthorListChunk = (lastAuthorListChunk->next = new IntListChunk());
	}
	
	lastAuthorListChunk->next = 0;
}

void Article::draw()
{
	draw(posY);
}

void Article::draw(const int y)
{
	if (inTable) {
		inTable->drawTable();
	} else {
		posY = y;
		if (posY < 0)
			posY = 0;
		
		// Check whether the desired region is already in bufWin
		if (y < bufWinTextBeginY || y+parentWinHeight > bufWinY+bufWinHeight) {
			GrSetGCUseBackground(gc, GR_TRUE);
			GrSetGCForeground(gc, WHITE);
			GrFillRect(bufWin, gc, 0, 0, bufWinWidth, bufWinHeight);
			GrSetGCForeground(gc, BLACK);
			GrSetGCUseBackground(gc, GR_FALSE);
			//GrClearWindow(bufWin, 0);
			
			Text::Chunk::lastx = -1;	// see comment at declaration of lastx
			
			if (y < bufWinTextBeginY) {
				/* It seems that the user is scolling up. As it is very likely
				that he will scroll further up, we set bufWinY so that we
				accelerate future upscroling. */
				bufWinTextBeginY = y - bufWinHeight + parentWinHeight;
			} else {
				/* It seems that the user is scolling down. As it is very likely
				that he will scroll further down, we set bufWinY so that we
				accelerate future downscroling. */
				bufWinTextBeginY = y;
			}
			
			
			// Find last markup State that isn't below y
			while (curMS->next  &&  curMS->next->y < bufWinTextBeginY)
				curMS = curMS->next;
			while (curMS->prev  &&  curMS->y + curMS->fontSize > bufWinTextBeginY)
				curMS = curMS->prev;
			
			if (bufWinTextBeginY > posY - curMS->fontSize) {
				bufWinY = posY - curMS->fontSize;	// this is important for word wrapping
			} else {
				bufWinY = bufWinTextBeginY;
				bufWinTextBeginY += curMS->fontSize;
			}
			
			MarkupState ms(*curMS);
			
			// ms.y will most probably be set to a negative initial value. That's ok.
			ms.y -= bufWinY;
			
			/* Define the position where the next ms should be appended to
			the list if curMS is the last in the list. */
			int nextMSposy = ms.y + bufWinHeight;
			
			ms.writeSettingsToGC(gc);
			
			Element *curElement = ms.nextElement;
			
			while (curElement && ms.y < bufWinHeight) {
				curElement->draw(bufWin, gc, ms);
				curElement = curElement->next;
				
				if (
					curMS->next == 0  &&  ms.y > nextMSposy  &&
					// for the next line, see comment at declaration of lastx
					(Text::Chunk::lastx != ms.x || Text::Chunk::lasty != ms.y)
				) {
					curMS->next = new MarkupState(ms);
					curMS->next->prev = curMS;
					curMS = curMS->next;
					curMS->y += bufWinY;
					curMS->nextElement = curElement;
					
					nextMSposy += bufWinHeight;
				}
			}
		}
		
		GrCopyArea(parentWin, gc, 0, 0, parentWinWidth, parentWinHeight, bufWin, 0, y-bufWinY, 0);
	}
}

void Article::scrollUp()
{
	if (inTable) inTable->scrollUp();
	else {
		posY -= 15;
		draw(posY);
	}
}

void Article::scrollDown()
{
	if (inTable) inTable->scrollDown();
	else {
		posY += 15;
		draw(posY);
	}
}

void Article::scrollLeft()
{
	if (inTable) inTable->scrollLeft();
}

void Article::scrollRight()
{
	if (inTable) inTable->scrollRight();
}


void Article::doDrawTable(Table *table, bool needNewBufWin)
{
	if (needNewBufWin) {
		bufWinWidth = int(parentWinWidth * 1.5);
		bufWinHeight = parentWinHeight * 2;
		GrDestroyWindow(bufWin);
		bufWin = GrNewPixmap(bufWinWidth, bufWinHeight, 0);
	}
	
	hoveredLink = 0;
	inTable = table;
	bufWinY = -bufWinHeight;	// forces redraw on bufwin
	table->drawTable();
}

void Article::doReturnFromTable()
{
	bufWinWidth = parentWinWidth;
	drawRegionRight = parentWinWidth;
	bufWinHeight = parentWinHeight * 2;
	bufWin = GrNewPixmap(bufWinWidth, bufWinHeight, 0);
	
	hoveredLink = 0;
	inTable = 0;
	bufWinX = 0;
	bufWinY = -bufWinHeight;	// forces redraw on bufwin
	draw(posY);
}

void Table::drawTable()
{
	// Check whether the desired region is already in bufWin
	if (
		posX < bufWinX || posX+Article::parentWinWidth > bufWinX+bufWinWidth ||
		posY < bufWinY || posY+Article::parentWinHeight > bufWinY+bufWinHeight
	) {
		GrSetGCUseBackground(Article::gc, GR_TRUE);
		GrSetGCForeground(Article::gc, WHITE);
		GrFillRect(Article::bufWin, Article::gc, 0, 0, bufWinWidth, bufWinHeight);
		GrSetGCForeground(Article::gc, BLACK);
		GrSetGCUseBackground(Article::gc, GR_FALSE);
		
		if (posY < bufWinY) bufWinY = posY + Article::parentWinHeight - bufWinHeight;
		else bufWinY = posY;
		
		if (posX < bufWinX) bufWinX = posX + Article::parentWinWidth - bufWinWidth;
		else bufWinX = posX;
		
		int y = -bufWinY;
		
		Row *curRow = firstRow;
		while (curRow && y + borderwidth + 2*cellpadding + curRow->height <= 0) {
			y += borderwidth + 2*cellpadding + curRow->height;
			curRow = curRow->nextRow;
		}
		
		for (
			;
			curRow;
			curRow = curRow->nextRow
		) {
			drawRegionRight = -bufWinX;
			int colid=0;
			
			for (
				Cell *curCell = curRow->firstCell;
				curCell;
				curCell = curCell->nextCell
			) {
				//DELAYED: rowspan
				MarkupState ms;
				ms.tableindent = drawRegionRight + cellpadding + borderwidth;
				ms.x = ms.tableindent;
				
				if (ms.x >= bufWinY+bufWinWidth) break;
				
				ms.indent = ms.tableindent;
				ms.y = y + borderwidth + cellpadding;
				ms.writeSettingsToGC(Article::gc);
				
				int xbefore = drawRegionRight;
				
				for (int i = 0; i<curCell->colspan; ++i)
					drawRegionRight += borderwidth + 2*cellpadding + columnwidths[colid++];
				drawRegionRight -= cellpadding;
				
				if (drawRegionRight >= 0) {
					for (
						Element *curElement = curCell->next;
						curElement;
						curElement = curElement->next
					) {
						curElement->draw(Article::bufWin, Article::gc, ms);
					}
					
					drawRegionRight += cellpadding;
					
					Row *tmpRow = curRow;
					int cellbottom = y;
					for (unsigned char ii=0; ii<curCell->rowspan; ++ii) {
						cellbottom += tmpRow->height + borderwidth + 2*cellpadding;
						tmpRow = tmpRow->nextRow;
					}
					for (unsigned char i=0; i<borderwidth; ++i) {
						GrLine(Article::bufWin, Article::gc, xbefore, y+i, drawRegionRight, y+i);
						GrLine(Article::bufWin, Article::gc, xbefore+i, y, xbefore+i, cellbottom);
					}
				} else {
					drawRegionRight += cellpadding;
				}
			}
			
			y += borderwidth + 2*cellpadding + curRow->height;
		}
		
		for (unsigned char i=0; i<borderwidth; ++i) {
			GrLine(Article::bufWin, Article::gc, 0, y+i, drawRegionRight, y+i);
			GrLine(Article::bufWin, Article::gc, drawRegionRight+i, 0, drawRegionRight+i, y+borderwidth-1);
		}
	}
	
	
	GrCopyArea(Article::parentWin, Article::gc, 0, 0, Article::parentWinWidth, Article::parentWinHeight, Article::bufWin, posX-bufWinX, posY-bufWinY, 0);
}

void Article::hoverLink()
{
	int minLinkPosY = 0;
	if (inTable) {
		hoveredLink = inTable->getFirstLink();
		minLinkPosY = inTable->posY;
	} else {
		hoveredLink = firstLink;
		minLinkPosY = posY;
	}
	
	/* +10 because links where only a part is visible should also
	 * be considered, if no other links are completely visible */
	while (hoveredLink && hoveredLink->beginY+10 < minLinkPosY)
		hoveredLink = hoveredLink->nextLink;
	
	if (hoveredLink) {
		if (hoveredLink->beginY > minLinkPosY+parentWinHeight) {
			hoveredLink = 0;
		} else {
			if (hoveredLink->beginY < minLinkPosY) {
				TextLink *cur = hoveredLink;
				while (cur  &&  cur->beginY < minLinkPosY)
					cur = cur->nextLink;
				if (cur && cur->beginY < minLinkPosY+parentWinHeight) {
					hoveredLink = cur;
				} else {
					minLinkPosY -= 10;
				}
			}
			if (hoveredLink->beginY+20 > minLinkPosY+parentWinHeight) {
				//+20 required because beginY might actually be too low if there was line wrapping
				minLinkPosY += 20;
			}
			bufWinY = -bufWinHeight;	//prevents Article::draw from using buffer
			
			if (inTable) {
				inTable->posY = minLinkPosY;
				inTable->drawTable();
			} else {
				draw(minLinkPosY);
			}
		}
	}
}

void Article::unhoverLink()
{
	bufWinY = -bufWinHeight;			//prevents Article::draw from using buffer
	hoveredLink = 0;
	
	if (inTable) inTable->drawTable();
	else draw(posY);
}

void Article::hoverNextLink()
{
	if (hoveredLink && hoveredLink->nextLink) {
		int minLinkPosY = 0;
		if (inTable) {
			minLinkPosY = inTable->posY;
		} else {
			minLinkPosY = posY;
		}
		
		bufWinY = -bufWinHeight;		//prevents Article::draw from using buffer
		
		if (hoveredLink->nextLink->beginY == -1) {
			/* If the next link has not yet been drawn, we don't
			 * know its position. In this case we unhover the hovered
			 * link and scroll down some pixels. Unhovering is important
			 * because the hovered link might come out of sight and it
			 * might confuse users if a link is hovered that is not visible. */
			hoveredLink = 0;
			
			minLinkPosY += 15;
		} else {
			hoveredLink = hoveredLink->nextLink;
			
			if (hoveredLink->beginY+20 > minLinkPosY+parentWinHeight) {
				//+20 required because beginY might actually be too low if there was line wrapping
				minLinkPosY = hoveredLink->beginY;
			}
		}
		
		if (inTable) {
			inTable->posY = minLinkPosY;
			inTable->drawTable();
		} else {
			draw(minLinkPosY);
		}
	}
}

void Article::hoverPreviousLink()
{
	int minLinkPosY = 0;
	TextLink *resFirstLink = 0;
	if (inTable) {
		resFirstLink = inTable->getFirstLink();
		minLinkPosY = inTable->posY;
	} else {
		resFirstLink = firstLink;
		minLinkPosY = posY;
	}
	
	if (hoveredLink && hoveredLink != resFirstLink) {
		TextLink *cur = resFirstLink;
		while (cur->nextLink != hoveredLink)
			cur = cur->nextLink;
		/* A double linked list would be faster here, but
		 * it would also need more ram. */
		
		hoveredLink = cur;
		bufWinY = -bufWinHeight;		//prevents Article::draw from using buffer
		
		if (hoveredLink->beginY < minLinkPosY) {
			minLinkPosY = hoveredLink->beginY;
		}
		
		if (inTable) {
			inTable->posY = minLinkPosY;
			inTable->drawTable();
		} else {
			draw(minLinkPosY);
		}
	}
}


void Text::Chunk::draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms, bool reallyDraw, uint1 &curwidth) const
{
	unsigned char *cur = str;
	unsigned char *end = str + size;
	
	unsigned char *wordbegin = cur;
	int textwidth = 0;
	int textheight = 0;
	
	bool samePosAsLastChunk = (ms.x == lastx  &&  ms.y == lasty);
	
	int linestartPosx = ms.x;
	
	if (size && isspace(*cur)) {
		if (reallyDraw && ms.inHoveredLink) {
			GrSetGCForeground(gc, BLACK);
			GrFillRect(win, gc, ms.x, ms.y, ms.spaceWidth, ms.fontSize);
			GrSetGCForeground(gc, WHITE);
		}
		ms.x += ms.spaceWidth;
		++cur;
	}
	while (cur != end) {
		wordbegin = cur;
		
		while (cur != end && !isspace(*cur))
			++cur;
		
		ttk_font font = ttk_gc_get_font(gc);
		textheight = ttk_text_height(font);

		string tmp(wordbegin, cur);
		textwidth = ttk_text_width(font, tmp.c_str());

		if (ms.x + textwidth > drawRegionRight) {
			// This means that we have to wrap to the next line.
			
			if (reallyDraw  &&  (ms.fontStyle & MarkupState::fsUnderlined)  &&  linestartPosx != ms.x) {
				// we assume that line style of gc is set correctly
				GrLine(win, gc, linestartPosx, ms.y + textheight, ms.x-1, ms.y + textheight);
				/* The line must be less than textheight pixels below ms.y,
				 * because it must be within the area that is copied when the
				 * word is post-wrapped around to a new line. */
			}
				
			if (samePosAsLastChunk  &&  wordbegin == str) {
				// See comment at declaration of lastx
				ms.y += ms.fontSize;
				
				if (reallyDraw) {
					if (lasty < 0) {
						lastheight += lasty;
						lasty = 0;
					}
					if (lasty + lastheight  >  bufWinHeight) {
						lastheight = bufWinHeight - lasty;
					}
					
					if (lastheight > 0) {
						GrCopyArea(
							win, gc, ms.indent, ms.y,	// destination
							lastwidth, lastheight,		// size
							win, lastx-lastwidth, lasty, 0	// source
						);
						
						//Clear the original area
						GrSetGCForeground(gc, WHITE);
						GrFillRect(win, gc, lastx-lastwidth, lasty, lastwidth, lastheight);
						GrSetGCForeground(gc, ms.inHoveredLink ? WHITE : BLACK);
					}
				} else {
					if (lastx-lastwidth > curwidth)
						curwidth = lastx-lastwidth;
				}
				
				ms.x = ms.indent + lastwidth;
				samePosAsLastChunk = false;
			} else {
				if (!reallyDraw  &&  ms.x > curwidth)
					curwidth = ms.x;
				ms.y += ms.fontSize;
				ms.x = ms.indent;
			}
			
			linestartPosx = ms.x;
		}
		
		if (reallyDraw) {
			string tmp(wordbegin, cur);
			ttk_text(win->srf, ttk_gc_get_font(gc), ms.x, ms.y,
					ttk_ap_getx("window.fg")->color,
					tmp.c_str());
		}
		
		ms.x += textwidth;
		if (cur!=end) {
			/* This means that we have found a space. Don't
			check for "if (isspace(*cur))", as this might crash
			if the first char *after* the bzip_stream is a space by chance. */
			if (reallyDraw && ms.inHoveredLink) {
				GrSetGCForeground(gc, BLACK);
				GrFillRect(win, gc, ms.x, ms.y, ms.spaceWidth, textheight);
				GrSetGCForeground(gc, WHITE);
			}
			ms.x += ms.spaceWidth;
			++cur;
		}
	}
	
	if (reallyDraw  &&  (ms.fontStyle & MarkupState::fsUnderlined)  &&  linestartPosx != ms.x) {
		GrLine(win, gc, linestartPosx, ms.y + textheight, ms.x-1, ms.y + textheight);
	}
	
	if (size && !isspace(*(end-1))) {
		// If the chunk did not end in a space
		// See comment at declaration of lastx
		
		if (samePosAsLastChunk  &&  wordbegin == str) {
			/* This means that lastx/y was set to the same position
			 * as ms before we read this chunk AND that this chunk
			 * doesn't contain any spaces AND that there was no need
			 * to wrap around to a new line when writing the only
			 * word in this chunk (because samePosAsLastChunk would
			 * have been set to false if the word was wrapped around).
			 */
			lastwidth += textwidth;
			if (textheight > lastheight)  lastheight = textheight;
		} else {
			lasty = ms.y;
			lastwidth = textwidth;
			lastheight = textheight;
		}
		
		lastx = ms.x;
		
	} else lastx = -1;
}


void Text::draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const
{
	uint1 unused = 0;
	for (Chunk *i=firstChunk; i; i=i->next)
		i->draw(win, gc, ms, true, unused);
}

void Text::calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const
{
	for (Chunk *i=firstChunk; i; i=i->next)
		i->draw(0, gc, ms, false, curCellWidth);
}


ListItemBegin::ListItemBegin()
{
	depth = readUint1();
	number = readUint2();
}

void ListItemBegin::doDraw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms, bool reallyDraw) const
{
	ms.indent = ms.tableindent + 5*depth + 15;
	ms.y += ms.fontSize + 3;
	ms.x = ms.indent;
	
	if (number == 0) {
		if (reallyDraw)
			GrFillEllipse(win, gc, ms.x-6, ms.y + ms.fontSize/2, 2, 2);
	} else {
		char buf[10];
		sprintf(buf, "%d.", number);
		int length = strlen(buf);
		
		GR_SIZE textwidth, textheight, textbase;
		GrGetGCTextSize(gc, buf, length, GR_TFUTF8, &textwidth, &textheight, &textbase);
		textwidth += ms.spaceWidth;
		
		if (textwidth + ms.tableindent > ms.x) {
			ms.x = textwidth + ms.tableindent;
		}
		
		if (reallyDraw) {
			GrText(
				win, gc, ms.x-textwidth, ms.y+textbase,
				buf, length, GR_TFUTF8
			);
			/* This does not underline list item numbers even if ms.fontStyle
			 * contains fsUnderlined. However, this might even make sense. */
		}
	}
}

void ListItemBegin::draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const
{
	doDraw(win, gc, ms, true);
}

void ListItemBegin::calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const
{
	doDraw(0, gc, ms, false);
	if (ms.x > curCellWidth) curCellWidth = ms.x;
}


void ListItemEnd::draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const
{
	ms.indent = ms.tableindent;
}



TextLink::TextLink()
  : nextLink(0), beginY(-1)
{
	if (bzip_stream->next() != scString) {
		errorState = erExpectedHref;
		text = 0;
		href = 0;
	} else {
		text = new Text();
		
		if (bzip_stream->cur() == scBlockBegin) {
			if (bzip_stream->next() != csDummy || bzip_stream->next() != scString) {
				errorState = erTextLinkNotTerminated;
				return;
			}
			href = new Text();
		} else href = 0;
		
		if (bzip_stream->cur() != scBlockEnd || bzip_stream->next() != blockTextLink) {
			errorState = erTextLinkNotTerminated;
			return;
		}
	}
}


void TextLink::draw(GR_WINDOW_ID win, GR_GC_ID &gc, MarkupState &ms) const
{
	beginY = ms.y + bufWinY;
	/* This is not really correct if we wrap around to a new line,
	 * but the [un]hover*Link methods handle this with care. */
	
	ms.fontStyle ^= MarkupState::fsUnderlined;
	
	if (hoveredLink == this) {
		ms.inHoveredLink = true;
		GrSetGCForeground(gc, WHITE);
		GrSetGCBackground(gc, BLACK);
		GrSetGCUseBackground(gc, GR_TRUE);
		
		text->draw(win, gc, ms);
		
		GrSetGCForeground(gc, BLACK);
		GrSetGCBackground(gc, WHITE);
		GrSetGCUseBackground(gc, GR_FALSE);
		ms.inHoveredLink = false;
	} else {
		text->draw(win, gc, ms);
	}
	
	ms.fontStyle ^= MarkupState::fsUnderlined;
}

void TextLink::calculateSize(GR_GC_ID &gc, MarkupState &ms, uint1 &curCellWidth) const
{
	ms.fontStyle ^= MarkupState::fsUnderlined;
	text->calculateSize(gc, ms, curCellWidth);
	ms.fontStyle ^= MarkupState::fsUnderlined;
}




void setFontFromMS(GR_GC_ID &gc, MarkupState &ms)
{

	static char fontname[256];
	
	sprintf(
		fontname, "epodia%dsize%dstyle%d",
		ms.fontFamily, ms.fontSize, ms.fontStyle & (MarkupState::fsBold|MarkupState::fsItalic)
	);
	
	ttk_font curfont = ttk_get_font(fontname, ms.fontSize);
	ttk_gc_set_font(gc, curfont);
	
	char *space = "i";		// which is the correct char for a space? (" " doesn't work)
	GR_SIZE textwidth, unused;
	GrGetGCTextSize(gc, space, 1, GR_TFUTF8, &textwidth, &unused, &unused);
	ms.spaceWidth = textwidth;
}


bool Article::linkHovered() {
	return hoveredLink;
}


void Article::clickHoveredLink(bool (callback)(const char*))
{
	if (hoveredLink) {
		hoveredLink->clickLink(callback);
	}
}

/**
 * @return   true, iff cell does not fit in colWidthGuess.
 */
bool Table::calculateSizeOfCell(const int &rowid, const int &colid, Element *el, GR_GC_ID &gc, unsigned char &cellWidth, unsigned short &cellHeight, const unsigned short &rowHeight)
{
	MarkupState ms;
	ms.writeSettingsToGC(gc);
	cellWidth = 0;
	
	while (el) {
		el->calculateSize(gc, ms, cellWidth);
		el = el->next;
	}
		
	// respect that the last line might be the longest.
	if (ms.x > cellWidth)
		cellWidth = ms.x;
	
	cellHeight = ms.y + ms.fontSize;
	
	return (cellHeight > rowHeight  ||  cellWidth > drawRegionRight);
}

void Table::calculateCellSizes()
{
	/* Def.: The 'maximum column width' be the width of the parent window
	 *       minus the amount of pixels that the table is moved whenever
	 *       the user scrolles horizontaly plus one.
	 *
	 * Thus, if a column has the 'maximum column width', it is guaranteed
	 * that it is still possible to adjust this column on the screen so that
	 * the whole width of it can be seen without any need of horizontal
	 * scrolling.
	 *
	 * Def.: The 'optimal column width' be the width of a column which is
	 *       (i)   at most the 'maximum column width' (see above);
	 *       (ii)  at least the width of any cell when drawn with
	 *             linebreaks wherever possible;
	 *       (iii) the minimum width that would require no row of the table
	 *             to be higher than it would be if all columns of the
	 *             table had the 'maximum column width'.
	 *       If these rules are contradicting in a particular case, then
	 *       topmost rules have a higher priority than the ones below them.
	 *
	 * Thus, a table whose columns all have the 'optimal column width'
	 * - is exactly as high as a table whose columns all have the 'maximum
	 *   column width;
	 * - has no column with a width larger than the 'maximum column width';
	 * - is the narrowest of all possible tables that meet theese two features.
	 *
	 * This function calculates the 'optimal column width' of each column,
	 * as well as the height of each row if the table was displayed with
	 * optimal column widths.
	 */
	
	
	const unsigned char maxColWidth = Article::parentWinWidth - 15 + 1;
	int drawRegionRightBefore = drawRegionRight;
	drawRegionRight = maxColWidth;
	
	/* At first, we calculate the size of each cell when drawn width maximum
	 * column width and for each row, we store the height of the highest cell
	 * in the array rowHeights and in row->height. */
	
	GR_GC_ID gc = pz_get_gc(1);	//Probably better to use something static here
	
	unsigned short rowHeights[rowCount];
	unsigned short cellHeights[rowCount][colCount];
	unsigned char cellWidths[rowCount][colCount];
	
	unsigned char rowspanInCol[colCount];
	for (int i=0; i<colCount; ++i)
		rowspanInCol[i] = 1;
	
	int rowid = 0;
	int colid = 0;
	
	Cell *cellAddr[rowCount][colCount];
	for (rowid=0; rowid<rowCount; ++rowid)
		for (colid=0; colid<colCount; ++colid)
			cellAddr[rowid][colid] = 0;
	
	rowid = 0;
	colid = 0;
	
	for (
		Row *curRow = firstRow;
		curRow;
		curRow = curRow->nextRow
	) {
		colid = 0;
		rowHeights[rowid] = 0;
		curRow->height = 0;
		
		for (
			Cell *curCell = curRow->firstCell;
			curCell && colid < colCount;
			curCell = curCell->nextCell
		) {
			while (--rowspanInCol[colid] && colid<colCount) {
				++colid;
			}
			if (colid == colCount) break;
			
			MarkupState ms;
			ms.writeSettingsToGC(gc);
			cellWidths[rowid][colid] = 0;
			
			for (
				Element *curElement = curCell->next;
				curElement;
				curElement = curElement->next
			) {
				/* IMPORTANT: Note that global var "drawRegionRight"
				is used here to pass the max. allowed cellwidth. */
				curElement->calculateSize(gc, ms, cellWidths[rowid][colid]);
			}
			
			// respect that the last line might be the longest.
			if (ms.x > cellWidths[rowid][colid])
				cellWidths[rowid][colid] = ms.x;
			
			if (ms.y + ms.fontSize > rowHeights[rowid] && curCell->rowspan==1) {
				rowHeights[rowid] = ms.y + ms.fontSize;
				curRow->height = ms.y + ms.fontSize;
			}
			
			cellHeights[rowid][colid] = ms.y + ms.fontSize;
			
			for (int i=0; i<curCell->colspan; ++i) {
				rowspanInCol[colid] = curCell->rowspan;
				++colid;
			}
			cellAddr[rowid][colid - curCell->colspan] = curCell;
		}
		
		++rowid;
	}
	
	if (!columnwidths) columnwidths = new uint1[colCount];
	
	/* Now we iterate thru each column and try to shrink it as much as possible
	 * without having to use more vertical space. */

	for (colid=0; colid<colCount; ++colid) {
		unsigned char maxwidth = maxColWidth;
		unsigned char minwidth = 0;
		drawRegionRight = maxwidth;
		int failedCell = -1;
		int lastFailed = -1;
		
		/* We first guess a colmn width that we believe is near
		* the optimal width. Then we check whether all cells fit
		* into a column of this width. If they do, we look for
		* an even smaller column width. If they don't, we increase
		* the column width again, but we never increase it to
		* a value that is higher than the smallest width that
		* is already proofed to be sufficient. */
		
		while (maxwidth != minwidth) {
			if (failedCell == -1) {
				unsigned int maxOccupyRatio = 0;
				
				for (rowid=0; rowid<rowCount; ++rowid) {
					if (
						cellAddr[rowid][colid] &&
						cellAddr[rowid][colid]->rowspan == 1 &&
						cellAddr[rowid][colid]->colspan == 1
					) {
						unsigned long int curOccupyRatio =
							(cellWidths[rowid][colid] * cellHeights[rowid][colid] * 256)
							/ ((drawRegionRight * rowHeights[rowid])+1);
						// (+1 to avoid division by zero)
						
						if (curOccupyRatio > maxOccupyRatio)
							maxOccupyRatio = curOccupyRatio;
					}
				}
				
				drawRegionRight = drawRegionRight * maxOccupyRatio / 256;
			} else {
				drawRegionRight = (cellHeights[failedCell][colid] * cellWidths[failedCell][colid])
					/ (rowHeights[failedCell]+1);
				// (+1 to avoid division by zero)
			}
			
			/* We have to make sure that the next guess for drawRegionRight
			 * is between minwidth (incl.) and maxwidth (excl.) for
			 * the algorithmn to terminate. If the guess we made based
			 * on the ratio of needed and provided space in the "worst-case"
			 * cell isn't between these bounds, it is obviously a useless
			 * guess. In this case we fall back to a strategy that doesn't
			 * use as much "meta information" (such as needed/provided
			 * ratio) but eleminates many wrong possibilities in one step.
			 * Not that Veryfing that a guess was too low can be done faster
			 * than veryifying that a guess was too high. Thus, we tend to
			 * guess low values. */
			if (drawRegionRight >= maxwidth  ||  drawRegionRight < minwidth) {
				drawRegionRight = (minwidth + minwidth + maxwidth) / 3;
			} else {
				drawRegionRight = (drawRegionRight + minwidth) / 2;
			}
			
			
			/* We first check whether the cell that failed last time fits this
			 * time. If it doesn't, we're done. If it does, we have to check all
			 * the other cells in the column. */
			
			if (lastFailed != -1  &&  calculateSizeOfCell(
				lastFailed, colid, cellAddr[lastFailed][colid]->next, gc,
				cellWidths[lastFailed][colid], cellHeights[lastFailed][colid],
				rowHeights[lastFailed]
			)) {
				minwidth = drawRegionRight + 1;
			} else {
				for (rowid=0; rowid<rowCount; ++rowid) {
					if (
						rowid != lastFailed  &&  cellAddr[rowid][colid]
						&& cellAddr[rowid][colid]->rowspan==1  && 
						calculateSizeOfCell(
							rowid, colid, cellAddr[rowid][colid]->next, gc,
							cellWidths[rowid][colid], cellHeights[rowid][colid],
							rowHeights[rowid]
					)) break;
				}
				
				if (rowid == rowCount) {
					maxwidth = drawRegionRight;
					failedCell = -1;
				} else {
					minwidth = drawRegionRight + 1;
					failedCell = rowid;
				}
			}
			
			if (failedCell != -1)  lastFailed = failedCell;
		}
		
		columnwidths[colid] = minwidth;
	}
	
	drawRegionRight = drawRegionRightBefore;
}



void Article::leaveTable()
{
	if (inTable)  inTable->leaveTable();
}

}	//end of Namespace encyclopodia
