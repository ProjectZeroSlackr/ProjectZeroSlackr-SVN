Installing SansaLinux
1. Copy the 2 files initrd and linux.bin in the sansa-directory to the root of your Sansa
2. If you want MPD-support(Music Player Daemon), copy the file usrfs in the sansa/usrfs-mpd dir to the root of your sansa,
   if you don't want MPD-support, copy the file usrfs in the sansa/usrfs-nompd dir to the root of your Sansa
3. Run sansapatcher for normal e200's and use the file in "bootloader-Rhapsody" for e200R's

Running SansaLinux

The Bootloader that comes with SansaLinux is a modified Rockbox-bootloader.
If you have installed the Rockbox player software (doesn't come with SansaLinux)
it will boot to rockbox as default(when no key is pressed during player-startup).
If you don't have installed rockbox it will boot the original firmware at startup.

To start Linux just press the right (">>") button during startup of the Sansa.


Please Note:

If you installed the MPD-usrfs, SansaLinux might hang when initializing MPD. Leave it there for
a few minutes, then reset your Sansa (holding power for ~20 secs). After that, MPD should work.
If not, you have to use the no-mpd usrfs.

MPD is hanging, because it needs a lot of memory, especially for music-database creation. All unneeded
plugins are now removed from the usrfs-mpd, but if you have too much music on your Sansa, MPD will although hang.

