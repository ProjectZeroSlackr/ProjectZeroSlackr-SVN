#!/bin/sh
#set -vx
#exec >> /opt/Media/iDoom/Misc/Launch-iDoom.log 2>&1

# Format: $binary -file $pwad_path (-freedoom)
killall -15 ZeroLauncher >> /dev/null 2>&1
backlight brightness 12 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/iDoom
else
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/iDoom -file "\"$1\"" "$2"
fi
