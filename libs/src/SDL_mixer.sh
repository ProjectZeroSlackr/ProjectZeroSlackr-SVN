#!/bin/sh
#
# SDL_mixer Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 18, 2008
#
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL_mixer Auto-Compiling Script"
	echo ""
	echo "[SDL_mixer doesn't seem to compile nicely"
	echo " on Cygwin - using pre-built files]"
	mkdir SDL_mixer
	cp -rf src/SDL_mixer/pre-built/include SDL_mixer/
	if [ $SANSA ]; then
		cp -rf src/SDL_mixer/pre-built/lib-sansalinux SDL_mixer/lib
	else
		cp -rf src/SDL_mixer/pre-built/lib SDL_mixer/
	fi
	echo ""
	echo "==========================================="
	exit
fi
# Requires libmad and SDL
if [ ! -d libmad ]; then
	echo ""
	echo "[SDL_mixer requires libmad]"
	./src/libmad.sh
fi
if [ ! -d SDL ]; then
	echo ""
	echo "[SDL_mixer requires SDL]"
	./src/SDL.sh
fi
echo ""
echo "==========================================="
echo ""
echo "SDL_mixer Auto-Compiling Script"
echo ""
# Cleanup
if [ -d SDL_mixer ]; then
	echo "> Removing old SDL_mixer directory..."
	rm -rf SDL_mixer
fi
# Extract source
echo "> Extracting source..."
tar -xf src/SDL_mixer/SDL_mixer-1.2.8.tar.gz
mv SDL_mixer-1.2.8 SDL_mixer
# Compiling
cd SDL_mixer
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CC=arm-uclinux-elf-gcc CFLAGS=-I$(pwd)/../libmad/include LDFLAGS="-L$(pwd)/../libmad/lib -elf2flt" --host=arm-uclinux-elf --with-sdl-prefix=$(pwd)/../SDL --enable-music-mp3-mad-gpl --prefix=$(pwd) >> build.log 2>&1
make install >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="