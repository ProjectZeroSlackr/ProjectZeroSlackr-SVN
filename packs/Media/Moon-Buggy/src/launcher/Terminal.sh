#!/bin/sh
#set -vx
#exec >> /opt/Media/Moon-Buggy/Misc/Terminal.log 2>&1

stty erase "^H"
cd /opt/Media/Moon-Buggy
/opt/Media/Moon-Buggy/Moon-Buggy
read -p "Press any key to exit..."
exit