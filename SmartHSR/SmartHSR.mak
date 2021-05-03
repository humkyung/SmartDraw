# Microsoft Developer Studio Generated NMAKE File, Based on hiddenDll.dsp
!IF "$(CFG)" == ""
CFG=hiddenDll - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hiddenDll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hiddenDll - Win32 Release" && "$(CFG)" != "hiddenDll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hiddenDll.mak" CFG="hiddenDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hiddenDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hiddenDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hiddenDll - Win32 Release"

OUTDIR=d:\project\bin
INTDIR=.\Release

ALL : "..\..\..\..\Bin\hiddenDll.dll" ".\Release\hiddenDll.pch"


CLEAN :
	-@erase "$(INTDIR)\3d_FaceElement.obj"
	-@erase "$(INTDIR)\ArcElement.obj"
	-@erase "$(INTDIR)\BOXElement.obj"
	-@erase "$(INTDIR)\bspnode.obj"
	-@erase "$(INTDIR)\BSPTree.obj"
	-@erase "$(INTDIR)\ConeElement.obj"
	-@erase "$(INTDIR)\CTorElement.obj"
	-@erase "$(INTDIR)\CylnElement.obj"
	-@erase "$(INTDIR)\DomeElement.obj"
	-@erase "$(INTDIR)\ElbowElement.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\entry.obj"
	-@erase "$(INTDIR)\ExportFunc.obj"
	-@erase "$(INTDIR)\FACEMESHElement.obj"
	-@erase "$(INTDIR)\FLANElement.obj"
	-@erase "$(INTDIR)\geo.obj"
	-@erase "$(INTDIR)\hidden.obj"
	-@erase "$(INTDIR)\hiddenDll.obj"
	-@erase "$(INTDIR)\hiddenDll.pch"
	-@erase "$(INTDIR)\HSRBoundingBox.obj"
	-@erase "$(INTDIR)\HSRCore.obj"
	-@erase "$(INTDIR)\HSRElementFactory.obj"
	-@erase "$(INTDIR)\HSRExport.obj"
	-@erase "$(INTDIR)\HSRFace.obj"
	-@erase "$(INTDIR)\HSRImport.obj"
	-@erase "$(INTDIR)\HSRLayer.obj"
	-@erase "$(INTDIR)\HSRMemManager.obj"
	-@erase "$(INTDIR)\HSRNormalFace.obj"
	-@erase "$(INTDIR)\HSROption.obj"
	-@erase "$(INTDIR)\HSRParser.obj"
	-@erase "$(INTDIR)\HSRPipe.obj"
	-@erase "$(INTDIR)\HSRPostProcessor.obj"
	-@erase "$(INTDIR)\HSRPreProcessor.obj"
	-@erase "$(INTDIR)\HSRReader.obj"
	-@erase "$(INTDIR)\HSRRevolvedElement.obj"
	-@erase "$(INTDIR)\HSRRule.obj"
	-@erase "$(INTDIR)\HsrScanner.obj"
	-@erase "$(INTDIR)\HSRView.obj"
	-@erase "$(INTDIR)\HSRViewFormat.obj"
	-@erase "$(INTDIR)\Intent.obj"
	-@erase "$(INTDIR)\IOLib.obj"
	-@erase "$(INTDIR)\IsEntity.obj"
	-@erase "$(INTDIR)\IsError.obj"
	-@erase "$(INTDIR)\IsLine3d.obj"
	-@erase "$(INTDIR)\IsPlane3d.obj"
	-@erase "$(INTDIR)\IsPoint2d.obj"
	-@erase "$(INTDIR)\IsPoint3d.obj"
	-@erase "$(INTDIR)\IsQuat.obj"
	-@erase "$(INTDIR)\IsRect.obj"
	-@erase "$(INTDIR)\IsUserData.obj"
	-@erase "$(INTDIR)\IsVect2d.obj"
	-@erase "$(INTDIR)\IsVect3d.obj"
	-@erase "$(INTDIR)\IsVolume.obj"
	-@erase "$(INTDIR)\LineElement.obj"
	-@erase "$(INTDIR)\MESHElement.obj"
	-@erase "$(INTDIR)\PreHLINEFormat.obj"
	-@erase "$(INTDIR)\PreHNormalFace.obj"
	-@erase "$(INTDIR)\Primitive.obj"
	-@erase "$(INTDIR)\RTorElement.obj"
	-@erase "$(INTDIR)\SphereElement.obj"
	-@erase "$(INTDIR)\SSLCElement.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SweepElbow.obj"
	-@erase "$(INTDIR)\TeeElement.obj"
	-@erase "$(INTDIR)\TemplateElement.obj"
	-@erase "$(INTDIR)\TorsElement.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\ViewBoundary.obj"
	-@erase "$(OUTDIR)\hiddenDll.exp"
	-@erase "$(OUTDIR)\hiddenDll.lib"
	-@erase "..\..\..\..\Bin\hiddenDll.dll"
	-@erase ".\hiddenDll.res"
	-@erase ".\Release\hiddenDll.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "d:\프로젝트\itklib\include" /I "d:\프로젝트\boost_1_32_0" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HIDDENDLL_EXPORTS" /D "VER_02" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x412 /fo"hiddenDll.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hiddenDll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ITKGeo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.4 /dll /profile /map:"$(INTDIR)\hiddenDll.map" /machine:I386 /out:"..\..\..\..\Bin\hiddenDll.dll" /implib:"$(OUTDIR)\hiddenDll.lib" /libpath:"d:\프로젝트\bin" 
