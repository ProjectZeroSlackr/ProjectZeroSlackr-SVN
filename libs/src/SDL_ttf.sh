#!/bin/sh
#
# SDL_ttf Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 18, 2008
#
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL_ttf Auto-Compiling Script"
	echo ""
	echo "[SDL_ttf doesn't seem to compile nicely on"
	echo " Cygwin - using pre-built files]"
	mkdir SDL_ttf
	cp -rf src/SDL_ttf/pre-built/include SDL_ttf/
	if [ $SANSA ]; then
		cp -rf src/SDL_ttf/pre-built/lib-sansalinux SDL_ttf/lib
	else
		cp -rf src/SDL_ttf/pre-built/lib SDL_ttf/
	fi
	echo ""
	echo "==========================================="
	exit
fi
# Requires freetype and SDL
if [ ! -d freetype ]; then
	echo ""
	echo "[SDL_ttf requires freetype]"
	./src/freetype.sh
fi
if [ ! -d SDL ]; then
	echo ""
	echo "[SDL_ttf requires SDL]"
	./src/SDL.sh
fi
echo ""
echo "==========================================="
echo ""
echo "SDL_ttf Auto-Compiling Script"
echo ""
# Cleanup
if [ -d SDL_ttf ]; then
	echo "> Removing old SDL_ttf directory..."
	rm -rf SDL_ttf
fi
# Extract source
echo "> Extracting source..."
tar -xf src/SDL_ttf/SDL_ttf-2.0.9.tar.gz
mv SDL_ttf-2.0.9 SDL_ttf
# Compiling
cd SDL_ttf
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CC=arm-uclinux-elf-gcc LDFLAGS=-elf2flt --host=arm-uclinux-elf --with-sdl-prefix=$(pwd)/../SDL --with-freetype-prefix=$(pwd)/../freetype --without-x --without-GL --prefix=$(pwd) >> build.log 2>&1
make install >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="