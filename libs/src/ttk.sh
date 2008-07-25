#!/bin/sh
#
# ttk Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ttk Auto-Compiling Script"
echo ""
# Cygwin check
#if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
#	CYGWIN=1
#fi
# Cleanup
if [ -d ttk ]; then
	echo "> Removing old ttk directory..."
	rm -rf ttk
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/ttk/ ttk
# Compiling
cd ttk
# Compiling libttk.a with hotdog is broken, but not with SDL
#if [ $CYGWIN ]; then
#	echo "> Reverting rev 2383..."
#	echo "  (which breaks Cygwin compiling)"
#	patch -p0 -t -i ../src/ttk/ttk-cygwin-compile.patch >> build.log
#fi
# I own the character design copyright for Noblesse, but not for Ren
# Besides, Noblesse looks better ; )
echo "> Patching with Noblesse icon..."
patch -p0 -t -i ../src/ttk/ttk-Noblesse-icon.patch >> build.log
if [ $SANSA ]; then
	echo "> Patching for SansaLinux..."
	patch -p0 -t -i ../src/ttk/ttk-sansalinux.patch >> build.log
	cp -rf ../src/ttk/libSDL-sansa.a libs/SDL/libSDL.a
fi
#patch -p0 -t -i ../src/ttk/ttk-Ren-icon.patch >> build.log
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
if [ $SANSA ]; then
	echo "  (building for SansaLinux)"
	make NOMWIN=1 NOX11=1 NOHDOG=1 SANSA=1 >> build.log 2>&1
else
	make NOMWIN=1 NOX11=1 NOHDOG=1 >> build.log 2>&1
fi
echo ""
cd ..
LIB=ttk/build/ipod-sdl/libttk.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="