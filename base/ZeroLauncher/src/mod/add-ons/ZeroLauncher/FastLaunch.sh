#!/bin/sh
#set -vx
exec >> /opt/ZeroLauncher/Misc/FastLaunch.log 2>&1

# ZeroLauncher script - called at startup
# Default pack loading same as
# "exec /opt/ZeroLauncher/ZeroLauncher -l /opt:/opt/ZeroLauncher/Add-ons"
cd /opt/ZeroLauncher
exec /opt/ZeroLauncher/ZeroLauncher
