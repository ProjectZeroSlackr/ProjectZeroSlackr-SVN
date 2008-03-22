#!/bin/sh
#
# Auto-Lumping Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 21, 2008
#
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "      Full ZeroSlackr Lumping Script"
echo "         [ because I'm lazy ; ) ]"
echo ""
echo "    This script will compile everything"
echo "    and stick it in a convenient '_lump'"
echo "    folder that can be copied directly"
echo "    to your iPod."
echo ""
echo "==========================================="
echo "==========================================="
echo ""
# release
LUMP="_lump"
if [ -d $LUMP ]; then
	rm -rf $LUMP
fi
mkdir $LUMP
# libs
cd libs
./build.sh
cd ..
# base
# Note: building the kernel takes too long ; P
#for folder in base/*
if [ ! -d base/Kernel/build/release ]; then
	cd base/Kernel
	./build.sh
	mv build/release release
	rm -rf build
	mkdir build
	mv release build/
	cd ../..
fi
cp -rf base/Kernel/build/release/* $LUMP/
BASE="base/Loader2 base/Userland base/ZeroLauncher"
for folder in $BASE
do
	cd $folder
	./build.sh
	# I need to find a better way of doing this;
	# can't seem to get it working with "mv"
	cp -rf build/release/* ../../$LUMP/
	rm -rf build
	cd ../..
done
# packs
for folder in packs/*
do
	cd $folder
	./build.sh
	cp -rf build/release/* ../../$LUMP/
	rm -rf build
	cd ../..
done
# done
echo ""
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "                   Fin!"
echo ""
echo "    Now just copy the entire content of"
echo "    the '_lump' folder to your iPod, run"
echo "    the 'patch.bat' or 'patch.sh' file,"
echo "    and everything will be 'installed' ; )"
echo ""
echo "         AutoLump script by Keripo"
echo ""
echo "==========================================="
echo "==========================================="
echo ""