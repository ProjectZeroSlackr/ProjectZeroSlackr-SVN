#!/bin/sh
#set -vx

gen=`/bin/ipodinfo -g`

# Clock sync for those who can
if [ $gen -ge 3 ]; then
  hwclock --hctosys
fi