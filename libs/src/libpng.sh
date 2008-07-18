#!/bin/sh
#
# libpng Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
# Requires zlib
if [ ! -d zlib ]; then
	echo ""
	echo "[libpng requires zlib]"
	./src/zlib.sh
fi
echo ""
echo "==========================================="
echo ""
echo "libpng Auto-Compiling Script"
echo ""
# Cleanup
if [ -d libpng ]; then
	echo "> Removing old libpng directory..."
	rm -rf libpng
fi
# Extract source
echo "> Extracting source..."
tar -xf src/libpng/libpng-1.2.29.tar.gz
mv libpng-1.2.29 libpng
# Compiling
cd libpng
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CC=arm-uclinux-elf-gcc CFLAGS="-I$(pwd)/../zlib/include" LDFLAGS="-L$(pwd)/../zlib/lib -elf2flt" --host=arm-uclinux-elf --prefix=$(pwd) >> build.log 2>&1
make install >> build.log 2>&1
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="