#!/bin/sh
#
# Podzilla0-SVN Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 5, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Podzilla0-SVN Auto-Building Script"
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
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
# Symlink the libraries
echo "> Symlinking libraries..."
cd ..
for library in ../../../libs/pz0/libs/*
do
	ln -s $library ./
done
# Compiling
echo "> Compiling..."
cd compiling
export PATH=/usr/local/bin:$PATH
#Note: if you want to compile with MikMod suppot, see the
#"ReadMe from Keripo.txt" in the "mikmod" library folder
# in "/libs/pz0/libs" and compile with:
#make IPOD=1 MPDC=1 MIKMOD=1 >> ../build.log
make IPOD=1 MPDC=1 >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
if [ -e compiling/podzilla.elf.bflt ]; then
	cp -rf compiling/podzilla.elf.bflt compiled/Podzilla0-SVN
else
	cp -rf compiling/podzilla compiled/Podzilla0-SVN
fi
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/Podzilla0-SVN
cp -rf ../compiled/Podzilla0-SVN $PACK/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../compiling/ChangeLog $DOCS/
cp -rf ../compiling/README $DOCS/
mkdir $DOCS/tuxchess
cp -rf ../compiling/tuxchess/README $DOCS/tuxchess/
cp -rf ../compiling/tuxchess/README.chess $DOCS/tuxchess/
cp -rf ../compiling/tuxchess/README.license $DOCS/tuxchess/
cp -rf ../compiling/tuxchess/TODO $DOCS/tuxchess/
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