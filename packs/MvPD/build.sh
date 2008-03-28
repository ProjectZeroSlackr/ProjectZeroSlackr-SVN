#!/bin/sh
#
# MvPD Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "MvPD Auto-Building Script"
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
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
mkdir schemes
cd schemes
for scheme in ../../src/mod/schemes/*
do
	tar -xf $scheme
done
cd ..
mkdir videos
cd videos
for video in ../../src/mod/videos/*
do
	tar -xf $video
done
cd ..
cd release
# Files
PACK=ZeroSlackr/opt/MvPD
cp -rf ../../src/hex/* $PACK/
cp -rf ../schemes/* $PACK/Schemes/
cp -rf ../videos/* $PACK/Videos/Clips/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="