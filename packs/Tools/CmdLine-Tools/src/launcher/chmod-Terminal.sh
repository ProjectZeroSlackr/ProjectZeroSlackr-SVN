#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Tools/Misc/chmod-Terminal.log 2>&1

stty erase "^H"
echo "chmod -c -R ugo+rwx \"$1\""
chmod -c -R ugo+rwx "$1"
read -p "Press any key to exit..."
exit