#!/bin/sh
#set -vx
exec >> /opt/Zillae/Floyd2illA/Misc/FastLaunch.log 2>&1

killall -15 ZeroLauncher
cd /opt/Zillae/Floyd2illA
exec /opt/Zillae/Floyd2illA/Floyd2illA
