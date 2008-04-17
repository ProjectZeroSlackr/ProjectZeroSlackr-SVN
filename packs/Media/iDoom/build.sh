#!/bin/sh
#
# iDoom Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "iDoom Auto-Building Script"
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
unzip -q ../src/orig/iDoom1_3-src.zip
mv iDoom compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd compiling
patch -p0 -t -i ../../src/patches/iDoom-All.patch >> ../build.log
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="ttk launch"
for lib in $LIBS
do
	if [ ! -d $LIBSDIR/$lib ]; then
		cd $LIBSDIR
		echo "  - Building "$lib"..."
		./src/$lib.sh
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
make >> ../build.log 2>&1
# Apply ZeroSlackr custom patches
echo "> Compiling iFreeDoom..."
patch -p0 -t -i ../../src/patches/iFreeDoom.patch >> ../build-iFreeDoom.log
make clean >> ../build-iFreeDoom.log 2>&1
make >> ../build-iFreeDoom.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/iDoom compiled/
cp -rf compiling/iFreeDoom compiled/
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk >> ../build.log
cd ..
# Creating release
echo "> Creating 'release' folder..."
unzip -q ../src/orig/iDoom1_3.zip
unzip -q ../src/orig/freedm-0.6.zip
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/iDoom
cp -rf ../compiled/iDoom $PACK/
cp -rf ../compiled/iFreeDoom $PACK/
cp -rf ../iDoom/doom1.wad $PACK/IWADs/
cp -rf ../iDoom/keys.key $PACK/Conf/
mv -f ../freedm-0.6/freedm.wad $PACK/IWADs/
mkdir $PACK/Launch
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../iDoom/readme.txt $DOCS/
mkdir $DOCS/FreeDoom
mv -f ../freedm-0.6/* $DOCS/FreeDoom/
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