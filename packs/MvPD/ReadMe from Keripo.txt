Last updated: March 22, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Docs/Original or the license that comes with the
original/ported software.

Overview:
MoviePod
- MoviePod (also known as MvPD or MV Player) is compressed video
  player specially designed for iPodLinux. It playbacks compressed
  videos converted to the non-standard, special .mvpd format
  (podzilla legacy has a built-in video player but it is only capable
  of playing uncompressed .avi files which also need pre-converting).
- As of the moment, MV Player only works for iPod nanos and photos;
  if you wish for support for your iPod model, you may try contacting
  the creator (Gaspode) to help.
- A sample, preconverted YouTube video of Miku-Hatsune of Vocaloid2
  is provided in the ZeroSlackr pack. To convert your own videos,
  see the Video Converters link below (sMooVePoD recommended).
- The menu version of MV Player is also skinnable with a few schemes
  included in the ZeroSlackr pack (to switch schemes you must edit the
  "Conf/mvpd.conf" file manually). To create your own scheme, see the
  tutorial link below.
For more information, see:
- http://www.justgeek.de/moviepod/
- http://ipodlinux.org/Mv_player
- http://ipodlinux.org/Mv_player/Video_Converters
- http://ipodlinux.org/Mv_player/Schemes
- http://ipodlinux.org/forums/viewtopic.php?t=24546 (scheme tutorial)


Original:
- Authors: Gaspode
- Source: Website
- Link:
  http://www.justgeek.de/moviepod/
  http://www.justgeek.de/moviepod/preview/version.0.0.99-rc2/mv_player-0.0.99-rc2.tar.gz
  http://ipodlinux.org/Mv_player/Schemes
- Date: Jan 30, 2007
- Version: 0.0.99-rc2

ZS Version:
- Modder: Keripo
- Type: Hex edit
- Date: March 3, 2008
- Version: B X.X

Modifications:
- Hex'd mvpd.cfg paths:
  /mvpd/mvpd.cfg -> Conf/mvpd.cfg
  /mnt/mvpd/mvpd.cfg -> /opt/MvPD/mvpd.cfg
- comes with pre-converted clip of promotional video of
  Hatsune Miku (Vocaloid2) singing "Miku Miku ni Shiteageru"
  converted with sMooVePoD 0.8 http://www.consumption.net/user/greeze/smoovepod/
  from YouTube video: http://www.youtube.com/watch?v=nEshhGNzfSg
  (originally from: http://www.nicovideo.jp/watch/sm1359820)
  (translated here: http://www.youtube.com/watch?v=3rsBLRFONEs)
- made ZS friendly

Launch Module:
- FastLaunches MvPD
- launch video files from file browser

To do:
- none; maybe ask for source code so don't need hex edit?

Changelog 
[keep untouched till beta release]