#!/bin/sh
#
# Libs Auto-Cleaning Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: March 27, 2008
#
LIBS="bzip2 tar hotdog ttk"
for lib in $LIBS
do
	rm -rf $lib
done