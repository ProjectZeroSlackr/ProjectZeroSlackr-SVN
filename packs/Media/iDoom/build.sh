#!/bin/sh
#
# iDoom/hDoom Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Oct 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "iDoom/hDoom Auto-Building Script"
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
unzip -q ../src/orig/iDoom1_3-src.zip
mv iDoom iDoom_src
unzip -q ../src/orig/doomsrc.zip
tar -xf linuxdoom-1.10.src.tgz
mv linuxdoom-1.10 hDoom
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cd iDoom_src
patch -p0 -t -i ../../src/patches/iDoom-All.patch >> ../build-iDoom.log
cd ..
cd hDoom
patch -p0 -t -i ../../src/patches/hDoom-All.patch >> ../build-hDoom.log
patch -p0 -t -i ../../src/patches/hDoom-ZS-Friendly.patch >> ../build-hDoom.log
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="hotdog ttk launch"
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
# Compiling iDoom
echo "> Compiling iDoom..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build-iDoom.log' file."
echo "  If building fails, check the log file."
cd iDoom_src
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make >> ../build-iDoom.log 2>&1
cd ..
# Compiling hDoom
echo "> Compiling hDoom..."
cd hDoom
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make >> ../build-hDoom.log
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf iDoom_src/iDoom compiled/
cp -rf hDoom/linux/hDoom compiled/
# Launch module
echo "> Building ZeroLauncher launch module..."
cp -rf ../src/launcher ./
cd launcher
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f ../launch/launch.mk
cd ..
# Creating release
echo "> Creating 'release' folder..."
unzip -q ../src/orig/iDoom1_3.zip
unzip -q ../src/orig/freedm-0.6.2.zip
cp -rf ../src/release ./
cd release
# Files
PACK=ZeroSlackr/opt/Media/iDoom
cp -rf ../compiled/iDoom $PACK/
cp -rf ../compiled/hDoom $PACK/
cp -rf ../iDoom/doom1.wad $PACK/IWADs/
cp -rf ../iDoom/keys.key $PACK/Conf/
mv -f ../freedm-0.6.2/freedm.wad $PACK/IWADs/
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../iDoom/readme.txt $DOCS/iDoom/
cp -rf ../README.TXT $DOCS/hDoom/
HDOOM_DOCS="ChangeLog DOOMLIC.TXT README.b README.book TODO"
for doc in $HDOOM_DOCS
do
	cp -rf ../hDoom/$doc $DOCS/hDoom/
done
mv -f ../freedm-0.6.2/* $DOCS/FreeDoom/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
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