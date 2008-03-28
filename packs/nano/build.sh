#!/bin/sh
#
# nano Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "nano Auto-Building Script"
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
tar zxf ../src/orig/nano.tgz
mv nano compiling
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/nano
cp -rf ../../src/hex/nano $PACK/
cp -rf ../../src/mod/nano-soundtrack.mp3 $PACK/Misc/
cp -rf ../compiling/data $PACK/Data
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf ../../src/hex/Hex-edits.txt $DOCS/
DOCSORIG=$DOCS/Original
cp -rf ../compiling/nano.nfo $DOCSORIG/
cp -rf ../compiling/readme.txt $DOCSORIG/
# Archive documents
cd $PACK/Misc
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