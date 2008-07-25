#!/bin/sh
#
# libmad Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 21, 2008
#
echo ""
echo "==========================================="
echo ""
echo "libmad Auto-Compiling Script"
echo ""
# Cleanup
if [ -d libmad ]; then
	echo "> Removing old libmad directory..."
	rm -rf libmad
fi
# Extract source
echo "> Extracting source..."
tar -xf src/libmad/libmad-0.15.1b.tar.gz
mv libmad-0.15.1b libmad
# Compiling
cd libmad
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc LDFLAGS=-elf2flt --host=arm-uclinux-elf --enable-fpm=arm --prefix=$(pwd) >> build.log 2>&1
echo "  - make install..."
make install >> build.log
echo ""
cd ..
LIB=libmad/lib/libmad.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="