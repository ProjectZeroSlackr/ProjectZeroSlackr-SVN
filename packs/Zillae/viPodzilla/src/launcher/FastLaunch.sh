#!/bin/sh
#set -vx
exec >> /opt/viPodzilla/Misc/FastLaunch.log 2>&1

cd /opt/viPodzilla
exec /opt/viPodzilla/viPodzilla
