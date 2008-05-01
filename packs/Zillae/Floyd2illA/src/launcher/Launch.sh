#!/bin/sh
#set -vx
exec >> /opt/Zillae/Floyd2illA/Misc/FastLaunch.log 2>&1

cd /opt/Zillae/Floyd2illA
exec /opt/Zillae/Floyd2illA/Floyd2illA
