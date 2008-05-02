#!/bin/sh
#
# Libs Auto-Cleaning Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: May 2, 2008
#
for lib in src/*.sh
do
	rm -rf `basename $lib .sh`
done