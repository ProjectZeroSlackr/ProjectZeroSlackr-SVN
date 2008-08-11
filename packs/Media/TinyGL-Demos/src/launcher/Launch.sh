#!/bin/sh
#set -vx
#exec >> /opt/Media/TinyGL-Demos/Misc/$1.log 2>&1

# Format: $binary
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 78
cd /opt/Media/TinyGL-Demos
exec /opt/Media/TinyGL-Demos/$1
