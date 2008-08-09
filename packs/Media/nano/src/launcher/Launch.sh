#!/bin/sh
#set -vx
#exec >> /opt/Media/nano/Misc/Launch.log 2>&1

killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 75
backlight brightness 12
cd /opt/Media/nano
exec /opt/Media/nano/nano
