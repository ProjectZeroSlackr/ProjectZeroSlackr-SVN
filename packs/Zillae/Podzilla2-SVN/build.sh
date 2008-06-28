#!/bin/sh
#
# Podzilla2-SVN Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Jun 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Podzilla2-SVN Auto-Building Script"
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new compiling directory
echo "> Setting up build directory..."
mkdir build
cd build
mkdir compiling
# Update with SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/podzilla2 official-svn
cp -r official-svn/* compiling/
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
rm -rf compiling/modules/.svn
cp -r ../src/mod/* compiling/
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="ttk launch"
for lib in $LIBS
do
	if [ ! -d $LIBSDIR/$lib ]; then
		cd $LIBSDIR
		echo "  - Building "$lib"..."
		./src/$lib.sh
		echo ""
		cd $DIR
	fi
	ln -s $LIBSDIR/$lib ./
done
# Compiling
echo "> Compiling..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
# Spring Cleaning
# Loop doesn't seem to work here unfortunately
cp -rf compiling/podzilla compiled/Podzilla2-SVN
cp -rf compiling/modules compiled/modules
rm -rf compiled/modules/.mods
rm -rf compiled/modules/Makefile
rm -rf compiled/modules/*/Makefile
rm -rf compiled/modules/*/Config
rm -rf compiled/modules/*/*.o
rm -rf compiled/modules/*/*.c
rm -rf compiled/modules/*/*.h
rm -rf compiled/modules/*/.svn
for file in compiled/modules/*/*.mod; do
	mv $file $file.o
done
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk
cd ..
# Creating release
echo "> Creating 'release' folder..."
cp -rf ../src/release ./
cd release
# Files
PACK=ZeroSlackr/opt/Zillae/Podzilla2-SVN
cp -rf ../compiled/Podzilla2-SVN $PACK/
# Sort Modules
USRLIB=ZeroSlackr/usr/lib
cp -rf ../compiled/modules/* $USRLIB/Unsorted/
PATCHED="browser mpdc podwrite"
for module in $PATCHED
do
	cp -rf ../compiling/PATCHED.txt $USRLIB/Unsorted/$module/
done
DEL="libstdcxx mymodule pthreadstubs sleep"
for module in $DEL
do
	rm -rf $USRLIB/Unsorted/$module
done
ALL="about browser browser-ext mpd mpdc podwrite terminal textinput tidial tiwidgets tixtensions"
for module in $ALL
do
	mv $USRLIB/Unsorted/$module $USRLIB/All/
done
ARCADE="bluecube invaders ipobble lights minesweeper othello pong steroids tictactoe tunnel vortex wumpus"
for module in $ARCADE
do
	mv $USRLIB/Unsorted/$module $USRLIB/Arcade/
done
DEV="experimental lithp ticursive tifbk tikana tikeypad timlwheelboard timorse tiosk tiptext tithumbscript tiunihex tiwheelboard"
for module in $DEV
do
	mv $USRLIB/Unsorted/$module $USRLIB/Dev/
done
MEDIA="cube mandelpod matrix truchet"
for module in $MEDIA
do
	mv $USRLIB/Unsorted/$module $USRLIB/Media/
done
MISC="animated_deco headerclock"
for module in $MISC
do
	mv $USRLIB/Unsorted/$module $USRLIB/Misc/
done
TOOLS="calc calendar clocks clocks7seg colorpicker dialer mouse poddraw podpaint resistors"
for module in $TOOLS
do
	mv $USRLIB/Unsorted/$module $USRLIB/Tools/
done
cp -rf ../launcher/* $PACK/Launch/
# Documents
echo "> Copying over documents..."
DOCS=$PACK/Misc/Docs
echo "  - Podzilla2-SVN docs"
cp -rf ../../License.txt $PACK/
cp -rf ../../"ReadMe from Keripo.txt" $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
echo "  - ttk docs"
TTK=../ttk
mkdir -p $DOCS/ttk
cp -rf $TTK/API/API.tex $DOCS/ttk/
cp -rf $TTK/COPYING $DOCS/ttk/
cp -rf $TTK/README $DOCS/ttk/
echo "  - PZ2 docs"
OFFSVN=../official-svn
mkdir -p $DOCS/ucdl
cp -rf $OFFSVN/contrib/ucdl/README $DOCS/ucdl/
mkdir -p $DOCS/pz2
cp -rf $OFFSVN/API.tex $DOCS/pz2/
cp -rf $OFFSVN/COPYING $DOCS/pz2/
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Archive documents
cd $PACK/Misc
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
tar -cf Docs.tar Docs
gzip --best Docs.tar
rm -rf Docs
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="