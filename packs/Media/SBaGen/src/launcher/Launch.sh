#!/bin/sh
#set -vx
#exec >> /opt/Media/SBaGen/Misc/Launch.log 2>&1

# Format: $binary $beat_path
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu 66
sleep 3s
if [ -z "$1" ]; then
	cd /opt/Media/SBaGen
	exec /opt/Media/SBaGen/SBaGen /opt/Media/SBaGen/Beats/basics/prog-chakras-1.sbg
else
	cd /opt/Media/SBaGen
	exec /opt/Media/SBaGen/SBaGen "\"$1\""
fi
