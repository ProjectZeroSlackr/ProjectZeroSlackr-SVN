#!/bin/sh
#set -vx
exec >> /opt/Zillae/viPodzilla/Misc/Launch.log 2>&1

killall -15 ZeroLauncher >> /dev/null 2>&1
cd /opt/Zillae/viPodzilla
exec /opt/Zillae/viPodzilla/viPodzilla
