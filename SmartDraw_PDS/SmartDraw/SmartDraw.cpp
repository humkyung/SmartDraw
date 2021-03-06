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

// SmartDraw.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include <util/FileTools.h>
#include <util/FileVersion.h>
#include <Splash/SplashScreenFx.h>
#include "SmartDraw.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SmartDrawDoc.h"
#include "SmartDrawView.h"
#include "SmartDrawDocData.h"

#include "LoginDlg.h"
#include "../../SmartDrawModule/aDraw_DraFile.h"
#include "PluginManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmartDrawApp

BEGIN_MESSAGE_MAP(CSmartDrawApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSmartDrawApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

/******************************************************************************
	@author     humkyung
	@date       2013-08-18
    @class
	@function   SetupExceptionHandler
	@return     void
	@brief
******************************************************************************/
#ifdef UNICODE
static void SetupExceptionHandler()
{
	BT_InstallSehFilter();
	BT_SetSupportEMail(_T("zbaekhk@gmail.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
	BT_SetSupportServer(_T("localhost"), 9999);
	BT_SetSupportURL(_T("http://www.solutionware.co.kr"));

	// ŽëÇ¥ÀûÀÎ ŒÓŒºµéÀº ŽÙÀœ°ú °°ŽÙ.
	// BTF_DETAILEDMODE : ¹ÌŽÏŽýÇÁ³ª ·Î±×ÆÄÀÏµîÀ» ŸÐÃàÇØŒ­ žðµÎ ÀüŒÛÇØÁØŽÙ.
	// Á€ÀÇÇÏÁö ŸÊÀ» °æ¿ì ±âº»ÀûÀÎ Á€ºžžž ÀüŒÛÇØÁØŽÙ.
	// BTF_SCREENCAPTURE : œºÅ©ž°ŒŠÀ» Ã·ºÎÇÑŽÙ.

	// ¹ÌŽÏŽýÇÁ¿¡ ÂüÁ¶º¯Œö±îÁö ³²±äŽÙ.
	BT_SetFlags( BTF_DETAILEDMODE | BTF_SCREENCAPTURE );

	// Log»ýŒº
	int g_jBT_LogSound = BT_OpenLogFile( _T("SmartDraw.log") , BTLF_TEXT );
	BT_SetLogSizeInEntries( g_jBT_LogSound, 1024 );
	BT_SetLogFlags( g_jBT_LogSound, BTLF_SHOWLOGLEVEL | BTLF_SHOWTIMESTAMP );
	BT_SetLogLevel( g_jBT_LogSound, BTLL_INFO );
	LPCTSTR pLogFileName = BT_GetLogFileName( g_jBT_LogSound );
	BT_AddLogFile( pLogFileName );
}
#endif

#pragma comment(linker, "\"/manifestdependency:type='win32'\
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 
// CSmartDrawApp construction

CSmartDrawApp::CSmartDrawApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSmartDrawApp object

CSmartDrawApp theApp;

/**
	@brief	show demo splash
	@author	humkyung
	@date	2014.04.25
*/
int ShowDemoSplash()
{
	CSplashScreenFx *pSplash = new CSplashScreenFx();
	if(pSplash)
	{
		pSplash->Create(GetDesktopWindow() , _T(""), 0 , CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
		pSplash->SetBitmap(IDB_DEMO,255,255,255);
		pSplash->Show();
		pSplash->SetActiveWindow();
	}

	return ERROR_SUCCESS;
}

// CSmartDrawApp initialization
BOOL CSmartDrawApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	if( FALSE == CSingleInstance::Create (_T("8495FE09-95A1-4be9-ACE2-3A5B380E19AF")))
		return FALSE ;
#ifdef	UNICODE
	SetupExceptionHandler();        /// 2014.04.16 added by humkyung
#endif

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CLoginDlg dlg;
	if(IDOK == dlg.DoModal())
	{	
		CString rServerFolderPath = dlg.GetServerFolderPath();
		docData.SetServerFolderPath( rServerFolderPath );
	}else	return FALSE;
	if(-1 != docData.GetUser().Find(_T("DEMO"))) ShowDemoSplash();

	this->LoadPlugins();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(PRODUCT_NAME);
	LoadStdProfileSettings(0);  // Load standard INI file options

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	InitShellManager();	///for browser button

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_SMARTDRAWTYPE,
		RUNTIME_CLASS(CSmartDrawDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSmartDrawView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	this->CreateImageList();	/// 2012.02.11 added by humkyung

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	/// call DragAcceptFiles only if there's a suffix
	///  In an MDI app, this should occur immediately after setting m_pMainWnd

	/// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if(CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	
	if(!dlg.m_rProjectName.IsEmpty())
	{
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
		cmdInfo.m_strFileName   = dlg.m_rProjectName;
	}

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// App command to run the dialog
void CSmartDrawApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSmartDrawApp customization load/save methods

void CSmartDrawApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_PROJECT);

	bNameValid = strName.LoadString(IDS_DRA);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_DRA);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_VIEW);		/// 2012.02.12 added by humkyung
	GetContextMenuManager()->AddMenu(_T("IDR_POPUP_SETTING") , IDR_POPUP_SETTING);	/// 2016.03.11 added by humkyung
	{
		HMENU hMenu = GetContextMenuManager()->GetMenuByName(_T("IDR_POPUP_SETTING"));
		CMenu* pMenu = CMenu::FromHandle(hMenu);
		pMenu->GetSubMenu(0)->RemoveMenu(ID_EDIT_ELEVATION_TYPE_BY_SPEC,MF_BYCOMMAND);
		//pMenu->GetSubMenu(0)->ModifyMenu(ID_EDIT_ELEVATION_TYPE_BY_SPEC,MF_BYCOMMAND|MF_STRING,ID_EDIT_ELEVATION_TYPE_BY_SPEC,_TR("Edit Elevation Type By Spec"));
	}
	GetContextMenuManager()->AddMenu(_T("IDR_OUTPUT_POPUP") , IDR_OUTPUT_POPUP);	/// 2017.07.03 added by humkyung
}

void CSmartDrawApp::LoadCustomState()
{
}

void CSmartDrawApp::SaveCustomState()
{
}

// CSmartDrawApp message handlers

/**
	@brief	open new database(*.mdb) file

	@author	HumKyung.BAEK

	@date	2010.05.10
*/
int CSmartDrawApp::OpenNewDocument(const CString& rFilePath)
{
	/// close all opended documents - 2012.02.11 added by humkyung
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL)
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->CloseAllDocuments(TRUE);
		}
	}
	/// up to here

	CString strDocName;
	CDocTemplate* pSelectedTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);

		POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
		while(NULL != docPos)
		{
			CDocument *pDoc = pSelectedTemplate->GetNextDoc(docPos);

			CString rTemp[2];
			rTemp[0] = rFilePath; rTemp[1] = pDoc->GetTitle();
			rTemp[0].MakeUpper(); rTemp[1].MakeUpper();
			if(rTemp[0] == rTemp[1]) return ERROR_SUCCESS;
		}

		pSelectedTemplate->OpenDocumentFile(rFilePath);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @function   __SortByDrawingType
    @return     bool
    @param      Plugin* lhs
    @param      Plugin* rhs
    @brief
