::Last updated: Apr 18, 2008
::~Keripo

@ECHO OFF

:PATCH
ECHO =====================================================
ECHO Project ZeroSlackr Auto-Patching Script
ECHO by Keripo
ECHO - Patching iPod with Loader2...
ipodpatcher -ab loader.bin
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
ECHO - Patching files will be moved to the boot/patch
ECHO   folder; if you ever need them again, copy them
ECHO   to the root of your iPod and execute this script.
ECHO =====================================================
MD boot\patch
MOVE /Y loader.bin boot\patch\
MOVE /Y ipodpatcher boot\patch\
MOVE /Y ipodpatcher.exe boot\patch\
MOVE /Y ipodpatcher-mac boot\patch\
PAUSE
MOVE /Y patch.sh boot\patch\
MOVE /Y patch.bat boot\patch\
EXIT
