#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Tools/Misc/file-Terminal.log 2>&1

stty erase "^H"
$1 "$2"
read -p "Press any key to exit..."
exit