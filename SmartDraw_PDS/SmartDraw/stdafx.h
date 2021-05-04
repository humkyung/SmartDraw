// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxwinforms.h>		// MFC Windows Forms support
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <afxdisp.h>
#include <atlbase.h>
//#include <mscoree.h>
#include <comutil.h>

#include <IsTools.h>
#include <afxsock.h>    /// for CSocket
#ifdef	UNICODE
#include <BugTrap.h>
#endif
/*
#include "Socket/CommandObject.h"
#include "Socket/ClientSocket.h"
*/

#include <string>
#include <afxdlgs.h>
using namespace std;

#define	PRODUCT_NAME		_T("SmartDraw for PDS")
#define	PROJECT_MDB_NAME	_T("SmartDraw_Project.mdb")
#define DB_PASSWORD			_T("")
#define	PROJECT_TABLE_NAME	_T("T_SMARTDRAW_PROJECT")
#define	DISCIPLINE_NAME		_T("'ELECTRIC'")
#define	PROVIDER			_T("Provider=Microsoft.Jet.OLEDB.4.0;")
#define	LICENDPOINT			"http://www.solutionware.co.kr/LicWebService/LicWebService.asmx"

#define	DEFAULT_GRID_ROW_HEIGHT	25

#define	ID_DRA_TREE_LIST_CTRL		WM_USER + 1
#define	ID_PROJECT_SETTING			WM_USER + 2
#define	ID_GENERATION_OPTIONS		WM_USER + 3
#define	ID_GENERATION_DRA			WM_USER + 4
#define	ID_GENERATION_GENERATE		WM_USER + 5
#define	ID_EDIT_REVISION_DATA		WM_USER + 6
#define	ID_BATCH_PLOTTING			WM_USER + 7
#define	ID_OPTION_SETTING			WM_USER + 8
#define	ID_BORDER_CHANGED			WM_USER + 9
#define	ID_INSERT_REV_NO			WM_USER + 10
#define	ID_FILL_UP_MATCH_LINE_DATA	WM_USER + 11
#define	ID_GENERATE_REPORT			WM_USER + 12
#define	ID_EDIT_PIPE_DIA_TABLE		WM_USER + 13
#define	ID_UPDATE_TITLE_BLOCK		WM_USER + 14

#define	ICON_ROOT			0
#define	ICON_SETTING		1
#define	ICON_DRA			2
#define	ICON_VIEW			3

#define	SETTING_ICON		0
#define	EQUIPMENT_ICON		1
#define	NOZZLE_ICON			2
#define	REDUCER_ICON		3
#define	INSTRUMENT_ICON		4
#define	TREE_ICON			5
#define	LINE_NO_ICON		6
#define	STRUCTURE_ICON		7
#define	INSIDE_VIEW_ICON	8
#define	OUTSIDE_VIEW_ICON	9
#define	DIMENSION_ICON		10
#define	AREA_ICON			11
#define	BORDER_ICON			12
#define	CABLE_TRAY_ICON		13
#define	SPECIAL_ITEM_ICON	14
#define	OTHER_ICON			15
#define	HATCHING_ICON		16
#define	MARK_ICON			17
#define	OTHER_GENERATION	18
#define	SUPPORT_ICON		19
#define	REVISION_CHART_ICON	20
#define	NOZZLE_CHART_ICON	21
#define	TITLE_BLOCK_ICON	22
#define	GRID_NO_ICON		23
#define	KEY_PLAN_ICON		24
#define	FOLDER_ICON			25
#define	DATABASE_ICON		26
#define	PLUGIN_ICON			27
#define	EQP_HATCHING_ICON	28
#define	PF_HATCHING_ICON	29
#define	HLR_ICON			30
#define	VALVE_ICON			31

typedef struct ProjectInfoTag
{
	STRING_T m_rProjectNo , m_rProjectName , m_rProjectDesc;
	STRING_T m_rProjectClient;
	STRING_T m_sPassword;	/// 2012.12.27 added by humkyung
}ProjectInfo;

typedef struct tagRGBQuad
{
	BYTE r,g,b;
}RGBQuad;

#define	KEY_DOWN(vk_code) ((::GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#ifdef _UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

class CSmartDrawDoc;
extern CSmartDrawDoc* GetMDIActiveDocument();
extern BOOL FileExist(CString rFilePath);
extern CString GetExecPath(void);
extern COLORREF DarkenColor(COLORREF col,const double& factor);
extern BOOL executeCommandLine(CString cmdLine, DWORD & exitCode);
extern CString _TR(const CString&);

#define  SD_MESSAGE	(WM_USER + 1)
#define  SD_LOG		(WM_USER + 2)

//#pragma comment(lib,"mscoree.lib ")
#include <afxdhtml.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


