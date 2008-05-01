#!/bin/sh
#set -vx
#exec >> /var/log/script.log 2>&1

# Clock sync for those who can
gen=`/bin/ipodinfo -g`
if [ $gen -ge 3 ]; then
  hwclock --hctosys
fi