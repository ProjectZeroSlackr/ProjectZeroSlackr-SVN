#!/bin/sh
#set -vx
#exec >> /opt/MPD/Misc/Initiate.log 2>&1

# This script is to be used by non-PZ2-based applications

/bin/ifconfig lo 127.0.0.1
/opt/MPD/MPD-ke /opt/MPD/Conf/mpd.conf