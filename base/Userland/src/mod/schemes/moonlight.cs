\name Moonlight
# Default scheme for ZeroLauncher, based of Black Glass.
# by Keripo
# Last updated: March 22, 2008

\def clear	#FF00FF
\def black	#000
\def white	#fff
\def slate	#304370
\def dkgray	#505050
\def grad	<#222 to #111 to #333 with #555 @:1,0,55%,0>

header:
	bg => b@moonlight/titlebar-mono.png m@moonlight/titlebar-mini.png p@moonlight/titlebar-photo.png n@moonlight/titlebar-nano.png v@moonlight/titlebar-video.png,
	fg => white,
	line => black,
	accent => slate,
	shine => slate,
	shadow => dkgray

music:
	bar => grad,
	bar.bg => slate

battery:
	border => #666,
	bg => black,
	bg.low => #777,
	bg.charging => #444,
	fill.normal => slate +1,
	fill.low => slate +1,
	fill.charge => black +1,
	chargingbolt => #666

lock:
	border => black,
	fill => slate

loadavg:
	bg => black,
	fg => slate,
	spike => dkgray

window:
	bg => b@moonlight/background-mono.png m@moonlight/background-mini.png p@moonlight/background-photo.png n@moonlight/background-nano.png v@moonlight/background-video.png,
	fg => white,
	border => white -3

dialog:
	bg => black,
	fg => white,
	line => #666,
	title.fg => white,
	button.bg => black,
	button.fg => #777,
	button.border => dkgray,
	button.sel.bg => #333,
	button.sel.fg => white,
	button.sel.border => white,
	button.sel.inner => #222 +1

error:
	bg => black,
	fg => white,
	line => #666,
	title.fg => #700,
	button.bg => black,
	button.fg => #777,
	button.border => dkgray,
	button.sel.bg => #333,
	button.sel.fg => white,
	button.sel.border => white,
	button.sel.inner => #222 +1

scroll:
	box => #222 +3,
	bg => clear +1,
	bar => slate +2

input:
	bg => black,
	fg => white,
	selbg => slate,
	selfg => black,
	border => white,
	cursor => dkgray

menu:
	bg => clear,
	fg => white,
	choice => slate,
	icon => #222,
	hdrbg => grad +2 *1,
	hdrfg => white,
	selbg => slate,
	selfg => white,
	selchoice => black,
	icon0 => #222,
	icon1 => #222,
	icon2 => #111,
	icon3 => black

slider:
	border => black,
	bg => slate,
	full => grad

textarea:
	bg => clear,
	fg => white

box:
	default.bg => <#222 to #333 to #000>,
	default.fg => white,
	default.border => #444,
	selected.bg => <#fff to #ccc to #000>,
	selected.fg => white,
	selected.border => white,
	special.bg => <#005 to #006 to #002>,
	special.fg => white,
	special.border => #008

button:
	default.bg => <#222 to #111 to #333 with #555 @:0,0,55%,0>,
	default.fg => white,
	default.border => #333,
	selected.bg => <#333 to #222 to #444 with #666 @:0,0,55%,0>,
	selected.fg => white,
	selected.border => #444,
