#!/bin/sh
#
# zlib Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: May 2, 2008
#
echo ""
echo "==========================================="
echo ""
echo "zlib Auto-Compiling Script"
echo ""
if [ -d libmad ]; then
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
make CC=arm-elf-gcc LDFLAGS="-L. libz.a -elf2flt" EXE="" prefix=$PWD >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="