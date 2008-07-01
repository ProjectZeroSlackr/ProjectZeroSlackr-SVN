\name O Canada
# A special scheme for July 1st, Canada Day!
# by by Keripo
# Last updated: July 1, 2008

\def clear		#FF00FF
\def black		#000000
\def white		#FFFFFF
\def lightred	#FF8080
\def red		#FF0000
\def darkred	#C10000
\def grad		<red to lightred to darkred with red @:1,0,55%,0>

header:
	bg => <lightred to red with darkred @:1,0,55%,0>,
	fg => white,
	line => darkred,
	accent => white,
	shadow => black,
	shine => white,
	gradient.top => red,
	gradient.middle => lightred,
	gradient.bottom => white,
	gradient.bar => red
music:
	bar.bg => clear,
	bar => grad,
	bar.border => darkred
battery:
	border => red,
	bg => lightred,
	bg.low => #FF8000,
	bg.charging => #FFFF00,
	fill.normal => darkred,
	fill.low => red,
	fill.charge => darkred
	chargingbolt => #FFFF00
lock:
	border => darkred,
	fill => red
loadavg:
	bg => lightred,
	fg => darkred,
	spike => darkred
window:
	bg => b@o-canada/background-mono.png m@o-canada/background-mini.png p@o-canada/background-photo.png n@o-canada/background-nano.png v@o-canada/background-video.png,
	fg => black,
	border => darkred
dialog:
	bg => white,
	fg => black,
	line => red,
	title.fg => black,
	button.bg => white,
	button.fg => black,
	button.border => darkred,
	button.sel.bg => red,
	button.sel.fg => white,
	button.sel.border => darkred,
	button.sel.inner => white
error:
	bg => white,
	fg => red,
	line => darkred,
	title.fg => red,
	button.bg => white,
	button.fg => black,
	button.border => darkred,
	button.sel.bg => red,
	button.sel.fg => white,
	button.sel.border => darkred,
	button.sel.inner => white
scroll:
	box => red +3,
	bg => clear +1,
	bar => darkred *1 +2
input:
	bg => white,
	fg => black,
	selbg => red,
	selfg => white,
	border => darkred,
	cursor => red
menu:
	bg => clear,
	fg => black,
	choice => red,
	icon => black,
	hdrfg => white,
	hdrbg => grad *2,
	selbg => red,
	selfg => white,
	selchoice => red,
	icon0 => white,
	icon1 => white,
	icon2 => white,
	icon3 => white
slider:
	border => darkred,
	bg => clear,
	full => grad
textarea:
	bg => clear,
	fg => black
box:
	default.bg => white,
	default.fg => black,
	default.border => darkred,
	selected.bg => red,
	selected.fg => white,
	selected.border => darkred,
	special.bg => red,
	special.fg => white,
	special.border => white
button:
	default.bg => white,
	default.fg => black,
	default.border => darkred,
	selected.bg => red,
	selected.fg => white,
	selected.border => darkred
