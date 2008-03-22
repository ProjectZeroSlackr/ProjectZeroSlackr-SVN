#!/bin/sh
#set -vx
#exec >> /opt/iBoy/Misc/FastLaunch.log 2>&1

# iBoy asks for command arguments in the form
# "basename dirname"
# e.g. the rom "/opt/iBoy/Roms/ChipTheChick.gb"
# uses "exec /opt/iBoy/iBoy "ChipTheChick.gb" /opt/iBoy/Roms/"

cd /opt/iBoy/Roms
exec /opt/iBoy/iBoy