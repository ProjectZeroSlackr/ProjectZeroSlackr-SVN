#!/bin/sh
#
# Podfather Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 5, 2008
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
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../compiling/podfather.txt $DOCS/
# Archive documents
cd $PACK/Misc
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