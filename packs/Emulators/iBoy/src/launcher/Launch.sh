#!/bin/sh
#set -vx
#exec >> /opt/Emulators/iBoy/Misc/Launch.log 2>&1

# Format: $binary $rom_file $rom_dir
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 75 >> /dev/null
if [ -z "$1" ]; then
	cd /opt/Emulators/iBoy/Roms
	# Chip the Chick example
	# Note that it is recommended that there be no spaces in the path
	# as rom loading screws up if the "SaveForRom" option is selected
	# and a .conf file is created for that rom
	#exec /opt/Emulators/iBoy/iBoy ChipTheChick.gb /opt/Emulators/iBoy/Roms
	exec /opt/Emulators/iBoy/iBoy
else
	cd /opt/Emulators/iBoy/Roms
	exec /opt/Emulators/iBoy/iBoy "\"$1\"" "\"$2\""
fi
