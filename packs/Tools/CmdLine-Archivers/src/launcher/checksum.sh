#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/checksum.log 2>&1

# Format: $binary $file
echo "md5sum \"$1\""
md5sum "$1"
echo "sha1sum \"$1\""
sha1sum "$1"
echo "sum \"$1\""
sum "$1"
read -p "Press any key to exit..."
exit