******************************************************************************/
static bool __SortByDrawingType(Plugin* lhs , Plugin* rhs)
{
	STRING_T sDrawingTypes[3]={_T("Piping") , _T("CableTray") , _T("OpeningHole")};

	LPCTSTR pDrawingType1 = lhs->pfGetDrawingName();
	STRING_T* at1 = find(sDrawingTypes , sDrawingTypes + sizeof(sDrawingTypes)/sizeof(sDrawingTypes[0]) , STRING_T(pDrawingType1));
	long l1 = at1 - sDrawingTypes;
	LPCTSTR pDrawingType2 = rhs->pfGetDrawingName();
	STRING_T* at2 = find(sDrawingTypes , sDrawingTypes + sizeof(sDrawingTypes)/sizeof(sDrawingTypes[0]) , STRING_T(pDrawingType2));
	long l2 = at2 - sDrawingTypes;

	return (l1 < l2);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   LoadPlugins
    @return     int
    @brief
******************************************************************************/
int CSmartDrawApp::LoadPlugins()
{
	CPluginManager& inst = CPluginManager::GetInstance();

	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);
	
	vector<CString> oFoundFiles;
	CFileTools::FindFiles(oFoundFiles , sExecPath + _T("Drawings") , _T("*.dll") , false);
	for(vector<CString>::iterator itr = oFoundFiles.begin();itr != oFoundFiles.end();++itr)
	{
		HMODULE hModule = AfxLoadLibrary(*itr);
		if(hModule)
		{
			Plugin *pPlugin = (Plugin*)calloc(1 , sizeof(Plugin));
			if(NULL != pPlugin)
			{
				pPlugin->hModule = hModule;
				
				pPlugin->pfGetDrawingName = (GetDrawingName)GetProcAddress(hModule , ("_GetDrawingName@0"));
				pPlugin->pfGetHsrType = (GetHsrType)GetProcAddress(hModule , ("_GetHsrType@0"));
				pPlugin->pfAnnotate = (Annotate)GetProcAddress(hModule , ("_Annotate@24"));
				pPlugin->pfCreatePropertyPage = (CreatePropertyPage)GetProcAddress(hModule , ("_CreatePropertyPage@12"));
				pPlugin->pfSaveSetting = (SaveSetting)GetProcAddress(hModule , ("_SaveSetting@0"));
				pPlugin->pfClosePropertyPage  = (ClosePropertyPage)GetProcAddress(hModule , ("_ClosePropertyPage@4"));
				pPlugin->pfInitReport = (InitReport)GetProcAddress(hModule , ("_InitReport@4"));
				pPlugin->pfReport = (Report)GetProcAddress(hModule , ("_Report@20"));

				inst.m_oPluginList.push_back( pPlugin );
			}
		}
	}

	stable_sort(inst.m_oPluginList.begin() , inst.m_oPluginList.end() , __SortByDrawingType);

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	humkyung
	@date	2010.70.07
*/
int CSmartDrawApp::CreateImageList(void)
{
	m_aDrawImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1);
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_SETTING));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_EQUIPMENT));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_NOZZLE));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_REDUCER));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_INSTRUMENT));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_TREE));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_LINE_NO));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_STRUCTURE));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_INSIDE_VIEW));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_OUTSIDE_VIEW));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_DIMENSION));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_AREA));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_BORDER));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_CABLE_TRAY));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_SPECIAL_ITEM));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_OTHER));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_HATCHING));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_MARK));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_GENERATION));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_SUPPORT));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_REVISION_CHART));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_NOZZLE_CHART));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_TITLE_BLOCK));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_GRID_NO));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_KEY_PLAN));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_FOLDER));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_DATABASE));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_PLUGIN));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_EQP_HATCHING));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_PF_HATCHING));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_HLR));
	m_aDrawImages.Add(AfxGetApp()->LoadIcon(IDI_VALVE));	/// 2015.09.01 added by humkyung

	return 0;
}

/**
	@brief	show product version
	@author	humkyung
	@date	2014.04.16
*/
void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	TCHAR szModuleName[MAX_PATH+1] = {0,}; 
 	(void)GetModuleFileName(AfxGetInstanceHandle(), szModuleName, MAX_PATH); 
 	CFileVersion oVersion; 
 	if(TRUE == oVersion.Open(szModuleName))
 	{ 
 		CString sVersion = oVersion.GetFileVersion();
		SetDlgItemText(IDC_STATIC_VERSION , PRODUCT_NAME + CString(_T(" Ver: ")) + sVersion);
 	}
}
