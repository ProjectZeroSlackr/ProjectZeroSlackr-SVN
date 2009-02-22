Last updated: Aug 13, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
iDoom
- iDoom is a port by Benjamin Eriksson and Mattias Pierre
  of the original id game Doom to iPodLinux.
- It is one of the most well known and widely played game for
  iPodLinux and has a long history, appearing in a number of
  past publications (magazines and online) featuring iPodLinux.
- ZeroSlackr uses a modified iDoom that allows for unrestricted
  usage of "-file". An added "-freedoom" switch will use the
  FreeDoom DeathMatch IWAD (freedm.wad) in replacement for the
  commercial "doom2.wad" file. Note that the FreeDM IWAD is
  not a single-player wad file and is used only for the IWAD
  lumps. The FreeDoom IWAD, unfortunately, is not compatible
  with vanilla Doom.
hDoom
- hDoom is a port by Keripo of the original id game Doom to
  iPodLinux.
- Since iDoom was written and last updated before the
  introduction of the 5.5G iPod video and all its LCD routines
  are very model-specific, iDoom does not run on 5.5Gs.
- As a coding exercise, Keripo ported the vanilla Doom source
  code to iPodLinux using the hotdog graphics engine for
  blitting and using the sdldoom and iDoom source codes as
  reference.
- hDoom runs at full speed and works on all colour iPods.
  There is no support for monochrome iPods as hotdog does
  not seem to work on monochrome iPods.
- hDoom allows for strafing via the scroll wheel and pixel
  blending has also been added for slightly smoother graphics.
  No key remapping feature (i.e. the "keys.key" that iDoom
  uses), however, has been added as there was originally none.
- Like iDoom, has also been modified for unrestricted
  usage of "-file" and has an added "-freedoom" switch.

For more information, see:
- http://idoom.hyarion.com/
- http://ipodlinux.org/wiki/IDoom
- http://en.wikipedia.org/wiki/Doom

Usage:
- FastLaunch hDoom through "Media > iDoom > FastLaunch"
- Files with a ".wad" extension have their default
  action set to hDoom.  
- PWADs intended to supplement doom.wad should be placed in the
  "PWADs/Doom" folder and those for doom2.wad should be placed
  in the "PWADs/DoomII" folder.
- If a legimate "doom2.wad" is owned, it should replace the
  "freedm.wad" file (or may try keeping as doom2.wad for usage
  with the normal iDoom launcher, but will screw up wad browsing).
- You may want to edit the "Conf/keys.key" file if you want to
  change settings, etc. hDoom cannot have its controls changed.
- If you find yourself unable to launch Doom II PWADs, use
  the FreeDM IWAD. Doom II PWADs need a doom2.wad or freedm.wad
  while Doom I PWADs do not.
- Note that since save files are kept in a common folder, saves
  from one PWAD will not work on other PWADs. Thus, remember
  which slots correspond to each PWAD you save with. I do not
  plan on adding PWAD-specific saves in the future.


iDoom:
- Authors: Benjamin Eriksson (Hyarion) and Mattias Pierre (Jobbe)
- Source: Website
- Link:
  http://idoom.hyarion.com/
- Date: May 31, 2006
- Version: 1.3

hDoom:
- Authors: Keripo
- Source: Project ZeroSlackr
- Link:
  none
- Date: Jun 22, 2006
- Version: 1.10 K1

Freedom:
- Authors: Jonathan Dowland et al. (see SourceForge page)
- Source: Website
- Link:
  http://freedoom.sourceforge.net/download/
- Date: Feb 28, 2008
- Version: 0.6

ZS Version:
- Modder: Keripo
- Type: iDoom Mod and ZS Recompile, hDoom Porter
- Date: Jun 22, 2008
- Version: B X.X

Modifications:
- many modifications; see iDoom-All.patch file
- for hDoom port, see hDoom-All.patch file
- removed "-file" limitations, added "-freedoom" switch
- made ZS friendly
- eliminated almost all boot-text
- removed "-file" limitation
- removed quit confirmation

Launch Module:
- FastLaunches using current settings without any PWAD
- launch wad files from file browser with either iDoom or hDoom
- setting to choose to launch with either iDoom or hDoom
- setting to choose to launch with or without FreeDM IWAD

To do:
- none

Changelog 
[keep untouched till beta release]