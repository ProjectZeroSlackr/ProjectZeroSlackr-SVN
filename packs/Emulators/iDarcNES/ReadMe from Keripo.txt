Last updated: Apr 23, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
iDarcNES
- iDarcNES is a port of DarcNES to iPodLinux.
- DarcNES is a multi-system emulator. but only
  NES and SMS emulation works at the moment
  It is the main NES (Nintendo Entertainment
  System) emulator for iPodLinux and also a good
  SMS (Sega Master System/Sega Game Gear)
  emulator but lacks sound support (thus iGameGear
  is preferred).
- iDarcNES works on iPod nanos, videos and
  photos and, due to its ARM optimization,
  is able to reach 90-100% speed on most games.
- iDarcNES was originally ported by zaphod but
  later sped up by zacaj through the usage of
  the hotdog graphics engine.
- iDarcNES was never really completed and has
  many buggy/missing features. The ZeroSlackr
  version has a few small changes but an entire
  rewrite is very much needed.
For more information, see:
- http://ipodlinux.org/wiki/IDarcNES
- http://www.dridus.com/~nyef/darcnes/
- http://en.wikipedia.org/wiki/Nintendo_Entertainment_System
- http://ipodlinux.org/oldforums/viewtopic.php?t=18245

Usage:
- FastLaunch through "Emulators > iDarcNES > FastLaunch"
  to start the "BombSweeper" game or use
  "Emulators > iDarcNES > Roms" to select a rom.
- Files with a ".nes" extension have their default
  action set to iDarcNES. Files with a ".sms" extension
  can also be opened with iDarcNES by holding centre.
- Rom files should be placed in the "Roms" folder.
  Public domain rom files can be optained through various
  internet websites while commercial rom files should be dumped
  from real NES game cartriages (downloading commercial
  roms that you do not own is ILLEGAL). Public domain roms can
  be found here: http://www.pdroms.de/files/nes/
- Keep in mind that while iDarcNES is in "working" status,
  it is _far_ from stable and has many bugs; expect issues.


Original:
- Authors: Nyef, zaphod, zacaj
- Source: Wiki upload
- Link:
  http://ipodlinux.org/wiki/Image:Darcnes.zip
- Date: Mar 18, 2008
- Version: ????

ZS Version:
- Modder: Keripo
- Type: ZS Mod and Recompile
- Date: Apr 23, 2008
- Version: B X.X

Modifications:
- made ZS friendly
- general cleanup of code
- added BombSweeper and StarsField PD roms
  (forgot where gotten from)

Launch Module:
- FastLaunches iDarcNES with BombSweeper rom
- launch rom files from file browser

To do:
- REDO EVERYTHING!!!
  (Code too messy so might as well report with
  ARM optimizations from zaphod and hotdog from
  zacaj)
- Use iBoy's menu
- DarcNES is a _multiple-system_ emulator; more than
  just NES and SMS should work!

Changelog:
[keep untouched till beta release]