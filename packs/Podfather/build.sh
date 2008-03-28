#!/bin/sh
#
# Podfather Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Podfather Auto-Building Script"
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
mkdir compiling
cd compiling
unzip -x -q ../../src/orig/2005-hooy_program_-_podfather_IPOD_demo.zip
cd ..
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/Podfather
cp -rf ../compiling/podfather $PACK/Podfather
cp -rf ../compiling/fif.mod $PACK/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
DOCSORIG=$DOCS/Original
cp -rf ../compiling/podfather.txt $DOCSORIG/
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