#!/bin/sh
#
# Podzilla0-Lite Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated:Apr 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Podzilla0-Lite Auto-Building Script"
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
# Update with SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/legacy/podzilla official-svn
cp -r official-svn/* compiling/
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cp -r ../src/mod/* compiling/
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
# Symlink the libraries
echo "> Symlinking libraries..."
cd ..
for library in ../../../../libs/pz0/libs/*
do
	ln -s $library ./
done
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
export PATH=/usr/local/bin:$PATH
#Note: if you want to compile with MikMod suppot, see the
#"ReadMe from Keripo.txt" in the "mikmod" library folder
# in "/libs/pz0/libs" and compile with:
#make IPOD=1 MPDC=1 MIKMOD=1 >> ../build.log
make IPOD=1 MPDC=1 >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
if [ -e compiling/Podzilla0-Lite.elf.bflt ]; then
	cp -rf compiling/Podzilla0-Lite.elf.bflt compiled/Podzilla0-Lite
else
	cp -rf compiling/Podzilla0-Lite compiled/Podzilla0-Lite
fi
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
PACK=ZeroSlackr/opt/Podzilla0-Lite
cp -rf ../compiled/Podzilla0-Lite $PACK/
mkdir $PACK/Launch
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../../src/mod $PACK/Misc/Mod
cp -rf ../compiling/ChangeLog $DOCS/
cp -rf ../compiling/README $DOCS/
# Archive documents
cd $PACK/Misc
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
tar -cf Mod.tar Mod
gzip --best Mod.tar
rm -rf Mod
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