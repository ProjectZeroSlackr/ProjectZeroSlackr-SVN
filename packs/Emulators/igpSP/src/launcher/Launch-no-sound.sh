#!/bin/sh
#set -vx
#exec >> /opt/Emulators/igpSP/Misc/Launch-no-sound.log 2>&1

# Format: $binary $rom_path
killall -15 ZeroLauncher
if [ -z "$1" ]; then
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-no-sound /opt/Emulators/igpSP/Roms/SuperWings.gba
else
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-no-sound "\"$1\""
fi
