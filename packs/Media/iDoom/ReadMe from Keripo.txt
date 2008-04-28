Last updated: Apr 23, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
iDoom
- iDoom is a port of the original id game Doom to iPodLinux
- It is one of the most well known and widely played game for
  iPodLinux and has a long history, appearing in a number of
  past publications (magazines and online) featuring iPodLinux.
- ZeroSlackr uses a modified iDoom that allows for unrestricted
  usage of "-file". An alternate launcher is also provided that
  uses the FreeDoom project's IWAD, allowing for the "-file" of
  Doom II PWADs without needing a commercial "doom2.wad" file.

For more information, see:
- http://idoom.hyarion.com/
- http://ipodlinux.org/IDoom
- http://en.wikipedia.org/wiki/Doom

Usage:
- FastLaunch through "Media > iDoom > FastLaunch" or
  launch iDoom without parameters through
  "Media > iDoom > iDoom" or iFreeDoom without parameters
  through "Media > iDoom > iFreeDoom".
- Files with a ".wad" extension have their default
  action set to iDoom.  
- PWADs intended to supplement doom.wad should be placed in the
  "PWADs/Doom" folder and those for doom2.wad should be placed
  in the "PWADs/DoomII" folder.
- If a legimate "doom2.wad" is owned, it should replace the
  "freedm.wad" file (or may try keeping as doom2.wad for usage
  with the normal iDoom launcher, but will screw up wad browsing).
- You may want to edit the "Conf/keys.key" file if you want to
  change settings, etc.


Original:
- Authors: Benjamin Eriksson (Hyarion) and Mattias Pierre (Jobbe)
- Source: Website
- Link:
  http://idoom.hyarion.com/
- Date: May 31, 2006
- Version: 1.3

Freedom:
- Authors: Jonathan Dowland et al. (see SourceForge page)
- Source: Website
- Link:
  http://freedoom.sourceforge.net/download/
- Date: Feb 28, 2008
- Version: 0.6

ZS Version:
- Modder: Keripo
- Type: Mod and ZS Recompile
- Date: Apr 23, 2008
- Version: B X.X

Modifications:
- many modifications; see iDoom-All.patch file 
- made ZS friendly
- eliminated almost all boot-text
- removed "-file" limitation
- removed quit confirmation
- additional iFreeDoom which uses the FreeDoom DeathMatch
  IWAD to replace doom2.wad. The FreeDoom DM IWAD is used
  instead of the normal one due to incompatibility and size.

Launch Module:
- FastLaunches iDoom with shareware wad
- launch wad files from file browser with either iDoom
  (Doom PWADs) or iFreeDoom (DoomII PWADs)
- setting to choose to launch with either iDoom or iFreeDoom

To do:
- furthur modify to eliminate need for two binaries (probably add a "-free" switch)
- add strafing? (map to clickwheel)

Changelog 
[keep untouched till beta release]