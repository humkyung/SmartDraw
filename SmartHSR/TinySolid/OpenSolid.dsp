# Microsoft Developer Studio Project File - Name="OpenSolid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OpenSolid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenSolid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenSolid.mak" CFG="OpenSolid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenSolid - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OpenSolid - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "OpenSolid"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenSolid - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "d:\project\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENSOLID_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "d:\project\itklib\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENSOLID_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 itkgeo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"d:\project\bin"

!ELSEIF  "$(CFG)" == "OpenSolid - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "d:\project\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENSOLID_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "d:\project\itklib\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENSOLID_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ITKGeo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"C:\Program Files\PlantWAVE_CADWorx/OpenSolid.dll" /pdbtype:sept /libpath:"d:\project\bin"

!ENDIF 

# Begin Target

# Name "OpenSolid - Win32 Release"
# Name "OpenSolid - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\body.cpp
# End Source File
# Begin Source File

SOURCE=.\Cone.cpp
# End Source File
# Begin Source File

SOURCE=.\ConeSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\DelaunayEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\face3d.cpp
# End Source File
# Begin Source File

SOURCE=.\facet.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\MeshEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\MeshPos2d.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenSolid.cpp
# End Source File
# Begin Source File

SOURCE=.\PlaneSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\SphereSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\surface3d.cpp
# End Source File
# Begin Source File

SOURCE=.\SurfacePara.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\TinyFacetContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\TorusSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\wedge.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\body.h
# End Source File
# Begin Source File

SOURCE=.\Cone.h
# End Source File
# Begin Source File

SOURCE=.\ConeSurface.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\delaunay.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\DelaunayEdge.h
# End Source File
# Begin Source File

SOURCE=.\face3d.h
# End Source File
# Begin Source File

SOURCE=.\facet.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\MeshEdge.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\MeshPos2d.h
# End Source File
# Begin Source File

SOURCE=.\OpenSolid.h
# End Source File
# Begin Source File

SOURCE=.\PlaneSurface.h
# End Source File
# Begin Source File

SOURCE=.\PolyCurve.h
# End Source File
# Begin Source File

SOURCE=.\SphereSurface.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\surface3d.h
# End Source File
# Begin Source File

SOURCE=.\SurfacePara.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\TinyFacetContainer.h
# End Source File
# Begin Source File

SOURCE=.\TorusSurface.h
# End Source File
# Begin Source File

SOURCE=.\wedge.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libmesh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libmesh\delaunay.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\DT.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\DT.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\grid.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\GridMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\GridMethod.h
# End Source File
# Begin Source File

SOURCE=.\libmesh\libmesh2d.cpp
# End Source File
# Begin Source File

SOURCE=.\libmesh\libmesh2d.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
