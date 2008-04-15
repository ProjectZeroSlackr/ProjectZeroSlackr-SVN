#!/bin/sh
#set -vx
#exec >> /opt/SBaGen/Misc/FastLaunch.log 2>&1

cd /opt/SBaGen
exec /opt/SBaGen/SBaGen /opt/SBaGen/Beats/basics/prog-chakras-1.sbg
