#!/bin/sh
# Last updated: Apr 17, 2008
# ~Keripo

echo "====================================================="
echo "Project ZeroSlackr Auto-Patching Script"
echo "by Keripo"
echo ""
echo "- Patching iPod with Loader2..."
echo ""
if [ `uname` == "Darwin" ]; then
	./ipodpatcher-mac -ab loader.bin
else
	./ipodpatcher -ab loader.bin
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
	echo "- Patching files will be moved to the boot/patch"
	echo "  folder; if you ever need them again, copy them"
	echo "  to the root of your iPod and execute this script."
	echo ""
	echo "====================================================="
	mkdir -p boot/patch
	mv -f loader.bin boot/patch/
	mv -f ipodpatcher* boot/patch/
	read -p "Press any key to exit . . ."
	mv -f patch.bat boot/patch/
	mv -f patch.sh boot/patch/
fi