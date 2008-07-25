#!/bin/sh
#
# libjpeg Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
echo ""
echo "==========================================="
echo ""
echo "libjpeg Auto-Compiling Script"
echo ""
# Cleanup
if [ -d libjpeg ]; then
	echo "> Removing old libjpeg directory..."
	rm -rf libjpeg
fi
# Extract source
echo "> Extracting source..."
tar -xf src/libjpeg/jpegsrc.v6b.tar.gz
mv jpeg-6b libjpeg
# Compiling
cd libjpeg
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc LDFLAGS=-elf2flt --host=arm-uclinux-elf --prefix=$(pwd) >> build.log 2>&1
mkdir lib include
echo "  - make install-lib..."
make install-lib >> build.log
echo ""
cd ..
LIB=libjpeg/lib/libjpeg.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="