#!/bin/sh
#
# CmdLine-Tools Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 18, 2008
#
echo ""
echo "==========================================="
echo ""
echo "CmdLine-Tools Auto-Building Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN=1
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
# Extract source
echo "> Extracting source..."
tar zxf ../src/orig/Ipodgawk.tar.gz
tar zxf ../src/orig/Ipodgrep.tar.gz
tar zxf ../src/orig/Ipodsed.tar.gz
tar zxf ../src/orig/diffutils-2.8.1.tar.gz
tar zxf ../src/orig/john-1.7.2.tar.gz
unzip -q ../src/orig/lolcode.zip -d ./
# Symlink the libraries
echo "> Symlinking libraries..."
DIR=$(pwd)
LIBSDIR=../../../../libs
LIBS="ttk launch"
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
echo "  - diffutils"
if [ $CYGWIN ]; then
	echo "    Note: diffutils doesn't seem to"
	echo "    compile nicely on Cygwin - skipping"
else
	cd diffutils-2.8.1
	./configure CC=arm-elf-gcc LDFLAGS=-elf2flt --host=arm-elf >> ../diffutils.log 2>&1
	make >> ../diffutils.log 2>&1
	cd ..
fi
echo "  - cmdlineutils"
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/cmdlineutils
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/libipod
export PATH=/usr/local/bin:$PATH
cd libipod
make IPOD=1 >> ../cmdlineutils.log 2>&1
cd ..
cd cmdlineutils
make IPOD=1 >> ../cmdlineutils.log 2>&1
cd ..
echo "  - John the Ripper"
cd john-1.7.2
patch -p0 -t -i ../../src/patches/john-the-ripper-arm.patch >> ../john.log
cd src
export PATH=/usr/local/bin:$PATH
make linux-arm >> ../../john.log 2>&1
cd ../..
echo "  - lolcode flex and bison interpreter"
cd lolcode
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
cp -rf ../../src/mod/lolcode/* ./
./build-ipod >> ../lolcode.log 2>&1
cd ..
# Copy over compiled file
echo "> Copying over compiled files..."
mkdir compiled
if [ $CYGWIN ]; then
	echo "  Note: skipping diffutils"
else
	cp -rf diffutils-2.8.1/src/cmp compiled/
	cp -rf diffutils-2.8.1/src/diff compiled/
	cp -rf diffutils-2.8.1/src/diff3 compiled/
	cp -rf diffutils-2.8.1/src/sdiff compiled/
fi
cp -rf gawk/gawk compiled/
cp -rf grep/grep compiled/
cp -rf sed/sed compiled/
cp -rf cmdlineutils/asciichart compiled/
cp -rf cmdlineutils/backlight compiled/
cp -rf cmdlineutils/contrast compiled/
cp -rf cmdlineutils/font compiled/
cp -rf cmdlineutils/lsi compiled/
cp -rf cmdlineutils/pause compiled/
cp -rf cmdlineutils/raise compiled/
if [ -e john-1.7.2/run/john.elf.bflt ]; then
	cp -rf john-1.7.2/run/john.elf.bflt compiled/john
else
	cp -rf john-1.7.2/run/john compiled/
fi
cp -rf lolcode/lol compiled/
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
PACK=ZeroSlackr/opt/Tools/CmdLine-Tools
cp -rf ../compiled/* $PACK/Bin/
cp -rf ../john-1.7.2/run/*.chr $PACK/John/
cp -rf ../john-1.7.2/run/john.conf $PACK/John/
cp -rf ../john-1.7.2/run/password.lst $PACK/John/
cp -rf ../lolcode/example.lol $PACK/LOLCODE/
cp -rf ../launcher/* $PACK/Launch/
# Documents
# Too many original docs; done by hand
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Archive documents
#cd $PACK/Misc
#tar -cf Docs.tar Docs
#gzip --best Docs.tar
#rm -rf Docs
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="