Last updated: Sept 29, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
MV Player
- MV Player (also known as MvPD or MoviePod) is compressed video
  player specially designed for iPodLinux. It playbacks compressed
  videos converted to the non-standard, special .mvpd format
  (podzilla legacy has a built-in video player but it is only capable
  of playing uncompressed .avi files which also need pre-converting).
- As of the moment, MV Player only works for iPod nanos and photos;
  if you wish for support for your iPod model, you may try contacting
  the creator (Gaspode) to help.
For more information, see:
- http://www.justgeek.de/moviepod/
- http://ipodlinux.org/Mv_player
- http://ipodlinux.org/Mv_player/Video_Converters
- http://ipodlinux.org/Mv_player/Schemes
- http://ipodlinux.org/forums/viewtopic.php?t=24546 (scheme tutorial)

Usage:
- FastLaunch through "Media > MV Player > FastLaunch" or
  use "edia > MV Player > Videos" to select a video to play.
- Files with a ".mvpd" extension have their default
  action set to MV Player.
- A sample, preconverted YouTube video of Miku-Hatsune of Vocaloid2
  is provided in the ZeroSlackr pack. To convert your own videos,
  see the Video Converters link above (sMooVePoD recommended) and
  place the converted *.mvpd into one of the folders in "Video".
- The menu version of MV Player is also skinnable with a few schemes
  included in the ZeroSlackr pack (to switch schemes you must edit the
  "Conf/mvpd.conf" file manually). To create your own scheme, see the
  tutorial link above.


Original:
- Authors: Gaspode
- Source: Website
- Link:
  http://www.justgeek.de/moviepod/
  http://www.justgeek.de/moviepod/preview/version.0.0.99-rc2/mv_player-0.0.99-rc2.tar.gz
- Date: Jan 30, 2007
- Version: 0.0.99-rc2

ZS Version:
- Modder: Keripo
- Type: Hex edit
- Date: Apr 23, 2008
- Version: B X.X

Modifications:
- Hex'd mvpd.cfg paths:
  /mvpd/mvpd.cfg -> Conf/mvpd.cfg
  /mnt/mvpd/mvpd.cfg -> ../Conf/mvpd.cfg
- comes with pre-converted clip of promotional video of
  Hatsune Miku (Vocaloid2) singing "Miku Miku ni Shiteageru"
  converted with sMooVePoD 0.8 http://www.consumption.net/user/greeze/smoovepod/
  from YouTube video: http://www.youtube.com/watch?v=UnpSAMnGi78&fmt=18
  (originally from: http://www.nicovideo.jp/watch/sm1359820)
- made ZS friendly
- user-made MvPD schemes:
  http://ipodlinux.org/Mv_player/Schemes (many)
  http://www.ipodlinux.org/forums/viewtopic.php?t=29385 (newground)
  http://www.ipodlinux.org/forums/viewtopic.php?p=182123#182123 (rx-7)
  http://www.ipodlinux.org/forums/viewtopic.php?t=23008
  (jwmp - had to download from http://www.ipodlinux.org/forums/viewtopic.php?t=22532)

Launch Module:
- FastLaunches MV Player
- launch video files from file browser

To do:
- none; maybe ask for source code so don't need hex edit?

Changelog 
[keep untouched till beta release]