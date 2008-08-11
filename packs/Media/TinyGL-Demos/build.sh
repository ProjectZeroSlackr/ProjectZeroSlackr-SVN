#!/bin/sh
#
# TinyGL-Demos Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 9, 2008
#
echo ""
echo "==========================================="
echo ""
echo "TinyGL-Demos Auto-Building Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[TinyGL-Demos compiling not yet"
	echo " supported for SansaLinux - skipping]"
	echo ""
	echo "==========================================="
	exit
fi
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new compiling directory
echo "> Setting up build directory..."
mkdir build
cd build
BUILDDIR=$(pwd)
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="hotdog TinyGL ttk launch"
for lib in $LIBS
do
	if [ ! -d $LIBSDIR/$lib ]; then
		cd $LIBSDIR
		echo "  - Building "$lib"..."
		./src/$lib.sh
		echo ""
		cd $DIR
	fi
	ln -s $LIBSDIR/$lib ./
done
cd $DIR
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf $LIBSDIR/TinyGL/examples/gears compiled/Gears
cp -rf $LIBSDIR/TinyGL/examples/iv compiled/ImageViewer
cp -rf $LIBSDIR/TinyGL/examples/mech compiled/Mech
cp -rf $LIBSDIR/TinyGL/examples/spin compiled/Spin
cp -rf $LIBSDIR/TinyGL/examples/texobj compiled/Texobj
# Water demo doesn't seem to work ; /
#cp -rf $LIBSDIR/TinyGL/examples/water compiled/Water
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk
cd ..
# Creating release
echo "> Creating 'release' folder..."
cp -rf ../src/release ./
cd release
# Files
PACK=ZeroSlackr/opt/Media/TinyGL-Demos
cp -rf ../compiled/* $PACK/
cp -rf ../launcher/* $PACK/Launch/
# Documents
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="