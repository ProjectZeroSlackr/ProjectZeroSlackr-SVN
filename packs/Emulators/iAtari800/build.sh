#!/bin/sh
#
# iAtari800 Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 31, 2008
#
echo ""
echo "==========================================="
echo ""
echo "iAtari800 Auto-Building Script"
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
BUILDDIR=$(pwd)
# Extract source
echo "> Extracting source..."
tar -xf ../src/orig/atari800-2.0.3.tar.gz
mv atari800-2.0.3 compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd compiling
cp -rf ../../src/mod/* ./
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="SDL ttk launch"
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
cd compiling
cd src
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
chmod ugo+rwx ./configure-ipod.sh
echo "  - Running configure script..."
./configure-ipod.sh >> ../../build.log 2>&1
echo "  - make..."
make >> ../../build.log
cd ..
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf compiling/src/atari800 compiled/iAtari800
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
PACK=ZeroSlackr/opt/Emulators/iAtari800
cp -rf ../compiled/iAtari800 $PACK/
cp -rf ../launcher/* $PACK/Launch/
unzip -o -q ../../src/orig/roms.zip -d $PACK/Roms
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../../src/mod $PACK/Misc/Mod
FILES="../src/atari800.man ../atari800.spec ../COPYING ../README.1ST BUGS cart.txt ChangeLog CREDITS emuos.txt FAQ pokeysnd.txt README TODO USAGE"
for file in $FILES
do
	cp -rf ../compiling/DOC/$file $DOCS/
done
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
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