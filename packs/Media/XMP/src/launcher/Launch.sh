#!/bin/sh
#set -vx
#exec >> /opt/Media/XMP/Misc/Launch.log 2>&1

# Format: $binary $mod_path
cpu_speed 75 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Media/XMP
	/opt/Media/XMP/XMP /opt/Media/XMP/Mods/a-depres.xm
else
	cd /opt/Media/XMP
	/opt/Media/XMP/XMP "$1"
fi
cpu_speed 72 >> /dev/null
