#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/bzip2-c.log 2>&1

# Format: $binary -vz9 $file
cd "\"$1\""
bzip2 -vz9 "\"$2\""
