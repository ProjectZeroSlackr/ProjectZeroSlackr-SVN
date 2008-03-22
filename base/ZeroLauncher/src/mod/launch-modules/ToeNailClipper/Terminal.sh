#!/bin/sh
#set -vx
#exec >> /opt/ToeNailClipper/Misc/Terminal.log 2>&1

stty erase "^H"
cd /opt/ToeNailClipper
/opt/ToeNailClipper/ToeNailClipper
echo "ToeNailClipper exited."
echo "Hope you had fun."
read -p "Press any key to exit..."
exit