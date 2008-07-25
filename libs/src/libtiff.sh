#!/bin/sh
#
# libtiff Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
# Requires zlib and libjpeg
if [ ! -d zlib ]; then
	echo ""
	echo "[libtiff requires zlib]"
	./src/zlib.sh
fi
if [ ! -d libjpeg ]; then
	echo ""
	echo "[libtiff requires libjpeg]"
	./src/libjpeg.sh
fi
echo ""
echo "==========================================="
echo ""
echo "libtiff Auto-Compiling Script"
echo ""
# Cleanup
if [ -d libtiff ]; then
	echo "> Removing old libtiff directory..."
	rm -rf libtiff
fi
# Extract source
echo "> Extracting source..."
tar -xf src/libtiff/tiff-4.0.0beta2.tar.gz
mv tiff-4.0.0beta2 libtiff
# Compiling
cd libtiff
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc CFLAGS="-DPATH_MAX=1024" LDFLAGS=-elf2flt --host=arm-uclinux-elf --with-zlib-include-dir=$(pwd)/../zlib/include --with-zlib-lib-dir=$(pwd)/../zlib/lib --with-jpeg-include-dir=$(pwd)/../libjpeg/include --with-jpeg-lib-dir=$(pwd)/../libjpeg/lib --prefix=$(pwd) >> build.log 2>&1
echo "  - make install..."
make install >> build.log 2>&1
echo ""
cd ..
LIB=libtiff/lib/libtiff.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="