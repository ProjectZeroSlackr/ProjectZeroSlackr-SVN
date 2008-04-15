#!/bin/sh
#
# ToeNailClipper Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 15, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ToeNailClipper Auto-Building Script"
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
# Copying full source
echo "> Copying over source..."
cp -r ../src/full/* compiling/
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
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
arm-elf-gcc toenailclipper-mod.c -o ToeNailClipper -elf2flt >> ../build.log
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf compiling/ToeNailClipper compiled/
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk >> ../build.log
cd ..
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/ToeNailClipper
cp -rf ../compiled/ToeNailClipper $PACK/
cp -rf ../launcher/* $PACK/
# Documents
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="