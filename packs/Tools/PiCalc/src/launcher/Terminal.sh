#!/bin/sh
#set -vx
#exec >> /opt/PiCalc/Misc/Terminal.log 2>&1

stty erase "^H"
cd /opt/PiCalc
/opt/PiCalc/PiCalc
echo "PiCalc exited."
echo "Output saved to \"/opt/PiCalc/Output\"."
read -p "Press any key to exit..."
exit