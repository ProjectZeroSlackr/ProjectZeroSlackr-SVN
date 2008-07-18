#!/bin/sh
#
# Flite Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Flite Auto-Building Script"
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
tar xf ../src/orig/flite-1.3-current.tar.gz
mv flite-1.3-current compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
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
# Compiling
echo "> Compiling..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
echo "  Note 2: Building Flite is processor"
echo "  intensive; if you have an old computer"
echo "  it is recommended that you skip this."
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
./configure CC=arm-elf-gcc --host=arm-elf >> ../build.log 2>&1
make >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/bin/flite compiled/Flite
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
PACK=ZeroSlackr/opt/Tools/Flite
cp -rf ../compiled/Flite $PACK/
cp -rf ../../src/mod/Text/* $PACK/Text/
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
FILES="doc ACKNOWLEDGEMENTS COPYING README"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCS/
done
mv $DOCS/doc/alice $PACK/Text/Alice.txt
mv $DOCS/doc/intro.txt $PACK/Text/Intro.txt
#sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
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