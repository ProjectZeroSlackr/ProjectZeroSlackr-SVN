Last updated: Apr 27, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
ZacZilla
- ZacZilla is a customized build of the proof-of-concept
  TTKzilla, which was podzilla0 written in TTK to demo
  TTK's features.
- ZacZilla integrates many old podzilla2 modules using the
  TTK component of TTKzilla but also includes most/all PZ0
  features.
- Since TTKzilla uses TTK for graphics, unlike PZ0, TTKzilla
  and ZacZilla will run smoothly on iPod videos.
- The ZeroSlackr version of ZacZilla has numerous buggy/broken
  PZ2 modules removed and does not support MPD; the reason is
  purely lack of interest/effort/reason.
For more information, see:
- http://ipodlinux.org/wiki/ZacZilla
- http://sourceforge.net/projects/zaczilla/
- http://ipodlinux.org/wiki/Ttkzilla
- http://ipodlinux.org/wiki/Podzillae

Usage:
- FastLaunch through "Zillae > ZacZilla".


Original:
- Authors: zacaj (based on iPL Dev's ttkzilla)
- Source: SVN (note: source may have come directly from zacaj)
- Link:
  https://zaczilla.svn.sourceforge.net/svnroot/zaczilla/trunk/SRC/
- Date: Aug 31, 2007
- Version: Revision 1

ZS Version:
- Modder: Keripo
- Type: Mod and ZS Recompile
- Date: Apr 27, 2008
- Version: B X.X

Modifications:
- fixed settings saving problem
- changed scheme load/saving to avoid symlink
- replaced all "/mnt/aj/" and "/hp/aj/" with "/opt/Zillae/ZacZilla/Data/"
- finished a few more application path modifications (ie legacy savegame files)
- some customizations
- removed some non-functional items (too many to list; if they have a .c file
  but don't exist in the Makefile, they're probably removed)
- removed iconui (won't work with ZacZilla)

Launch Module:
- FastLaunches ZacZilla

To do:
- remove more non-funtional items
- none else really; this was just a proof-of-concept-based project
- compile with PZ0's MPDc? (not worth effort though)

Changelog 
[keep untouched till beta release]