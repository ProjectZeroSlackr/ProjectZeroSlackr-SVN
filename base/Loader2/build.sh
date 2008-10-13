#!/bin/sh
#
# Loader2 & iPodPatcher Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Oct 12, 2008
#
echo ""
echo "==========================================="
echo ""
if [ $SANSA ]; then
	echo "SansaPatcher Auto-Building Script"
else
	echo "Loader2 & iPodPatcher Auto-Building Script"
fi
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
	echo ""
fi
mkdir build
cd build
BUILDDIR=$(pwd)
# SansaLinux use pre-compiled files
if [ $SANSA ]; then
	echo "> Using pre-built files..."
	# Make release
	cp -rf ../src/release-sansa release
	cd release
	# Files
	cp -rf ../../src/mod-sansa/* ./
	# Documents
	DOCS=docs/sansapatcher
	cp -rf "../../ReadMe from Keripo.txt" $DOCS/
	cp -rf ../../License.txt $DOCS/
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
	exit
fi
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
cd compiling
# Apply errorlevel patch; see Rockbox task #8827
# http://www.rockbox.org/tracker/task/8827
echo "> Applying errorlevel patch..."
for file in ../../../src/patches/ipodpatcher/*; do
	patch -p0 -t -i $file >> build.log
done
# Compiling
echo "> Compiling..."
make >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir ../compiled
if [ -e ipodpatcher.exe ]; then
	cp -rf ipodpatcher.exe ../compiled/ipodpatcher-win.exe
elif [ `uname` == "Darwin" ]; then
	cp -rf ipodpatcher ../compiled/ipodpatcher-mac
elif [ 'uname -m' == "x86-64" ]; then
	cp -rf ipodpatcher ../compiled/ipodpatcher-linux-64
else
	cp -rf ipodpatcher ../compiled/ipodpatcher-linux-32
fi
cd ..
cd ..
# Make release
cp -rf ../src/release ./
cd release
# Files
cp -rf ../../src/mod/* ./
cp -rf ../loader2/compiled/* ./patch-files/
cp -rf ../ipodpatcher/compiled/* ./patch-files/
chmod -fR ugo+rwx ./*
chmod -fR ugo+rwx ./patch-files/*
# Documents
DOCS=docs/loader2
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf ../../src/patches $DOCS/Patches
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
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