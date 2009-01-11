#!/bin/sh
#set -vx
#exec >> /opt/Media/hWolf3D/Misc/Launch.log 2>&1

killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 78 >> /dev/null
backlight brightness 12 >> /dev/null
cd /opt/Media/hWolf3D/Wolf3D-Retail
exec /opt/Media/hWolf3D/Wolf3D-Retail/Wolf3D-Retail
