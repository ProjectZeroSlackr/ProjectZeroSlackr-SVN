#!/bin/sh
#
# tar Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "tar Auto-Compiling Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo "[tar doesn't seem to compile"
	echo " nicely on Cygwin - skipping]"
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
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo "  Note: tar building on Cygwin is a pain;"
	echo "  expect lots of problems and it to fail."
fi
export PATH=/usr/local/bin:$PATH
./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf --disable-largefile --prefix=$(pwd) >> build.log
make install >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="