#!/bin/sh
#set -vx
#exec >> /opt/Media/MV-Player/Misc/Launch.log 2>&1

# Format: $binary $video_path
if [ -z $1 ]; then
	cd /opt/Media/MV-Player
	exec /opt/Media/MV-Player/MV-Player
else
	cd /opt/Media/MV-Player
	exec /opt/Media/MV-Player/MV-Player-CLI "$1"
fi
