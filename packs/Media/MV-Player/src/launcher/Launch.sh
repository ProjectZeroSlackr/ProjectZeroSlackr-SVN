#!/bin/sh
#set -vx
#exec >> /opt/Media/MV-Player/Misc/Launch.log 2>&1

# Format: $binary $video_path
killall -15 ZeroLauncher >> /dev/null 2>&1
# Set cpu_speed higher if  using higher compressed videos
cpu_speed 69
backlight brightness 12
if [ -z "$1" ]; then
	cd /opt/Media/MV-Player
	exec /opt/Media/MV-Player/MV-Player
else
	cd /opt/Media/MV-Player
	exec /opt/Media/MV-Player/MV-Player-CLI "\"$1\""
fi
