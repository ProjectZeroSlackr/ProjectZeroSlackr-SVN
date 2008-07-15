#!/bin/sh
#set -vx
#exec >> /opt/Media/XMP/Misc/Launch.log 2>&1

# Format: $binary $mod_path
killall -15 ZeroLauncher
if [ -z "$1" ]; then
	cd /opt/Media/XMP
	exec /opt/Media/XMP/XMP /opt/Media/XMP/Mods/a-depres.xm
else
	cd /opt/Media/XMP
	exec /opt/Media/XMP/XMP "\"$1\""
fi
