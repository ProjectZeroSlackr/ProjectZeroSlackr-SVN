Last updated: Aug 3, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
iAtari800
- iAtari800 is a port of Atari800 to iPodLinux.
- Atari800 is an Atari 800, 800XL, 130XE and 5200
  emulator. There are two iPodLinux ports; one based
  on the SDL port using SDL and one written from
  scratch by Keripo using hotdog.
- ZeroSlackr uses the hotdog version as it is faster
  and has more features; the Sansa uses the SDL version
  instead
For more information, see:
- http://atari800.sourceforge.net/index.html
- http://en.wikipedia.org/wiki/Atari_8-bit_family
- http://en.wikipedia.org/wiki/Atari_5200
- http://www.atariarchives.org/

Usage:
- FastLaunch through "Emulators > iAtari800 > FastLaunch"
  to start the "Dog Daze Deluxe" game or use
  "Emulators > iAtari800 > Disk Images" to select a
  disk image.
- Files with a ".atr", ".dcm" or ".xfd" extension have their
  default action set to iAtari800. Note that these are not the
  only file formats that Atari800 supports; see "USAGE"
  in the "Misc/Docs.tar.gz" archive.  
- Disk images/software should be placed in the "Disks" 
  folder. Free disk images/software can be obtained through
  the Atari Program Exchange (APX) collection found at
  http://www.atariarchives.org/APX/showindex.php?sort=pop
  Other commercial disk images should be dumped from real
  disks/cartridges, etc. (downloading commercial disk
  images that you do not own is ILLEGAL).
- Due to lack of input methods, the scroll wheel is used
  for joystick "start" and "select". For input mapping,
  see "iAtari800 ReadMe from Keripo.txt" in the
  "Misc/Docs.tar.gz" archive.
- Tweak the refresh rate, scaling, smoothing and COP
  setting through the "Emulators > iAtari800" menu for
  better gameplay.


Original:
- Authors: David Firth et al.
- Source: Website
- Link:
  http://atari800.sourceforge.net/
  http://sourceforge.net/project/downloading.php?
    group_id=40606&filename=atari800-2.0.3.tar.gz
- Date: July 11, 2007
- Version: 2.0.3

ZS Version:
- Modder: Keripo
- Type: Porter
- Date: Aug 3, 2008
- Version: 2.0.3 K2, B X.X

Modifications:
- ported to iPodLinux, made ZS friendly
- added rom files from idiology's Atari8bit website:
  http://idiology.com/atari/support/roms.zip
- added Bumpov's Dogs, Dog Daze Deluxe, Claim Jumper
  and Dog Daze disk images from Gray Chang's website:
  http://ktchang.cnc.net/emulator/using_emulator.shtml

Launch Module:
- FastLaunches iAtari800 with Dog Daze Deluxe disk image
- launch disk images from file browser
- changeable launch settings

To do:
- none; port complete
- wait for hotdog to be ported to Sansa

Changelog:
[keep untouched till beta release]