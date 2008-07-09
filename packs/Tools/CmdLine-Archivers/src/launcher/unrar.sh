#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/unrar.log 2>&1

# Format: $binary x -o+ -y $file
cd "\"$1\""
unrar x -o+ -y "\"$2\""
