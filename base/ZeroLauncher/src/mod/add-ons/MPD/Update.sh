#!/bin/sh
#set -vx
#exec >> /opt/Base/MPD/Misc/Initiate.log 2>&1

# This script is to be used by non-PZ2-Based applications
/bin/ifconfig lo 127.0.0.1
/opt/Base/MPD/MPD-ke --update-db /opt/Base/MPD/Conf/mpd.conf