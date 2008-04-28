Last updated: Apr 23, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
viPodzilla
- viPodzilla is a customized build of podzilla legacy (PZ0).
- It was one of the older PZ0 forks that were being worked on
  during PZ0's development and one of the most widely used
  during the time due to its frequent updating.
- Originally the first zilla to implement usage of the viP
  text editor, it is now most known for its unique,
  circle-based GUI on a somewhat similar level to iPodVX.
- Since it is based off podzilla legacy, viPodzilla
  will not run smoothly on iPod videos (5G/5.5Gs),
  which were introduced after PZ0 was abandoned.
- Due to lack of source code (e.g. most links are dead now),
  the ZeroSlackr pack uses a hex edited viPodzilla binary
  with no MPD support.
For more information, see:
- http://ipodlinux.org/ViPodzilla
- http://ipodlinux.org/Category:Podzillae

Usage:
- FastLaunch through "Zillae > viPodzilla".


Original:
- Authors: Zsombor Kovacs (zsk009) et al.
- Source: Jonrelay's mirror
- Link:
  www.ipodlinux.org/Vipodzilla
  http://www.kreativekorp.com/ipl/vipodzilla.tgz
  http://ipodlinux.org/images/7/7e/ViPodzilla.tgz (connect4 pics)
- Date: March 3, 2006 (mirrored)
- Version: ???

ZS Version:
- Modder: Keripo
- Type: Hex edit
- Date: Apr 23, 2008
- Version: B X.X

Modifications:
- Lots of hex editing:
  /etc/ -> Data/
  /home/ -> Data/
  /hp/vipodzilla/ -> Data/
  hp/images/ -> Data/
  /etc/podzilla.conf -> Conf/podzilla.conf
  /bin/viP -> Data/viP
  /etc/viP.conf -> Data/viP.conf
  /root/.vip_dict -> Data/.vip_dict
  /sbin/pd -> Data/pd
  /PureData -> Data/PdPod
  /hp/vipodzilla/doom/iDoom -> Data/doom/iDoom.sh
  /etc/KMData/ -> Data/KMData/
  /usr/share/musictools/ -> Data/
  /hp/connect4/ -> Data/connect4/
  nxsnake/boot_image.gif -> Data/boot_image.gif
  nxsnake/banner_image.gif -> Data/banner_image.gif
  map.bmp -> Data/mn
  bucket.bmp -> Data/buckt
  bomb.bmp -> Data/bmb
  /Notes/ -> /mnt/
  /usr/share/dict/ptextwords -> Data/dict/ptextwords
  .tuxchess -> Data/chess
  /metallo.dwi -> Data/ddr.dwi
  worm_highscore -> Data/worm.txt
  simplesnake_highscore -> Data/simplesnake.txt
  %Y-%m-%d_%H%M.ppm -> Data/%m%d%H%M.ppm

Launch Module:
- FastLaunches viPodzilla

To do:
- none; find source and do a proper recompile?

Changelog 
[keep untouched till beta release]