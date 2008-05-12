#!/bin/sh
#set -vx
#exec >> /opt/Emulators/igpSP/Misc/Launch-with-sound.log 2>&1

# Format: $binary $rom_path
if [ -z "$1" ]; then
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-with-sound /opt/Emulators/igpSP/Roms/SuperWings.gba
else
	cd /opt/Emulators/igpSP/Roms
	exec /opt/Emulators/igpSP/igpSP-with-sound "$1"
fi
