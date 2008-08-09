#!/bin/sh
#
# Moon-Buggy Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 9, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Moon-Buggy Auto-Building Script"
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
tar zxf ../src/orig/moon-buggy-0.5.52.tar.gz
mv moon-buggy-0.5.52 compiling
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="ncurses ttk launch"
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
echo "  - Running configure script..."
./configure --with-curses-includedir=../ncurses/include --with-curses-libs="-L../ncurses/lib -lncurses" --host=arm-elf >> ../build.log
echo "  - make..."
make CC=arm-uclinux-elf-gcc LDFLAGS=-elf2flt >> ../build.log
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf compiling/moon-buggy compiled/Moon-Buggy
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
PACK=ZeroSlackr/opt/Media/Moon-Buggy
cp -rf ../compiled/Moon-Buggy $PACK/
cp -rf ../launcher/* $PACK/Launch/
# Documents
DOCS=$PACK/Misc/Docs
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
FILES="ANNOUNCE AUTHORS ChangeLog COPYING moon-buggy.6 moon-buggy.info moon-buggy.lsm NEWS README TODO"
for file in $FILES
do
	cp -rf ../compiling/$file $DOCS/
done
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
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