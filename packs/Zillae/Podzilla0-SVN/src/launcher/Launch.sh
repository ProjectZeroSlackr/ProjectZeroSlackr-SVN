#!/bin/sh
#set -vx
exec >> /opt/Zillae/Podzilla0-SVN/Misc/FastLaunch.log 2>&1

killall -15 ZeroLauncher
cd /opt/Zillae/Podzilla0-SVN
exec /opt/Zillae/Podzilla0-SVN/Podzilla0-SVN
