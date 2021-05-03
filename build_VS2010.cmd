SET yyyymmdd=%DATE:~0,4%-%DATE:~5,2%-%DATE:~8,2%
SET hhmmss=%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%
SET hhmmss=%hhmmss: =0%

REM call .\datetime.bat SmartDraw
REM call .\setvars.bat
REM SET AUTOBUILDINC="../Bin/xautobuild.exe"
SET NSIS="C:\Program Files (x86)\NSIS\makensis.exe"
SET CANDLE="C:\Program Files (x86)\WiX Toolset v3.11\bin\candle.exe"
SET LIGHT="C:\Program Files (x86)\WiX Toolset v3.11\bin\light.exe"

@ECHO ON
REM MKDIR Result\%datetime%

REM copy dll files
COPY "..\istools\lib\vc100\is_vc100.dll" ".\SmartDraw_PDS\Setup\is_vc100.dll"
COPY "..\istools\lib\vc100\isutil_vc100.dll" ".\SmartDraw_PDS\Setup\isutil_vc100.dll"
COPY "..\istools\lib\vc100\isgui_vc100.dll" ".\SmartDraw_PDS\Setup\isgui_vc100.dll"
COPY "..\istools\lib\vc100\AutoUp_vc100.dll" ".\SmartDraw_PDS\Setup\AutoUp_vc100.dll"
COPY "..\istools\lib\vc100\DgnV7Lib_vc100.dll" ".\SmartDraw_PDS\Setup\DgnV7Lib_vc100.dll"
COPY "..\istools\lib\vc100\vftlib_vc100.dll" ".\SmartDraw_PDS\Setup\vftlib_vc100.dll"
COPY "..\istools\lib\vc100\aMesh2d_vc100.dll" ".\SmartDraw_PDS\Setup\aMesh2d_vc100.dll"
COPY "..\istools\lib\vc100\aImage_vc100.dll" ".\SmartDraw_PDS\Setup\aImage_vc100.dll"

COPY "..\istools\3rdparty\BugTrap\BugTrap.dll" ".\SmartDraw_PDS\Setup\BugTrap.dll"

COPY ".\lib\DgnLib_vc100.dll" ".\SmartDraw_PDS\Setup\DgnLib_vc100.dll"
COPY ".\lib\SmartDrawRsc_vc100.dll" ".\SmartDraw_PDS\Setup\SmartDrawRsc_vc100.dll"
COPY ".\lib\SmartDrawModule_vc100.dll" ".\SmartDraw_PDS\Setup\SmartDrawModule_vc100.dll"
COPY ".\lib\SmartDrawAnno_vc100.dll" ".\SmartDraw_PDS\Setup\SmartDrawAnno_vc100.dll"
COPY ".\lib\SmartHSR_vc100.dll" ".\SmartDraw_PDS\Setup\SmartHSR_vc100.dll"
COPY ".\lib\SmartExchanger_vc100.dll" ".\SmartDraw_PDS\Setup\SmartExchanger_vc100.dll"
COPY ".\lib\SmartSolid_vc100.dll" ".\SmartDraw_PDS\Setup\SmartSolid_vc100.dll"
COPY ".\lib\SmartDrawPDSModule_vc100.dll" ".\SmartDraw_PDS\Setup\SmartDrawPDSModule_vc100.dll"
COPY ".\lib\aBaseTool_vc100.dll" ".\SmartDraw_PDS\Setup\aBaseTool_vc100.dll"

REM delete old files for setup
Del .\SmartDraw_PDS\Setup\*.wixpdb
Del .\SmartDraw_PDS\Setup\*.wixobj
Del .\SmartDraw_PDS\Setup\*.msi

REM %CANDLE% ".\SmartDraw_PDS.wxs" -out .\Setup\
REM IF %ERRORLEVEL% NEQ 0 goto :ERROR
REM %LIGHT% -out ".\Setup\SmartDraw_PDS-%1.msi" ".\Setup\SmartDraw_PDS.wixobj" -ext WixUIExtension -ext WixUtilExtension
REM IF %ERRORLEVEL% NEQ 0 goto :ERROR

REM %VS2008% /rebuild Release "build\build.net.sln" /out "Result\%datetime%\build.net.log"
REM %VS2010% /rebuild Release "build\build_VS2010.sln" /out "Result\%datetime%\build.log"
REM if %ERRORLEVEL% NEQ 0 goto failure

mkdir .\Setup
mkdir .\SmartDraw_PDS\Setup\Installation
Del .\SmartDraw_PDS\Setup\Installation\*.exe
%NSIS% ".\SmartDraw_PDS\Setup\SmartDraw_VS2010.nsi"
if %ERRORLEVEL% NEQ 0 goto :ERROR

:EOF
ECHO process completed
exit

:ERROR
exit /b %ERRORLEVEL%