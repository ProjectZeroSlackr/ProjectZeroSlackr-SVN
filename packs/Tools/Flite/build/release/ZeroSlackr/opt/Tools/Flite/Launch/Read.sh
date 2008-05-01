#!/bin/sh
#set -vx
#exec >> /opt/Tools/Flite/Misc/Read.log 2>&1

# Format: $binary -f $text_file_path
if [ -z "$1" ]; then
	cd /opt/Tools/Flite
	exec /opt/Tools/Flite/Flite -f /opt/Tools/Flite/Text/Welcome.txt
else 
	cd /opt/Tools/Flite
	exec /opt/Tools/Flite/Flite -f "$1"
fi
