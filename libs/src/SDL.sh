#!/bin/sh
#
# SDL Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL Auto-Compiling Script"
	echo ""
	echo "[SDL compiling not yet"
	echo " supported for SansaLinux - skipping]"
	echo ""
	echo "==========================================="
	exit
fi
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL Auto-Compiling Script"
	echo ""
	echo "[SDL doesn't seem to compile"
	echo " nicely on Cygwin - skipping]"
	echo ""
	echo "==========================================="
	exit
fi
echo ""
echo "==========================================="
echo ""
echo "SDL Auto-Compiling Script"
echo ""
# Cleanup
if [ -d SDL ]; then
	echo "> Removing old SDL directory..."
	rm -rf SDL
fi
# Extract source
echo "> Extracting source..."
tar -xf src/SDL/SDL-1.2.13.tar.gz
mv SDL-1.2.13 SDL
# Compiling
cd SDL
echo "> Updating iPod port..."
cp -rf ../src/SDL/SDL_ipodvideo.c src/video/ipod/
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CFLAGS="-D__unix__" --host=arm-uclinux-elf LDFLAGS=-Wl,-elf2flt --enable-ipod --disable-cdrom --disable-video-opengl --disable-threads --prefix=$(pwd) >> build.log 2>&1
make install >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="