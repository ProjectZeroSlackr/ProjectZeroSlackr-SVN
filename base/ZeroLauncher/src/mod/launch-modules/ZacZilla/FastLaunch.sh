#!/bin/sh
#set -vx
exec >> /opt/ZacZilla/Misc/FastLaunch.log 2>&1

cd /opt/ZacZilla
exec /opt/ZacZilla/ZacZilla
