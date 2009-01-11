#!/bin/sh
#set -vx
#exec >> /opt/Emulators/iGameGear/Misc/Launch.log 2>&1

# Format: $binary $rom_path --fskip $# --display $type (--japan)
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 78 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Emulators/iGameGear
	exec /opt/Emulators/iGameGear/iGameGear /opt/Emulators/iGameGear/Roms/Nibbles.gg --fskip 5 --display center
else
	cd /opt/Emulators/iGameGear
	exec /opt/Emulators/iGameGear/iGameGear "\"$1\"" --fskip "$2" --display "$3" "$4"
fi
