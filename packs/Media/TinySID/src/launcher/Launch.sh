#!/bin/sh
#set -vx
#exec >> /opt/Media/TinySID/Misc/Launch.log 2>&1

# Format: $binary $sid_path
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu 78
if [ -z "$1" ]; then
	cd /opt/Media/TinySID
	exec /opt/Media/TinySID/TinySID /opt/Media/TinySID/SIDs/Freeze.sid
else
	cd /opt/Media/TinySID
	exec /opt/Media/TinySID/TinySID "\"$1\""
fi
