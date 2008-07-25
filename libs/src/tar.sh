#!/bin/sh
#
# tar Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
echo ""
echo "==========================================="
echo ""
echo "tar Auto-Compiling Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo "[tar doesn't seem to compile nicely on"
	echo " Cygwin - using pre-built files]"
	cp -rf src/tar/pre-built tar
	echo ""
	echo "==========================================="
	exit
fi
# Cleanup
if [ -d tar ]; then
	echo "> Removing old tar directory..."
	rm -rf tar
fi
# Extract source
echo "> Extracting source..."
tar -xf src/tar/tar-1.16.2-20070123.tar.gz
mv tar-1.16.2-20070123 tar
# Compiling
cd tar
echo "> Compiling..."
export PATH=/usr/local/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf --disable-largefile --prefix=$(pwd) >> build.log 2>&1
echo "  - make install..."
make install >> build.log 2>&1
echo ""
cd ..
LIB=tar/lib/libtar.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="