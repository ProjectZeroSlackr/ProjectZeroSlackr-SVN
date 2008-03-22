#!/bin/sh
#set -vx
exec >> /opt/Podzilla2-SVN/Misc/FastLaunch.log 2>&1

# To customize where PZ2-SVN loads its modules from,
# add a "-l" followed by a single path from where you
# wish to load modules from. If you want to load from
# multiple folders, separate the paths with colons (":").
# For example, the line:
# "exec /opt/Podzilla2-SVN/Podzilla2-SVN -l /usr/lib/All:/usr/lib/Tools"
# only loads modules from "/usr/lib/All" and "/usr/lib/Tools"
# See the other "*.sh" files in the "Launch" folder
# for more examples

cd /opt/Podzilla2-SVN
exec /opt/Podzilla2-SVN/Podzilla2-SVN
