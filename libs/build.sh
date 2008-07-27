#!/bin/sh
#
# Libs Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: July 27, 2008
#
# Build in logical order to prevent rebuilding due to dependencies
#for script in src/*.sh
#do
#	./$script
#done
LIBS="hotdog ttk bzip2 tar zlib libjpeg libpng libtiff libmad freetype SDL SDL_image SDL_mixer SDL_ttf"
for lib in $LIBS
do
	./src/$lib.sh
done
