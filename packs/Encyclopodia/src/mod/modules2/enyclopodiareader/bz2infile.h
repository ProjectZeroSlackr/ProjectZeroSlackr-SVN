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
#ifndef IKNOWBz2infile_H
#define IKNOWBz2infile_H

#include <stdio.h>
#include <bzlib.h>

namespace encyclopodia {

/**
 * A class for easy access to bz2 compressed files
 *
 * @author Robert Bamler
 */
class Bz2infile{
	private:
		FILE *file;
		bz_stream *stream;
		char *outbuf, *outbufpos, *outbufend, *inbuf;
		
		/* INBUFSIZE/OUTBUFSIZE-ratio could be optimized by
		   counting the times inbuf or outbuf are full */
		static const unsigned int INBUFSIZE  = 1000;
		static const unsigned int OUTBUFSIZE = 2000;
		static const int small = 1;		// if set to 1, bz2 uses less memory but is slower
		
		long firstBlockOffset;
	
	public:
		Bz2infile();
		
		void open(FILE *pfile);
		
		/**
		 * Seeks to the given position in the associated file and starts
		 * decompressing from this position. The specified position must
		 * be the beginning of a logical bz2 file (you can verify this by
		 * checking whether the file has the magic word "BZh" at the
		 * specified position).
		 * @param begin the address of the new block begin
		 * @param origin same as in function ftell from <iosbase>. Note that, if origin is SEEK_END, then the value of the parameter begin must be negative.
		 */
		void setBlockBegin(const size_t &begin, const int &origin);
		
		inline void goToFirstBlock() {
			setBlockBegin(firstBlockOffset, SEEK_SET);
		}
		
		void decompressMore();
		
		/**
		 * Sets the output buffer position to the next char and returns it.
		 * If the output buffer position is already at the end of the output
		 * buffer, some decompressing will be made to refill the output buffer.
		 * Note that calling next() after construction of a Bz2infile will return
		 * the SECOND char in the given block. You normally want to call cur()
		 * first to get the first character, then continuously call next(). This
		 * implementation has been chosen for efficiency reasons.
		 */
		inline unsigned char next() {				// get next char
			if (++outbufpos == outbufend)
				decompressMore();
			return *outbufpos;
		}
		
		inline unsigned int nextUint(unsigned char length) {
			unsigned int ret = next();
			while (--length) {
				ret <<= 8;
				ret += next();
			}
			return ret;
		}
		
		/**
		 * Returns the current char. Does not decompress any data, because the
		 * current char is always already in the output buffer.
		 */
		inline unsigned char cur() const {			// get current char
			return *outbufpos;
		}
		
		/**
		 * Decompresses data and throws the decompressed data away until it reaches
		 * the given position in the decompressed output stream. This function
		 * must not be called with a position that lays before the current position
		 * in the output buffer. It can, however, be called with the exactly the
		 * current position which will have no effect at all.
		 * 
		 * When you
		 * 1. Construct a new Bz2infile
		 * 2. Call seekForwardTo(100)
		 * 3. Call cur()
		 * then cur() will return the 101th byte of the decompressed data stream.
		 * (Because the first byte has the index 0)
		 * 
		 * When you instead
		 * 1. Construct a new Bz2infile
		 * 2. Call next() at most 100 times
		 * 2. Call seekForwardTo(100)
		 * 3. Call cur()
		 * Then cur() will return exactly the same as in the example above.
		 * 
		 * This function is supposed to be faster than continuously calls of next().
		 */
		void seekForwardTo(size_t numDecompressedBytes);
		
		~Bz2infile();
};

}

#endif
