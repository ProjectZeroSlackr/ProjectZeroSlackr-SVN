#!/bin/sh
#set -vx
#exec >> /root/Packs/iDarcNES/Misc/iDarcNES-FastLaunch.log 2>&1

#clear
cd /root/Packs/iDarcNES
exec /root/Packs/iDarcNES/iDarcNES /root/Files/iDarcNES/Roms/BombSweeper.nes
#>> /root/Packs/iDarcNES/Misc/iDarcNES-FastLaunch.log 2>&1
