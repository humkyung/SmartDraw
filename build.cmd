call .\datetime.bat SmartDraw
call .\setvars.bat
SET AUTOBUILDINC="../Bin/xautobuild.exe"

@ECHO ON
MKDIR Result\%datetime%

REM copy dll files
COPY "..\shared\istools\lib\vc90\is_vc90.dll" ".\SmartDraw_PDS\Setup\is_vc90.dll"
COPY "..\shared\istools\lib\vc90\isutil_vc90.dll" ".\SmartDraw_PDS\Setup\isutil_vc90.dll"
COPY "..\shared\istools\lib\vc90\isgui_vc90.dll" ".\SmartDraw_PDS\Setup\isgui_vc90.dll"
COPY "..\shared\istools\AutoUp\lib\AutoUp_vc90.dll" ".\SmartDraw_PDS\Setup\AutoUp_vc90.dll"
COPY "..\shared\istools\lib\vc90\DgnV7Lib_vc90.dll" ".\SmartDraw_PDS\Setup\DgnV7Lib_vc90.dll"
COPY "..\shared\istools\lib\vc90\vftlib_vc90.dll" ".\SmartDraw_PDS\Setup\vftlib_vc90.dll"
COPY "..\shared\istools\lib\vc90\aMesh2d_vc90.dll" ".\SmartDraw_PDS\Setup\aMesh2d_vc90.dll"
COPY "..\shared\istools\lib\vc90\aImage_vc90.dll" ".\SmartDraw_PDS\Setup\aImage_vc90.dll"

COPY "aBaseTool\lib\aBaseTool_vc90.dll" ".\SmartDraw_PDS\Setup\aBaseTool_vc90.dll"
COPY "lib\DgnLib_vc90.dll" ".\SmartDraw_PDS\Setup\DgnLib_vc90.dll"

REM ..\..\Bin\xautobuild.exe -i .\DgnLib\DgnLib.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartSolid\SmartSolid.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartHSR\SmartHSR.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartDrawRsc\SmartDrawRsc.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartDrawModule\SmartDrawModule.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartDrawPDSModule\SmartDrawPDSModule.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartDrawAnno\SmartDrawAnno.rc
REM ..\..\Bin\xautobuild.exe -i .\Drawing\PipingDrawing\PipingDrawing\PipingDrawing.rc
REM ..\..\Bin\xautobuild.exe -i .\Drawing\CableTrayDrawing\CableTrayDrawing\CableTrayDrawing.rc
REM ..\..\Bin\xautobuild.exe -i .\Drawing\OpeningHoleDrawing\OpeningHoleDrawing\OpeningHoleDrawing.rc
REM ..\..\Bin\xautobuild.exe -i .\SmartDraw_PDS\SmartDraw\SmartDraw.rc
%VS2008% /rebuild Release "build\build.net.sln" /out "Result\%datetime%\build.net.log"
%VS2008% /rebuild Release "build\build.sln" /out "Result\%datetime%\build.log"
if %ERRORLEVEL% NEQ 0 goto failure

REM build setup file
pushd .\SmartDraw_PDS\Setup

%NSIS% "SmartDraw.nsi"
if %ERRORLEVEL% NEQ 0 goto failure

popd
cd ..\..\

exit
:failure
echo.Failure >> Result\%datetime%\build.log
exit /b %ERRORLEVEL%
