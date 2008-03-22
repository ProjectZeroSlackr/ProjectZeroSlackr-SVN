\name Blood Cross
# Last updated: March 14, 2008
# ~Keripo

# Due to a bug in TTK, using #FF00FF will allow for transparency ; )
\def clear	#FF00FF
\def black	#000000
\def white	#ffffff
\def gray	#808888
\def nearblack	#eeeeee
\def red	#D40500
\def darkred	#9E0400
\def grapbot	#000000
\def grapmid	#333333
\def graptop	#555555
\def grapbar	#888888

# Original titlebar pic from
# http://www1.istockphoto.com/file_thumbview_approve/704742/2/
#  istockphoto_704742_bloody_sky_background.jpg
header:
	bg => m@bloodcross/titlebar-mini.png b@bloodcross/titlebar-mono.png n@bloodcross/titlebar-nano.png p@bloodcross/titlebar-photo.png v@bloodcross/titlebar-video.png,
	fg => white,
	line => darkred,
	accent => #6ae,
	shadow => black,
	shine => #888888,
	gradient.top => graptop,
	gradient.middle => grapmid,
	gradient.bottom => grapbot,
	gradient.bar => grapbar +1

music:
	bar => <vert #000000 to #9E0400 to #D40500>,
	bar.bg => clear

# Battery pic from
# http://www.t-shirtking.com/graphics/153-00160.jpg
battery:
	border => black,
	bg => <vert #a0adb8 to #d8e2e6>,
	fill.normal => @bloodcross/battery.png,
	fill.low => #C03020,
	fill.charge => @bloodcross/battery.png,
	bg.low =>  <vert #a0adb8 to #d8e2e6>,
	bg.charging => <vert #a0adb8 to #d8e2e6>

lock:
	border => #282C28,
	fill => #383C40

loadavg:
	bg => #E8F4E8,
	fg => #68D028,
	spike => #C0D0D8

# Original background pic from
# http://www.proa.org/exhibicion/serrano/sala2/12.html
window:
	bg => m@bloodcross/background-mini.png b@bloodcross/background-mono.png n@bloodcross/background-nano.png p@bloodcross/background-photo.png v@bloodcross/background-video.png,
	fg => white,
	border => gray -1

dialog:
	bg => black,
	fg => white,
	line => gray,
	title.fg => white,
	button.bg => black,
	button.fg => white,
	button.border => gray,
	button.sel.bg => <vert darkred to red>,
	button.sel.fg => white,
	button.sel.border => black,
	button.sel.inner => <vert darkred to red> +1

error:
	bg => black,
	fg => white,
	line => gray,
	title.fg => white,
	button.bg => black,
	button.fg => white,
	button.border => gray,
	button.sel.bg => <vert darkred to red>,
	button.sel.fg => white,
	button.sel.border => black,
	button.sel.inner => <vert darkred to red> +1

scroll:
	box => black +2,
	bg => clear +1,
	bar => <horiz red to darkred to #111111> +1

input:
	bg => black,
	fg => white,
	selbg => <vert darkred to red>,
	selfg => white,
	border => gray,
	cursor => #808080

menu:
	bg => clear,
	fg => white,
	hdrbg => <darkred to black to darkred with darkred @:1,0,55%,0> +2 *1,
	hdrfg => white,
	choice => nearblack,
	icon => nearblack,
	selbg => <vert darkred to red>,
	selfg => white,
	selchoice => black,
	icon0 => black,
	icon1 => clear,
	icon2 => clear,
	icon3 => white

slider:
	border => white,
	bg => <horiz black to #555555 to #4F4A4B>,
	full => <horiz red to darkred to #111111>

textarea:
	bg => white,
	fg => clear

box:
	default.bg => <vert darkred to red>,
	default.fg => white,
	default.border => black,
	selected.bg => <vert #3f80de to #2f63d5 to #1e41cd>,
	selected.fg => black,
	selected.border => black,
	special.bg => <vert #d5d6d5 to #d1cfd1 to #c5c6c5>,
	special.fg => white,
	special.border => black

button:
	default.bg => black,
	default.fg => white,
	default.border => gray,
	selected.bg => <vert darkred to red>,
	selected.fg => white,
	selected.border => black
