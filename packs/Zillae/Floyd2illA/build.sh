#!/bin/sh
#
# Floyd2illA Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Apr 14, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Floyd2illA Auto-Building Script"
echo ""
# Cleanup
if [ -d build ]; then
	echo "> Removing old build directory..."
	rm -rf build
fi
# Make new compiling directory
echo "> Setting up build directory..."
mkdir build
cd build
mkdir compiling
# Extract source
# Repackaged source files; original no longer available
echo "> Extracting source..."
cd compiling
tar zxf ../../src/repack/src_orig_repacked.tar.gz
cd ..
# Apply ZeroSlackr custom patches
echo "> Applying ZeroSlackr patches..."
cp -r ../src/mod/* compiling/
cd compiling
for file in ../../src/patches/*; do
	patch -p0 -t -i $file >> ../build.log
done
# Symlink the libraries
echo "> Symlinking libraries..."
cd ..
for library in ../../../../libs/pz0/libs/*
do
	ln -s $library ./
done
# Compiling
echo "> Compiling..."
echo "  Note: All warnings/errors here will"
echo "  be logged to the 'build.log' file."
echo "  If building fails, check the log file."
cd compiling
export PATH=/usr/local/bin:$PATH
#Note: if you want to compile with MikMod suppot, see the
#"ReadMe from Keripo.txt" in the "mikmod" library folder
# in "/libs/pz0/libs" and compile with:
#make IPOD=1 MPDC=1 MIKMOD=1 >> ../build.log
make IPOD=1 MPDC=1 >> ../build.log 2>&1
# Copy over compiled file
echo "> Copying over compiled files..."
cd ..
mkdir compiled
if [ -e compiling/Floyd2illA.elf.bflt ]; then
	cp -rf compiling/Floyd2illA.elf.bflt compiled/Floyd2illA
else
	cp -rf compiling/Floyd2illA compiled/Floyd2illA
fi
# Creating release
echo "> Creating 'release' folder..."
tar -xf ../src/release.tar.gz
cd release
# Files
PACK=ZeroSlackr/opt/Floyd2illA
cp -rf ../compiled/Floyd2illA $PACK/
# Documents
# Too many original docs; done by hand
cp -rf "../../ReadMe from Keripo.txt" $PACK/
cp -rf ../../License.txt $PACK/
cp -rf ../../src/patches $PACK/Misc/Patches
cp -rf ../../src/mod $PACK/Misc/Mod
# Archive documents
cd $PACK/Misc
tar -cf Patches.tar Patches
gzip --best Patches.tar
rm -rf Patches
tar -cf Mod.tar Mod
gzip --best Mod.tar
rm -rf Mod
tar -cf Docs.tar Docs
gzip --best Docs.tar
rm -rf Docs
# Done
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="