#!/bin/sh
#
# Hotdog-Demos Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 21, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Hotdog-Demos Auto-Building Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[Hotdog-Demos compiling not yet"
	echo " supported for SansaLinux - skipping]"
	echo ""
	echo "==========================================="
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
BUILDDIR=$(pwd)
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
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Building hotdog demos..."
cd $LIBSDIR/hotdog/demos
make clean >> $DIR/build.log 2>&1
make IPOD=1 >> $DIR/build.log 2>&1
cd ..
echo "  - Updating hotdog-ex SVN..."
if [ ! -d hotdog-ex ]; then
	svn co --quiet http://svn.so2.sytes.net/repos/ipod/hotdog-ex/ hotdog-ex
fi	
cd hotdog-ex
echo "  - Applying ZeroSlackr patches..."
patch -p0 -t -i $DIR/../src/patches/hotdog-ex-all.patch >> $DIR/build.log
echo "  - Building hotdog-ex demos..."
make clean >> $DIR/build.log 2>&1
make bolly IPOD=1 >> $DIR/build.log 2>&1
make bouncy IPOD=1 >> $DIR/build.log 2>&1
make imagebits IPOD=1 >> $DIR/build.log 2>&1
make spinner IPOD=1 >> $DIR/build.log 2>&1
cd $DIR
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
cp -rf $LIBSDIR/hotdog/demos/anim compiled/Anim-Demo
cp -rf $LIBSDIR/hotdog/demos/hdpong compiled/HD-Pong
cp -rf $LIBSDIR/hotdog/demos/select compiled/Select-Demo
cp -rf $LIBSDIR/hotdog/demos/prim compiled/Prim-Demo
mkdir compiled/Hotdog-EX
cp -rf $LIBSDIR/hotdog/hotdog-ex/bolly compiled/Hotdog-EX
cp -rf $LIBSDIR/hotdog/hotdog-ex/bouncy compiled/Hotdog-EX
cp -rf $LIBSDIR/hotdog/hotdog-ex/spinner compiled/Hotdog-EX
mkdir compiled/Hotdog-EX/PNGs
cp -rf $LIBSDIR/hotdog/hotdog-ex/imagebits compiled/Hotdog-EX
cp -rf $LIBSDIR/hotdog/hotdog-ex/jerry.png compiled/Hotdog-EX/PNGs/
cp -rf $LIBSDIR/hotdog/demos/anim/bg.png compiled/Hotdog-EX/PNGs/stormy.png
cp -rf $LIBSDIR/hotdog/demos/prim/bg.png compiled/Hotdog-EX/PNGs/kore.png
cp -rf $LIBSDIR/hotdog/demos/select/bg.png compiled/Hotdog-EX/PNGs/blue-sun.png
rm -rf compiled/*/*.c
rm -rf compiled/*/*.o
rm -rf compiled/*/*.gdb
rm -rf compiled/*/Makefile
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
PACK=ZeroSlackr/opt/Media/Hotdog-Demos
cp -rf ../compiled/* $PACK/
cp -rf ../launcher/* $PACK/Launch/
# Documents
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="