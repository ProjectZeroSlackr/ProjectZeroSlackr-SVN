#!/bin/sh
#set -vx
#exec >> /root/Packs/iDarcNES/Misc/iDarcNES-FastLaunch.log 2>&1

#clear
cd /root/Packs/iGameGear
exec /root/Packs/iGameGear/iGameGear /root/Files/iGameGear/Roms/Nibbles.gg --fskip 4 --display center
#>> /root/Packs/iDarcNES/Misc/iDarcNES-FastLaunch.log 2>&1