LINK32_OBJS= \
	"$(INTDIR)\3d_FaceElement.obj" \
	"$(INTDIR)\ArcElement.obj" \
	"$(INTDIR)\BOXElement.obj" \
	"$(INTDIR)\bspnode.obj" \
	"$(INTDIR)\BSPTree.obj" \
	"$(INTDIR)\ConeElement.obj" \
	"$(INTDIR)\CTorElement.obj" \
	"$(INTDIR)\CylnElement.obj" \
	"$(INTDIR)\DomeElement.obj" \
	"$(INTDIR)\ElbowElement.obj" \
	"$(INTDIR)\Element.obj" \
	"$(INTDIR)\entry.obj" \
	"$(INTDIR)\ExportFunc.obj" \
	"$(INTDIR)\FACEMESHElement.obj" \
	"$(INTDIR)\FLANElement.obj" \
	"$(INTDIR)\geo.obj" \
	"$(INTDIR)\hidden.obj" \
	"$(INTDIR)\hiddenDll.obj" \
	"$(INTDIR)\HSRBoundingBox.obj" \
	"$(INTDIR)\HSRCore.obj" \
	"$(INTDIR)\HSRElementFactory.obj" \
	"$(INTDIR)\HSRExport.obj" \
	"$(INTDIR)\HSRFace.obj" \
	"$(INTDIR)\HSRImport.obj" \
	"$(INTDIR)\HSRLayer.obj" \
	"$(INTDIR)\HSRMemManager.obj" \
	"$(INTDIR)\HSRNormalFace.obj" \
	"$(INTDIR)\HSROption.obj" \
	"$(INTDIR)\HSRParser.obj" \
	"$(INTDIR)\HSRPipe.obj" \
	"$(INTDIR)\HSRPostProcessor.obj" \
	"$(INTDIR)\HSRPreProcessor.obj" \
	"$(INTDIR)\HSRReader.obj" \
	"$(INTDIR)\HSRRevolvedElement.obj" \
	"$(INTDIR)\HSRRule.obj" \
	"$(INTDIR)\HsrScanner.obj" \
	"$(INTDIR)\HSRView.obj" \
	"$(INTDIR)\HSRViewFormat.obj" \
	"$(INTDIR)\Intent.obj" \
	"$(INTDIR)\IOLib.obj" \
	"$(INTDIR)\LineElement.obj" \
	"$(INTDIR)\MESHElement.obj" \
	"$(INTDIR)\PreHLINEFormat.obj" \
	"$(INTDIR)\PreHNormalFace.obj" \
	"$(INTDIR)\Primitive.obj" \
	"$(INTDIR)\RTorElement.obj" \
	"$(INTDIR)\SphereElement.obj" \
	"$(INTDIR)\SSLCElement.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SweepElbow.obj" \
	"$(INTDIR)\TeeElement.obj" \
	"$(INTDIR)\TemplateElement.obj" \
	"$(INTDIR)\TorsElement.obj" \
	"$(INTDIR)\ViewBoundary.obj" \
	"$(INTDIR)\IsEntity.obj" \
	"$(INTDIR)\IsError.obj" \
	"$(INTDIR)\IsLine3d.obj" \
	"$(INTDIR)\IsPlane3d.obj" \
	"$(INTDIR)\IsPoint2d.obj" \
	"$(INTDIR)\IsPoint3d.obj" \
	"$(INTDIR)\IsQuat.obj" \
	"$(INTDIR)\IsRect.obj" \
	"$(INTDIR)\IsUserData.obj" \
	"$(INTDIR)\IsVect2d.obj" \
	"$(INTDIR)\IsVect3d.obj" \
	"$(INTDIR)\IsVolume.obj" \
	".\hiddenDll.res"

