#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/tar-d.log 2>&1

# Format: $binary -xf $file
cd "\"$1\""
tar -xf "\"$2\""
