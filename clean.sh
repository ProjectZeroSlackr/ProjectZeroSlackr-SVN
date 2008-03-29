#!/bin/sh
#
# Auto-Cleaning Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 28, 2008
#
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "      Full ZeroSlackr Cleaning Script"
echo "         [ because I'm lazy ; ) ]"
echo ""
# lump
LUMP="_lump"
if [ -d $LUMP ]; then
	echo "> Removing $LUMP..."
	rm -rf $LUMP
fi
# libs
echo "> Cleaning libs..."
cd libs
./clean.sh
cd ..
# base
echo "> Cleaning base..."
# Note: building the kernel takes too long ; P
if [ -d base/Kernel/build/release ]; then
	echo "  - Keeping kernel..."
	mv base/Kernel/build/release base/Kernel/release
	mv base/Kernel/build/.svn base/Kernel/.svn-release
	rm -rf base/Kernel/build
	mkdir base/Kernel/build
	mv base/Kernel/release base/Kernel/build/
	mv base/Kernel/.svn-release base/Kernel/build/.svn
else
	rm -rf base/Kernel/build
fi
#for folder in base/*
BASE="base/Loader2 base/Userland base/ZeroLauncher"
for folder in $BASE
do
	rm -rf $folder/build
done
# packs
echo "> Cleaning packs..."
for folder in packs/*
do
	rm -rf $folder/build
done
# backups
./rm-backups.sh
# done
echo ""
echo "                   Fin!"
echo ""
echo "         AutoClean script by Keripo"
echo ""
echo "==========================================="
echo "==========================================="
echo ""