@ECHO ON
if defined ProgramFiles(x86) (
SET VC6="C:\Program Files (x86)\Microsoft Visual Studio\COMMON\MSDev98\Bin\MSDEV.EXE"
SET VS2008="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
SET VS2010="C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"
SET NSIS="C:\\Program Files (x86)\\NSIS\\makensis.exe"
) else (
SET VC6="C:\Program Files\Microsoft Visual Studio\COMMON\MSDev98\Bin\MSDEV.EXE"
SET VS2008="C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
SET VS2010="C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"
SET NSIS="C:\\Program Files\\NSIS\\makensis.exe"
)
SET AUTOBUILDINC="../../Bin/xautobuild.exe"
SET LOGVIEWER="../../Bin/msdevLogViewer.exe"
