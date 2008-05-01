#!/bin/sh
#set -vx
#exec >> /opt/Media/iDoom/Misc/Launch-iDoom.log 2>&1

# There are currently two binaries in use;
# one which uses the "doom2.wad" file (iDoom)
# and one which uses the "freedoom.wad" file
# (iFreedoom). You will have to experiment
# to see which binary works best with your
# PWAD. To load iDoom/iFreedoom with a PWAD,
# use the "-file" switch.
# Just like in the original Doom, any number
# of files can follow the "-file" switch.

# Format: $binary -file $pwad_path
if [ -z "$1" ]; then
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/iDoom
else
	cd /opt/Media/iDoom
	exec /opt/Media/iDoom/iDoom -file "$1"
fi
