#!/bin/sh
#set -vx
#exec >> /opt/Media/TinyGL-Demos/Misc/ImageViewer.log 2>&1

# Format: $binary $pic_folder
cd /opt/Media/TinyGL-Demos
exec /opt/Media/TinyGL-Demos/ImageViewer "\"$1\"/*"
