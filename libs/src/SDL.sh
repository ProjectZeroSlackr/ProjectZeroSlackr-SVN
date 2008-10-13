#!/bin/sh
#
# SDL Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Oct 12, 2008
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
	cp -rf src/SDL/pre-built SDL
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
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/SDL/src SDL/src-official-svn
cp -rf SDL/src-official-svn/* SDL/src/
# Compiling
echo "> Compiling..."
cd SDL
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CFLAGS="-D__unix__" --host=arm-uclinux-elf LDFLAGS=-Wl,-elf2flt --enable-ipod --disable-cdrom --disable-video-opengl --disable-threads --prefix=$(pwd) >> build.log 2>&1
echo "  - make install..."
make install >> build.log 2>&1
echo "  - Patching for removal of iconv support"
patch -p0 -t -i ../src/SDL/SDL_config.h.patch >> build.log 2>&1
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