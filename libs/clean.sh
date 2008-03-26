#!/bin/sh
#
# Libs Auto-Cleaning Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 25, 2008
#
LIBS="bzip2 hotdog ttk"
for lib in $LIBS
do
	rm -rf $lib
done