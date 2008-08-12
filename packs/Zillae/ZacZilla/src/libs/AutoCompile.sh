#!/bin/sh
#
# ZacZilla Library Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 12, 2008
#
# Basically straight from the ZacZilla wiki page:
# http://ipodlinux.org/ZacZilla/Building
# See the wiki page for archive sources
#
# May try to include MPD later.
#
# Cleanup
if [ -d libitunesdb ]; then
	DEL="libitunesdb ipp mp4ff helix-aacdec logs"
	echo "  - Removing old builds"
	for library in $DEL
	do
		rm -rf $library
	done
fi
# iTunes Library
mkdir logs
echo "  - Compiling libitunesdb"
mkdir libitunesdb
tar zvxf src/libitunesdb_0.5.2.tar.gz >> logs/build-libitunesdb.log 2>&1
mv libitunesdb_0.5.2 libitunesdb/ipod
cd libitunesdb/ipod
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CFLAGS=-Wl,-elf2flt  --host=arm-uclinux-elf >> ../../logs/build-libitunesdb.log 2>&1
make >> ../../logs/build-libitunesdb.log 2>&1
cd ..
cd ..
# IPP Library
echo "  - Compiling ippsa11"
tar zxf src/ippsa11_lnx.tar.gz >> logs/build-ippsa11.log 2>&1
mv ipp/ippsa11/* ipp/
rmdir ipp/ippsa11
# MP4 Decoder
echo "  - Compiling mp4ff"
tar zxf src/mp4ff.tar.gz >> logs/build-mp4ff.log 2>&1
cd mp4ff
mv src ipod
cd ipod
make IPOD=1 >> ../../logs/build-mp4ff.log 2>&1
cd ..
cd ..
# AAC Decoder
echo "  - Compiling helix-aacdec"
tar zxf src/helix-aacdec.tar.gz >> logs/build-helix-aacdec.log 2>&1
cd helix-aacdec
./make-build-dirs.sh >> ../logs/build-helix-aacdec.log 2>&1
cd ipod
make IPOD=1 >> ../../logs/build-helix-aacdec.log 2>&1
cd ..
cd ..
# Thats all the podzilla 0 libraries used by ZacZilla for now
