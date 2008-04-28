#!/bin/sh
#set -vx
exec >> /opt/Base/ZeroLauncher/Misc/Launch.log 2>&1

# ZeroLauncher script - called at startup
# Format: $binary -l $module_dirs
if [ -z $1 ]; then
	cd /opt/Base/ZeroLauncher
	# ZeroLauncher has location hard-coded in
	# /opt will also work since ZeroLauncher searches for launch
	# modules recursively but this is done such that
	# /opt/Base/ZeroLauncher/Add-ons is loaded first
	#exec /opt/Base/ZeroLauncher/ZeroLauncher -l /opt/Base:/opt/Emulators:/opt/Media:/opt/Tools:/opt/Zillae
	exec /opt/Base/ZeroLauncher/ZeroLauncher
else
	cd /opt/Base/ZeroLauncher
	exec /opt/Base/ZeroLauncher/ZeroLauncher -l "$1"
fi
