#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Archivers/Misc/zip.log 2>&1

# Format: $binary -r $target.zip $target
cd "\"$1\""
zip -r "\"$2.zip\"" "\"$2\""
