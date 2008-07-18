#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Tools/Misc/lol-Terminal.log 2>&1

stty erase "^H"
if [ -z "$1" ]; then
	lol
else
	lol < "$1"
fi
read -p "Press any key to exit..."
exit