Last updated: March 25, 2008
~Keripo

Build individual libraries running
"./src/$lib.sh" while in "libs" or
run "./build.sh" to build everything
(or "./clean.sh" to remove all built
libraries)

bzip2 source downloaded from
http://www.bzip.org/downloads.html

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
