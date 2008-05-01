#!/bin/sh
#set -vx
#exec >> /opt/Tools/Flite/Misc/Convert.log 2>&1

# Format: $binary $text_path $wav_path
cd /opt/Tools/Flite
exec /opt/Tools/Flite/Flite -f "$1" -o "$1.wav"
