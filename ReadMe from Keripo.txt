Last updated: Apr 15, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.


== Overview ==

Project ZeroSlackr is a custom iPodLinux installation
system. The aim of the project is to provide a simple,
coherent, easy-to-use and newbie friendly method of
installing iPodLinux on their iPods and trying out
community-generated content. For more information on
iPodLinux, see iPodLinux's main website:
http://ipodlinux.org

The core of the system is the ZeroSlackr Framework,
consisting of a modified kernel, a modified,
imaged userland, and ZeroLauncher. The kernel is patched
for compatibility with certain applications and allows
usage of the modified userland. The userland itself
takes advantage of the loop-mount startup method by
being contained as a filesystem image (rather than a
partition), thus allowing a partitionless install.
The default launcher of ZeroSlackr is ZeroLauncher, a
customized build of podzilla2 designed specifically
for the launching of other applications.

The ZeroSlackr Framework is designed to allow for the
easy integration/installation of various iPodLinux
applications/content by providing a standard hierarchy
and method of launching (ZeroLauncher). All applications
are almost fully self-contained (e.g. all related files
are stored in the application's directory). Installation
of packs is usually a simple drag-drop, whereas
uninstallation can usually be a simple directory deletion.

Numerous applications have been modified to fit into the
ZeroSlackr Framework. The majority of ZeroSlackr's content
is modified files or rebuilds from source. Since some of
the content was originally only designed for certain models
or were developed before the introduction of certain models,
not all content will work on all iPods. As well, due to a
loss in source code in some applications, hex-edited files
are sometimes used.

Project ZeroSlackr is a project well in development and
things are expected to be buggy and in need of fixing. If
you find fixable bugs/issues, please post/describe them
in the latest ZeroSlackr thread on the iPodLinux forums.

Also keep in mind that this is currently a one-man project
(done by me, Keripo) for the most part and done out of
free time. If you are an experienced programmer/developer
and are willing to actively join in, feel free to contact
me. If you are a happy end user, feel free to drop by in
a ZeroSlackr forum thread and say a word of thanks. As this
project is for my own interest, I do not ask for donations
or the sort, but if you that nice, feel free to via
ZeroSlackr's SourceForge page. I myself have been working
on this through my 2gb iPod nano (for which I also rely on
soley for music as well) but I don't plan on buying a new
iPod nor iPhone/iPod Touch.

Either way, I hope you enjoy using ZeroSlackr and remember
to leave a word of thanks somewhere or, better yet, help
contribute back to the iPodLinux community somehow, be it
making and releasing a scheme, writing tutorials, porting
new applications, or even writing your own. ; )

~Keripo


== Compiling ==

I've taken the time and liberty of writing neat little
compiling scripts that will do all the dirty work for you.
The only requirement is a development environment (Linux
is suggested though things can be done via Mac OS X or
Cygwin for Windows) and a bit of knowledge/common sense.

Each pack will have a "build.sh" script. To compile the pack,
all you need to "cd" to that directory and type "./build.sh"
and sit back. All packs also symlink the "ttk" and "launch"
libraries for compiling the launch modules outside PZ2.
If compiling goes smoothly and the echo'd output is all
nice and smooth, you will find the compiled files in the new
"build/release" folder already set up and ready to be added
to the ZeroSlackr Framework. If there are issues, look at
the "build/build.log" file.

For convenience I've also written "lump.sh", "clean.sh",
"chmod-all.sh", and "rm-backups.sh" scripts. They can be
used by doing a "cd" to the proper directory, then doing
"./lump.sh", "./clean.sh", or "./rm-backups.sh". The
"rm-backups.sh" script is mainly for Linux as annoying
*~ backup files are often created here and there; the script
carries out a search-and-destroy mission on these files.
The "chmod-all.sh" script will conveniently execute
"chmod ugo+rwx" on all the script files so you dont' have to.
The "lump.sh" script will recursively enter
each pack's folder, execute the pack's "build.sh", copy the
content of that pack's "build/release" into one large "_lump"
folder that can be directly copied to an iPod (note that the
"build" folders will be deleted to conserve space so if there
are any building problems, build that pack separately and see
the "build.log" file). The "clean.sh" script, on the other
hand, recursively enters each folder and removes the "build"
folder. The exception to both of these scripts are the
podzilla0 libraries (for which I have forgotten how to build
and thus just have precompiled libraries), the "launch"
library, and packs that have the "SKIP.txt" file.
