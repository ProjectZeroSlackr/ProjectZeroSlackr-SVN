#!/bin/sh
#
# zlib Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
echo ""
echo "==========================================="
echo ""
echo "zlib Auto-Compiling Script"
echo ""
# Cleanup
if [ -d zlib ]; then
	echo "> Removing old zlib directory..."
	rm -rf zlib
fi
# Extract source
echo "> Extracting source..."
tar -xf src/zlib/zlib-1.2.3.tar.gz
mv zlib-1.2.3 zlib
# Compiling
echo "> Compiling..."
cd zlib
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure --prefix=$(pwd) >> build.log
echo "  - make install..."
make install CC=arm-uclinux-elf-gcc LDFLAGS="-L. libz.a -elf2flt" EXE="" >> build.log
echo "  - make minigzip..."
make minigzip CC=arm-uclinux-elf-gcc LDFLAGS="-L. libz.a -elf2flt" EXE="" >> build.log
echo ""
cd ..
LIB=zlib/lib/libz.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="