#!/bin/sh
#
# SDL Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 25, 2008
#
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL Auto-Compiling Script"
	echo ""
	echo "[SDL doesn't seem to compile nicely on"
	echo " Cygwin - using pre-built files]"
	mkdir SDL
	cp -rf src/SDL/pre-built/include SDL/
	if [ $SANSA ]; then
		cp -rf src/SDL/pre-built/lib-sansalinux SDL/lib
	else
		cp -rf src/SDL/pre-built/lib SDL/
	fi
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
if [ $SANSA ]; then
	echo "> Adding SansaLinux port..."
	cp -rf ../src/SDL/SDL_ipodvideo.c src/video/ipod/
	cp -rf ../src/SDL/lcd-as-memframe.S src/video/ipod/
else
	echo "> Updating iPodLinux port..."
	cp -rf ../src/SDL/SDL_ipodvideo.c src/video/ipod/
fi
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CFLAGS="-D__unix__" --host=arm-uclinux-elf LDFLAGS=-Wl,-elf2flt --enable-ipod --disable-cdrom --disable-video-opengl --disable-threads --prefix=$(pwd) >> build.log 2>&1
if [ $SANSA ]; then
	echo "  - Patching for SansaLinux..."
	patch -p0 -t -i ../src/SDL/Makefile-sansalinux.patch >> build.log
	patch -p0 -t -i ../src/SDL/build-deps-sansalinux.patch >> build.log
	echo "  - make install..."
	make install SANSA=1 >> build.log 2>&1
else
	echo "  - make install..."
	make install >> build.log 2>&1
fi
echo ""
cd ..
LIB=SDL/lib/libSDL.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="