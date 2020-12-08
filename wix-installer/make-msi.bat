@echo off
goto start
--------------------------------------
This batch file is purposed to automate
to build the msi installers for the x86
and x64 platforms.

To correctly work the caller must call
this file with 3 argument when the first
argument is a file name for the x86
installer (without the ".msi" extension)
and the second argument is a file name
for the x64 installer. Third argument allows
build the installers without the License
Agreement dialog. To use this option set
the third argument equal 1. Third argument is
optional.
Fourth argument allows define any variable.
For example VIEWER_FREE defines the "VIEWER_FREE"
variable, VIEWER_FREE=1 defines "VIEWER_FREE" and
set it to 1.

Example:
make-msi.bat tightvnc-2.5.1-setup-32bit tightvnc-2.5.1-setup-64bit 1 VIEWER_FREE

--------------------------------------

:start

set NO_LICENSE_VALUE=0
if %3arg == 1arg set NO_LICENSE_VALUE=1

set VAR1=-d%4
if %VAR1% == -d set VAR1= 

set OUTPUT_MSI_NAME=%1
set TARGET_PLATFORM=x86
set OUT_DIR=..\Release
call :build_msi

set OUTPUT_MSI_NAME=%2
set TARGET_PLATFORM=x64
set OUT_DIR=..\x64\Release
call :build_msi

goto :exit


:build_msi

candle.exe -dConfiguration=Release -dOutDir=%OUT_DIR%\ -dPlatform=%TARGET_PLATFORM% -dTargetDir=%OUT_DIR%\ -dTargetExt=.msi -dTargetFileName=%OUTPUT_MSI_NAME%.msi -dTargetName=%OUTPUT_MSI_NAME% -dTargetPath=%OUT_DIR%\%OUTPUT_MSI_NAME%.msi -dNoLicense=%NO_LICENSE_VALUE% %VAR1% -out %OUT_DIR%\wixobj\ -arch %TARGET_PLATFORM% -ext WixFirewallExtension.dll -ext WixUtilExtension.dll -ext WixUIExtension.dll components.wxs custom_actions.wxs features.wxs gui.wxs gui_pages.wxs Properties.wxs setup.wxs

Light.exe -cultures:null -ext WixFirewallExtension.dll -ext WixUtilExtension.dll -ext WixUIExtension.dll -out %OUT_DIR%\%OUTPUT_MSI_NAME%.msi -pdbout %OUT_DIR%\setup.wixpdb -spdb %OUT_DIR%\wixobj\components.wixobj %OUT_DIR%\wixobj\custom_actions.wixobj %OUT_DIR%\wixobj\features.wixobj %OUT_DIR%\wixobj\gui.wixobj %OUT_DIR%\wixobj\gui_pages.wixobj %OUT_DIR%\wixobj\Properties.wixobj %OUT_DIR%\wixobj\setup.wixobj

goto :eof


:exit
