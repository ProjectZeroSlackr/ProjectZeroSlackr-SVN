#!/bin/sh
#
# iPodMAME Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 14, 2008
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
# Symlink the libraries
echo "> Symlinking libraries..."
cd ..
ln -s ../../../libs/hotdog ./
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
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/iPodMAME
cp -rf ../compiled/iPodMAME $PACK/
cp -rf ../compiling/ipodmame.ini $PACK/Conf/
cp -rf ../compiling/roms/hellopac/* $PACK/Roms/hellopac/
cp -rf ../compiling/roms/matrxpac/* $PACK/Roms/matrxpac/
unzip -o -q ../../src/orig/aa.zip -d $PACK/Roms/aarmada/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf ../../src/patches $PACK/Misc/Patches
DOCSORIG=$DOCS/Original
FILES="READ_ME.TXT readme.ipl.txt readme.txt README.UNIX romlist.ipl.txt whatsnew.txt"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCSORIG/
done
mkdir $DOCSORIG/roms
cp -rf ../compiling/roms/readme.txt $DOCSORIG/roms/
cp -rf ../compiling/romlist.ipl.txt $PACK/Roms/
# Archive documents
cd $PACK/Misc
tar -cf Docs.tar Docs
gzip --best Docs.tar
rm -rf Docs
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