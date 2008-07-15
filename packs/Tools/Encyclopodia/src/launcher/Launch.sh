#!/bin/sh
#set -vx
exec >> /opt/Tools/Encyclopodia/Misc/Launch.log 2>&1

killall -15 ZeroLauncher
cd /opt/Tools/Encyclopodia
exec /opt/Tools/Encyclopodia/Encyclopodia
