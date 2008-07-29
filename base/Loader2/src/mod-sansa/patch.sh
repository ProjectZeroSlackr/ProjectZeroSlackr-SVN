#!/bin/sh
# Last updated: July 29, 2008
# ~Keripo

echo "====================================================="
echo "Project ZeroSlackr Auto-patch-files Script"
echo "by Keripo"
echo ""
echo "Note: If you have a Rhapsody model, you should have"
echo "already copied over \"patch-files/rhapsody/pp5022.mi4\""
echo "to the root of your Sansa. If not, do so. You might"
echo "have to then run this script again."
echo ""
echo "- patching Sansa with Loader2..."
echo ""
echo "  Warning: Using an unpatched sansapatcher; even if"
echo "  patching is successful, this script may report"
echo "  failure"
echo ""
if [ `uname` == "Darwin" ]; then
	./patch-files/sansapatcher-mac
elif [ 'uname -m' == "x86-64" ]; then
	echo "  Error: I currently do not have any 64-bit sansapatcher"
	echo "  build. If you are able to compile on a 64-bit Linux"
	echo "  environment, please contact me."
	echo ""
	read -p "Press any key to exit . . ."
	exit
else
	./patch-files/sansapatcher-linux-32
fi
if [ $? != 0 ]; then
	echo ""
	echo "- sansapatcher failed!"
	echo "- Make sure you have root privileges,"
	echo "  then try running this script again."
	echo ""
	echo "====================================================="
	read -p "Press any key to finish . . ."
else
	echo ""
	echo "- sansapatcher successful!"
	echo ""
	echo "- Disconnect your Sansa now and hold the fastforward/"
	echo "  right button to boot into ZeroSlackr. Do not press"
	echo "  any key to boot into the Rockbox/original firmware."
	echo ""
	echo "- Make sure to read all of ZeroSlackr's documents"
	echo "  starting with \"ReadMe from Keripo.txt\""
	echo ""
	echo "- patch files will be moved to the boot/patch"
	echo "  folder; if you ever need them again, copy them"
	echo "  to the root of your Sansa and execute this script."
	echo ""
	echo "====================================================="
	mkdir -p boot/patch
	mv -f patch-files boot/patch/
	mv -f rhapsody boot/patch/
	read -p "Press any key to exit . . ."
	mv -f patch.bat boot/patch/
	mv -f patch.sh boot/patch/
fi