Last updated: July 21, 2008
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
community-generated content. For a full list of
features, see Features.txt For frequently asked
questions and a list of terminology, see FAQ.txt.
For an updated list of changes, see Changelog.txt.
For a rough ToDo list, see ToDo.txt. For more
information on iPodLinux, see iPodLinux's main
website/wiki: http://ipodlinux.org

Project ZeroSlackr has also been ported over to
SansaLinux although not all Slackr packs work at the
moment. While the ZeroSlackr Framework is slightly
different, the majority of things are the same for both
the iPod and Sansa. Keep in mind that all documentation
is written for the iPod and thus not everything will
also apply/work for the Sansa (you will have to
experiment a bit as I do not have a Sansa myself and
thus cannot provide proper support for it).

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
iPod nor iPhone/iPod Touch. I may buy a Sansa e200 as I
am currently depending on others with the SansaLinux port.

Either way, I hope you enjoy using ZeroSlackr and remember
to leave a word of thanks somewhere or, better yet, help
contribute back to the iPodLinux community somehow, be it
making and releasing a scheme, writing tutorials, porting
new applications, or even writing your own. ; )

Enjoy!

~Keripo
