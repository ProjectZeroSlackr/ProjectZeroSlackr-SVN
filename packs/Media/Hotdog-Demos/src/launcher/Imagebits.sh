#!/bin/sh
#set -vx
#exec >> /opt/Media/Hotdog-Demos/Misc/Imagebits.log 2>&1

# Format: $binary $png
cd /opt/Media/Hotdog-Demos/Hotdog-EX
exec /opt/Media/Hotdog-Demos/Hotdog-EX/Imagebits "\"$1\""
