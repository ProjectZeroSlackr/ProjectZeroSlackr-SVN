#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/gzip-c.log 2>&1

# Format: $binary -v9 $file
cd "\"$1\""
gzip -v9 "\"$2\""
