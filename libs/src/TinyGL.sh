#!/bin/sh
#
# TinyGL Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Nov 22, 2008
#
# Requires hotdog
LIBS="hotdog"
for lib in $LIBS; do
	if [ ! -d $lib ]; then
		echo ""
		echo "[TinyGL requires $lib]"
		./src/$lib.sh
	fi
done
echo ""
echo "==========================================="
echo ""
echo "TinyGL Auto-Compiling Script"
echo ""
# Cleanup
if [ -d TinyGL ]; then
	echo "> Removing old TinyGL directory..."
	rm -rf TinyGL
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet http://svn.so2.ath.cx/repos/ipod/tinygl TinyGL
# Compiling
echo "> Compiling..."
cd TinyGL
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 >> build.log 2>&1
echo ""
cd ..
LIB=TinyGL/lib/libTinyGL.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="