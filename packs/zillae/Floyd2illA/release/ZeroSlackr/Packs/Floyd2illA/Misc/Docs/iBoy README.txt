

IBOY Version 0.1.0 README


	Quickstart

- You will need iPodLinux on a 4G or Mini iPod
- Compile (binary on sf.net)
- Edit iboy.cfg and copy it to /etc
- Execute iboy without parameters
- Have problems or want to know all features? Read on!
	
	
	Intro
	
iBoy of course is GPL Software. See LICENSE for more
information.


	Features
	
- Emulating original Gameboy in 4 grayscales.
- Configurable throug a general config file, most options
  overloadable throug per-rom config files. 
- Graphical menu through Hold Button
- Load and Save of game state in several slots on a
  per-rom basis.
- Native assembler code to speed things up a bit
- Experimental coprocessor support
- Experimental sound
- 4G iPods fully supported


	Future plans and/or not supported

- 1,2,3G not supported, eventually never, speed issues
- Mini, Photo (Nano?) not yet supported
- GB Cardrigde Ram, RTC not yet supported
- Full speed not yet reached


	Configuration
	
The main configuration file is iboy.cfg which must be 
located in /etc. There are VERY strict format rules for 
the configuration files, which are:

<variable>=<integer>   or   <variable>=<literal>

Where <variable> is a configurable property of iboy,
<integer> is a number, also used for boolean values as
1 for 'true' and 0 for 'false' and <literal> is a sequence
of characters standing for their self or representing 
an integer number (as a symbolic constant).

- One of those two can be used each line
- Each variable=<value> pair must be immediately followed
  by '\n'
- No special characters, other whitespaces or comments allowed


The two most important entries are:

romdir=/abspath/
savedir=/abspath/

They must be set in iboy.cfg. Please notice the
closing / which is expected and make sure that savedir 
is writeable. Roms in romdir must end with .gb and
should not be very longer than 10 chars. 


	Further Configuration

romfile=somename.gb		Start this, no menu at startup
						somename.gb is looked up in romdir
interlace=<0,1>			Ugly interlacing brings speed
						defaults to 0
y_offset=<0..16>		Shift gbscreen y_offset pixels up
						defaults to 16
y_flip=<0,1>			FIXME: pressing ??? toggles y_offset
						between 0 and original y_offset
						defaults to 1
oss_enabled=<0,1>		defaults to 0, will most likely 
						not work or sound ugly
						

	Keypad Configuration
	
Default is:
	
kb_up=PAD_B
kb_down=PAD_B
kb_left=PAD_B
kb_right=PAD_B
kb_center=PAD_A

kb_t0=PAD_UP
kb_t1=PAD_A
kb_t2=PAD_RIGHT
kb_t3=PAD_START
kb_t4=PAD_DOWN
kb_t5=PAD_SELECT
kb_t6=PAD_LEFT
kb_t7=PAD_B

This needs some explanation: kb_up, kb_down, etc are events
when the wheel is _pressed_. Apple would say 'play' instead of
kb_down. The corresponding Gameboy Pad action is PAD_yyy.
kb_t0 to kb_t7 are sections on the touchwheel starting at 
12:00 ('menu') clockwise in 45degree steps. Each
corresponding Gameboy Pad action is fired when the section
is _touched_. It makes sense to change these controls
on a per-rom basis.

Please notice that the PAD_ literals are symbolic constants.
If you want to disable a key or section use kb_xxx=0.


	Per-ROM Configuration
	
Per-rom config files can be located in 'romdir', 
then they must have the same name as the ROM but end 
with .cfg. They overrule iboy.cfg but they don't have
to be used.
Example: mario.gb in /home/roms/ --> mario.cfg in /home/roms/

You probably want to set: interlace, y_offset, y_flip
or the kb_ Keypad variables in the per-rom config files.


	Compiling
	
Just use the Makefile. Options are:
make IPOD_VERSION=IPOD_MINI
make IPOD_VERSION=IPOD_4G		
make						(same as IPOD_VERSION=IPOD_4G)


	FAQ
	
Q:	Why is there no FAQ?
A:	Because nobody asks questions but this one.


	Ducky and Fellni want to thank...
	
aegray for great support and coding work	
bholland in advance for mini support

and of course all other supporters, users and especially 
the developers of gnuboy, the piece of gnu software written 
so portable and generic that it was quite easy to adopt to 
run on Apple iPods. We also appreciate the fantastic work 
of the iPod Linux team.

Thank you!


	Is this really nesessary?

Apple, iPod, Nintendo, Gameboy are registered trademarks 
and copyrighted and protected in a lot of ways I don't 
exactly know, see their licenses etc. etc. to know 
what you may or, more likely, may not do.




GNUBOY README


  INTRO

Welcome to gnuboy, one of the few pieces of Free Software to emulate
the Game Boy handheld game console. Written in ANSI C with a few
optional assembler optimizations for particular cpus, gnuboy supports
a wide range of host systems, and has been tested successfully on:

  GNU/Linux
  FreeBSD
  OpenBSD
  BeOS
  Linux/390 (IBM S/390 Mainframe)
  SunOS/Sun Ultra60
  IRIX/SGI O2
  IRIX/SGI Indy
  AIX/Unknown
  DR-DOS
  MS-DOS
  Windows DOS box
  Windows 9x/NT/2k

Additionally, gnuboy should run on any other *nix variants that have
ANSI C compilers and that are remotely POSIX compliant. As gnuboy is
Free Software, you're welcome to fix any problems you encounter
building it for a particular system, or to port it to entirely new
systems.


  EMULATION

