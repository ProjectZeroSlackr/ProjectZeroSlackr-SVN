Last updated: Jan 13, 2008
~Keripo


igpSP 0.9-2xb K7 build
port by Keripo with permission from Exophase


== Intro ==

This is a port of Exophase's amazing GameBoy Advance emulator,
originally written for the PSP, later ported over to GP2X, and
now ported to iPodLinux (it has also been ported to a few other
platforms such as the iPhone, etc.). For more information on
the emulator itself (gpSP), see the documents in the "docs"
directory. For information on the iPodLinux port, see the iPL
wiki page (http://ipodlinux.org/wiki/Igpsp) or search the
iPodLinux forums (http://ipodlinux.org/oldforums/). Currently
igpSP only runs on colour iPods (i.e. iPod colour/photo/
nano/video) and the Sansa e200.


== History ==

igpSP was first ported to iPodLinux by Zaphod and then later
optimized by zacaj. After a period of inactivity, Keripo took
Zaphod's work and did a proper port of the very latest version
of gpSP as a separate build target, rewritting a large chunk
of Zaphod's code and adding in many iPod-specific features and
optimizations. As of the moment, the emulator is able to run at
around 50-80% real-time speed (depending on the game/homebrew)
compared to the full-speed PSP and GP2X versions. The Sansa e200
port is functional but buggy and incomplete.


== Code ==

Even though the iPod's hardware capability is far below that of
the PSP and GP2X both in raw processing power and dedicated
hardware, the iPodLinux port takes advantage of numerous things
to achieve a decent, somewhat "playable" speed. The iPod port
is partially based on the GP2X and desktop versions and takes
advantage of ARM optimizations from the GP2X version. The
iPod-optimized "hotdog" graphics engine is used instead of
SDL and the iPod's co-processor (COP) is used for synchonizing
screen updates. Numerous iPod-specific options/tweaks have
been added to a modified menu to improve emulation/speed.

Sound support and timers are removed for the "no-sound" build
and offers a significant emulation speed boost. Scaling and
centring code is used and directly operates on what is blit to
the screen rather than the GBA screen. The scaling/centring
algorithms work using ratios between the GBA screen dimensions
and the iPod's screen dimensions. The smoothing code "smooths"
the outputed screen by blending nearby pixels from the GBA
screen (four pixels for "Type 1" and eight for "Type 2")
through getting their RGB values and rebuilding a new pixel.
Turning off alpha blending may offer game speed-ups (depending
on how much alpha blending is used by the game) at the price of
graphics/proper emulation). The synchronization flag can also
be turned off though it is unlikely to offer any significant
speed boost as the iPod is usually maxed out anyway.

Many iPod-specific settings have also been added that just make
the iPod port more complete (i.e. input and hardware settings).
The input settings are so that the user can re-map methods of
input to accomidate the iPod's limited input methods. There
are various hardware settings which directly access the iPod's
hardware either for better performance or usage.

It is interesting to note that the iPod's performance at 66MHz
is en par with the PSP's performance at 66MHz and the iPod's
no-sound performance at 81MHz is almost matched with the PSP's
100MHz performance. The main limiting factor is raw processing
power (there is only so much that can be improved through code
optimizations). Note, however, that overclocking the iPod to
81MHz causes the iPod to heat up very quickly and can cause
iPodLinux to become unstable or shut down (after a certain
temperature). Thus, normal usage should be measured in terms
of 78MHz.

To accommodate the Sansa e200's lack of touch input, the
Record button is used as an input modifier (i.e. similar to a
Ctrl or Alt key on a keyboard). Due to how video blitting is
done, however, the screen sometimes shifts to the right when
entering/exiting the emulator's menu and stays like that.
This is due to the non-concurrency of screen updating with
hotdog and gpSP's screen code (particularly the memcpy used
by the "enter_menu()" function) - I am currently unable to
think of a way around this. As well, backlight brightness
control is unavailable for the Sansa as the code has not
yet been ported over from the Rockbox code.


== Preparing ==

Usage of igpSP is the same as with the PSP and GP2X versions.

