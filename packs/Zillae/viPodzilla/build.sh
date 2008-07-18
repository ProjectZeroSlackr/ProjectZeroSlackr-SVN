#!/bin/sh
#
# viPodzilla Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 18, 2008
#
echo ""
echo "==========================================="
echo ""
echo "viPodzilla Auto-Building Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[viPodzilla compiling not yet"
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
LIBS="ttk launch"
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
PACK=ZeroSlackr/opt/Zillae/viPodzilla
cp -rf ../../src/hex/viPodzilla $PACK/
cp -rf ../launcher/* $PACK/Launch/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="