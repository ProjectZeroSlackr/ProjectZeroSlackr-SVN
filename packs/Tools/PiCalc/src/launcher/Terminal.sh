#!/bin/sh
#set -vx
#exec >> /opt/Tools/PiCalc/Misc/Terminal.log 2>&1

stty erase "^H"
cd /opt/Tools/PiCalc
/opt/Tools/PiCalc/PiCalc
echo "PiCalc exited."
echo "Output saved to \"/opt/Tools/PiCalc/Output\"."
read -p "Press any key to exit..."
exit