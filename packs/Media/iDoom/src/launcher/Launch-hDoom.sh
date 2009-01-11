#!/bin/sh
#set -vx
#exec >> /opt/Media/iDoom/Misc/Launch-hDoom.log 2>&1

# Format: $binary -file $pwad_path (-freedoom)
killall -15 ZeroLauncher >> /dev/null 2>&1
backlight brightness 12 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/hDoom
else
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/hDoom -file "\"$1\"" "$2"
fi
