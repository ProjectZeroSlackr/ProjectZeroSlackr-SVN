#!/bin/sh
#
# PiCalc Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "PiCalc Auto-Building Script"
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
tar zxf ../src/orig/pi-agm.tgz
mv pi-agm compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
# Compiling
echo "> Compiling..."
cd src
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
# Note that we build the fft-hartley target;
# For different options, type "make" and read the output
make fft-hartley CC=arm-elf-gcc LDFLAGS="-elf2flt -o" >> ../build.log
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/src/pi-agm compiled/PiCalc
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/PiCalc
cp -rf ../compiled/PiCalc $PACK/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf ../../src/patches $PACK/Misc/Patches
DOCSORIG=$DOCS/Original
cp -rf ../compiling/docs $DOCSORIG/
cp -rf ../compiling/features.txt $DOCSORIG/
cp -rf ../compiling/src/fftstuff/hartley/version.txt $DOCSORIG/
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