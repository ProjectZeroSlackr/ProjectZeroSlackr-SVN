#!/bin/sh
#set -vx
exec >> /opt/Floyd2illA/Misc/FastLaunch.log 2>&1

cd /opt/Floyd2illA
exec /opt/Floyd2illA/Floyd2illA
