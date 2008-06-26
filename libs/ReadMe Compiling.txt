Last updated: May 2, 2008
~Keripo

Build individual libraries running
"./src/$lib.sh" while in "libs" or
run "./build.sh" to build everything
(or "./clean.sh" to remove all built
libraries)

bzip2 source downloaded from
http://www.bzip.org/downloads.html

tar source downloaded from
ftp://download.gnu.org.ua/pub/alpha/tar
(version 1.16.2-20070123 had to be used
since version 1.18.90 or higher don't
seem to compile nicely)

zlib source downloaded from
http://www.zlib.net/zlib-1.2.3.tar.gz

hotdog and ttk source downloaded from
the official SVN. hotdog needs to be
built before ttk.

Note that the ttk compiling does not
"make install" and that used libraries
should be symlinked to the build
directory when compiling applications.
Also, if you are using Cygwin, ttk will
automatically be patched since SDL
does build workingly with Cygwin (e.g.
"sdl-config" doesn't work on Cygwin).

The "pz0" libraries should not be touched
since they are precompiled (I've long
forgotten how to compile them). The
itunesdb and mikmod libraries are
depreciated and do not work but they're
only used by PZ0 and for historical
purposes.

The "launch" library is just shared files
(consisting of a stripped-down PZ2 makefile
and the "pz.h" file) used by all packs for
compiling the pack's launch module outside
of PZ2. All packs symlink the "launch" and
"ttk" libraries for this.

