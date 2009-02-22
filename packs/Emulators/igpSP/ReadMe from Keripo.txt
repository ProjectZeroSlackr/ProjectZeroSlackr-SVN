Last updated: Jan 13, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
igpSP
- igpSP is a port of the Game Boy Advance
  emulator gpSP to iPodLinux.
- gpSP is a highly optimized emulator, full speed
  emulator originally written for the PSP and later
  ported to the GP2X (and even later to the iPhone).
- The GP2X version makes use of an ARM dynamic
  recompiler (dyna-rec), allowing for realtime speeds.
- The GP2X version was first ported to iPodLinux by
  Zaphod, then later improved by zacaj.
- The latest igpSP version is a proper re-port by Keripo
  and is very stable and relatively fast. It is able to
  achieve emulation speeds between 50-80%.  
- The ZeroSlackr version has a few path changes for
  ZS-friendliness.
For more information, see:
- http://ipodlinux.org/wiki/IgpSP
- http://ipodlinux.org/oldforums/viewtopic.php?t=29996
- http://en.wikipedia.org/wiki/Nintendo_Entertainment_System
- http://ipodlinux.org/oldforums/viewtopic.php?t=18245

Usage:
- Dump your own 'gba_bios.bin' file and place it in the "Data"
  directory. For information on how to do that, see the
  "igpSP ReadMe from Keripo.txt" in "Misc/Docs.tar.gz" archive.
- FastLaunch through "Emulators > igpSP > FastLaunch"
  to start igpSP or use "Emulators > igpSP > Roms" to directly
  select a rom.
- Use the "Emulators > igpSP > Sound" toggle to chose which
  build (no-sound or with-sound) to use. Off is recommended.
- Files with a ".gba" extension have their default
  action set to igpSP.
- Rom files should be placed in the "Roms" folder.
  Public domain rom files can be optained through various
  internet websites while commercial rom files should be dumped
  from real GBA game cartriages (downloading commercial
  roms that you do not own is ILLEGAL). Public domain roms can
  be found here: http://www.pdroms.de/files/gameboyadvance/
  For instructions on dumping your own ROMs, see
  "igpSP ReadMe from Keripo.txt" in "Misc/Docs.tar.gz" archive.


Original:
- Authors: Exophase
- Source: GP2X forums
- Link:
  http://www.gp32x.com/board/index.php?showtopic=36286
  http://exophase.devzero.co.uk/gpsp09-2xb_src.tar.bz2
- Date: Apr 9, 2007
- Version: 0.9-2xb

ZS Version:
- Modder: Keripo
- Type: Porter
- Date: Jan 13, 2008
- Version: 0.9-2xb K7, B X.X

Modifications:
- made ZS friendly; see "igpSP ReadMe from Keripo.txt" in
  "Misc/Docs.tar.gz" archive for port details
- added Super Wings (http://www.pdroms.de/files/1597/)

Launch Module:
- FastLaunches igpSP with Super Wings rom
- Toggle to use no-sound or with-sound builds
- launch rom files from file browser

To do:
- none; further optimize emulator and keep up with Exophase
- see igpSP's "ReadMe from Keripo.txt" for igpSP development
  notes (worked on separatly from ZeroSlackr pack)

Changelog:
[keep untouched till beta release]