#!/bin/sh
#
# Encyclopodia Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 24, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Encyclopodia Auto-Building Script"
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
BUILDDIR=$(pwd)
mkdir compiling
# Update with SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/podzilla2 official-svn
cp -r official-svn/* compiling/
cp -r ../src/mod/* compiling/
# Keep some of the official modules
echo "> Copying over selected official modules..."
KEEP="libstdcxx pthreadstubs textinput tidial tiwidgets"
for module in $KEEP
do
	mv compiling/modules/$module compiling/modules2/
done
mv compiling/modules/Makefile compiling/modules2/
rm -rf compiling/modules/*
mv compiling/modules2/* compiling/modules/
rm -rf compiling/modules2
rm -rf compiling/modules/libstdcxx/Config
rm -rf compiling/modules/pthreadstubs/Config
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
LIBSDIR=../../../../libs
LIBS="bzip2 ttk launch"
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
if [ $SANSA ]; then
	echo "  (building for SansaLinux)"
	make IPOD=1 SANSA=1 >> ../build.log 2>&1
else
	make IPOD=1 >> ../build.log 2>&1
fi
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
# Spring Cleaning
# Loop doesn't seem to work here unfortunately
cp -rf compiling/podzilla compiled/Encyclopodia
cp -rf compiling/modules compiled/modules
rm -rf compiled/modules/.mods
rm -rf compiled/modules/Makefile
rm -rf compiled/modules/*/Makefile
rm -rf compiled/modules/*/Config
rm -rf compiled/modules/*/*.o
rm -rf compiled/modules/*/*.c
rm -rf compiled/modules/*/*.h
rm -rf compiled/modules/*/*.cc
rm -rf compiled/modules/*/*.s
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
PACK=ZeroSlackr/opt/Tools/Encyclopodia
cp -rf ../compiled/Encyclopodia $PACK/
cp -rf ../compiled/modules/* $PACK/Modules/
# Current PZ2 module port only allows for one epodia file
#cp -rf ../../src/libraries/ipodlinux-wiki-articles-2008-03-07.epodia $PACK/Library/
cp -rf ../../src/libraries/enwiktionary-20080613-pages-articles.epodia $PACK/Library/
cp -rf ../launcher/* $PACK/Launch/
# Documents
echo "> Copying over documents..."
DOCS=$PACK/Misc/Docs
echo "  - Encyclopodia docs"
cp -rf ../../License.txt $PACK/
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../src/libraries/Creation-Instructions.txt $DOCS/
cp -rf ../../src/libraries/iPL-Export-List.txt $DOCS/
cp -rf ../../src/libraries/ipodlinux-wiki-articles-YYYY-MM-DD.epodia $DOCS/
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
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
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