#!/bin/sh
# Last updated: Oct 12, 2008
# ~Keripo

echo "====================================================="
echo "Project ZeroSlackr Auto-patch-files Script"
echo "by Keripo"
echo ""
echo "- patching iPod with Loader2..."
echo ""
if [ `uname` == "Darwin" ]; then
	echo "  Warning: Using an unpatched Mac version"
	echo "  of ipodpatcher; even if patching is successful,"
	echo "  this script may report failure. If you are"
	echo "  able to compile on Mac OS X, please contact me."
	echo "  Warning: If ipodpatcher gives you an unmounting"
	echo "  error and fails, copy this script file and the"
	echo "  'patch-files' folder to your computer and run"
	echo "  the script from your computer (instead of from"
	echo "  your iPod). Also make sure you run it from"
	echo "  Terminal with 'sudo'."
	./patch-files/ipodpatcher-mac -ab patch-files/loader.bin
elif [ 'uname -m' == "x86-64" ]; then
	echo "  Warning: Using an unpatched 64-bit Linux version"
	echo "  of ipodpatcher; even if patching is successful,"
	echo "  this script may report failure. If you are"
	echo "  able to compile on 64-bit Linux, please contact me."
	./patch-files/ipodpatcher-linux-64 -ab patch-files/loader.bin
else
	./patch-files/ipodpatcher-linux-32 -ab patch-files/loader.bin
fi
if [ $? != 0 ]; then
	echo ""
	echo "- ipodpatcher failed!"
	echo "- Make sure you have root privileges,"
	echo "  then try running this script again."
	echo ""
	echo "====================================================="
	read -p "Press any key to finish . . ."
else
	echo ""
	echo "- ipodpatcher successful!"
	echo ""
	echo "- Disconnect your iPod now and let it reboot"
	echo "  to Loader2. Select the \"ZeroSlackr\" option to"
	echo "  boot into ZeroSlackr or select the \"Apple OS\""
	echo "  option to boot into the original firmware."
	echo ""
	echo "- When in the Apple OS, force reboot your iPod to"
	echo "  go back to Loader2. For instructions on rebooting,"
	echo "  see: http://ipodlinux.org/Key_combinations"
	echo ""
	echo "- Make sure to read all of ZeroSlackr's documents"
	echo "  starting with \"ReadMe from Keripo.txt\""
	echo ""
	echo "- patch files will be moved to the boot/patch"
	echo "  folder; if you ever need them again, copy them"
	echo "  to the root of your iPod and execute this script."
	echo ""
	echo "====================================================="
	mkdir -p boot/patch
	mv -f patch-files boot/patch/
	read -p "Press any key to exit . . ."
	mv -f patch.bat boot/patch/
	mv -f patch.sh boot/patch/
fi