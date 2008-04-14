#!/bin/sh
#
# SBaGen Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 5, 2008
#
echo ""
echo "==========================================="
echo ""
echo "SBaGen Auto-Building Script"
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
tar -xf ../src/orig/sbagen-1.4.4.tgz
mv sbagen-1.4.4 compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cp -rf ../src/mod/* compiling/
# Compiling
echo "> Compiling..."
cd compiling
export PATH=/usr/local/bin:$PATH
rm -rf sbagen
./mk-ipod >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
if [ -e compiling/sbagen.elf.bflt ]; then
	cp -rf compiling/sbagen.elf.bflt compiled/SBaGen
else
	cp -rf compiling/sbagen compiled/SBaGen
fi
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/SBaGen
cp -rf ../compiled/SBaGen $PACK/
cp -rf ../compiling/examples/* $PACK/Beats/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
FILES="ChangeLog.txt COPYING.txt focus.txt holosync.txt README.txt SBAGEN.txt wave.txt"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCS/
done
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