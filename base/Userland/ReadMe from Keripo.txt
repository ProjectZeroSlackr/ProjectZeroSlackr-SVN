Last updated: Apr 5, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the license that comes
with the original/ported software.

Overview:
Userland
- This is a modified iPodLinux userland. The iPodLinux
  userland relies mostly on BusyBox and the minix shell.
- ZeroSlackr uses a modified, imaged userland used for
  the loop-mount installation method. Many userland folders
  have been replaced with symbolic links to a counterpart
  that can be located on a FAT32 partition, allowing
  for easy access to these folders by Windows users.
- The ZeroSlackr userland also includes various unofficial
  fonts and schemes created by various users.
For more information, see:
- http://ipodlinux.org/Installation:_Winpod_without_datacorruption
- http://ipodlinux.org/Userland
- http://en.wikipedia.org/wiki/Userland_(computing)

Usage:
- Since ZeroSlackr uses a userland image instead of a
  partition, the actual, full Linux partition is not
  easily accessible.
- Since /usr is symlinked, any binaries placed in /usr/bin
  will also be accessible just like binaries in /bin.
- Symlinks were made such that modification of the userland
  is not needed; if you do need to modify/access the userland
  however, you can do so via the old, depreciated "start" file
  method (see http://ipodlinux.org/Start_File) or
  loop-mounting the image on Linux/Mac OS X (see
  "/boot/docs/userland/Loop-mount Instructions.txt").


Original:
- Authors: DataGhost & AriX
- Source: AriX's iPod Application Manager
- Link:
  http://ipodlinux.org/forums/viewtopic.php?t=23450 (thread)
  http://winxblog.com/iPodLinuxManagerbeta.zip
- Date: March 11, 2007?
- Version: 1.1

ZS Version:
- Modder: Keripo
- Type: Recreated (except busybox binary)
- Date: March 19, 2008
- Version: B X.X

Modifications:
- loop-mount bootup:
  - most files from official userland
  - busybox binary from DataGhost
  - modified "pre-rc" file
- modified loop-mount userland:
  (see changes applied by /src/mod/*)
  - modified rc file 
  - getLoader2AutoExec
  - modified Podzilla0-Lite
  - symlinked podzilla, zerolauncher and mpd binaries
  - symlinks:
	/root -> /mnt/ZeroSlackr
	/opt -> /root/opt
	/usr -> /root/usr
	/etc/rc.d -> /root/etc/rc.d
  - added fonts:
    (separate "font.lst" file for PZ0 based zillae)
    - Snap: http://www.dansfloyd.com/floydzilla.html
    - other fonts from wiki scheme downloads
  - added schemes:
    - Moonlight by Keripo
    - Bloodcross by Keripo
    - Halloween by Keripo (won't fix)
    - schemes still on scheme page (has version #s)
    - schemes uploaded to wiki (has no version #, some repacked)

Modifying the Userland:
- see src/images/Loop-mount Instructions.txt
  for creation and modification instructions.

To do:
- fix up own schemes / make more

Changelog:
[keep untouched till beta release]