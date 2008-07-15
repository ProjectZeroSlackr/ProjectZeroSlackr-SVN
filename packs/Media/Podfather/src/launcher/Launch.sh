#!/bin/sh
#set -vx
#exec >> /opt/Media/Podfather/Misc/Launch.log 2>&1

killall -15 ZeroLauncher
cd /opt/Media/Podfather
exec /opt/Media/Podfather/Podfather
