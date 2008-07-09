#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/gzip-d.log 2>&1

# Format: $binary -vd $file
cd "\"$1\""
gzip -vd "\"$2\""
