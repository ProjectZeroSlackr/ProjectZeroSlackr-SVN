#!/bin/sh
#set -vx
exec >> /opt/Encyclopodia/Misc/FastLaunch.log 2>&1

cd /opt/Encyclopodia
exec /opt/Encyclopodia/Encyclopodia