gnuboy emulates nearly all aspects of the (Color) Gameboy, including
all of the following and much more:

  Full GBZ80 instruction set.
  Scanline-based LCD engine.
  Ten sprites per scanline limit.
  Support for all CGB graphics extensions.
  Sprite DMA, HDMA, and GDMA.
  All four sound channels including digital samples.
  MBC1, MBC2, MBC3 (including clock), and MBC5 mappers.
  Pad, timer, divide counter, and other basic hardware registers.
  CGB double-speed CPU mode.

Aspects not emulated at this time include:

* Serial IO (link cable).
  Undocumented 'extra' ram in OAM space on Gameboy Color.
  Wave pattern memory corruption when sound channel 3 is played.
  All Super Gameboy extensions.
* HuC1, HuC3, and other obscure mappers.
  Sorting sprites by X coordinate in DMG mode.
  HALT instruction skipping in DMG mode.
  CPU stalls during HDMA and GDMA.

Only the two marked by * are known to affect the playability of
actual games or demos; the rest are just listed for completeness'
sake.


  FEATURES

In addition to basic emulation, gnuboy provides the following
features:

  Highly flexible keybinding and configuration subsystem.
  State saving and loading at any point.
  Very precise timing/synchronization, preserved across save/load.
  Joystick support on Linux, DOS, and all SDL-based ports.
  Fully customizable palettes for DMG games.
  Screen scaling by a factor of 2, 3, or 4 in all ports.
  Hardware-based screen scaling on platforms where it's available.
  Debug traces to stdout.
  Dynamic palette allocation when run in 256-color modes...
  OR simulated 3/3/2 bits per channel in 256-color modes.

For information on configuring and using these features, see the
additional documentation in... (??)


  COMPATIBILITY

Out of over 300 results reported by testers, all games are known to
work perfectly on gnuboy with the following exceptions:

  Fighting Phoenix (Japanese) is expected not to work since it uses
  the HuC1 memory controller, which is not implemented. There has been
  no official report so far, however.

  Robopon Star and Sun both require HuC3 which is not implemented.
  Both hang immediately after the first intro screen and are thus
  unplayable.

  Monster Go! Go! Go! (Japanese) is unplayable. The cause of the
  problem is not known, but reports indicate that it might be a bad
  dump.

  Final Fantasy Legend III freezes momentarily when entering the
  temple with the portal to the Talon, and the portal animation itself
  appears incorrectly. All other emulators tested seem to have this
  problem as well. Cause unknown, could be mapper-related. Does not
  significantly affect gameplay.

  Bubble Bobble 2 has some minor tile glitches right before gameplay
  actually begins. Cause unknown. Does not affect gameplay.

  Alone in the Dark is reported to have minor visual glitches. I
  haven't seen it myself so I can't judge their severity.

  Zelda: Oracle of Ages is reported to have a visual glitch at the
  beginning of the game. I haven't seen it, but presumably it does not
  affect gameplay significantly.

Please report any other incompatibilities discovered directly to
gnuboy@aerifal.cx, so that they can be documented and hopefully
fixed.


  FUTURE / WISHLIST

Here's a brief list of what may appear in gnuboy in the future:

  Screenshots.
  Integrated debugger.
  Super Gameboy support.
  Serial link over the internet.
  Serial link to a real Gameboy with a custom cable.
  Configurable color filters to provide more authentic LCD look.
  Custom colorization of DMG games on a per-tile basis.
  Support for more colorspaces in the hardware scaler.
  Recording audio.
  GBS player built from the same source tree.
  Full recording and playback of emulation.
  So-called "high level emulation" of certain typical dumb loops.

Features that are not likely to appear soon or at all include:

  Rumble support - this would be nice, but SDL doesn't seem to support
  force-feedback yet. We'll see about it in the long-term though.

  Eagle/2xSaI/etc. - probably not feasible since these libraries don't
  appear to be compatible with the terms of the GPL. We might work on
  our own interpolation engine eventually, but that's low priority.

  GUI/GUI-like features - such things are best handled by external
  front-ends. We might eventually add a mechanism for external
  programs to communicate with gnuboy and reconfigure it while it's
  running, however.

  Plugins - NO! The way I see it, plugins are just an attempt to work
  around the GPL. In any case, even if you are adding plugin support
  yourself, you are bound by the terms of the GPL when linking ANY
  code to gnuboy, including dynamic-linked modules. However we'd
  rather not deal with this mess to begin with.

  Compressed ROMs/Saves - this one is very iffy. On most systems, this
  is redundant; *nix users can just pipe the rom through a
  decompression program, and Windows users can just double-click or
  drag files from their favorite GUI unzipper program. Linking to zlib
  isn't really acceptable since it's massively bloated and we don't
  want to include it with gnuboy or add external dependencies. We may,
  however, write our own tiny decompressor to use at some point.

Ideas and suggestions for other features are welcome, but won't
necessarily be used. You're of course also free to add features
yourself, and if they fit well into the main tree they may eventually
get included in the official release. See the file HACKING for more
details on modifying and/or contributing.


  THANKS

Thanks goes out to everyone who's expressed interest in gnuboy by
writing -- users, porters, authors of other emulators, and so forth.
Apologies if we don't get a personal response out to everyone, but
either way, consider your feedback appreciated.


  EPILOGUE

OK, that looks like about it. More to come, stick around...



                                          -Laguna  <laguna@aerifal.cx>











