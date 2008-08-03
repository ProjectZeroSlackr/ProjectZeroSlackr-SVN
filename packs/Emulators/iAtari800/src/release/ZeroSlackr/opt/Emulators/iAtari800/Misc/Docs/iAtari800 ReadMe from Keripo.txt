Last updated: Aug 3, 2008
~Keripo


iAtari800 2.0.3 K2 build
port by Keripo


== Intro ==

iAtari800 is a port of Atari800, an Atari 800,
800XL, 130XE and 5200 emulator, to iPodLinux.
There are currently two ports; one based on the
SDL port and one written completely from scratch
using iPod-optimized code and the hotdog library.
Both versions only run on colour iPods (i.e.
iPod colour/photo/nano/video). Atari800 was
originally ported to iPodLinux independently by
puzzud through modifying the SDL port but only
supported iPod nanos and had poor input mapping;
iAtari800 is a completely separate port by Keripo.
For more information on Atari800, see the Atari800
SourceForge project page.


== Code ==

The SDL version uses the original SDL code for
video blitting whereas the hotdog version uses the
iPod-optimized hotdog graphics engine. Both versions
use the iPod's co-processor (COP) for the blitting
so speed is drastically improved (at the cost of
occasional graphical glitches).

The hotdog version has fullscreen, scaled, and
uncropped scale types. Fullscreen mode ignores the
black border and stretches to screen dimensions.
Scaled ignores the black border and scales with the
original Atari screen dimensions (336 x 240).
Uncropped includes the black border and stretches to
screen dimensions. The hotdog version also has added
pixel smoothing to make things look less pixelated
on smaller screens. The SDL version only has the
uncropped mode and no pixel smoothing.

The SDL version uses SDL key events for input. The
hotdog version uses wheel touches and direct console
reading, allowing for more and faster input. Note
that consequently, the key mapping for the two versions
are different; see the "== Input ==" section.


== Controls ==

Due to the limited input methods of the iPod (notably
a lack of a keyboard equivalent), iAtari800's key mapping
will only allow playing of joystick-based games; do
not run software requiring text input.

The following is the key mapping of the SDL version
of iAtari800. Note that the scroll wheel is used due
to the iPod's limited number of buttons. The wheel
should be scrolled about a third turn for the input
to trigger. A temporary lag may follow after using
the scroll wheel.

In-game input:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left
FAST FORWARD     = right
CENTRE           = fire
SCROLL RIGHT     = start
SCROLL LEFT      = select
HOLD SWITCH      = emulator menu

In-menu input:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left
FAST FORWARD     = right
CENTRE           = enter
SCROLL RIGHT     = 'x' character
SCROLL LEFT      = 'y' character
HOLD SWITCH      = exit menu

The following is the key mapping of the hotdog
version of iAtari800. Keep in mind that this version
uses touch-based input for in-game, thus allowing
better mapping and the additional "option" key.

In-game input:
TOUCH UP         = up
TOUCH DOWN       = down
TOUCH LEFT       = left
TOUCH RIGHT      = right
CENTRE           = fire
TOUCH DOWN-RIGHT = start
TOUCH DOWN-LEFT  = select
TOUCH UP-RIGHT   = option
HOLD SWITCH      = emulator menu

In-menu input:
MENU             = up
PAUSE/PLAY       = down
REWIND           = left
FAST FORWARD     = right
CENTRE           = enter
SCROLL RIGHT     = 'x' character
SCROLL LEFT      = 'y' character
HOLD SWITCH      = exit menu


== Command-line Options ==

"-fullscreen"      Run fullscreen and cropped [default]
"-scaled"          Run scaled and cropped
"-uncropped"       Run fullscreen and uncropped
"-smoothing"       Blend pixels [default]
"-no-smoothing"    Do not blend pixels (faster but pixelated)
"-cop"             Use co-processor [default]
"-no-cop"          Do not use co-processor (slower but no graphical glitches)


== To Do ==

Unlike igpSP, I do not plan on adding many additional
iPod-specific features. I am also not very familiar
with Atari's themselves (never having seen a real one)
so there isn't much that I'd change concerning the
emulator itself. Apart from fixes and occasional
updates to sync with the main source code, I consider
the iAtari800 port complete without anything else left
to to ; )


== Changelog ==
K2:
- Cleaned up SDL-based port, added COP and screen modes
- New port using iPod-optimized hotdog library and
  touch-based input (preferred over SDL port)
K1:
- Initial, unoptimized SDL-based port

