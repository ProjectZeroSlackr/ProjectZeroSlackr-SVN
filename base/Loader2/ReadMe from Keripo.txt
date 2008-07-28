Last updated: July 28, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the license that comes
with the original/ported software.

Overview:
Loader2 & iPodPatcher
- This is the main bootloader for iPodLinux. It is capable
  of loading both the iPodLinux kernel and the original
  Apple OS as well as Rockbox. It is installed via Rockbox's
  ipodpatcher tool.
- Loader2 can be configured and customized. See the
  "loader.cfg" file.
- Rockbox can also be installed by extracting the latest
  Rockbox build directly to the iPod and editing the
  "loader.cfg" file.
- SansaLinux support has not been added yet. It will be
  done through Rockbox's sansapatcher.
For more information, see:
- http://ipodlinux.org/Loader2
- http://www.rockbox.org/twiki/bin/view/Main/IpodPatcher
- http://en.wikipedia.org/wiki/Bootloader

Usage:
- Since Loader2 is a bootloader written from scratch,
  it has its own special syntax.
- For proper usage of Loader2 and its various options/syntax,
  see the provided "loader.cfg" file as well as
  the iPodLinux wiki page: http://ipodlinux.org/Loader2
- The iPodLinux kernel accepts most standard kernel arguments;
  see "/docs/kernel/Original/kernel-parameters.txt"
- ZeroSlackr's userland uses getLoader2AutoExec instead of
  getLoader2Args. For more information on how to use that,
  read the comments of "loader.cfg".
- Loader2 has also been patched to play a boot tune on startup.
  The default boot tune should be familiar to Final Fantasy
  gamers ; )


Loader 2:
- Authors: iPL Core Devs
- Source: SVN
- Link:
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/ipodloader2
- Date: Apr 23, 2008
- Version: Revision 2418

ZS Version:
- Modder: Keripo
- Type: Mod and Recompile
- Date: Jun 27, 2008
- Version: B X.X

iPodPatcher:
- Authors: Rockbox Devs
- Source: SVN
- Link:
  svn://svn.rockbox.org/rockbox/trunk/rbutil/ipodpatcher
- Date: Apr 23, 2008
- Version: Revision 17227

Modifications:
- added patch to fix ipodpatcher error levels Rockbox's Flyspray
  task #8827: http://www.rockbox.org/tracker/task/8827
- added easy "patch" scripts for Windows and Linux
  (untested on Macs and 64-bit Linux)
- added pre-compiled ipodpatcher binaries
  for Windows (compiled by Keripo), 32-bit Linux
  (compiled by Keripo), 64-bit Linux (Rockbox wiki) and
  Mac OS X (extracted from Rockbox wiki by AriX)

To do:
- find someone to help recompile ipodpatcher on 64-bit Linux
  and Mac OS X
- FastLaunch.sh searcher
- sub-menu support (look at iBoot Advanced / iLoadz)

Changelog:
[keep untouched till beta release]