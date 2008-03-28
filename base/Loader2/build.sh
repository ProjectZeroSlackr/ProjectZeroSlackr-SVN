#!/bin/sh
#
# Loader2 & iPodPatcher Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Loader2 & iPodPatcher Auto-Building Script"
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
	echo ""
fi
mkdir build
cd build
# Loader 2
echo "Loader 2:"
# Make new compiling directory
echo "> Setting up build directory..."
mkdir loader2
cd loader2
mkdir compiling
# Update with SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/ipodloader2 official-svn
cp -r official-svn/* compiling/
cd compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
for file in ../../../src/patches/*; do
	patch -p0 -t -i $file >> build.log
done
# Compiling
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir ../compiled
cp -rf loader.bin ../compiled/
cd ..
cd ..
# Loader 2
echo ""
echo "iPodPatcher:"
# Make new compiling directory
echo "> Setting up build directory..."
mkdir ipodpatcher
cd ipodpatcher
mkdir compiling
# Update with SVN
echo "> Updating SVN..."
svn co --quiet svn://svn.rockbox.org/rockbox/trunk/rbutil/ipodpatcher official-svn
cp -r official-svn/* compiling/
# Compiling
echo "> Compiling..."
cd compiling
make >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir ../compiled
if [ -e ipodpatcher.exe ]; then
	cp -rf ipodpatcher.exe ../compiled/
else
	cp -rf ipodpatcher ../compiled/
fi
cd ..
cd ..
# Make release
tar -xf ../src/release.tar.gz
cd release
# Files
cp -rf ../loader2/compiled/* ./
cp -rf ../ipodpatcher/compiled/* ./
# Documents
DOCS=boot/docs/loader2
cp -rf "../../ReadMe from Keripo.txt" "$DOCS/ReadMe from Keripo.txt"
cp -rf ../../License.txt $DOCS/License.txt
cp -rf ../../src/patches $DOCS/Patches
# Archive documents
cd $DOCS
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="