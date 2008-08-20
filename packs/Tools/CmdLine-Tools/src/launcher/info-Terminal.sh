#!/bin/sh
#set -vx
#exec >> /opt/Tools/CmdLine-Tools/Misc/info-Terminal.log 2>&1

stty erase "^H"
echo "> hostname, id"
hostname
id
read -p "Press any key to continue..."
echo "> date, uptime"
date
uptime
read -p "Press any key to continue..."
echo "> df"
df
read -p "Press any key to continue..."
echo "> ps"
ps
read -p "Press any key to exit..."
exit