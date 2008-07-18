#!/bin/sh
#
# Userland Image Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 17, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Userland Image Auto-Building Script"
echo ""
# SansaLinux not supported yet
if [ $SANSA ]; then
	echo "[SansaLinux uses a different userland"
	echo " than iPodLinux - please use the"
	echo " pre-made userland instead]"
	echo ""
	echo "==========================================="
	echo ""
	exit
fi
echo "> Extracting"
tar -xf userland-zs-8mb.ext3.tar.gz
tar -xf userland-zs-16mb.ext3.tar.gz
tar -xf userland-zs-32mb.ext3.tar.gz
echo "> Mounting userland images..."
mkdir ipl-8mb
mkdir ipl-16mb
mkdir ipl-32mb
mount -t ext3 -o loop,rw userland-zs-8mb.ext3 ipl-8mb
mount -t ext3 -o loop,rw userland-zs-16mb.ext3 ipl-16mb
mount -t ext3 -o loop,rw userland-zs-32mb.ext3 ipl-32mb
echo "> Removing previous content..."
rm -rf ipl-8mb/*
rm -rf ipl-16mb/*
rm -rf ipl-32mb/*
echo "> Generating userland files..."
cd ipl-8mb
# Ignore all the "implausibly old time stamp" warnings
tar -xzf ../../orig/ipod_fs_220606.tar.gz >> /dev/null 2>&1
tar -xzf ../../mod/zeroslackr-userland-mod.tar.gz
echo "> Applying loop-mount modifications..."
cp -rf ../../mod/loop-mount.sh etc
cd etc
./loop-mount.sh
cd ../..
echo "> Duplicating content..."
cp -rf ipl-8mb/* ipl-16mb
cp -rf ipl-8mb/* ipl-32mb
echo "> Unmounting and cleanup..."
umount ipl-8mb
umount ipl-16mb
umount ipl-32mb
rm -rf ipl-8mb
rm -rf ipl-16mb
rm -rf ipl-32mb
echo "> Archiving..."
tar -cf userland-zs-8mb.ext3.tar userland-zs-8mb.ext3
gzip -f --best userland-zs-8mb.ext3.tar
rm -rf userland-zs-8mb.ext3
tar -cf userland-zs-16mb.ext3.tar userland-zs-16mb.ext3
gzip -f --best userland-zs-16mb.ext3.tar
rm -rf userland-zs-16mb.ext3
tar -cf userland-zs-32mb.ext3.tar userland-zs-32mb.ext3
gzip -f --best userland-zs-32mb.ext3.tar
rm -rf userland-zs-32mb.ext3
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="