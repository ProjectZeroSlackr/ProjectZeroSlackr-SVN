#!/bin/sh
#
# hotdog Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 22, 2008
#
echo ""
echo "==========================================="
echo ""
echo "hotdog Auto-Compiling Script"
echo ""
# Cleanup
if [ -d hotdog ]; then
	echo "> Removing old hotdog directory..."
	rm -rf hotdog
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/hotdog/ hotdog
# Compiling
cd hotdog
echo "> Patching for SansaLinux..."
patch -p0 -t -i ../src/hotdog/hotdog-sansalinux.patch >> build.log
cp -rf ../src/hotdog/hotdog_lcd_sansa.S ./
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 >> build.log
echo ""
cd ..
LIB=hotdog/ipod/libhotdog.a
if [ -e $LIB ]; then
	echo "Fin!"
else
	echo "Error! Library not compiled. File \"$LIB\" not found!"
fi
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="