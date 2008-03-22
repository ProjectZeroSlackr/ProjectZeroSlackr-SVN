#!/bin/sh
#
# Encyclopodia Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 19, 2008
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
LIBS=../../../libs
if [ ! -d $LIBS/ttk ]; then
	echo "  - Building libraries..."
	cd $LIBS
	./build.sh
	cd ../packs/Encyclopodia/build
fi
ln -s $LIBS/ttk ttk
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
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
# Disable for now - seems like font choosing
# doesn't work with the PZ2 module (article.cc)
# Maybe I'll look furthur into it later
#tar -xf compiling/fonts.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/Encyclopodia
cp -rf ../compiled/Encyclopodia $PACK/
cp -rf ../compiled/modules/* $PACK/Modules/
#cp -rf ../fonts $PACK/Fonts
cp -rf ../../src/libraries/ipodlinux-wiki-articles-2008-03-07.epodia $PACK/Library/
# Documents
echo "> Copying over documents..."
DOCS=$PACK/Misc/Docs
DOCSORIG=$DOCS/Original
echo "  - Encyclopodia docs"
cp -rf ../../License.txt $DOCS
cp -rf ../../"ReadMe from Keripo.txt" $DOCS
cp -rf ../../src/libraries/Creation-Instructions.txt $DOCS
cp -rf ../../src/libraries/iPL-Export-List.txt $DOCS
cp -rf ../../src/libraries/ipodlinux-wiki-articles-YYYY-MM-DD.epodia $DOCS
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
tar -cf Docs.tar Docs
gzip --best Docs.tar
rm -rf Docs
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="