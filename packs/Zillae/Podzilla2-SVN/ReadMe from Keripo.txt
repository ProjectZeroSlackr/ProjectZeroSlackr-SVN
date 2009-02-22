Last updated: Oct 12, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the documents found in
Misc/Docs.tar.gz or the license that comes with the
original/ported software.

Overview:
Podzilla2-SVN
- Podzilla2-SVN is an SVN build of podzilla2 (also known
  as podzilla or PZ2).
- Podzilla2 is the official GUI of iPodLinux, replacing
  the now defunct podzilla0.
- It is written as a series of APIs allowing for the extension
  of functionality through modules; PZ2 is also skinable.
- The ZeroSlackr pack comes with all official SVN modules
  presorted into categories for which ZeroLauncher can select
  for loading.
- The ZeroSlackr version just has a few string changes for
  ZeroSlackr Framework compatibility (most previous patches
  have already been accepted into the official SVN).
For more information, see:
- http://ipodlinux.org/wiki/Podzilla
- http://ipodlinux.org/wiki/Special:Module
- http://ipodlinux.org/wiki/Special:Scheme
- http://ipodlinux.org/wiki/IPLCSE
- http://ipodlinux.org/wiki/Creating_podzilla2_schemes
- http://ipodlinux.org/oldforums/viewtopic.php?p=47290#47290 (epic post)
- http://en.wikipedia.org/wiki/Graphical_user_interface

Usage:
- Podzilla2-SVN is used in the same way as a normal podzilla2
  but in the context of the ZeroSlackr Framework.
- FastLaunch through "Zillae > Podzilla2-SVN > FastLaunch"
  or select one of the many different launch options to run
  their respective scripts (which will load modules only
  in certain folders).
- The special launch options can be hidden through their
  toggles in "Zillae > Podzilla2-SVN > Settings".
- Like with podzilla2, schemes should be placed in ZeroSlackr's
  "/usr/share/schemes" while modules be placed in one of the
  organized folders of "/usr/lib/" where they will/will not
  be loaded depending on the launch option.
- To make your launch option, edit the "FastLaunch.sh" script
  or edit one of the pre-defined launch scripts in "Launches".


Original:
- Authors: iPL Core Devs
- Source: SVN
- Link: https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/apps/ipod/podzilla2
- Date: Oct 12, 2008
- Version: Revision 2437

ZS Version:
- Modder: Keripo
- Type: Mod and ZS Recompile
- Date: Oct 12, 2008
- Version: B X.X

Modifications:
(see patch files of source for details)
- modified paths
- custom default settings
- same module modifications as ZeroLauncher

Launch Module:
- multiple pre-set module folder loads
- settings for toggling on-off menu items

To do:
- none

Changelog:
[keep untouched till beta release]