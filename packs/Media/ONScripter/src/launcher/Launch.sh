#!/bin/sh
#set -vx
#exec >> /opt/Media/ONScripter/Misc/Launch.log 2>&1

# Format: $binary --force-button-shortcut --disable-rescale -f $font_path -r $gamedata_path
killall -15 ZeroLauncher >> /dev/null 2>&1
# Due to lack of sound, ONScripter actually runs faster than realtime
# At 57MHz, text displaying speed is slowed down to normal
cpu 57
backlight brightness 12
if [ -z "$1" ]; then
	cd /opt/Media/ONScripter
	# Because I only have a nano ; P
	exec /opt/Media/ONScripter/ONScripter-En --force-button-shortcut --disable-rescale -f /opt/Media/ONScripter/Data/mikachan.ttf -r "\"/opt/Media/ONScripter/VisualNovels/Higurashi Demo (nano)\""
else
	cd /opt/Media/ONScripter
	exec /opt/Media/ONScripter/ONScripter-En --force-button-shortcut --disable-rescale -f /opt/Media/ONScripter/Data/mikachan.ttf -r "\"$1\""
fi