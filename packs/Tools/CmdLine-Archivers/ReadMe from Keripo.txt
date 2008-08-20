Last updated: Aug 20, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
CmdLine-Archivers
- This is a collection of various command-line
  archiving tools ported to iPodLinux.
- Some of these have been recompiled while others
  are directly copied from their pre-compiled sources
  since they do not require recompilation.
- md5sum, sha1sum and sum have been added via busybox.
- For information on what each binary does, see their
  respective links below.

Usage:
- Since the "Bin" has been added to $PATH, all
  binaries are accessible to the shell and terminal.
  Thus they can be used in shell scripts or via the
  terminal module.
- Hold the centre/select button on an item in the
  file browser to access the archiving options.
- Note that bzip2 requires a bit of memory so if you
  are unable to compress with bzip2, reboot your iPod
  and try again. lpaq4 requires a lot of free memory
  (6MB for level 0 and 12MB for 1) and thus will not
  work with any GUI overhead (use from ZeroLauncher).
- lpaq4 was not ported to be functional but rather as
  a proof-of-concept port. While the compression level
  may be higher than bzip2 or gzip, do not expect it
  to be useful nor fast ; )


Original:
  
  gzip
  imphasing, Apr 1, 2006
  http://ipodlinux.org/Gnutools
  http://ipodlinux.org/forums/viewtopic.php?t=9148
  http://ipodlinux.org/Image:Ipodgzip.tar
  
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
  
  lpaq4
  Keripo, Jun 25, 2008
  http://cs.fit.edu/~mmahoney/compression/#lpaq

ZS Version:
- Modder: Keripo
- Type: Porter / ZS Recompile / Repack
- Date: Jun 25, 2008
- Version: B X.X

Modifications:
- make ZS friendly

Launch Module:
- file handler for zip, unzip, unrar, tar, bzip2 and gzip
- file handler for md5sum, sha1sum and sum
- "/opt/Tools/CmdLine-Tools/Bin" added to $PATH for access
  by the shell

To do:
- port more command-line archivers!!!

Changelog:
[keep untouched till beta release]