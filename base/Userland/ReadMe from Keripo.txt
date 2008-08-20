Last updated: Aug 19, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the license that comes
with the original/ported software.

Overview:
Userland
- This is the iPodLinux/SansaLinux userland, which
  consists mainly of BusyBox and a few kernel modules.
- ZeroSlackr uses a modified userland stored on an ext2
  filesystem image for the loop-mount boot method on
  iPods or booting with initrd on Sansas. Many folders
  have been replaced with symbolic links to a counterpart
  that can be located on a FAT32 partition, allowing
  for easy access to these folders by Windows users.
  Both the iPodLinux and SansaLinux versions also use a
  "sandbox" ext3 image for storing variable data/configs.
- The ZeroSlackr userland also includes various unofficial
  fonts and schemes created by various users.
For more information, see:
- http://ipodlinux.org/Installation:_Winpod_without_datacorruption
- http://ipodlinux.org/Userland
- http://en.wikipedia.org/wiki/Userland_(computing)
- http://en.wikipedia.org/wiki/Initrd
- http://busybox.net/
- http://en.wikipedia.org/wiki/BusyBox


Usage:
- Since ZeroSlackr uses a userland image/initrd instead of
  a real ext2 partition, the actual userland files are not
  easily accessible (nor are they intended to be). In
  general, the userland files should not be changed and any
  variable data requiring a Linux partition should be saved
  to /sandbox (which is stored on the sandbox ext3 image).  
- Symlinks were made such that modification of the userland
  is not needed. All directories under the "/ZeroSlackr"
  folder are symlinked to their counterparts (with the
  exception of "/etc" as only "/etc/rc.d" is symlinked rather
  than all of "/etc").
- If you do need to modify/access the ext2 image however, you
  can do so via the old, depreciated "start" file method
  (see http://ipodlinux.org/Start_File) or loop-mounting
  the image on Linux (see "Loop-mount Instructions.txt" in
  "/docs/userland/").
- Since "/usr" is symlinked, any binaries placed in "/usr/bin"
  will also be accessible just like binaries in "/bin".
- New schemes should be placed into "/usr/share/schemes" and
  new fonts should be placed into "/usr/share/fonts".
- All PZ2 module configurations are saved in "/sandbox".
- ZeroSlackr's userland uses getLoader2AutoExec instead of
  getLoader2Args. For more information on how to use that,
  read the comments of "loader.cfg".

Original:
- Authors: DataGhost & AriX
- Source: AriX's iPod Application Manager
- Link:
  http://ipodlinux.org/forums/viewtopic.php?t=23450 (thread)
  http://winxblog.com/iPodLinuxManagerbeta.zip
- Date: Mar 11, 2007?
- Version: 1.1

SansaLinux:
- Porter: Sebastian Duell
- Type: Ported
- Date: July 15, 2008
- Version: 1.11

ZS Version:
- Modder: Keripo
- Type: Recreated and updated busybox
- Date: Aug 19, 2008
- Version: B X.X

Modifications:
- loop-mount boot:
  - updated busybox, kept old minix sh for size
  - modified "pre-rc" file
- modified loop-mount userland:
  (see /docs/userland/Mod/*)
  - updated busybox (1.12 SVN) with tons more utils
  - modified rc file (keeps launching ZeroLauncher)
  - added getLoader2AutoExec
  - added /sandbox
  - symlinks:
    /bin/cd -> minix
    /bin/mpd -> /opt/Base/MPD/MPD-ke
    /bin/podzilla -> /bin/zerolauncher
    /bin/zerolauncher
      -> /opt/Base/ZeroLauncher/ZeroLauncher
    /root -> /mnt/ZeroSlackr
    /opt -> /root/opt
    /usr -> /root/usr
    /etc/rc.d -> /root/etc/rc.d
  - added cpu_speed and backlight binaries (kept in
    /usr/bin for space reasons)
  - added fonts:
    (separate "font.lst" file for PZ0 based zillae)
    - Encyclopodia:
      http://sourceforge.net/project/showfiles.php?group_id=146648
    - Snap: http://www.dansfloyd.com/floydzilla.html
    - other fonts from wiki scheme downloads
  - added schemes:
    - 5-y.2-d, Moonlight, Bloodcross and Halloween by Keripo
    - schemes still on scheme page (has version #s)
    - schemes uploaded to wiki (has no version #, some repacked)

To do:
- try rebuilding SansaLinux kernel and bootloader to use
  loop-mount boot method instead of a false initrd

Changelog:
[keep untouched till beta release]