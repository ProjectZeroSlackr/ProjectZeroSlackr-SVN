#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/unzip.log 2>&1

# Format: $binary -o $file
cd "\"$1\""
unzip -o "\"$2\""
