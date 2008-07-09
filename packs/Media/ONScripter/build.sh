#!/bin/sh
#
# ONScripter Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 9, 2008
#

echo ""
echo "==========================================="
echo ""
echo "ONScripter Auto-Building Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	echo "[ONScripter doesn't seem to compile"
	echo " nicely on Cygwin - skipping]"
	echo ""
	echo "==========================================="
	echo ""
	exit
fi
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new compiling directory
echo "> Setting up build directory..."
mkdir build
cd build
# SVN source unstable and causes a bus error
#mkdir compiling
# Update with SVN
#echo "> Updating SVN..."
#svn co --quiet http://svn.haeleth.net/onscr/trunk/ official-svn
#cp -r official-svn/* compiling/
# Extract source
echo "> Extracting source..."
tar -jxf ../src/orig/onscripter-en-20080214-src.tar.bz2
mv onscripter-en-20080214 compiling
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cp -rf ../src/mod/Makefile.iPodLinux compiling/
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
cd ..
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="bzip2 freetype libmad zlib libjpeg libpng SDL SDL_image SDL_mixer SDL_ttf ttk launch"
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
cd compiling
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make -f Makefile.iPodLinux >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/onscripter compiled/ONScripter-En
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
PACK=ZeroSlackr/opt/Media/ONScripter
cp -rf ../compiled/ONScripter-En $PACK/
cp -rf ../launcher/* $PACK/Launch/
# Because I only have a nano ; P ~Keripo
tar zxf "../../src/mod/Higurashi Demo (nano).tar.gz"
mv "Higurashi Demo (nano)" $PACK/VisualNovels/
#tar zxf "../../src/mod/Higurashi Demo (photo).tar.gz"
#mv "Higurashi Demo (photo)" $PACK/VisualNovels/
#tar zxf "../../src/mod/Higurashi Demo (video).tar.gz"
#mv "Higurashi Demo (video)" $PACK/VisualNovels/
cp -rf ../../src/tools $PACK/Tools
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
mkdir $PACK/Misc/Mod
cp -rf ../../src/mod/Makefile.iPodLinux $PACK/Misc/Mod/
#FILES="BUGS CHANGES COPYING README"
FILES="CHANGES COPYING README"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCS/
done
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