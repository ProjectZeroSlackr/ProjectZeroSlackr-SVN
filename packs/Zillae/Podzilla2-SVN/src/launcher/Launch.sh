#!/bin/sh
#set -vx
exec >> /opt/Zillae/Podzilla2-SVN/Misc/Launch.log 2>&1

# Format: $binary -l $module_dirs
killall -15 ZeroLauncher >> /dev/null 2>&1
if [ -z "$1" ]; then
	cd /opt/Zillae/Podzilla2-SVN
	# Since module loading is recursive, just using "/usr/lib" is fine
	# Podzilla2-SVN already has "/usr/lib" built-in so no arguments are needed
	#exec /opt/Zillae/Podzilla2-SVN/Podzill2-SVN -l "/usr/lib/All:/usr/lib/Arcade:/usr/lib/Dev:/usr/lib/Media:/usr/lib/Misc:/usr/lib/Tools:/usr/lib/Unsorted"
	exec /opt/Zillae/Podzilla2-SVN/Podzilla2-SVN
else
	cd /opt/Zillae/Podzilla2-SVN
	exec /opt/Zillae/Podzilla2-SVN/Podzilla2-SVN -l "$1"
fi
