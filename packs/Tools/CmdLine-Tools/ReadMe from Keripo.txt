Last updated: May 1, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
CmdLine-Tools
- This is a collection of various command-line tools
  ported or written for iPodLinux.
- Some of these have been recompiled while others
  are directly copied from their pre-compiled sources
  since they do not require recompilation.
- For information on what each binary does, see their
  respective links below.

Usage:
- Since the "Bin" has been added to $PATH, all
  binaries are accessible to the shell and terminal.
  Thus they can be used in shell scripts or via the
  terminal module.
- The CmdLine-Tools launch module adds file handlers
  for usage of the archiving tools; hold the centre/
  select button on an item in the file browser to
  access the archiving options.


Original:
  
  gnu tools
  (grep, gawk, sed, gzip)
  imphasing, Apr 1, 2006
  http://ipodlinux.org/Gnutools
  http://ipodlinux.org/forums/viewtopic.php?t=9148
  http://ipodlinux.org/Image:Ipodgrep.tar
  http://ipodlinux.org/Image:Ipodgawk.tar.gz
  http://ipodlinux.org/Image:Ipodsed.tar
  http://ipodlinux.org/Image:Ipodgzip.tar

  diffutils
  (cmp, diff, diff3, sdiff)
  Keripo, March 9, 2008
  http://ftp.gnu.org/pub/gnu/diffutils/

  commandlineutils
  (asciichart, backlight, contrast, font, lsi, pause, raise):
  Jonrelay, Revision 2113 (pre-"Great Befuddlement")
  http://ipodlinux.org/forums/viewtopic.php?t=705
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/cmdlineutils

  unzip and unrar
  Rufus, Oct 8, 2007
  http://ipodlinux.org/forums/viewtopic.php?t=28199
  http://www.box.net/shared/ib9ai6yzcm
  
  tar
  Keripo, March 27, 2008
  (see /libs)
  ftp://download.gnu.org.ua/pub/alpha/tar
  
  bzip2
  (see /libs)
  Keripo, March 27, 2008
  http://www.bzip.org/downloads.html

  zip
  Keripo, March 12, 2008
  http://sourceforge.net/project/showfiles.php?group_id=118012&package_id=128993

  John the Ripper
  coob, Mar 14, 2005
  http://ipodlinux.org/forums/viewtopic.php?t=1324
  http://www.openwall.com/john/f/john-1.7.2.tar.gz
  http://web.archive.org/web/20050404001647/http://booc.coob.org/john-1.6.37-arm.diff
  
ZS Version:
- Modder: Keripo
- Type: ZS Recompile / Repack
- Date: May 1, 2008
- Version: B X.X

Modifications:
- make ZS friendly

Launch Module:
- file handler for zip, unzip, unrar, tar, bzip2 and gzip
- "/opt/Tools/CmdLine-Tools/Bin" added to $PATH for access
  by the shell
- "/opt/Tools/CmdLine-Tools/John" is set to $JOHN for John
  the ripper

To do:
- port more command line tools!!!

Changelog:
[keep untouched till beta release]