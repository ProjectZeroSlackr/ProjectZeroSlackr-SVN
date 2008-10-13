Last updated: Oct 12, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview
ZeroLauncher
- This is the main GUI launcher for ZeroSlackr.
- ZeroLauncher is a custom build of podzilla2
  modified specifically for the purpose of launching
  other iPodLinux applications (such as podzilla2) as
  ZeroSlackr packs.
- It can also act as a normal podzilla2 build but
  requires modules to use a "PackInfo" file instead
  of "Module" and requires the module file to end with
  a file extension of ".zl" rather than ".mod.o".
For more information, see:
- http://ipodlinux.org/Podzilla
- http://en.wikipedia.org/wiki/Graphical_user_interface

Usage:
- The ZeroSlackr userland launches ZeroLauncher by default
  in an endless loop (or rather as long as
  "/opt/Base/ZeroLauncher/Launch/Launch.sh" exists)
  until the user chooses the "Power off" or "Reboot" option
  from the ZeroLauncher menu (or until iPodLinux freezes,
  in which the user should manually reboot by holding the
  correct key combinations for force rebooting their iPod
  (see http://ipodlinux.org/Key_combinations).
- ZeroLauncher loads special podzilla2 modules from
  folders in "/opt" and "/opt/Base/ZeroLauncher/Add-ons".
- To add normal podzilla2 modules to ZeroLauncher, rename
  the "*.mod.o" file to "*.zl" and rename "Module" to
  "PackInfo" and place the module folder into
  "/opt/Base/ZeroLauncher/Add-ons"
  (it is recommended that normal modules be used by
  Podzilla2-SVN instead of ZeroLauncher as ZeroLauncher
  should be limited to only what is necessary for
  launching other apps).
- ZeroLauncher can also be FastLaunch'd through
  "Zillae > ZeroLauncher" but such is pretty pointless.


Original:
- Authors: iPL Core Devs
- Source: SVN
- Link:
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/podzilla2
- Date: Oct 12, 2008
- Version: Revision 2437

ZS Version:
- Modder: Keripo
- Type: Mod and ZS Recompile
- Date: Oct 12, 2008
- Version: B X.X

Modifications:
(see patch files of source for details)
- modified paths
- custom default settings
- ZeroSlackr sub-menus
- MPD check via mpdc module
- Ncurses check
- loads modules with ".zl" extension and "PackInfo" file
- loads modules from predefined locations
  (see "Launch/Launch.sh")
- browser module modified
  > added open_directory_title function
- mpdc module modified
  > added kill/initiate functions
- podwrite module modified
  > /Notes/ -> /mnt/Notes/

Launch Module:
- FastLaunches ZeroLauncher

Add-ons:
(note that modules modified here also
 have a variant/copy for Podzilla2-SVN)
- official modules:
  > about browser mpdc podwrite terminal
    textinput tidial tiwidgets tixtensions
- browser-ext:
  > various ZeroLauncher functions (checks and
    get_filename/dirname)
  > backlight toggle option
  > cpu speed changing function
  > script and binary exec handler (just like from PZ0)
  > modified mini-browser for specialty launching
- MPD:
  > rewritten based on courtc's code
  > fully functional and ZeroSlackr-friendly
  > "Enable MPD", and "Update next startup" settings
  > overclock iPod to 78MHz for smoother playback
  > source:
    http://so2.sys-techs.com/ipod/mpdmodule-src-0.4.tar.gz
- Ncurses:
  > provides terminfo files and enables check
  > see ncurses in /lib for source code
- podread:
  > ascii file handler
  > general code cleanup and modifications to
    make ZeroSlackr friendly
  > uses check_is_ascii_file
  > source:
    http://ipodlinux.org/modules/podread/podread-0.0.1-src.tar.gz

To do:
- include select ncurses tests/demos?
- replace with Insune (hopefully)
- USB support for Sansas (see SansaLinux website)

Changelog:
[keep untouched till beta release]