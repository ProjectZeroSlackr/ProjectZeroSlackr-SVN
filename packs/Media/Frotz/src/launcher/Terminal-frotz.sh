#!/bin/sh
#set -vx
#exec >> /opt/Media/Frotz/Misc/Terminal.log 2>&1

# Format: $binary $game_file_path
stty erase "^H"
/opt/Media/Frotz/Frotz "$1"
read -p "Press any key to exit..."
exit