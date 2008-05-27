#!/bin/sh
#
# Userland Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: May 26, 2008
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
cp -rf ../src/release ./
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
tar -xf ../src/images/userland-zs-8mb.ext3.tar.gz
mv userland-zs-8mb.ext3 release/boot/userland.ext3
# Documents
echo "> Copying documents..."
DOCS=release/boot/docs/userland
cp -rf "../ReadMe from Keripo.txt" "$DOCS/ReadMe from Keripo.txt"
cp -rf ../License.txt $DOCS/License.txt
cp -rf "../src/images/Loop-mount Instructions.txt" $DOCS/
mkdir $DOCS/Mod
cp -rf ../src/mod/loop-mount.sh $DOCS/Mod/
cp -rf ../src/mod/zeroslackr-userland-mod.tar.gz $DOCS/Mod/
cp -rf ../../../Changelog.txt release/
cp -rf ../../../License.txt release/
cp -rf "../../../ReadMe from Keripo.txt" release/
cp -rf "../../../To Do.txt" release/
cp -rf "../../../FAQ.txt" release/
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