First you need a legit BIOS dump of your GameBoy Advance named
'gba_bios.bin' and placed in the same directory as the igpSP
binary. The GBA BIOS is copyright material; distribution of the
file is illegal. Do not ask others for where to find it. For
instructions on dumping your BIOS file from your GBA hardware,
see http://wiki.pocketheaven.com/GBA_BIOS

After you have a legit BIOS dump, you will want to get some
games (ROMS) to use with the emulator. If you want to play
commercial games, you will have to dump them from your own
cartriages. Distribution of commercial ROM files is illegal.
There are, however, hundreds of free, public domain homebrew
games written for the GBA that will work on igpSP. To download
some of these free, public domain ROMs, visit PDRoms at
http://www.pdroms.de/files/gameboyadvance/
For instructions on dumping ROMS from your own cartridges,
see http://www.gameboy-advance.net/gba_roms/make_gba_roms.htm

Once you have your 'gba_bios.bin' and ROM dumps, copy the files
along with the igpSP binaries and the 'game_config.txt' file
directly to your iPod. You can place the ROM files anywhere you
want provide that you're willing to spend the time to browse
to them on your iPod later. All the other files need to be put
in the same location.

Now you're prepared to run the emulator. igpSP can be either
started up directly or run with a ROM file as its argument.
One method is to browse to the igpSP binary from podzilla2 and
execute it. You may also use Loader 2 to execute the igpSP
binary with a ROM file as an optional argument.


== Controls ==

Once you find your way into igpSP, you will want to know how
to navigate around. To change games, go to the "Load new game"
menu item and browse around for the ROM file you want to play
(gpSP recognizes only files with a '.gba' and '.bin' file
extension).

Switch between in-game and menu by using
the hold switch at the top of your iPod.

In the igpSP menu, the default controls are as followed:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left (on settings)
FAST FORWARD     = right (on settings)
CENTRE           = selection (of menu item)
HOLD SWITCH      = exit menu

In the igpSP menu, if the "Menu scrolling" setting is "On":
SCROLL LEFT      = up
SCROLL RIGHT     = down
REWIND           = left (on settings)
FAST FORWARD     = right (on settings)
CENTRE           = selection (of menu item)
MENU             = exit menu

In-game, controls are based on wheel touches as well as key
presses. They are as followed.
TOUCH UP         = up
TOUCH DOWN       = down
TOUCH LEFT       = left
TOUCH RIGHT      = right
TOUCH UP-RIGHT   = A
TOUCH UP-LEFT    = B
TOUCH DOWN-RIGHT = start
TOUCH DOWN-LEFT  = select
MENU             = up + B
PAUSE/PLAY       = down + B
REWIND           = left + B
FAST FORWARD     = right + B
CENTRE           = A
HOLD SWITCH      = enter menu

In game, if the "Map L/R Triggers" setting is "On":
REWIND           = left trigger
FAST FORWARD     = right trigger

In game, if the "Rapid Fire A & B" setting is "On":
TOUCH UP-RIGHT   = A (rapid fire)
TOUCH UP-LEFT    = B (rapid fire)

For the Sansa e200, in-game controls are based on using
the Record button as an input modifier:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left
FAST FORWARD     = right
CENTRE           = A
POWER            = B
HOLD SWITCH      = enter menu
RECORD + MENU    = select
RECORD + PLAY    = start
RECORD + REWIND  = left trigger
RECORD + FORWARD = right trigger
RECORD + CENTRE  = select
RECORD + POWER   = start

Note that igpSP sometimes freezes when exiting.
If this happens, just force reboot your iPod
(see the "Key_Combinations" iPodLinux wiki page).
For the Sansa e200, hold the Power button for 8 seconds.


== Tweaking ==

Once you get familiar with igpSP, you may want to tweak some
settings to improve speed for your game.

There are two builds of igpSP: one with sound and one without.
The build with sound will playback in-game sound as expected;
however, due to slow emulation speed from the iPod's
insufficient hardware, the sound output may be very choppy.
The build with no sound has sound support almost completely
removed and, as a result, is significantly faster. It is highly
recommended that the build without sound be used for speed.

