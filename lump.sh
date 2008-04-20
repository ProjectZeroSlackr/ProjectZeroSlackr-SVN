#!/bin/sh
./chmod-all.sh >> /dev/null 2>&1
#
# Auto-Lumping Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 17, 2008
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
if [ -e /bin/cygwin1.dll ]; then
	echo ""
	echo "    Building is recommended to be done on"
	echo "    Linux. If you are using Cygwin and"
	echo "    also have TortoiseSVN installed, SVN"
	echo "    downloading may sometimes fail. This"
	echo "    is a known TortoiseSVN issue. You will"
	echo "    have to either try recompiling the"
	echo "    packs disabling TortoiseSVN."
fi
echo ""
echo "==========================================="
echo "==========================================="
echo ""
# lump
SVNROOT=$(pwd)
LUMP=$SVNROOT"/_lump"
echo "> Creating "$LUMP" folder..."
if [ -d $LUMP ]; then
	rm -rf $LUMP
fi
mkdir $LUMP
# libs
echo "> Building libs..."
cd $SVNROOT
cd libs
./build.sh
# base
echo "> Building base..."
cd $SVNROOT
cd base
for folder in ./*
do
	cd $folder
	if [ -e SKIP.txt ]; then
		echo "  - Skipping building of "$folder"..."
		cp -rf build/release/* $LUMP/
	else
		./build.sh
		cp -rf build/release/* $LUMP/
		rm -rf build
	fi
	cd ..
done
# packs
echo "> Building packs..."
cd $SVNROOT
cd packs
for folder in ./*
do
	echo "  - Building packs in "$folder"..."
	cd $folder
	for pack in ./*
	do
		cd $pack
		if [ -e SKIP.txt ]; then
			echo "  - Skipping building of "$pack"/"$folder"..."
			cp -rf build/release/* $LUMP/
		else
			./build.sh
			cp -rf build/release/* $LUMP/
			rm -rf build
		fi
		cd ..
	done
	cd ..
done
# libs cleanup
echo "> Cleaning up lib..."
cd $SVNROOT
cd libs
./clean.sh
# .svn cleanup
echo "> Cleaning up .svn files..."
cd $LUMP
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# permissions
echo "> Setting all permissions..."
cd $SVNROOT
chmod -fR ugo+rwx $LUMP/*
# done
cd $SVNROOT
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
echo "       Auto-Lumping Script by Keripo"
echo ""
echo "==========================================="
echo "==========================================="
echo ""