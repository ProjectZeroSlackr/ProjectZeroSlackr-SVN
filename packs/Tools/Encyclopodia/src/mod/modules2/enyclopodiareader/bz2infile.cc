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
#include "bz2infile.h"
#include <pz.h>

namespace encyclopodia {

Bz2infile::Bz2infile()
  : file(0),
	stream(0),
	outbuf(new char[OUTBUFSIZE]),
	outbufpos(0),
	inbuf(new char[INBUFSIZE]) {
	if (inbuf == NULL || outbuf == NULL) {
		pz_error("malloc failed in bz2infile");
		exit(1);
	}
}

void Bz2infile::open(FILE *pfile)
{
	if (!pfile) {
		pz_error("pfile is null");
		return;
	}
	file = pfile;
	firstBlockOffset = ftell(file);
	setBlockBegin(0, SEEK_CUR);
}

void Bz2infile::setBlockBegin(const size_t &begin, const int &origin)
{
	if (inbuf == NULL) {
		pz_error("Inbuf is NULL, dying.  Probably using a statically "
			 "constructed Bz2infile");
		exit(1);
	}
	
	if (stream) {
		BZ2_bzDecompressEnd(stream);
		delete stream;
		stream = NULL;
	}
	stream = new bz_stream();
	
	stream->bzalloc = 0;
	stream->bzfree = 0;
	stream->opaque = 0;
	stream->next_in = inbuf;
	stream->next_out = outbuf;
	stream->avail_out = OUTBUFSIZE;

	fseek(file, begin, origin);
	stream->avail_in = fread(inbuf, 1, INBUFSIZE, file);

	if (BZ2_bzDecompressInit(stream, 0, small) != BZ_OK ) {
		pz_error("BZ2_bzDecompressInit failed.\n");
		outbufpos = outbuf;
		outbufend = outbuf + OUTBUFSIZE;
		// not implemented: error handling
		return;
	}
	if (stream->next_out != outbuf) {
		outbufpos = outbuf;
		outbufend = outbuf + stream->avail_out;
	} else decompressMore();
}


void Bz2infile::decompressMore()
{
	outbufpos = outbuf;
	
	stream->next_out = outbuf;
	stream->avail_out = OUTBUFSIZE;

	do {
		if (stream->avail_in == 0) {
			stream->next_in = inbuf;
			stream->avail_in = fread(inbuf, 1, INBUFSIZE, file);
		}
		
		int bz_errno = BZ_OK;
		if (
			(stream->avail_in == 0 || (bz_errno = BZ2_bzDecompress(stream)) != BZ_OK) &&
			(bz_errno != BZ_STREAM_END || BZ2_bzDecompressEnd(stream) != BZ_OK)
		) {
			outbufend = outbufpos + OUTBUFSIZE;
			// not implemented: error handling
			pz_error("Bzip error: %d", bz_errno);
			return;
		}

	} while (stream->next_out == outbuf);
	
	outbufend = outbuf + (OUTBUFSIZE - stream->avail_out);
}

void Bz2infile::seekForwardTo(size_t numDecompressedBytes)
{
	while (stream->total_out_lo32 <= numDecompressedBytes) {
		stream->next_out = outbuf;
		stream->avail_out = OUTBUFSIZE;
		
		if (stream->avail_in == 0) {
			stream->next_in = inbuf;
			stream->avail_in = fread(inbuf, 1, INBUFSIZE, file);
		}
		
		int bz_errno = BZ_OK;
		if (
			(stream->avail_in == 0 || (bz_errno = BZ2_bzDecompress(stream)) != BZ_OK) &&
			(bz_errno != BZ_STREAM_END || BZ2_bzDecompressEnd(stream) != BZ_OK)
		) {
			// not implemented: error handling
			pz_error("Bzip error: %d", bz_errno);
			outbufpos = outbuf;
			return;
		}
	}
	
	outbufend = outbuf + (OUTBUFSIZE - stream->avail_out);
	outbufpos = outbufend - (stream->total_out_lo32 - numDecompressedBytes);
}

Bz2infile::~Bz2infile()
{
	delete[] inbuf;
	delete[] outbuf;
}


}
