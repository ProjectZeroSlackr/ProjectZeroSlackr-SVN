#!/bin/sh
#
# hWolf3D Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Sept 18, 2008
#
echo ""
echo "==========================================="
echo ""
echo "hWolf3D Auto-Building Script"
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
mkdir compiling
# Update with Git
echo "> Updating Git..."
git clone git://git.icculus.org/relnev/wolf3d.git official-git >> build.log 2>&1
cp -r official-git/src/* compiling/
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
echo "  - make IPOD=1 WOLF=1 SHARE=1..."
make clean >> ../build.log
make IPOD=1 WOLF=1 SHARE=1 >> ../build.log
mv hwolf3d Wolf3D-Shareware
echo "  - make IPOD=1 WOLF=1..."
make clean >> ../build.log
make IPOD=1 WOLF=1 >> ../build.log
mv hwolf3d Wolf3D-Retail
echo "  - make IPOD=1 SPEAR=1 SHARE=1..."
make clean >> ../build.log
make IPOD=1 SPEAR=1 SHARE=1 >> ../build.log
mv hwolf3d SoD-Shareware
echo "  - make IPOD=1 SPEAR=1..."
make clean >> ../build.log
make IPOD=1 SPEAR=1 >> ../build.log
mv hwolf3d SoD-Retail
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/Wolf3D-Shareware compiled/
cp -rf compiling/Wolf3D-Retail compiled/
cp -rf compiling/SoD-Shareware compiled/
cp -rf compiling/SoD-Retail compiled/
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
PACK=ZeroSlackr/opt/Media/hWolf3D
cp -rf ../compiled/Wolf3D-Shareware $PACK/Wolf3D-Shareware/
cp -rf ../compiled/Wolf3D-Retail $PACK/Wolf3D-Retail/
cp -rf ../compiled/SoD-Shareware $PACK/SoD-Shareware/
cp -rf ../compiled/SoD-Retail $PACK/SoD-Retail/
cp -rf ../launcher/* $PACK/Launch/
unzip -o -q ../../src/orig/wolfdata.zip -d $PACK/Wolf3D-Shareware
unzip -o -q ../../src/orig/sdmdata.zip -d $PACK/SoD-Shareware
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../../src/mod $PACK/Misc/Mod
cp -rf ../official-git/docs/* $DOCS/
cp -rf ../compiling/NOTES $DOCS/
cp -rf ../compiling/README $DOCS/
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