\name 5-y.2-d
# Scheme using artwork by doujin artist Kantoku (NSFW: 5-y.2-d.jp).
# by by Keripo
# Last updated: Jun 19, 2008

\def clear		#FF00FF
\def cloud		#ECEAFF
\def light		#D5D0FD
\def lightish	#BAB5FD
\def medium		#9D93FD
\def darkish	#8479DF
\def dark		#6357D0
\def white		#FFFFFF
\def black		#000000

header:
	bg => b@5-y.2-d/titlebar-mono.png m@5-y.2-d/titlebar-mini.png p@5-y.2-d/titlebar-photo.png n@5-y.2-d/titlebar-nano.png v@5-y.2-d/titlebar-video.png,
	fg => black,
	line => medium,
	accent => darkish,
	shadow => dark,
	shine => cloud

music:
	bar.bg => cloud,
	bar => @5-y.2-d/scrollbar-horizontal.png +1

battery:
	border => dark,
	bg => cloud,
	bg.low => cloud,
	bg.charging => cloud,
	fill.normal => lightish,
	fill.low => <vert #FF8080 to #FF4040>,
	fill.charge => <vert #80FF80 to #40FF40>,
	chargingbolt => #3000AE

lock:
	border => dark,
	fill => darkish

loadavg:
	bg => cloud,
	fg => darkish,
	spike => lightish

window:
	bg => b@5-y.2-d/background-mono.png m@5-y.2-d/background-mini.png p@5-y.2-d/background-photo.png n@5-y.2-d/background-nano.png v@5-y.2-d/background-video.png,
	fg => black,
	border => medium -3

dialog:
	bg => white,
	fg => black,
	line => medium,
	title.fg => black,
	button.bg => cloud,
	button.fg => black,
	button.border => darkish,
	button.sel.bg => lightish,
	button.sel.fg => black,
	button.sel.border => darkish,
	button.sel.inner => lightish

scroll:
	box => darkish -1,
	bg => clear,
	bar => @5-y.2-d/scrollbar-vertical.png +1

input:
	bg => white,
	fg => black,
	selbg => medium,
	selfg => white,
	border => medium,
	cursor => medium

menu:
	bg => clear,
	fg => black,
	hdrbg => <vert darkish to lightish to light with lightish @:1,0,50,0>,
	hdrfg => white,
	choice => black,
	icon => black,
	selbg => lightish,
	selfg => black,
	selchoice => black,
	icon0 => medium,
	icon1 => dark,
	icon2 => clear,
	icon3 => black

slider:
	border => darkish -2,
	bg => cloud -1,
	full => @5-y.2-d/scrollbar-horizontal.png -1

textarea:
	bg => clear,
	fg => black

box:
	default.bg => cloud,
	default.fg => black,
	default.border => medium,
	selected.bg => medium,
	selected.fg => black,
	selected.border => medium,
	special.bg => cloud,
	special.fg => black,
	special.border => medium

button:
	default.bg => cloud,
	default.fg => black,
	default.border => medium,
	selected.bg => medium,
	selected.fg => black,
	selected.border => medium

error:
	bg => white,
	fg => black,
	line => darkish,
	title.fg => black,
	button.bg => cloud,
	button.fg => black,
	button.border => darkish,
	button.sel.bg => lightish,
	button.sel.fg => black,
	button.sel.border => darkish,
	button.sel.inner => lightish
