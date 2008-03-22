#!/bin/sh
#
# Executing this while in the /etc folder
# to convert the iPL userland to a
# ZeroSlackr loop-mount partition.

# Remove old folders
rm -rf ../root
rm -rf ../opt
rm -rf ../home
rm -rf ../etc/rc.d
rm -rf ../usr

# Create new symlinks
ln -s /mnt/ZeroSlackr ../root
ln -s /root/opt ../opt
ln -s /root/home ../home
ln -s /root/etc/rc.d ../etc/rc.d
ln -s /root/usr ../usr