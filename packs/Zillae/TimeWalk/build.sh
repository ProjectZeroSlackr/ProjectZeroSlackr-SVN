#!/bin/sh
#
# TimeWalk Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 5, 2008
#
echo ""
echo "==========================================="
echo ""
echo "TimeWalk Auto-Building Script"
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
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/TimeWalk
cd $PACK
for archive in ../../../../../src/hex/*.tar.gz
do
	tar -xf $archive
done
cd ../../..
# Documents
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/hex/Hex-edits.txt $PACK/
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="