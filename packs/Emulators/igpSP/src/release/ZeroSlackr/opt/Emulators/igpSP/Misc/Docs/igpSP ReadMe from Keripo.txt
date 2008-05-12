Last updated: May 11, 2008
~Keripo

== Intro ==
This is a port of Exophase's amazing GameBoy Advance emulator,
originally written for the PSP, later ported over to GP2X, and
now ported to iPodLinux (it has also been ported to a few other
platforms such as the iPhone, etc.). For more information on
the emulator itself (gpSP), see the documents in the "docs"
directory. For information on the iPodLinux port, see the iPL
wiki page (http://ipodlinux.org/Igpsp) or search the iPodLinux
forums (http://www.ipodlinux.org/forums/).

== History ==
igpSP was first ported to iPodLinux by Zaphod and then later
optimized by zacaj. After a period of inactivity, Keripo took
Zaphod's work and did a proper port of the very latest version
of gpSP as a separate build target, then further optimized the
port by incorporating some of zacaj's changes and rewriting and
cleaning up some of the code. As of the moment, the emulator is
able to run at around 40-70% real-time speed (depending on the
game/homebrew) compared to the full-speed PSP and GP2X versions.

== Code ==
Even though the iPod's hardware capability is far below that of
the PSP and GP2X both in raw processing power and dedicated
hardware, the iPodLinux port takes advantage of numerous things
to achieve a decent, somewhat "playable" speed. The iPod port
is partially based on the GP2X and desktop versions and takes
advantage of some ARM optimizations from the GP2X version.
The iPod-optimized "hotdog" graphics engine is used instead of
SDL and the iPod's co-processor (COP) is used for calculations
in screen blitting. Zaphod originally borrowed the COP code and
key/touch input code from ducky and fellni's iBoy (iPodLinux's
GB/GBC emulator). The menu used is slimmed down and with an
added "Alpha Blending" toggle to enable/disable alpha blending
(for which disabling offers some game speed-ups at the price of
not-so-nice graphics). The "no-sound" build also has most sound
related lines removed (initialization, syncing, timers, etc.)
and drastically improves speed.

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

Once you have your 'gba_bios.bin' and ROM dumps, copy the files
along with the igpSP binaries and the 'game_config.txt' file
directly to your iPod. You can place the ROM files anywhere you
want provide that you're willing to spend the time to browse
to them on your iPod later. All the other files need to be put
in the same location.

Now you're prepared to run the emulator. igpSP can be either
started up directly or run with a ROM file as its argument.
One method is to browse to the igpSP binary from podzilla2 and
execute it. You may also use Loader 2 but you will have to
change the working directory to the same location as your binary
(i.e. symlink 'minix' to 'cd', have Loader 2 execute a script
file which will 'cd' to the right location then execute your
igpSP binary with a ROM file as an optional argument).

== Controls ==
Once you find your way into igpSP, you will want to know how
to navigate around. To change games, go to the "Load new game"
menu item and browse around for the ROM file you want to play
(gpSP recognizes only files with a '.gba' and '.bin' file
extension).

Switch between in-game and menu by using
the hold switch at the top of your iPod.

In the igpSP menu, the controls are as followed:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left (on settings)
FAST FORWARD     = right (on settings)
CENTRE           = selection (of menu item)
HOLD SWITCH      = exit menu

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
REWIND           = left trigger
FAST FORWARD     = right trigger
CENTRE           = A
HOLD SWITCH      = enter menu

== Tweaking ==
Once you get familiar with igpSP, you may want to tweak some
settings to improve speed for your game.

There are two builds of igpSP: one with sound and one without.
The build with sound will playback in-game sound as expected;
however, due to slow emulation speed from the iPod's
insufficient hardware, the sound output may be very choppy
(especially for commercial games) and generally not worth
having on. The build with no sound has sound support completely
removed and, as a result, is significantly faster. It is highly
recommended that the build without sound be used for speed.

igpSP can also be sped up through setting tweaks at the cost
of proper emulation. In the 'Graphics and Sound' sub-menu are
settings pertaining to frameskipping and an added one for alpha
blending. The frameskip value is recommended to be set at 4 to
6 for speed (2 to 3 if things get too choppy). Depending on
your game, turning alpha blending off may help improve speeds
but at the cost of poorer graphics (i.e. areas that make use
of transparency will instead be solid colours). How you decide
to tweak your settings depends on both your game and how much
true-emulation you're willing to sacrifice for emulation speed.

== Thanks ==
- Exophase: Developer of the amazing gpSP emulator
- Zaphod:   First ported gpSP to iPodLinux
- zacaj:    Continued Zaphod's work with optimizations
- Keripo:   Re-ported gpSP cleanly and current maintainer
- iPL Devs: Developing hotdog and porting Linux to the iPod

