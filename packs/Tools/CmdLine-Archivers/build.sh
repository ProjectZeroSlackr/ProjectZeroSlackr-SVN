#!/bin/sh
#
# CmdLine-Archivers Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 11, 2008
#
echo ""
echo "==========================================="
echo ""
echo "CmdLine-Archivers Auto-Building Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN=1
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
tar zxf ../src/repack/unrar_repacked.tar.gz
tar zxf ../src/repack/unzip_repacked.tar.gz
unzip -q ../src/orig/zip232.zip -d zip
unzip -q ../src/orig/lpaq4.zip -d lpaq4
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="bzip2 tar zlib ttk launch"
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
# Compiling
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - gzip"
cd gzip
make >> ../gzip.log 2>&1
cd ..
echo "  - zip"
cd zip
cp -rf ../../src/mod/zip-Makefile Makefile
make ipod >> ../zip.log 2>&1
cd ..
echo "  - lpaq4"
cd lpaq4
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
arm-uclinux-elf-g++ lpaq4.cpp -O3 -march=armv4t -fomit-frame-pointer -s -o lpaq4 -elf2flt
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
if [ $CYGWIN ]; then
	echo "  Note: skipping tar"
else
	cp -rf tar/src/tar compiled/
fi
cp -rf gzip/gzip compiled/
cp -rf unrar/unrar compiled/
cp -rf unzip/unzip compiled/
cp -rf zip/zip compiled/
cp -rf zip/zipcloak compiled/
cp -rf zip/zipnote compiled/
cp -rf zip/zipsplit compiled/
cp -rf bzip2/bzip2 compiled/
cp -rf bzip2/bzip2recover compiled/
cp -rf lpaq4/lpaq4 compiled/
cp -rf zlib/minigzip compiled/
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
PACK=ZeroSlackr/opt/Tools/CmdLine-Archivers
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