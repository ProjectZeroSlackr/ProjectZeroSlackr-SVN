Last updated: 2010/09/28
~Keripo

This was my unfinished attempt at properly integrating
the Sansa e200 kernel changes into the official iPodLinux
SVN. Other parts of the port (e.g. podzilla2 support
for Sansa e200s) have already been integrated into the
official iPL SVN and a few other ZeroSlackr apps have
already been tweaked to work on Sansa e200s (the changes
are incomplete, however, and many may never be ported
due to the Sansa's different input scheme).

The original port was a combination of the old 2.4.24
kernel with outdated iPodLinux SVN patches, a few driver
files from Rockbox's SVN, and other changes all lumped
together in one big patch file. I started working on
separating these changes so it'd sync well with current
SVN code (e.g. a 2.4.24 kernel with SVN patches 
applied) and separating the Rockbox files, but never
ended up completing it (partially due to the iPL server
crash in 2009, which we still haven't completely
recovered from, plus the loss of interest in the 
project at large). Here's what I left off with in case
anyone is interested in picking up/cleaning up the port.

Note that the official kernel that we work with both
in Project ZeroSlackr and iPodLinux is the 2.4.32
kernel, so that'd be the next step. With the 2.4.32
kernel, however, a custom multi-bootloader needs to be
made such that SansaLinux can also grab boot arguments
the same way iPodLinux currently can.

orig/
- very original files from Sebastian Duell in his
  original port

uncleaned/
- original 2.4.24 kernel with changes from the
  original Sansa e200 patch
- the original patch file has some iPodLinux SVN
  patches mixed in
- drivers files from Rockbox mixed in to patch
- see build.txt for instructions on compiling

cleaner/
- my attempts at cleaning up the changes
- sansa-only-files/ are files added by the original
  patch; some of these are directly from the Rockbox
  project or take routines from Rockbox, so it may
  be better to just grab the lastest files from their
  SVN and integrate their functions rather than a
  copy-paste job
- comparison-files/ are files that are modified by
  the original patch; files
  from the modified Sansa version and the original iPL
  kernel with SVN patches applied
- linux-2.4.24-svn.diff is just a diff between the
  2.4.24 iPL kernel and the 2.4.24 iPL kernel with the
  latest patches from the iPL SVN
- linux-2.4.24-sansa-v1.diff is a diff between the
  2.4.24 iPL kernel and the modified Sansa version
- linux-2.4.24-svn-sansa-v1.diff is a diff between the
  2.4.24 iPL kernel with iPL SVN patches and the
  modified Sansa version
- because the modified Sansa version did NOT use the
  latest iPL SVN patches (but didn't use the original
  2.4.24 iPL kernel either - its somewhere in
  between), you will have to manually look through
  each change to figure out what the real changes are

Good luck!

~Keripo
  