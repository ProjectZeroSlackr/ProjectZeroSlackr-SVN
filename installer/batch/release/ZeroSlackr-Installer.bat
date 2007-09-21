:: Modified iPodLinux Manager.bat
:: Installs ZeroSlackr via non-partitioning method
:: By AriX, modified by Keripo
:: Note from Keripo to AriX
:: Is it possible to resize the entire application
:: or at least make the font bigger? ; )
@ECHO OFF
cd Files
GOTO Welcome

:Welcome
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Welcome to AriX and Keripo's iPodLinux and ZeroSlackr Installer! (Alpha: August 28, 2007)
SET watext2=~~Press Next if you are ready to detect your iPod, or Cancel if you are not ready at this time. When you press next, the installer will try to find your iPod.
SET watext3=~~This installer is a modified version of AriX's iPodLinux Manager. This will install iPodLinux and ZeroSlackr using a modified loader, kernel and userland compiled by Keripo through DataGhost's no-partitioning installation method.
START /w Resources\gui.exe NOBACK TB
IF Errorlevel 2 GOTO End
SET watext=
SET watext2=
SET watext3=
GOTO Detect

:Menu
SET watext=Your iPod has been found at drive %sys%:\.
SET watext2=~~Press next to select your install options.
START /w Resources\gui.exe TB
IF Errorlevel 2 GOTO End
if Errorlevel 1 GOTO Welcome
SET wainput=
SET waoutput=
SET waoutnum=
set watext=
set watext2=
GOTO Install

:Detect
:: Note from Keripo to AriX:
:: It looks like you've already got something set up
:: Fetching updates from a website = good ; )
::del updates.bat
::call wget http://winxblog.com/updates.bat
::call updates.bat
::CLS

::Searching for iPod...
IF EXIST D:\iPod_Control SET sys=D
IF NOT "%sys%"=="" GOTO HwName
IF EXIST E:\iPod_Control SET sys=E
IF NOT "%sys%"=="" GOTO HwName
IF EXIST F:\iPod_Control SET sys=F
IF NOT "%sys%"=="" GOTO HwName
IF EXIST G:\iPod_Control SET sys=G
IF NOT "%sys%"=="" GOTO HwName
IF EXIST H:\iPod_Control SET sys=H
IF NOT "%sys%"=="" GOTO HwName
IF EXIST I:\iPod_Control SET sys=I
IF NOT "%sys%"=="" GOTO HwName
IF EXIST J:\iPod_Control SET sys=J
IF NOT "%sys%"=="" GOTO HwName
IF EXIST K:\iPod_Control SET sys=K
IF NOT "%sys%"=="" GOTO HwName
IF EXIST L:\iPod_Control SET sys=L
IF NOT "%sys%"=="" GOTO HwName
IF EXIST M:\iPod_Control SET sys=M
IF NOT "%sys%"=="" GOTO HwName

:: If error:

SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Error!
SET watext2=~~Unfortunately, your iPod's Drive Name could not be found. Nothing has been changed.
SET watext3=~~Please make sure your iPod is visible in Windows Explorer and try again. Reboot your iPod into the Apple firmware and reconnect it to your computer. Make sure that it can be found in "My Computer", then run this installer again.
START /w Resources\gui.exe FINISH TB
IF Errorlevel 1 GOTO Welcome
SET watext=
SET watext2=
SET watext3=
GOTO EndEnd

:HwName
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Installing Loader 2...
SET watext2=~~Installing iPodLinux Loader 2.
SET watext3=~~This graphical loader allows you to boot the Apple OS, multiple iPodLinux kernels, and Rockbox.
call Resources\ipodpatcher.exe --install
GOTO Userland

:: Note from Keripo to AriX
:: Use 7-zip; it doesn't take that much longer but
:: download size will be much smaller.
:: You'll also be able to do batch extractions
:: (see the 7-zip documentation for extracting from a text list of files)
:: (the files are in the Resource folder)
:: Also (if you can) don't forget to add a screen here cause extraction
:: can take quite a few seconds ; P

:Userland
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Installing ZeroSlackr...
SET watext2=~~Press next to install iPodLinux and Keripo's ZeroSlackr base system.
SET watext3=
:: Disabled for now
::START /w Resources\gui.exe NOBACK TB

:: Userland Installation
FOR %%v IN (Downloads\Base\*.7z) DO Resources\7za.exe x %%v -o%sys%:\ -y
GOTO SlackrPacks

:SlackrPacks
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Installing Slackr Packs...
SET watext2=~~Press next to install all downloaded ZeroSlackr Slackr Packs...
SET watext3=
:: Disabled for now
::START /w Resources\gui.exe NOBACK TB

:: Package Installation
:: At the moment, I just have it extract everything indiscriminately
:: I'm counting on you to make some type of selection method that is
:: called before this ; )
FOR %%v IN (Downloads\Packs\*.7z) DO Resources\7za.exe x %%v -o%sys%:\ -y

call Resources\removedrive.exe %sys%:
GOTO Finish

:Finish
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Success!
SET watext2=~~iPodLinux and ZeroSlackr has sucessfully been installed on your iPod!
SET watext3=~~If your iPod has not already been ejected, use the Safely Remove Hardware tool in your taskbar and then select iPodLinux from the Loader 2 menu.
START /w Resources\gui.exe FINISH TB
IF Errorlevel 1 GOTO Welcome
SET watext=
SET watext2=
SET watext3=
GOTO EndEnd

:End
SET watitle=ZeroSlackr Installer Alpha
SET wabmp=Resources\ZeroSlackr-Installer-Logo.bmp
SET wabat=%TEMP%\wabat.bat
SET wasig=AriX n Keripo
SET waico=Resources\ZeroSlackr-Installer-Icon.ico
SET watext=~Error!
SET watext2=~~Unknown error has occured!
SET watext3=~~If you are here its probably cause this batch file screwed up. Try running this again.
START /w Resources\gui.exe FINISH TB
cd ..

:EndEnd
cd ..