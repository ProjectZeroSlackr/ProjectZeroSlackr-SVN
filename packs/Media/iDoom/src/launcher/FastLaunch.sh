#!/bin/sh
#set -vx
#exec >> /opt/iDoom/Misc/FastLaunch.log 2>&1

# There are currently two binaries in use;
# one which uses the "doom2.wad" file (iDoom)
# and one which uses the "freedoom.wad" file
# (iFreedoom). You will have to experiment
# to see which binary works best with your
# PWAD. To load iDoom/iFreedoom with a PWAD,
# use the "-file" switch. For example:
# "exec /opt/iDoom/iDoom -file /opt/iDoom/PWADs/Chex.wad"
# "exec /opt/iDoom/iFreedoom -file /opt/iDoom/PWADs/HalfLife.wad"
# Just like in the original Doom, any number
# of files can follow the "-file" switch.

cd /opt/iDoom
exec /opt/iDoom/iDoom