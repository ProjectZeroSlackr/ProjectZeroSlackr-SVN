Last updated: March 2, 2008
~Keripo

Source files are downloaded from the official SVN.

To compile the libraries, run the respective scripts.
The "build-hotdog.sh" script must be run before
the "build-ttk.sh" script.

To remove the libraries, delete the "logs", "hotdog" and
"ttk" folders. The "pz0" libraries should not be touched
since they are precompiled (I've long forgotten how to
compile them).

Note that the ttk compiling does not "make install"
and that used libraries should be symlinked to the
build directory when compiling applications. Also, if
you are using Cygwin, ttk will automatically be patched
since SDL does build workingly with Cygwin (e.g.
"sdl-config" doesn't work on Cygwin).
