#!/bin/sh
#set -vx
#exec >> /opt/Zillae/TimeWalk/Misc/Owen-OS.log 2>&1

stty erase "^H"
cd /opt/Zillae/TimeWalk/Owen-OS
echo "Controls: (h)old, (m)enu, (f)orward, re(w)ind, play(p), enter(centre)."
/opt/Zillae/TimeWalk/Owen-OS/owenipod
echo "Owen-OS exited."
read -p "Press any key to exit..."
exit