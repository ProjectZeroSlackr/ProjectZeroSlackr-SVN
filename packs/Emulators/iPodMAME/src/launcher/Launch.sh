#!/bin/sh
#set -vx
#exec >> /opt/Emulators/iPodMAME/Misc/Launch.log 2>&1

# Format: $binary $romset_name
killall -15 ZeroLauncher >> /dev/null 2>&1
cpu_speed 75
if [ -z "$1" ]; then
	cd /opt/Emulators/iPodMAME
	exec /opt/Emulators/iPodMAME/iPodMAME aarmada
else
	cd /opt/Emulators/iPodMAME
	exec /opt/Emulators/iPodMAME/iPodMAME "$1"
fi
