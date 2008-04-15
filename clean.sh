#!/bin/sh
#
# Auto-Cleaning Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 15, 2008
#
echo ""
echo "==========================================="
echo "==========================================="
echo ""
echo "      Full ZeroSlackr Cleaning Script"
echo "         [ because I'm lazy ; ) ]"
echo ""
# lump
SVNROOT=$(pwd)
LUMP=$SVNROOT"/_lump"
if [ -d $LUMP ]; then
	echo "> Removing "$LUMP"..."
	rm -rf $LUMP
fi
# libs
echo "> Cleaning libs..."
cd $SVNROOT
cd libs
./clean.sh
# base
echo "> Cleaning base..."
cd $SVNROOT
cd base
for folder in ./*
do
	cd $folder
	if [ -e SKIP.txt ]; then
		echo "  - Skipping cleaning of "$folder"..."
	else
		rm -rf build
	fi
	cd ..
done
# packs
echo "> Cleaning packs..."
cd $SVNROOT
cd packs
for folder in ./*
do
	echo "  - Cleaning packs in "$folder"..."
	cd $folder
	for pack in ./*
	do
		cd $pack
		if [ -e SKIP.txt ]; then
			echo "  - Skipping cleaning of "$pack"/"$folder"..."
		else
			rm -rf build
		fi
		cd ..
	done
	cd ..
done
# backups
cd $SVNROOT
./rm-backups.sh
# done
echo ""
echo "                   Fin!"
echo ""
echo "      Auto-Cleaning Script by Keripo"
echo ""
echo "==========================================="
echo "==========================================="
echo ""