#!/bin/sh
#
# Libs Auto-Building Script
# Created by Keripo
# For Project ZeroSlackr
# Last updated: Aug 9, 2008
#
# Build in logical order to prevent rebuilding due to dependencies
#for script in src/*.sh
#do
#	./$script
#done
LIBS="hotdog ttk TinyGL ncurses bzip2 tar zlib libjpeg libpng libtiff libmad freetype SDL SDL_image SDL_mixer SDL_ttf"
for lib in $LIBS
do
	./src/$lib.sh
done
