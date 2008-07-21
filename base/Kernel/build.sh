#!/bin/sh
#
# Kernel Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 21, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Kernel Auto-Building Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[Kernel compiling not yet"
	echo " supported for SansaLinux - skipping]"
	echo ""
	echo "==========================================="
	echo ""
	exit
fi
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new build directory
echo "> Setting up build directory..."
mkdir build
cd build
BUILDDIR=$(pwd)
# Extract vanilla kernel source
echo "> Extracting vanilla kernel source..."
tar xjf ../src/orig/linux-2.4.32.tar.bz2
mv linux-2.4.32 compiling
cd compiling
# Required patches to kernel
echo "> Applying uClinux patches..."
zcat ../../src/orig/uClinux-2.4.32-uc0.diff.gz | patch -p1 >> ../build-patching.log
# Update with SVN
echo "> Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/linux/2.4 ../official-svn
cp -rf ../official-svn/* ./
# Apply ZeroSlackr patches
echo "> Applying patches..."
for file in ../../src/patches/*; do
	patch -p1 -t -i $file >> ../build-zspatching.log
done
# Compiling
echo "> Compiling..."
echo "  This will take a long time; please"
echo "  be patient while things compile."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - Copying over .config..."
# Since the script is automatic, this .config file
# already has "yes" to a few new things 
#cp arch/armnommu/def-configs/ipod .config
cp ../../src/mod/config .config
echo "  - make oldconfig..."
make oldconfig >> ../build-make-oldconfig.log 2>&1
echo "  - make dep..."
make dep >> ../build-make-dep.log 2>&1
echo "  - make boot..."
make boot >> ../build-make-boot.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
cp -rf compiling/arch/armnommu/boot/Image compiled/vmlinux
# Create release folder
echo "> Creating 'release' folder..."
cp -rf ../src/release ./
cd release
# Files
cp -r ../compiled/vmlinux boot/
# Documents
DOCS=docs/kernel
cp -rf "../../ReadMe from Keripo.txt" "$DOCS/ReadMe from Keripo.txt"
cp -rf ../../License.txt $DOCS/License.txt
cp -rf ../../src/patches $DOCS/Patches
cp -rf ../../src/mod $DOCS/Mod
DOCSORIG=$DOCS/Original
cp -rf ../compiling/COPYING $DOCSORIG/
cp -rf ../compiling/CREDITS $DOCSORIG/
cp -rf ../compiling/MAINTAINERS $DOCSORIG/
cp -rf ../compiling/README $DOCSORIG/
cp -rf ../compiling/REPORTING-BUGS $DOCSORIG/
# Too much and really not needed
#cp -rf ../compiling/Documentation $DOCSORIG/
cp -rf ../compiling/Documentation/kernel-parameters.txt $DOCS/
# Delete .svn folders - directory change done in case of previous failure
cd $BUILDDIR
cd release
sh -c "find -name '.svn' -exec rm -rf {} \;" >> /dev/null 2>&1
# Archive documents
cd docs/kernel
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
tar -cf Mod.tar Mod
gzip --best Mod.tar
rm -rf Mod
tar -cf Original.tar Original
gzip --best Original.tar
rm -rf Original
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="