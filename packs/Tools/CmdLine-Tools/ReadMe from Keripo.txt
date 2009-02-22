Last updated: Aug 20, 2008
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
- A few of them are no longer compiled as they are
  included in busybox while quite few have been added
  via busybox.
- For information on what each binary does, see their
  respective links below.

Usage:
- Since the "Bin" has been added to $PATH, all
  binaries are accessible to the shell and terminal.
  Thus they can be used in shell scripts or via the
  terminal module.
- Hold the centre/select button on a file or directory
  in the file browser to set full permissions for it
  (chmod ugo+rwx).
- Hold the centre/select button on a text file in the
  file browser to edit it with vi or convert it from
  DOS format to UNIX format.
- Show system info though
  "Tools > CmdLine-Tools > System Info"
- FastLaunch LOLCODE interpreter through
  "Tools > CmdLine-Tools > LOLCODE FastLaunch" to start
  the interpreter (type in the commands via terminal) or
  use "Tools > CmdLine-Tools > LOLCODE Scripts" to select
  script to run.
- Files with a ".lol" extension have their default action
  set to the LOLCODE interpreter.


Original:
  
  gnu tools
  (grep, gawk, sed)
  imphasing, Apr 1, 2006
  http://ipodlinux.org/wiki/Gnutools
  http://ipodlinux.org/oldforums/viewtopic.php?t=9148
  http://ipodlinux.org/wiki/Image:Ipodgrep.tar
  http://ipodlinux.org/wiki/Image:Ipodgawk.tar.gz
  http://ipodlinux.org/wiki/Image:Ipodsed.tar
  
  diffutils
  (cmp, diff, diff3, sdiff)
  Keripo, March 9, 2008
  http://ftp.gnu.org/pub/gnu/diffutils/
  
  commandlineutils
  (asciichart, contrast, font, lsi, pause, raise)
  (note that backlight isn't included; use Keripo's version)
  Jonrelay, Revision 2113 (pre-"Great Befuddlement")
  http://ipodlinux.org/oldforums/viewtopic.php?t=705
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/cmdlineutils
  
  John the Ripper
  coob, Mar 14, 2005
  http://ipodlinux.org/oldforums/viewtopic.php?t=1324
  http://www.openwall.com/john/f/john-1.7.2.tar.gz
  http://web.archive.org/web/20050404001647/http://booc.coob.org/john-1.6.37-arm.diff
  
  lolcode flex and bison interpreter
  Keripo, Jun 28, 2008
  http://lolcode.com/contributions/flex-and-bison-implementation
  http://umich.edu/~rmurillo/lolcode.zip
  
ZS Version:
- Modder: Keripo
- Type: Porter / ZS Recompile / Repack
- Date: Jun 28, 2008
- Version: B X.X

Modifications:
- make ZS friendly

Launch Module:
- "/opt/Tools/CmdLine-Tools/Bin" added to $PATH for access
  by the shell
- "/opt/Tools/CmdLine-Tools/John" is set to $JOHN for John
  the ripper
- file handler for chmod, vi and dos2unix
- Show various system info through terminal
- Fastlaunches LOLCODE interpreter through terminal
- interpret LOLCODE scripts from file browser

To do:
- port more command line tools!!!

Changelog:
[keep untouched till beta release]