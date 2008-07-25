#!/bin/sh
#
# SDL_image Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo ""
	echo "==========================================="
	echo ""
	echo "SDL_image Auto-Compiling Script"
	echo ""
	echo "[SDL_image doesn't seem to compile nicely"
	echo " on Cygwin - using pre-built files]"
	mkdir SDL_image
	cp -rf src/SDL_image/pre-built/include SDL_image/
	if [ $SANSA ]; then
		cp -rf src/SDL_image/pre-built/lib-sansalinux SDL_image/lib
	else
		cp -rf src/SDL_image/pre-built/lib SDL_image/
	fi
	echo ""
	echo "==========================================="
	exit
fi
# Requires zlib, libjpeg, libpng, libtiff and SDL
LIBS="zlib libjpeg libpng libtiff SDL"
for lib in $LIBS; do
	if [ ! -d $lib ]; then
		echo ""
		echo "[SDL_image requires $lib]"
		./src/$lib.sh
	fi
done
echo ""
echo "==========================================="
echo ""
echo "SDL_image Auto-Compiling Script"
echo ""
# Cleanup
if [ -d SDL_image ]; then
	echo "> Removing old SDL_image directory..."
	rm -rf SDL_image
fi
# Extract source
echo "> Extracting source..."
tar -xf src/SDL_image/SDL_image-1.2.6.tar.gz
mv SDL_image-1.2.6 SDL_image
# Compiling
cd SDL_image
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc --host=arm-uclinux-elf --with-sdl-prefix=$(pwd)/../SDL --prefix=$(pwd) >> build.log 2>&1
echo "  - Patching Makefile..."
patch -p0 -t -i ../src/SDL_image/Makefile.patch >> build.log
echo "  - make install..."
make install >> build.log 2>&1
echo ""
cd ..
LIB=SDL_image/lib/libSDL_image.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="