#!/bin/sh
#
# ttk Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 14, 2008
#
echo ""
echo "==========================================="
echo ""
echo "ttk Auto-Compiling Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN=1
fi
# Cleanup
if [ -d ttk ]; then
	echo "> Removing old ttk directory..."
	rm -rf ttk
fi
# Update SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/libs/ttk/ ttk
# Compiling
cd ttk
if [ $CYGWIN ]; then
	echo "> Reverting rev 2383..."
	echo "  (which breaks Cygwin compiling)"
	patch -p0 -t -i ../src/ttk/ttk-cygwin-compile.patch >> build.log
fi
# I own the character design copyright for Noblesse, but not for Ren
# Besides, Noblesse looks better ; )
echo "> Patching with Noblesse icon..."
patch -p0 -t -i ../src/ttk/ttk-Noblesse-icon.patch >> build.log
#patch -p0 -t -i ../src/ttk/ttk-Ren-icon.patch >> build.log
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make NOMWIN=1 NOX11=1 NOHDOG=1 >> build.log 2>&1
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="