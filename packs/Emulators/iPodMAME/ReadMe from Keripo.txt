Last updated: Apr 22, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
iPodMAME
- iPodMAME is a port of MAME (Multiple Arcade
  Machine Emulator) to iPodLinux.
- Due to "some strange video driver bug",
  iPodMAME is only confirmed to work with
  iPod nanos and iPod videos (5G).
For more information, see:
- http://ipodlinux.org/Ipodmame
- http://umlautllama.com/projects/nanomame/
- http://mamedev.org/
- http://en.wikipedia.org/wiki/MAME

Usage:
- FastLaunch through "Emulators > iPodMAME > FastLaunch"
  to start the "Alien Armada" game or select a supported
  romset in "Emulators > iPodMAME".
- A list of working romsets can be found in the
  "Roms" directory. Place your legally obtained
  rom files in their proper folders. iPodMAME
  will not run a romset game if any of the rom
  files for that romset are missing/incorrect.
- If you have an iPod video and/or you want to
  configure the screen/controls, edit the
  "Conf/ipodmame.ini" file.


Original:
- Authors: imphasing, Yorgle, Nicola Salmoria et al.
- Source: SVN
- Link:
  http://opensvn.csie.org/ipodmame/
- Date: July 19, 2006
- Version: Revision 131

ZS Version:
- Modder: Keripo
- Type: ZS Recompile
- Date: Apr 22, 2008
- Version: B X.X

Modifications:
- made ZS friendly
- added aarmada rom from http://www.widel.com/files/aa.zip

Launch Module:
- individual launches for each supported romset
- full file checks for missing files before launch

To do:
- better, non-hardcoded launching mechanism (browser-style)

Changelog:
[keep untouched till beta release]