#!/bin/sh
#
# ttk Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Feb 25, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ttk Auto-Compiling Script"
echo ""
if [ -d ttk ]; then
	echo "> Removing old ttk directory..."
	rm -rf ttk
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/ttk/ ttk
# Compiling
cd ttk
if [ -e /bin/cygwin1.dll ]; then
	echo "> Reverting rev 2383..."
	echo "  (which breaks Cygwin compiling)"
	patch -p0 -t -i ../ttk-cygwin-compile.patch >> ../logs/build-ttk.log
fi
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make NOMWIN=1 NOX11=1 >> ../logs/build-ttk.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="