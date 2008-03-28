Last updated: March 28, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Docs/Original or the license that comes with the
original/ported software.

Overview:
Loader2 & iPodPatcher
- This is the main bootloader for iPodLinux. It is capable
  of loading both the iPodLinux kernel and the original
  Apple OS as well as Rockbox. It is installed via Rockbox's
  ipodpatcher tool.
- Loader2 can be configured and customized. See the
  "/boot/loader.cfg" file.
- Rockbox can also be installed by extracting the latest
  Rockbox build directly to the iPod and editing the
  "/boot/loader.cfg" file.
For more information, see:
- http://ipodlinux.org/Loader2
- http://www.rockbox.org/twiki/bin/view/Main/IpodPatcher
- http://en.wikipedia.org/wiki/Bootloader

Usage:
- Since Loader2 is a bootloader written from scratch,
  it has its own special syntax.
- For proper usage of Loader2 and its various options/syntax,
  see the provided "/boot/loader.cfg" file as well as
  the iPodLinux wiki page: http://ipodlinux.org/Loader2
- The iPodLinux kernel accepts most standard kernel arguments;
  see "/boot/docs/kernel/Original/kernel-parameters.txt"
- ZeroSlackr's userland uses getLoader2AutoExec instead of
  getLoader2Args. For more information on how to use that,
  read the comments of "/boot/loader.cfg".


Loader 2:
- Authors: iPL Core Devs
- Source: SVN
- Link:
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/ipodloader2
- Date: March 16, 2008
- Version: Revision 2403

ZS Version:
- Modder: Keripo
- Type: Clean Compile
- Date: March 19, 2008
- Version: B X.X

iPodPatcher:
- Authors: Rockbox Devs
- Source: SVN
- Link:
  svn://svn.rockbox.org/rockbox/trunk/rbutil/ipodpatcher
- Date: March 19, 2008
- Version: Revision 16718

Modifications:
- none so far
- added easy "patch" scripts for Windows and Linux (untested on Macs)
- added pre-compiled ipodpatcher binaries
  for Windows (compiled by Keripo), Linux x86 (compiled by Keripo),
  and Mac OS X (extracted from Rockbox wiki by AriX)

To do:
- custom settings
- FastLaunch.sh searcher
- sub-menu support (look at iBoot Advanced / iLoadz)
- intro piezo music? ; )

Changelog:
[keep untouched till beta release]