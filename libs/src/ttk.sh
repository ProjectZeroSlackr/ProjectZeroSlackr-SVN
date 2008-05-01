#!/bin/sh
#
# ttk Auto-Compiling Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 27, 2008
#
# Requires hotdog
if [ ! -d hotdog ]; then
	echo ""
	echo "[ttk requires hotdog]"
	./src/hotdog.sh
	echo ""
fi
echo ""
echo "==========================================="
echo ""
echo "ttk Auto-Compiling Script"
echo ""
# Cygwin check
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	CYGWIN="yes"
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
if [ "${CYGWIN}" ]; then
	echo "> Reverting rev 2383..."
	echo "  (which breaks Cygwin compiling)"
	patch -p0 -t -i ../src/ttk/ttk-cygwin-compile.patch >> build.log
fi
echo "> Compiling..."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
make NOMWIN=1 NOX11=1 >> build.log
echo ""
echo "Fin!"
echo ""
echo "Auto-Compiling script by Keripo"
echo ""
echo "==========================================="