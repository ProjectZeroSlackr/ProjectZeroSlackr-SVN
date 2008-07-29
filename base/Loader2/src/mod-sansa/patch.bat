::Last updated: July 29, 2008
::~Keripo

@ECHO OFF

:PATCH
ECHO =====================================================
ECHO Project ZeroSlackr Auto-patch-files Script
ECHO by Keripo
ECHO Note: If you have a Rhapsody model, you should have
ECHO   already copied over "patch-files/rhapsody/pp5022.mi4"
ECHO   to the root of your Sansa. If not, do so. You might
ECHO   have to then run this script again.
ECHO - patching Sansa with modified Rockbox bootloader...
patch-files\sansapatcher-win.exe
IF ERRORLEVEL 0 IF NOT ERRORLEVEL 1 GOTO :SUCCESS
GOTO :FAIL

:FAIL
ECHO - sansapatcher failed!
ECHO - Make sure you have full admin privileges,
ECHO   then try running this script again.
ECHO =====================================================
PAUSE
EXIT

:SUCCESS
ECHO - sansapatcher successful!
ECHO - Disconnect your Sansa now and hold the fastforward/
ECHO   right button to boot into ZeroSlackr. Do not press
ECHO   any key to boot into the Rockbox/original firmware.
ECHO - Make sure to read all of ZeroSlackr's documents
ECHO   starting with "ReadMe from Keripo.txt"
ECHO - patch files will be moved to the boot/patch
ECHO   folder; if you ever need them again, copy them
ECHO   to the root of your Sansa and execute this script.
ECHO =====================================================
MD boot\patch
MOVE /Y patch-files boot\patch\
MOVE /Y rhapsody boot\patch\
PAUSE
MOVE /Y patch.sh boot\patch\
MOVE /Y patch.bat boot\patch\
EXIT
