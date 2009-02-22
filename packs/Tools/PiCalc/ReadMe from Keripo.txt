Last updated: Apr 23, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
PiCalc
- PiCalc is a port of Carey Bloodworth's pi-agm to iPodLinux.
- The ZeroSlackr version uses the unoptimized hartley fft algorithm
  but is restricted by the iPod's memory limitations (in my trials,
  anything over 8k digits caused the iPod to freeze).
- My iPod calculated 8192 digits in 37 seconds (used "8k", "2", "-1")
  but the built-in timer is very likely to be inaccurate.
For more information, see:
- http://ipodlinux.org/oldforums/viewtopic.php?t=29515
- http://myownlittleworld.com/miscellaneous/computers/otherpiprograms.html

Usage:
- Terminal launch through "Tools > PiCalc".
- You need to type in all the commands in accordance
  to the printed out instructions.
- The calculated value will be saved as a text file in the
  "Output" folder.


Original:
- Authors: Carey Bloodworth
- Source: myownlittleworld.com mirror
- Link:
  http://web.archive.org/web/20030803090725/http://www.bloodworth.org/
  http://myownlittleworld.com/miscellaneous/computers/otherpiprograms.html
  http://myownlittleworld.com/miscellaneous/computers/files/pi-programs/source/pi-agm.tgz
- Date: Feb 24, 2000
- Version: 2.3.1

ZS Version:
- Modder: Keripo
- Type: Porter
- Date: Apr 23, 2008
- Version: B X.X

Modifications:
- made ZS friendly
- uses an unoptimized fft-hartley build

Launch Module:
- terminal launches PiCalc

To do:
- none; clean build
- maybe also package in different builds, but not worth it
- optimize for arm??? (I don't have the knowledge)

Changelog:
[keep untouched till beta release]