#!/bin/sh
#
# CmdLine-Tools Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo ""
echo "CmdLine-Tools Auto-Building Script"
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
tar zxf ../src/orig/Ipodgzip.tar.gz
tar zxf ../src/orig/Ipodgawk.tar.gz
tar zxf ../src/orig/Ipodgrep.tar.gz
tar zxf ../src/orig/Ipodsed.tar.gz
tar zxf ../src/orig/diffutils-2.8.1.tar.gz
tar zxf ../src/repack/unrar_repacked.tar.gz
tar zxf ../src/repack/unzip_repacked.tar.gz
unzip -q ../src/orig/zip232.zip -d zip
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../libs
LIBS="bzip2 tar"
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
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - gzip"
cd gzip
make >> ../gzip.log 2>&1
cd ..
echo "  - diffutils"
cd diffutils-2.8.1
./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf  >> ../diffutils.log 2>&1
make >> ../diffutils.log 2>&1
cd ..
echo "  - zip"
cd zip
cp -rf ../../src/mod/zip-Makefile Makefile
make ipod >> ../zip.log 2>&1
cd ..
echo "  - cmdlineutils"
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/cmdlineutils
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/libipod
export PATH=/usr/local/bin:$PATH
cd libipod
make IPOD=1 >> ../cmdlineutils.log 2>&1
cd ..
cd cmdlineutils
make IPOD=1 >> ../cmdlineutils.log 2>&1
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf diffutils-2.8.1/src/cmp compiled/
cp -rf diffutils-2.8.1/src/diff compiled/
cp -rf diffutils-2.8.1/src/diff3 compiled/
cp -rf diffutils-2.8.1/src/sdiff compiled/
cp -rf gawk/gawk compiled/
cp -rf grep/grep compiled/
cp -rf sed/sed compiled/
cp -rf cmdlineutils/asciichart compiled/
cp -rf cmdlineutils/backlight compiled/
cp -rf cmdlineutils/contrast compiled/
cp -rf cmdlineutils/font compiled/
cp -rf cmdlineutils/lsi compiled/
cp -rf cmdlineutils/pause compiled/
cp -rf cmdlineutils/raise compiled/
cp -rf gzip/gzip compiled/
cp -rf unrar/unrar compiled/
cp -rf unzip/unzip compiled/
cp -rf zip/zip compiled/
cp -rf zip/zipcloak compiled/
cp -rf zip/zipnote compiled/
cp -rf zip/zipsplit compiled/
cp -rf tar/src/tar compiled/
cp -rf bzip2/bzip2 compiled/
cp -rf bzip2/bzip2recover compiled/
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/CmdLine-Tools
USRBIN=ZeroSlackr/usr/bin
cp -rf ../compiled/* $USRBIN/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
# Archive documents
cd $PACK/Misc
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