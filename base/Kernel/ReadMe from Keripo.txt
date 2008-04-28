Last updated: Apr 27, 2008
~Keripo

All code/scripts written by Keripo are licensed under
GNU GPL - see License.txt. For the licensing of the
software/application, refer to the license that comes
with the original/ported software.

Overview:
Kernel
- This is the main iPodLinux kernel.
- ZeroSlackr uses a modified kernel that allows for
  the loop-mount installation method and is compatible
  with newer iBoy builds.
- It can also load a normal, partitioned iPodLinux
  installation (see the "/boot/loader.cfg" file).
For more information, see:
- http://ipodlinux.org/Experimental_Kernel
- http://ipodlinux.org/Kernel
- http://en.wikipedia.org/wiki/Kernel_(computer_science)

Usage:
- The kernel is used like any other Linux kernel.
- It can be loaded by Loader2 with kernel arguments.
- For the proper loader.cfg lines, see 
  "/boot/loader.cfg" and
  "/boot/docs/kernel/kernel-parameters.txt"


Vanilla Linux:
- Authors: Linux Core Devs
- Source: The Linux Kernel Archives
- Link:
  http://www.kernel.org/pub/linux/kernel/v2.4/linux-2.4.32.tar.bz2
- Date: ???
- Version: 2.4.32

uClinux Patches:
- Authors: uClinux Devs
- Source: uClinux website
- Link:
  http://www.uclinux.org/pub/uClinux/uClinux-2.4.x/uClinux-2.4.32-uc0.diff.gz
- Date: ???
- Version 2.4.32

iPodLinux Patches:
- Authors: iPL Core Devs
- Source: SVN + courtc
- Link:
  http://www.so2.sys-techs.com/ipod/linux/
  https://ipodlinux.svn.sourceforge.net/svnroot/ipodlinux/linux/2.4
- Date: Apr 27, 2008
- Version: Revision 2418

ZS Version:
- Modder: Keripo
- Type: Mod and Recompile
- Link:
  http://ipodlinux.org/images/8/81/Iboy_kernel_patches.zip (iBoy patches)
- Date: Apr 27, 2008
- Version: B X.X

Modifications:
- iBoy audio buffer patch
- Removed annoying "dma not active" message
- preconfigured .config file (with CONFIG_BLK_DEV_LOOP=y)

To do:
- none

Changelog:
[keep untouched till beta release]