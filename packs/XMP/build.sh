#!/bin/sh
#
# XMP Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "XMP Auto-Building Script"
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
# Extract source
echo "> Extracting source..."
tar zxf ../src/orig/xmp-2.5.1.tar.gz
mv xmp-2.5.1 compiling
# Updating with CVS
echo "> Updating CVS..."
cvs -Q -d:pserver:anonymous@xmp.cvs.sourceforge.net:/cvsroot/xmp co -d official-cvs xmp2 >> build.log 2>&1
cp -rf official-cvs/* compiling/
# Apply ZeroSlackr custom patches
cp -rf ../src/mod/* compiling/
echo "> Applying ZeroSlackr patches..."
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
# Compiling
echo "> Compiling..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure -q CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf --disable-alsa >> ../build.log 2>&1
rm -rf Makefiles.rules
mv Makefile-mod.rules Makefile.rules
make >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/src/main/xmp compiled/XMP
cp -rf compiling/anticipation.mod compiled/anticipation.mod
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/XMP
cp -rf ../compiled/XMP $PACK/
cp -rf ../compiled/anticipation.mod $PACK/Mods/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../../src/mod $PACK/Misc/Mod
DOCSORIG=$DOCS/Original
cp -rf ../compiling/docs/* $DOCSORIG/
rm -rf $DOCSORIG/Makefile
rm -rf $DOCSORIG/README.os2
rm -rf $DOCSORIG/CVS
rm -rf $DOCSORIG/formats/CVS
cp -rf ../compiling/README $DOCSORIG/
# Archive documents
cd $PACK/Misc
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
tar -cf Mod.tar Mod
gzip --best Mod.tar
rm -rf Mod
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