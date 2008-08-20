#!/bin/sh
#
# Userland Image Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 19, 2008
#
echo ""
echo "==========================================="
echo ""
echo "Userland Image Auto-Building Script"
echo ""
# Cygwin
if uname -o 2>/dev/null | grep -i "Cygwin" >/dev/null; then
	SKIP=1
fi
# Mac OSX
if [ `uname` == "Darwin" ]; then
	SKIP=1
fi
if [ $SKIP ]; then
	echo "    Many parts of this process can only be"
	echo "    done on Linux."
	echo ""
	echo "Fin!"
	echo ""
	echo "Auto-Building script by Keripo"
	echo ""
	echo "==========================================="
	exit
fi
echo "> Cleanup..."
umount userland >> /dev/null 2>&1
umount sansa >> /dev/null 2>&1
umount sandbox >> /dev/null 2>&1
rm -rf userland.ext2 sandbox.ext3 userland sansa sandbox
rm -rf busybox getLoader2AutoExec
echo "> Creating images..."
# 2MB userland image, 2MB initrd, 4MB sandbox image
dd if=/dev/zero of=userland.ext2 bs=512 count=4096 >> /dev/null 2>&1
echo "  - Enter 'y' to the below:"
echo ""
mkfs.ext2 -q userland.ext2
echo ""
tune2fs -c 0 userland.ext2 >> /dev/null
dd if=/dev/zero of=initrd bs=512 count=4096 >> /dev/null 2>&1
echo "  - Enter 'y' to the below:"
echo ""
mkfs.ext2 -q initrd
echo ""
tune2fs -c 0 initrd >> /dev/null
dd if=/dev/zero of=sandbox.ext3 bs=512 count=8192 >> /dev/null 2>&1
echo "  - Enter 'y' to the below:"
echo ""
mkfs.ext3 -q sandbox.ext3
echo ""
tune2fs -c 0 sandbox.ext3 >> /dev/null
echo "> Mounting userland images..."
mkdir userland
mkdir sansa
mkdir sandbox
mount -t ext2 -o loop,rw userland.ext2 userland
mount -t ext2 -o loop,rw initrd sansa
mount -t ext3 -o loop,rw sandbox.ext3 sandbox
echo "> Removing previous content..."
rm -rf userland/*
rm -rf sansa/*
rm -rf sandbox/*
echo "> Generating userland files..."
cd userland
# Ignore all the "implausibly old time stamp" warnings
tar -xzf ../../orig/ipod_fs_220606.tar.gz >> /dev/null 2>&1
rm -rf home usr etc/rc.d
tar -xzf ../../mod/zeroslackr-userland-mod.tar.gz
cd ..
echo "> Building busybox..."
echo "  - Updating SVN..."
svn co --quiet svn://busybox.net/trunk/busybox busybox
cd busybox
echo "  - Applying patches..."
patch -p0 -t -i ../../mod/busybox/busybox-All.patch >> build.log
cp -rf ../../mod/busybox/userland.config .config
export PATH=/usr/local/arm-uclinux-tools2/bin:/usr/local/arm-uclinux-elf-tools/bin:/usr/local/arm-uclinux-tools/bin:$PATH
echo "  - make install..."
echo "    Note: If busybox building hangs for a"
echo "    long time, check 'busybox/build.log'"
echo "    for '(NEW)' settings. If there are any,"
echo "    press 'n' a few times. This may happen"
echo "    when new config items are added to the"
echo "    busybox SVN"
make install busybox_unstripped LDFLAGS="-Wl,-elf2flt" >> build.log
echo "  - Copying over files..."
cp -rf _install/bin/* ../userland/bin/
cp -rf _install/sbin/* ../userland/bin/
cd ..
echo "> Building getLoader2AutoExec..."
echo "  - Updating SVN..."
svn co --quiet https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/ipodloader2/getLoader2Args getLoader2AutoExec
echo "  - Applying patches..."
cd getLoader2AutoExec
patch -p0 -t -i ../../mod/getLoader2AutoExec.patch >> build.log
echo "  - make..."
export PATH=/usr/local/bin:$PATH
make >> build.log
echo "  - Copying over files..."
cp -rf getLoader2AutoExec ../userland/bin/
cd ..
echo "> Generating initrd files..."
cp -rf userland/* sansa/
cd sansa
tar -xzf ../../mod/zeroslackr-userland-mod-sansalinux.tar.gz
cd ..
echo "> Unmounting and cleanup..."
umount userland
umount sansa
umount sandbox
rm -rf userland sansa sandbox
rm -rf busybox getLoader2AutoExec
echo "> Archiving..."
gzip -f --best userland.ext2
gzip -f --best initrd
gzip -f --best sandbox.ext3
echo ""
echo "Fin!"
echo ""
echo "Auto-Building script by Keripo"
echo ""
echo "==========================================="