igpSP can also be sped up through setting tweaks at the cost
of proper emulation. In the "Frameskip and Sound" or "Frameskip
Options" sub-menu are settings pertaining to frameskipping.
The default frameskip value is 4 though gameplay is still smooth
at 2-3. Frameskip values after 6 do not offer much improvement.

In the "iPod Tweaks and Options" sub-menu are many iPod-specific
options that may offer speed-ups or better emulator usage:
iPod Tweaks and Options:
- The "Scale type" setting will change the different styles of
  screen scaling; "Scale to width" is the default. Even though
  "Scale to width" and "Unscaled" does not cover the entire
  screen, extra centring calculations offsets any speed gains.
- The "Smooth type" setting will change how pixel blending
  is applied to the screen. "None" is the fastest without any
  smoothing but may look pixelated. "Smooth" will make the
  screen look smoother while "Smoother" will make the screen
  even smoother. Using smoothing will slow things down slightly
  but not by a noticable amount.
- The "Alpha Blending" setting controls whether or not alpha
  blending is used in the emulation. Turning alpha blending off
  may improve emulation speeds depending on your game but does
  so at the price of graphics and proper emulation.
- The "Synchronize" setting controls whether or not artificial
  delays are introduced to maintain real-time speed. Since the
  iPod is usually unable to achieve full-speed anyway, turning
  this off will most likely not help much (except for some low
  intensity games).
Input settings:
- Items in this submenu control how input is handled in-game
  and in the igpSP menu. See the above "== Controls =="
  section for more info.
Hardware settings:
- The "CPU Speed" setting controls the speed of your iPod's
  CPU. The default speed is "Overclocked" at 78MHz with
  "Underclock" being 66MHz, "Normal" being 75MHz and "Max
  Overclock" being 81MHz. While changing the CPU speed to 81MHz
  will allow for faster emulation, 81MHz will cause your iPod
  to warm up and become unstable after prolonged use (i.e.
  when used with backlight for over 20 minutes). USE 81MHz
  WITH CAUTION!
- The "Brightness" setting controls the brightness level of
  the iPod's backlight. Brightness control will only work with
  iPod nano and videos.
- The "Toggle Backlight" option toggles the backlight on/off.


== To Do ==
- Somehow fix video_blend expand_blend assembly to work on iPod?
- Wait for Exophase's next version of gpSP ; )


== Thanks ==
- Exophase:       Developer of the amazing gpSP emulator
- Zaphod:         First ported gpSP to iPodLinux
- ducky & fellni: Developers of the iPod-optimized iBoy emulator
- iPL Devs:       Developers of hotdog & iPodLinux, and various help
- Keripo:         Re-ported gpSP cleanly and current maintainer


== Changelog ==
K7:
- Added input mapping for the Sansa e200 (port still has video
  scaling issues)
K6:
- Fixed backlight setting (now setting instead of toggle)
- Moved hardware settings into new submenu
- Better screen smoothing code (now more "blend" than "blur")
- Added backlight brightness control
- Added .ini configuration file to save iPod-specific settings
- Removed monochrome support as it turns out hotdog doesn't
  work at all on monochromes
K5:
- Fixed sound output - no longer as crappy!
- Fixed L/R trigger mapping so touch doesn't conflict
- Rewrote scaling code and added centring code
  (scale to width and fullscreen for 5G iPods should be faster)
- Resorted menus a bit
- Completed no-sound hack (now sound support 100% removed)
- Added support for monochrome iPod
  (slower than colour iPods due to extra pixel conversions)
- Added volume control
- Added rapid-fire setting for touch A and B
- Added 66MHz CPU speed (mostly for PSP comparison purposes)
- Added synchronize_flag toggle (very slight speed-up)
- Added partial video_blend expand_normal assembly borrowed from
  GP2X build (screen blending faster and no more random pixels)
- Added screen smoothing setting (very minimal slowdown)

K4:
(no records kept - see iPL wiki upload comments or release post)

