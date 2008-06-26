#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/tar-c.log 2>&1

# Format: $binary -cf $target.zip $target
cd "$1"
tar -cf "$2.tar" "$2"
