Last updated: March 28, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
Encyclopodia
- This is a specialized eBook reader created for
  iPodLinux.
- It reads specially converted .epodia eBooks, acting as
  a handy reference tool for wiki dumps such as Wikipedia.
- Encyclopodia uses a modified podzilla2 build with
  the encyclopodiareader module and minimal other modules.
- The original Encyclopodia was a fork of the iPodLinux
  project but later a podzilla2 module was ported (by jsha).
- The podzilla2 module port is incomplete; there are
  many features from the original Encyclopodia that are
  missing in the PZ2 module and it is prone to many bugs
  (notably font display).
For more information, see:
- http://encyclopodia.sourceforge.net/
- http://en.wikipedia.org/wiki/Wikipedia:Encyclopodia

Usage:
- FastLaunch through "Tools > Encyclopodia".
- A pre-converted iPodLinux wiki dump is provided. To create
  your own .epodia file, see "Misc/Docs/Creation-Instructions.txt"
  or visit the official Encyclopodia website (where you can also
  download a preconverted Wikipedia dump) and place the .epodia
  file in the "Library" folder.
- Encyclopodia contains only the bare minimum in modules;
  if you really want to add more PZ2 modules, place them
  in the "Modules" folder.


Original:
- Authors: Robert Bamler, jsha, iPL Core Devs
- Source: Website, Website, SVN
- Link:
  http://encyclopodia.sourceforge.net/
  http://newview.org/jsha/hacks/encyclopodiareader-module.tar.gz
  http://newview.org/jsha/hacks/bzip-dirs.tar.gz
  http://ipodlinux.org/forums/viewtopic.php?p=64392#64392
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/podzilla2
- Date: Jan 30, 2006, March 15, 2006, March 16, 2008 
- Version: 0.9, 0.1, Revision 2403

ZS Version:
- Modder: Keripo
- Type: Mod and ZS Recompile
- Date: March 19, 2008
- Version: B X.X

Modifications:
(see patch files of source for details)
- modified podzilla2 with jsha's encyclopodiareader PZ2 module
- uses minimal modules;
  still needs to be launched from Loader2 due to lack of memory
- comes with iPodLinux wiki dump (dumped by Keripo March 7, 2008)
- made ZS friendly

Launch Module:
- FastLaunches Encyclopodia (better done from Loader2)

To do:
- fix font reading/loading (contacted Robert Bamler; wait first)

Changelog:
[keep untouched till beta release]