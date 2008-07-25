#!/bin/sh
#
# freetype Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 21, 2008
#
# Requires zlib
if [ ! -d zlib ]; then
	echo ""
	echo "[freetype requires zlib]"
	./src/zlib.sh
fi
echo ""
echo "==========================================="
echo ""
echo "freetype Auto-Compiling Script"
echo ""
# Cleanup
if [ -d freetype ]; then
	echo "> Removing old freetype directory..."
	rm -rf freetype
fi
# Extract source
echo "> Extracting source..."
tar -xf src/freetype/freetype-2.3.7.tar.gz
mv freetype-2.3.7 freetype
# Compiling
cd freetype
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc CFLAGS=-I$(pwd)/../zlib/include LDFLAGS="-L$(pwd)/../zlib/lib -elf2flt" --host=arm-uclinux-elf --prefix=$(pwd) >> build.log 2>&1
echo "  - make install..."
make install >> build.log 2>&1
echo ""
cd ..
LIB=freetype/lib/libfreetype.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="