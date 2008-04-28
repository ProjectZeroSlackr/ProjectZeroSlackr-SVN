#!/bin/sh
#
# CmdLine-Tools Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 27, 2008
#
echo ""
echo "==========================================="
echo ""
echo "CmdLine-Tools Auto-Building Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN="yes"
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
LIBSDIR=../../../../libs
LIBS="bzip2 tar ttk launch"
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
if [ "${CYGWIN}" ]; then
	echo "    Note: diffutils doesn't seem to"
	echo "    compile nicely on Cygwin - skipping"
else
	cd diffutils-2.8.1
	./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf >> ../diffutils.log 2>&1
	make >> ../diffutils.log 2>&1
	cd ..
fi
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
if [ "${CYGWIN}" ]; then
	echo "  Note: skipping diffutils and tar"
else
	cp -rf diffutils-2.8.1/src/cmp compiled/
	cp -rf diffutils-2.8.1/src/diff compiled/
	cp -rf diffutils-2.8.1/src/diff3 compiled/
	cp -rf diffutils-2.8.1/src/sdiff compiled/
	cp -rf tar/src/tar compiled/
fi
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
cp -rf bzip2/bzip2 compiled/
cp -rf bzip2/bzip2recover compiled/
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
PACK=ZeroSlackr/opt/Tools/CmdLine-Tools
cp -rf ../compiled/* $PACK/Bin/
cp -rf ../launcher/* $PACK/Launch/
# Documents
# Too many original docs; done by hand
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Archive documents
#cd $PACK/Misc
#tar -cf Docs.tar Docs
#gzip --best Docs.tar
#rm -rf Docs
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="