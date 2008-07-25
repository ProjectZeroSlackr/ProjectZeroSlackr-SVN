#!/bin/sh
#set -vx
#exec >> /opt/Emulators/igpSP/Misc/Launch-with-sound.log 2>&1

# Format: $binary $rom_path
killall -15 ZeroLauncher >> /dev/null 2>&1
if [ -z "$1" ]; then
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-with-sound
else
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-with-sound "\"$1\""
fi