"..\..\..\..\Bin\hiddenDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\Bin\hiddenDll.dll" ".\Release\hiddenDll.pch"
   post-build.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "hiddenDll - Win32 Debug"

OUTDIR=d:\프로젝트\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=d:\프로젝트\bin
# End Custom Macros

ALL : "$(OUTDIR)\hiddenDll.dll" ".\Debug\hiddenDll.pch"


CLEAN :
	-@erase "$(INTDIR)\3d_FaceElement.obj"
	-@erase "$(INTDIR)\ArcElement.obj"
	-@erase "$(INTDIR)\BOXElement.obj"
	-@erase "$(INTDIR)\bspnode.obj"
	-@erase "$(INTDIR)\BSPTree.obj"
	-@erase "$(INTDIR)\ConeElement.obj"
	-@erase "$(INTDIR)\CTorElement.obj"
	-@erase "$(INTDIR)\CylnElement.obj"
	-@erase "$(INTDIR)\DomeElement.obj"
	-@erase "$(INTDIR)\ElbowElement.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\entry.obj"
	-@erase "$(INTDIR)\ExportFunc.obj"
	-@erase "$(INTDIR)\FACEMESHElement.obj"
	-@erase "$(INTDIR)\FLANElement.obj"
	-@erase "$(INTDIR)\geo.obj"
	-@erase "$(INTDIR)\hidden.obj"
	-@erase "$(INTDIR)\hiddenDll.obj"
	-@erase "$(INTDIR)\hiddenDll.pch"
	-@erase "$(INTDIR)\HSRBoundingBox.obj"
	-@erase "$(INTDIR)\HSRCore.obj"
	-@erase "$(INTDIR)\HSRElementFactory.obj"
	-@erase "$(INTDIR)\HSRExport.obj"
	-@erase "$(INTDIR)\HSRFace.obj"
	-@erase "$(INTDIR)\HSRImport.obj"
	-@erase "$(INTDIR)\HSRLayer.obj"
	-@erase "$(INTDIR)\HSRMemManager.obj"
	-@erase "$(INTDIR)\HSRNormalFace.obj"
	-@erase "$(INTDIR)\HSROption.obj"
	-@erase "$(INTDIR)\HSRParser.obj"
	-@erase "$(INTDIR)\HSRPipe.obj"
	-@erase "$(INTDIR)\HSRPostProcessor.obj"
	-@erase "$(INTDIR)\HSRPreProcessor.obj"
	-@erase "$(INTDIR)\HSRReader.obj"
	-@erase "$(INTDIR)\HSRRevolvedElement.obj"
	-@erase "$(INTDIR)\HSRRule.obj"
	-@erase "$(INTDIR)\HsrScanner.obj"
	-@erase "$(INTDIR)\HSRView.obj"
	-@erase "$(INTDIR)\HSRViewFormat.obj"
	-@erase "$(INTDIR)\Intent.obj"
	-@erase "$(INTDIR)\IOLib.obj"
	-@erase "$(INTDIR)\IsEntity.obj"
	-@erase "$(INTDIR)\IsError.obj"
	-@erase "$(INTDIR)\IsLine3d.obj"
	-@erase "$(INTDIR)\IsPlane3d.obj"
	-@erase "$(INTDIR)\IsPoint2d.obj"
	-@erase "$(INTDIR)\IsPoint3d.obj"
	-@erase "$(INTDIR)\IsQuat.obj"
	-@erase "$(INTDIR)\IsRect.obj"
	-@erase "$(INTDIR)\IsUserData.obj"
	-@erase "$(INTDIR)\IsVect2d.obj"
	-@erase "$(INTDIR)\IsVect3d.obj"
	-@erase "$(INTDIR)\IsVolume.obj"
	-@erase "$(INTDIR)\LineElement.obj"
	-@erase "$(INTDIR)\MESHElement.obj"
	-@erase "$(INTDIR)\PreHLINEFormat.obj"
	-@erase "$(INTDIR)\PreHNormalFace.obj"
	-@erase "$(INTDIR)\Primitive.obj"
	-@erase "$(INTDIR)\pw_hsr.res"
	-@erase "$(INTDIR)\RTorElement.obj"
	-@erase "$(INTDIR)\SphereElement.obj"
	-@erase "$(INTDIR)\SSLCElement.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SweepElbow.obj"
	-@erase "$(INTDIR)\TeeElement.obj"
	-@erase "$(INTDIR)\TemplateElement.obj"
	-@erase "$(INTDIR)\TorsElement.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\ViewBoundary.obj"
	-@erase "$(OUTDIR)\hiddenDll.dll"
	-@erase "$(OUTDIR)\hiddenDll.exp"
	-@erase "$(OUTDIR)\hiddenDll.ilk"
	-@erase "$(OUTDIR)\hiddenDll.lib"
	-@erase "$(OUTDIR)\hiddenDll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /I "d:\프로젝트\itklib\include" /I "d:\프로젝트\boost_1_32_0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HIDDENDLL_EXPORTS" /D "VER_02" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x412 /fo"$(INTDIR)\pw_hsr.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hiddenDll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ITKGeo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.4 /dll /incremental:yes /pdb:"$(OUTDIR)\hiddenDll.pdb" /debug /machine:I386 /out:"$(OUTDIR)\hiddenDll.dll" /implib:"$(OUTDIR)\hiddenDll.lib" /pdbtype:sept /libpath:"d:\프로젝트\bin" 
