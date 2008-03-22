#!/bin/sh
#
# ZacZilla Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 21, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ZacZilla Auto-Building Script"
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
# Copying full source
echo "> Copying over source..."
cp -r ../src/full/* compiling/
# Symlink the libraries
echo "> Compiling libraries..."
cd ..
cp -r src/libs build/
cd build
cd libs
sh AutoCompile.sh
rm -rf AutoCompile.sh
cd ..
TTK=../../../libs
if [ ! -d $TTK/ttk ]; then
	cd $TTK
	./build.sh
	cd ../packs/ZacZilla/build
fi
ln -s $TTK/ttk ttk
# Compiling
echo "> Compiling..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 SDL=1 >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/ZacZilla compiled/
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/ZacZilla
cp -rf ../compiled/ZacZilla $PACK/ZacZilla
# Documents
DOCS=$PACK/Misc/Docs
DOCSORIG=$DOCS/Original
cp -rf "../../ReadMe from Keripo.txt" $DOCS/
cp -rf ../../License.txt $DOCS/
# Archive documents
cd $PACK/Misc
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