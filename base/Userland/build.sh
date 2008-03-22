#!/bin/sh
#
# Userland Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 7, 2008
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
tar -xf ../src/release.tar.gz
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
cp -rf ../src/mod $DOCS/Mod
cp -rf "../src/images/Loop-mount Instructions.txt" $DOCS/
# Archive documents
cd release/boot/docs
tar -cf userland.tar userland
gzip --best userland.tar
rm -rf userland
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="