LINK32_OBJS= \
	"$(INTDIR)\3d_FaceElement.obj" \
	"$(INTDIR)\ArcElement.obj" \
	"$(INTDIR)\BOXElement.obj" \
	"$(INTDIR)\bspnode.obj" \
	"$(INTDIR)\BSPTree.obj" \
	"$(INTDIR)\ConeElement.obj" \
	"$(INTDIR)\CTorElement.obj" \
	"$(INTDIR)\CylnElement.obj" \
	"$(INTDIR)\DomeElement.obj" \
	"$(INTDIR)\ElbowElement.obj" \
	"$(INTDIR)\Element.obj" \
	"$(INTDIR)\entry.obj" \
	"$(INTDIR)\ExportFunc.obj" \
	"$(INTDIR)\FACEMESHElement.obj" \
	"$(INTDIR)\FLANElement.obj" \
	"$(INTDIR)\geo.obj" \
	"$(INTDIR)\hidden.obj" \
	"$(INTDIR)\hiddenDll.obj" \
	"$(INTDIR)\HSRBoundingBox.obj" \
	"$(INTDIR)\HSRCore.obj" \
	"$(INTDIR)\HSRElementFactory.obj" \
	"$(INTDIR)\HSRExport.obj" \
	"$(INTDIR)\HSRFace.obj" \
	"$(INTDIR)\HSRImport.obj" \
	"$(INTDIR)\HSRLayer.obj" \
	"$(INTDIR)\HSRMemManager.obj" \
	"$(INTDIR)\HSRNormalFace.obj" \
	"$(INTDIR)\HSROption.obj" \
	"$(INTDIR)\HSRParser.obj" \
	"$(INTDIR)\HSRPipe.obj" \
	"$(INTDIR)\HSRPostProcessor.obj" \
	"$(INTDIR)\HSRPreProcessor.obj" \
	"$(INTDIR)\HSRReader.obj" \
	"$(INTDIR)\HSRRevolvedElement.obj" \
	"$(INTDIR)\HSRRule.obj" \
	"$(INTDIR)\HsrScanner.obj" \
	"$(INTDIR)\HSRView.obj" \
	"$(INTDIR)\HSRViewFormat.obj" \
	"$(INTDIR)\Intent.obj" \
	"$(INTDIR)\IOLib.obj" \
	"$(INTDIR)\LineElement.obj" \
	"$(INTDIR)\MESHElement.obj" \
	"$(INTDIR)\PreHLINEFormat.obj" \
	"$(INTDIR)\PreHNormalFace.obj" \
	"$(INTDIR)\Primitive.obj" \
	"$(INTDIR)\RTorElement.obj" \
	"$(INTDIR)\SphereElement.obj" \
	"$(INTDIR)\SSLCElement.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SweepElbow.obj" \
	"$(INTDIR)\TeeElement.obj" \
	"$(INTDIR)\TemplateElement.obj" \
	"$(INTDIR)\TorsElement.obj" \
	"$(INTDIR)\ViewBoundary.obj" \
	"$(INTDIR)\IsEntity.obj" \
	"$(INTDIR)\IsError.obj" \
	"$(INTDIR)\IsLine3d.obj" \
	"$(INTDIR)\IsPlane3d.obj" \
	"$(INTDIR)\IsPoint2d.obj" \
	"$(INTDIR)\IsPoint3d.obj" \
	"$(INTDIR)\IsQuat.obj" \
	"$(INTDIR)\IsRect.obj" \
	"$(INTDIR)\IsUserData.obj" \
	"$(INTDIR)\IsVect2d.obj" \
	"$(INTDIR)\IsVect3d.obj" \
	"$(INTDIR)\IsVolume.obj" \
	"$(INTDIR)\pw_hsr.res"

