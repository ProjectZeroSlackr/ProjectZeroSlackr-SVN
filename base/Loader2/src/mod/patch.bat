::Last updated: Oct 31, 2008
::~Keripo

@ECHO OFF

:PATCH
ECHO =====================================================
ECHO Project ZeroSlackr Auto-patch-files Script
ECHO by Keripo
ECHO - patching iPod with Loader2...
patch-files\ipodpatcher-win.exe -ab patch-files\loader.bin
IF ERRORLEVEL 0 IF NOT ERRORLEVEL 1 GOTO :SUCCESS
GOTO :FAIL

:FAIL
ECHO - ipodpatcher failed!
ECHO - Make sure you have full admin privileges,
ECHO   then try running this script again.
ECHO =====================================================
PAUSE
EXIT

:SUCCESS
ECHO - ipodpatcher successful!
ECHO - Disconnect your iPod now and let it reboot
ECHO   to Loader2. Select the "ZeroSlackr" option to
ECHO   boot into ZeroSlackr or select the "Apple OS"
ECHO   option to boot into the original firmware.
ECHO - When in the Apple OS, force reboot your iPod to
ECHO   go back to Loader2. For instructions on rebooting,
ECHO   see: http://ipodlinux.org/Key_combinations
ECHO - Make sure to read all of ZeroSlackr's documents
ECHO   starting with "ReadMe from Keripo.txt"
ECHO =====================================================
XCOPY /Q /Y patch-files\loader-winpod.cfg loader.cfg
PAUSE
EXIT
