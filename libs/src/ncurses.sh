#!/bin/sh
#
# ncurses Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 6, 2008
#
# Because ncurses takes way too long to compile
SKIP=1
if [ $SKIP == 1 ]; then
	echo ""
	echo "==========================================="
	echo ""
	echo "ncurses Auto-Compiling Script"
	echo ""
	echo "[ncurses takes way too long to compile"
	echo " from source - using pre-built files]"
	cp -rf src/ncurses/pre-built ncurses
	echo ""
	echo "==========================================="
	exit
fi
echo ""
echo "==========================================="
echo ""
echo "ncurses Auto-Compiling Script"
echo ""
# Cleanup
if [ -d ncurses ]; then
	echo "> Removing old ncurses directory..."
	rm -rf ncurses
fi
# Extract source
echo "> Extracting source..."
tar -xf src/ncurses/ncurses-5.6.tar.gz
mv ncurses-5.6 ncurses
# Compiling
cd ncurses
echo "> Compiling..."
echo "  Note: Compiling requires that your toolchain's"
echo "  ucontext.h is fixed and that you have ncurses"
echo "  installed on your host environment"
echo "  (e.g. native /bin/tic)"
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Running configure script..."
./configure CC=arm-uclinux-elf-gcc CFLAGS=-I$(pwd)/src/ncurses/ucontext-fix LDFLAGS=-elf2flt --host=arm-elf --with-build-cc=gcc --with-build-ldflags="" --prefix=/opt/Base/Ncurses --with-install-prefix=$(pwd)/installed --without-debug --without-cxx --without-cxx-binding --without-progs --disable-big-core --disable-largefile --disable-home-terminfo >> build.log 2>&1
echo "  - make..."
make >> build.log 2>&1
echo "  - make install..."
# Need local tic as iPL toolchain's doesn't work
export PATH=/bin:/usr/bin:$PATH
make install >> build.log 2>&1
# Cleanup
echo "  - cleanup..."
cd include
ln -s curses.h ncurses.h
cd ..
DIR=$(pwd)
cd installed/opt/Base/Ncurses
rm -rf bin include lib man
mkdir share/terminfo2
mkdir share/terminfo2/l
mkdir share/terminfo2/v
mkdir share/terminfo2/u
cp -rf share/terminfo/l/linux share/terminfo2/l/
cp -rf share/terminfo/v/vt102 share/terminfo2/v/
cp -rf share/terminfo/u/unknown share/terminfo2/u/
rm -rf share/terminfo
mv share/terminfo2 share/terminfo
cd $DIR
# Check
echo ""
cd ..
LIB=ncurses/lib/libncurses.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="
