#!/bin/sh
#set -vx
#exec >> /opt/Media/TinySID/Misc/Launch.log 2>&1

# Format: $binary $sid_path
cpu_speed 78 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Media/TinySID
	/opt/Media/TinySID/TinySID /opt/Media/TinySID/SIDs/Freeze.sid
else
	cd /opt/Media/TinySID
	/opt/Media/TinySID/TinySID "$1"
fi
cpu_speed 72 >> /dev/null
