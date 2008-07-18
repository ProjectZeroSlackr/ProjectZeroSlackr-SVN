#!/bin/sh
#
# hotdog Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "hotdog Auto-Compiling Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[hotdog compiling not yet"
	echo " supported for SansaLinux - skipping]"
	echo ""
	echo "==========================================="
	exit
fi
# Cleanup
if [ -d hotdog ]; then
	echo "> Removing old hotdog directory..."
	rm -rf hotdog
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/hotdog/ hotdog
# Compiling
echo "> Compiling..."
cd hotdog
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make IPOD=1 >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="