#!/bin/sh
# Last updated: Feb 21, 2009
# ~Keripo

echo "====================================================="
echo "Project ZeroSlackr Auto-patch-files Script"
echo "by Keripo"
echo ""
echo "- patching iPod with Loader2..."
echo ""
if [ `uname` == "Darwin" ]; then
	echo "  Error: ZeroSlackr does not support MacPods; please"
	echo "  use iTunes on a Windows computer to reformat your"
	echo "  iPod as a WinPod or install iPodLinux by following"
	echo "  instructions on the 'Manual Installation'"
	echo "   wiki page."
	echo ""
	echo "====================================================="
	read -p "Press any key to exit . . ."
elif [ 'uname -m' == "x86-64" ]; then
	echo "  Warning: Using an unpatched 64-bit Linux version"
	echo "  of ipodpatcher; even if patching is successful,"
	echo "  this script may report failure. If you are"
	echo "  able to compile on 64-bit Linux, please contact me."
	chmod +x ./patch-files/ipodpatcher-linux-64
	./patch-files/ipodpatcher-linux-64 -ab patch-files/loader.bin
else
	chmod +x ./patch-files/ipodpatcher-linux-32
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
	echo "====================================================="
	read -p "Press any key to exit . . ."
fi