"$(OUTDIR)\hiddenDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=d:\프로젝트\bin
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\hiddenDll.dll" ".\Debug\hiddenDll.pch"
   post-build.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hiddenDll.dep")
!INCLUDE "hiddenDll.dep"
!ELSE 
!MESSAGE Warning: cannot find "hiddenDll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hiddenDll - Win32 Release" || "$(CFG)" == "hiddenDll - Win32 Debug"
SOURCE=.\3d_FaceElement.cpp

"$(INTDIR)\3d_FaceElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArcElement.cpp

"$(INTDIR)\ArcElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\BOXElement.cpp

"$(INTDIR)\BOXElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bspnode.cpp

"$(INTDIR)\bspnode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\BSPTree.cpp

"$(INTDIR)\BSPTree.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ConeElement.cpp

"$(INTDIR)\ConeElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CTorElement.cpp

"$(INTDIR)\CTorElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CylnElement.cpp

"$(INTDIR)\CylnElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DomeElement.cpp

"$(INTDIR)\DomeElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ElbowElement.cpp

"$(INTDIR)\ElbowElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Element.cpp

"$(INTDIR)\Element.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\entry.cpp

"$(INTDIR)\entry.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ExportFunc.cpp

"$(INTDIR)\ExportFunc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FACEMESHElement.cpp

"$(INTDIR)\FACEMESHElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FLANElement.cpp

"$(INTDIR)\FLANElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\geo.cpp

"$(INTDIR)\geo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hidden.cpp

"$(INTDIR)\hidden.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hiddenDll.cpp

"$(INTDIR)\hiddenDll.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRBoundingBox.cpp

