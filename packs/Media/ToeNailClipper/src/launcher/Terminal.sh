#!/bin/sh
#set -vx
#exec >> /opt/Media/ToeNailClipper/Misc/Terminal.log 2>&1

stty erase "^H"
cd /opt/Media/ToeNailClipper
/opt/Media/ToeNailClipper/ToeNailClipper
echo "ToeNailClipper exited."
echo "Hope you had fun."
read -p "Press any key to exit..."
exit