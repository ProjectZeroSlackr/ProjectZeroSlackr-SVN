#!/bin/sh
#set -vx
#exec >> /opt/Media/iDoom/Misc/Launch-hDoom.log 2>&1

# Format: $binary -file $pwad_path (-freedoom)
if [ -z "$1" ]; then
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/hDoom
else
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/hDoom -file "\"$1\"" "$2"
fi
