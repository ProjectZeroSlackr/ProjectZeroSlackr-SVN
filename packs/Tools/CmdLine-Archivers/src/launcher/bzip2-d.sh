#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/bzip2-d.log 2>&1

# Format: $binary -vd $file
cd "$1"
bzip2 -vd "$2"