"$(INTDIR)\HSRBoundingBox.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRCore.cpp

"$(INTDIR)\HSRCore.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRElementFactory.cpp

"$(INTDIR)\HSRElementFactory.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRExport.cpp

"$(INTDIR)\HSRExport.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRFace.cpp

"$(INTDIR)\HSRFace.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRImport.cpp

"$(INTDIR)\HSRImport.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRLayer.cpp

"$(INTDIR)\HSRLayer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRMemManager.cpp

"$(INTDIR)\HSRMemManager.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRNormalFace.cpp

"$(INTDIR)\HSRNormalFace.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSROption.cpp

"$(INTDIR)\HSROption.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRParser.cpp

"$(INTDIR)\HSRParser.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRPipe.cpp

"$(INTDIR)\HSRPipe.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRPostProcessor.cpp

"$(INTDIR)\HSRPostProcessor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRPreProcessor.cpp

"$(INTDIR)\HSRPreProcessor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRReader.cpp

"$(INTDIR)\HSRReader.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRRevolvedElement.cpp

"$(INTDIR)\HSRRevolvedElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRRule.cpp

"$(INTDIR)\HSRRule.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HsrScanner.cpp

"$(INTDIR)\HsrScanner.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRView.cpp

"$(INTDIR)\HSRView.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HSRViewFormat.cpp

"$(INTDIR)\HSRViewFormat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\..\..\itklib\src\Intent.cpp

"$(INTDIR)\Intent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\itklib\src\utility\IOLib.cpp

"$(INTDIR)\IOLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\LineElement.cpp

"$(INTDIR)\LineElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MESHElement.cpp

"$(INTDIR)\MESHElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PreHLINEFormat.cpp

"$(INTDIR)\PreHLINEFormat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PreHNormalFace.cpp

"$(INTDIR)\PreHNormalFace.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\..\..\itklib\Primitive.cpp

"$(INTDIR)\Primitive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\pw_hsr.rc

!IF  "$(CFG)" == "hiddenDll - Win32 Release"


".\hiddenDll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hiddenDll - Win32 Debug"


"$(INTDIR)\pw_hsr.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RTorElement.cpp

"$(INTDIR)\RTorElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SphereElement.cpp

"$(INTDIR)\SphereElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SSLCElement.cpp

"$(INTDIR)\SSLCElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "hiddenDll - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "d:\프로젝트\itklib\include" /I "d:\프로젝트\boost_1_32_0" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HIDDENDLL_EXPORTS" /D "VER_02" /Fp"$(INTDIR)\hiddenDll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\hiddenDll.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "hiddenDll - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /I "d:\프로젝트\itklib\include" /I "d:\프로젝트\boost_1_32_0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HIDDENDLL_EXPORTS" /D "VER_02" /Fp"$(INTDIR)\hiddenDll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\hiddenDll.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SweepElbow.cpp

"$(INTDIR)\SweepElbow.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TeeElement.cpp

"$(INTDIR)\TeeElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TemplateElement.cpp

"$(INTDIR)\TemplateElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TorsElement.cpp

"$(INTDIR)\TorsElement.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ViewBoundary.cpp

"$(INTDIR)\ViewBoundary.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsEntity.cpp

"$(INTDIR)\IsEntity.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsError.cpp

"$(INTDIR)\IsError.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsLine3d.cpp

"$(INTDIR)\IsLine3d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsPlane3d.cpp

"$(INTDIR)\IsPlane3d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsPoint2d.cpp

"$(INTDIR)\IsPoint2d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsPoint3d.cpp

"$(INTDIR)\IsPoint3d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsQuat.cpp

"$(INTDIR)\IsQuat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsRect.cpp

"$(INTDIR)\IsRect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsUserData.cpp

"$(INTDIR)\IsUserData.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsVect2d.cpp

"$(INTDIR)\IsVect2d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsVect3d.cpp

"$(INTDIR)\IsVect3d.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\IsVolume.cpp

"$(INTDIR)\IsVolume.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

