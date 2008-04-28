#!/bin/sh
#set -vx
#exec >> /opt/Emulators/iDarcNES/Misc/Launch.log 2>&1

# Format: $binary $rom_path (--nosound)
if [ -z $1 ]; then
	cd /opt/Emulators/iDarcNES
	exec /opt/Emulators/iDarcNES/iDarcNES /opt/Emulators/iDarcNES/Roms/BombSweeper.nes
else
	cd /opt/Emulators/iDarcNES
	exec /opt/Emulators/iDarcNES/iDarcNES "$1" "$2"
fi
