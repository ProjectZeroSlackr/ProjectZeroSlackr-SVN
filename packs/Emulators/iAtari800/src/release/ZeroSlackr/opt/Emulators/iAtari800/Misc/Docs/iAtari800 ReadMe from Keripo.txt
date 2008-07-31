Last updated: July 31, 2008
~Keripo

iAtari800 2.0.3 K1 build
port by Keripo


== Intro ==
iAtari800 is a port of Atari800, an Atari 800,
800XL, 130XE and 5200 emulator, to iPodLinux.
The port is based on the SDL version but slimmed
down and modified for the iPod's controls.
Atari800 was originally ported to iPodLinux
independently by puzzud but supported only iPod
nanos and had poor input mapping; iAtari800 is
a completely separate port by Keripo.


== Controls ==

The following is Atari800's input mapping. Note
that the scroll wheel is used due to the iPod's
limited number of buttons. The below input will
only allow playing of joystick-based games; do
not run software requiring text input.

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

Note that for scrolling left/right, the wheel
should be scrolled about a third turn for the
input to trigger. A temporary lag may follow
after using the scroll wheel.


== Note on Menu == 

Note that the emulator menu is the original
and not optimized for the iPod. On smaller screened
iPods (e.g. iPod nano), the screen may be too small
to read clearly. It is recommended you download and
try the computer version of Atari800 to familiarize
yourself with the emulator's menu (press F1). This
issue may be solved in the future if I add pixel
blending code, but the menu font may still be too
small.

== Changelog ==
K1:
- Initial, unoptimized SDL port

