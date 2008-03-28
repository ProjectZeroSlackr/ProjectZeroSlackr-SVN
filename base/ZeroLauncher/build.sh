#!/bin/sh
#
# ZeroLauncher Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ZeroLauncher Auto-Building Script"
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
cp -r ../src/mod/* compiling/
# Keep some of the official modules
echo "> Copying over selected official modules..."
KEEP="about browser mpdc podwrite terminal textinput tidial tiwidgets tixtensions"
for module in $KEEP
do
	cp -rf compiling/modules/$module compiling/add-ons/
done
mv compiling/modules/Makefile compiling/add-ons/
rm -rf compiling/modules/*
mv compiling/launch-modules/* compiling/modules/
mv compiling/add-ons/* compiling/modules/
rm -rf compiling/launch-modules
rm -rf compiling/add-ons
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../libs
LIBS="ttk"
for lib in $LIBS
do
	if [ ! -d $LIBSDIR/$lib ]; then
		cd $LIBSDIR
		echo "  - Building "$lib"..."
		./src/$lib.sh
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
cp -rf compiling/podzilla compiled/ZeroLauncher
cp -rf compiling/modules compiled/launch-modules
# Spring Cleaning
rm -rf compiled/launch-modules/.mods
rm -rf compiled/launch-modules/Makefile
rm -rf compiled/launch-modules/*/Makefile
rm -rf compiled/launch-modules/*/Module
rm -rf compiled/launch-modules/*/Config
rm -rf compiled/launch-modules/*/*.c
rm -rf compiled/launch-modules/*/*.h
rm -rf compiled/launch-modules/*/*~
rm -rf compiled/launch-modules/*/.svn
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/ZeroLauncher
cp -rf ../compiled/ZeroLauncher $PACK/
cp -rf ../compiled/launch-modules/* ZeroSlackr/opt/
ADDONS="browser-ext "$KEEP
for module in $ADDONS
do
	mv ZeroSlackr/opt/$module $PACK/Add-ons/
done
PATCHED="browser mpdc podwrite"
for module in $PATCHED
do
	cp -rf ../compiling/PATCHED.txt $PACK/Add-ons/$module/
done
echo "  Note: For non-default added ZeroLauncher"
echo "  addons, you need to manually move their"
echo "  folders to 'ZeroLauncher/Add-ons'."
# Documents
echo "> Copying over documents..."
DOCS=$PACK/Misc/Docs
DOCSORIG=$DOCS/Original
echo "  - ZeroLauncher docs"
cp -rf ../../License.txt $DOCS
cp -rf ../../"ReadMe from Keripo.txt" $DOCS
cp -rf ../../src/patches $PACK/Misc/Patches
echo "  - ttk docs"
TTK=../ttk
mkdir -p $DOCSORIG/ttk
cp -rf $TTK/API/API.tex $DOCSORIG/ttk/
cp -rf $TTK/COPYING $DOCSORIG/ttk/
cp -rf $TTK/README $DOCSORIG/ttk/
echo "  - PZ2 docs"
OFFSVN=../official-svn
mkdir -p $DOCSORIG/ucdl
cp -rf $OFFSVN/contrib/ucdl/README $DOCSORIG/ucdl
mkdir -p $DOCSORIG/pz2
cp -rf $OFFSVN/API.tex $DOCSORIG/pz2
cp -rf $OFFSVN/COPYING $DOCSORIG/pz2
# Archive documents
cd $PACK/Misc
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
cd Docs
tar -cf Original.tar Original
gzip --best Original.tar
rm -rf Original
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="