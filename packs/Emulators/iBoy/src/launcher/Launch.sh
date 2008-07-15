#!/bin/sh
#set -vx
#exec >> /opt/Emulators/iBoy/Misc/Launch.log 2>&1

# Format: $binary $rom_file $rom_dir
killall -15 ZeroLauncher
if [ -z "$1" ]; then
	cd /opt/Emulators/iBoy/Roms
	exec /opt/Emulators/iBoy/iBoy ChipTheChick.gb /opt/Emulators/iBoy/Roms
else
	cd /opt/Emulators/iBoy/Roms
	exec /opt/Emulators/iBoy/iBoy "\"$1\"" "\"$2\""
fi
