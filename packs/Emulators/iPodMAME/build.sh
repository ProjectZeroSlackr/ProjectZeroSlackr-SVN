#!/bin/sh
#
# iPodMAME Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 30, 2008
#
echo ""
echo "==========================================="
echo ""
echo "iPodMAME Auto-Building Script"
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
svn co --quiet http://opensvn.csie.org/ipodmame/ official-svn
cp -r official-svn/* compiling/
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
# Compiling
echo "> Compiling..."
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 >> ../build.log
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/mame compiled/iPodMAME
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
PACK=ZeroSlackr/opt/Emulators/iPodMAME
cp -rf ../compiled/iPodMAME $PACK/
cp -rf ../compiling/ipodmame.ini $PACK/Conf/
mkdir $PACK/Roms/hellopac
cp -rf ../compiling/roms/hellopac/* $PACK/Roms/hellopac/
mkdir $PACK/Roms/matrxpac
cp -rf ../compiling/roms/matrxpac/* $PACK/Roms/matrxpac/
unzip -o -q ../../src/orig/aa.zip -d $PACK/Roms/aarmada/
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
FILES="READ_ME.TXT readme.ipl.txt readme.txt README.UNIX romlist.ipl.txt whatsnew.txt"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCS/
done
cp -rf ../compiling/roms/readme.txt $DOCS/readme-roms.txt
cp -rf ../compiling/romlist.ipl.txt $PACK/Roms/
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