#!/bin/sh
#
# Userland Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 19, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Userland Auto-Building Script"
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new build directory
echo "> Setting up build directory..."
mkdir build
cd build
BUILDDIR=$(pwd)
if [ $SANSA ]; then
	cp -rf ../src/release-sansa ./release
else
	cp -rf ../src/release ./
fi
# Update SVN
echo "> Updating scheme files..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/ttk/schemes release/ZeroSlackr/usr/share/schemes
echo "> Removing .svn folder..."
rm -rf release/ZeroSlackr/usr/share/schemes/.svn
echo "> Updating font files..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/ttk/fonts release/ZeroSlackr/usr/share/fonts
echo "> Removing .svn folder..."
rm -rf release/ZeroSlackr/usr/share/fonts/.svn
# Modifications
echo "> Adding schemes..."
for scheme in ../src/mod/schemes-wiki/*
do
	unzip -o -q $scheme -d release/ZeroSlackr/usr/share/schemes/
done
cp -rf ../src/mod/schemes/* release/ZeroSlackr/usr/share/schemes/
echo "> Adding fonts..."
rm -rf release/ZeroSlackr/usr/share/fonts/fonts.lst
cp -rf ../src/mod/fonts/* release/ZeroSlackr/usr/share/fonts/
echo "> Extracting userland..."
if [ $SANSA ]; then
	cp -rf ../src/images/initrd.gz ./
	gzip -df initrd.gz
	mv initrd release/
else
	cp -rf ../src/images/userland.ext2.gz ./
	gzip -df userland.ext2.gz
	mv userland.ext2 release/boot/
fi
cp -rf ../src/images/sandbox.ext3.gz ./
gzip -df sandbox.ext3.gz
mv sandbox.ext3 release/boot/
echo "> Building backlight and cpu_speed..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
arm-uclinux-elf-gcc ../src/mod/backlight.c -o backlight -elf2flt
cp -rf backlight release/ZeroSlackr/usr/bin/
arm-uclinux-elf-gcc ../src/mod/cpu_speed.c -o cpu_speed -elf2flt
cp -rf cpu_speed release/ZeroSlackr/usr/bin/
# Documents
echo "> Copying documents..."
cd release
# Userland documents
DOCS=docs/userland
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
cp -rf "../../src/images/Loop-mount Instructions.txt" $DOCS/
mkdir $DOCS/Mod
cp -rf ../../src/mod/zeroslackr-userland-mod.tar.gz $DOCS/Mod/
# Project ZeroSlackr documents
cp -rf ../../../../Changelog.txt ./
cp -rf ../../../../FAQ.txt ./
cp -rf ../../../../Features.txt ./
cp -rf ../../../../Install.txt ./
cp -rf ../../../../License.txt ./
cp -rf "../../../../ReadMe from Keripo.txt" ./
cp -rf ../../../../Thanks.txt ./
cp -rf "../../../../To Do.txt" ./
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Archive documents
cd $DOCS
tar -cf Mod.tar Mod
gzip --best Mod.tar
rm -rf Mod
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="