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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <assert.h>
#include <afxribboncheckbox.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "StringTable.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_MESSAGE(SD_MESSAGE, &CMainFrame::OnSmartDrawMessage)
	ON_MESSAGE(SD_LOG , &CMainFrame::OnSmartDrawLog)
	ON_UPDATE_COMMAND_UI(IDS_PROJECT_EXPLORER, &CMainFrame::OnUpdateViewProjectExplorer)
	ON_COMMAND(IDS_PROJECT_EXPLORER, &CMainFrame::OnViewProjectExplorer)
	ON_UPDATE_COMMAND_UI(IDS_PROPERTIES_WND, &CMainFrame::OnUpdateViewPropertyWindow)
	ON_COMMAND(IDS_PROPERTIES_WND, &CMainFrame::OnViewPropertyWindow)
	ON_UPDATE_COMMAND_UI(IDS_OUTPUT_WND, &CMainFrame::OnUpdateViewOutputWindow)
	ON_COMMAND(IDS_OUTPUT_WND, &CMainFrame::OnViewOutputWindow)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strWelcomeMsg = _T("Welcome ") + CSmartDrawDocData::GetInstance().GetUser();
	
	MyPane* pMyPane1 = new MyPane(IDS_STATUSBAR_PANE1, strWelcomeMsg, TRUE);
	pMyPane1->m_clrText = RGB(0,0,255);
	m_wndStatusBar.AddElement(pMyPane1, strWelcomeMsg);
	MyPane* pMyPane2 = new MyPane(IDS_STATUSBAR_PANE2, _T("") , TRUE);
	pMyPane2->m_clrText = RGB(255,0,0);
	m_wndStatusBar.AddElement(pMyPane2, _T(""));
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(IDS_STATUSBAR_PANE3, _T("") , TRUE), _T(""));
	
	/// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	/// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	/// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndProjectExplorer.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProjectExplorer);
	CDockablePane* pTabbedBar = NULL;
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	m_wndProperties.ShowPane(TRUE , FALSE , TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/******************************************************************************
    @brief		initialize ribbon bar
	@author     humkyung
    @date       2012-03-28
    @function   InitializeRibbon
    @return     void
******************************************************************************/
void CMainFrame::InitializeRibbon()
{
	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	/// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// Init main button:
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

	CStringTable& table = CStringTable::GetInstance();
	bNameValid = strTemp.LoadString(IDS_RIBBON_MANUAL);
	ASSERT(bNameValid);
	strTemp = table.GetString(_T("IDS_MANUAL")).c_str();
	pMainPanel->Add(new CMFCRibbonButton(IDS_RIBBON_MANUAL, strTemp, 0, 0));
	
	pMainPanel->Add(new CMFCRibbonSeparator());

	bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
	ASSERT(bNameValid);
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Add "SmartDraw" category
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CMFCRibbonCategory* pCategoryData = m_wndRibbonBar.AddCategory(_T("SmartDraw") , IDB_FILESMALL, IDB_FILELARGE);
	if(pCategoryData)
	{
		/// Create "Data" panel:
		strTemp = table.GetString(_T("IDS_SETTING")).c_str();
		CMFCRibbonPanel* pPanelData = pCategoryData->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));
		if(pPanelData)
		{
			strTemp = table.GetString(_T("IDS_PROJECT_SETTING")).c_str();
			CMFCRibbonButton* pBtnExcelExport = new CMFCRibbonButton(ID_PROJECT_SETTING , strTemp, 0, 15);
			pPanelData->Add(pBtnExcelExport);
		}

		/// Create "Generate" panel:
		strTemp = table.GetString(_T("IDS_GENERATION")).c_str();
		CMFCRibbonPanel* pPanelGeneration = pCategoryData->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));
		if(pPanelGeneration)
		{
			strTemp = table.GetString(_T("IDS_IMPORT_DRA")).c_str();
			CMFCRibbonButton* pBtnGenerateDra = new CMFCRibbonButton(ID_GENERATION_DRA , strTemp, 0, 12);
			pPanelGeneration->Add(pBtnGenerateDra);

			strTemp = table.GetString(_T("IDS_OPTIONS")).c_str();
			CMFCRibbonButton* pBtnSetting = new CMFCRibbonButton(ID_OPTION_SETTING , strTemp , 0, 17);
			pPanelGeneration->Add(pBtnSetting);

			pPanelGeneration->AddSeparator();
			strTemp.LoadString(ID_GENERATION_HLR);
			CMFCRibbonButton* pBtnGenerateHLR = new CMFCRibbonCheckBox(ID_GENERATION_HLR , _T("HLR"));
			pPanelGeneration->Add(pBtnGenerateHLR);
			strTemp.LoadString(ID_GENERATION_ANNOTATION);
			CMFCRibbonButton* pBtnGenerateAnnotation = new CMFCRibbonCheckBox(ID_GENERATION_ANNOTATION , _T("Annotation"));
			pPanelGeneration->Add(pBtnGenerateAnnotation);

			strTemp = table.GetString(_T("IDS_GENERATE")).c_str();
			CMFCRibbonButton* pBtnGenerate = new CMFCRibbonButton(ID_GENERATION_GENERATE , strTemp, 0, 14);
			pPanelGeneration->Add(pBtnGenerate);
		}

		strTemp = table.GetString(_T("IDS_REVISION")).c_str();
		CMFCRibbonPanel* pPanelRevision = pCategoryData->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));
		if(pPanelRevision)
		{
			strTemp = table.GetString(_T("IDS_REV_MANAGER")).c_str();
			CMFCRibbonButton* pBtnEditRevisionData = new CMFCRibbonButton(ID_EDIT_REVISION_DATA , strTemp , 0, 16);
			pPanelRevision->Add(pBtnEditRevisionData);

			strTemp = table.GetString(_T("IDS_INSERT_REV_NO")).c_str();
			CMFCRibbonButton* pBtnInsertRevNo = new CMFCRibbonButton(ID_INSERT_REV_NO, strTemp , 0, 18);
			pPanelRevision->Add(pBtnInsertRevNo);
		}

		strTemp = table.GetString(_T("IDS_UTILITY")).c_str();
		CMFCRibbonPanel* pPanelUtility = pCategoryData->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));
		if(pPanelUtility)
		{
			CMFCRibbonButton* pBtnUpdateTitleBlock = new CMFCRibbonButton(ID_UPDATE_TITLE_BLOCK, _TR("Update Title Block") , 19);
			pPanelUtility->Add(pBtnUpdateTitleBlock);

			strTemp = table.GetString(_T("IDS_FILL_UP_MATCH_LINE_DATA")).c_str();
			CMFCRibbonButton* pBtnFillUpMatchlineData = new CMFCRibbonButton(ID_FILL_UP_MATCH_LINE_DATA, strTemp , 16);
			pPanelUtility->Add(pBtnFillUpMatchlineData);

			strTemp = _TR("Report");
			CMFCRibbonButton* pBtnGenerateLineList = new CMFCRibbonButton(ID_GENERATE_REPORT, strTemp , 17);
			pPanelUtility->Add(pBtnGenerateLineList);

			CMFCRibbonButton* pBtnEditPipeDiaTable = new CMFCRibbonButton(ID_EDIT_PIPE_DIA_TABLE , _TR("Edit Pipe Dia. Table") , 18);
			pPanelUtility->Add(pBtnEditPipeDiaTable);

			strTemp = table.GetString(_T("IDS_PLOTTING")).c_str();
			CMFCRibbonButton* pBtnBatchPlotting = new CMFCRibbonButton(ID_BATCH_PLOTTING , strTemp, 0, 6);
			pPanelUtility->Add(pBtnBatchPlotting);
		}
	}

	CMFCRibbonCategory* pViewCategory = m_wndRibbonBar.AddCategory(_T("View") , IDB_FILESMALL, IDB_FILELARGE);
	if(pViewCategory)
	{
		strTemp = table.GetString(_T("IDS_VIEW")).c_str();
		CMFCRibbonPanel* pPanelView = pViewCategory->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));
		if(pPanelView)
		{
			strTemp = table.GetString(_T("IDS_PROJECT_EXPLORER")).c_str();
			CMFCRibbonButton* pBtnProjectExplorer = new CMFCRibbonButton(IDS_PROJECT_EXPLORER , strTemp , 0,-1);
			pPanelView->Add(pBtnProjectExplorer);

			strTemp = table.GetString(_T("IDS_PROPERTIES_WND")).c_str();
			CMFCRibbonButton* pBtnPropertyWindow = new CMFCRibbonButton(IDS_PROPERTIES_WND, strTemp , 0,-1);
			pPanelView->Add(pBtnPropertyWindow);

			strTemp = table.GetString(_T("IDS_OUTPUT_WND")).c_str();
			CMFCRibbonButton* pBtnOutputWindow = new CMFCRibbonButton(IDS_OUTPUT_WND , strTemp , 0,-1);
			pPanelView->Add(pBtnOutputWindow);
		}
	}

	/// Add elements to the right side of tabs:
	strTemp = table.GetString(_T("IDS_RIBBON_STYLE")).c_str();
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);

	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CreateDockingWindows
    @return     BOOL
    @brief
******************************************************************************/
BOOL CMainFrame::CreateDockingWindows()
{
	CStringTable& table = CStringTable::GetInstance();

	/// Create file view
	CString strFileView(table.GetString(_T("IDS_PROJECT_EXPLORER")).c_str());
	if (!m_wndProjectExplorer.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Project Explorer window\n");
		return FALSE; // failed to create
	}

	/// Create output window
	CString strOutputWnd(table.GetString(_T("IDS_OUTPUT_WND")).c_str());
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	/// Create properties window
	CString strPropertiesWnd(table.GetString(_T("IDS_PROPERTIES_WND")).c_str());;
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProjectExplorer.SetIcon(hFileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

LRESULT CMainFrame::OnSmartDrawMessage(WPARAM wParam, LPARAM lParam)
{
	this->DisplayMessage(LPCTSTR(wParam));

	return 0L;
}

/**
	@brief	display log

	@author	humkyung

	@date	2013.06.09
*/
LRESULT CMainFrame::OnSmartDrawLog(WPARAM wParam, LPARAM lParam)
{
	if(m_wndOutput.GetSafeHwnd())
	{
		m_wndOutput.DisplayMessage(wParam , lParam);
	}

	return 0L;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

/**
	@brief	display message on status ribbon bar
	@author	humkyung
	@date	2013.06.05
*/
int CMainFrame::DisplayMessage(LPCTSTR lpMsg)
{
	assert(lpMsg && "lpMsg is NULL");

	if(lpMsg)
	{
		((MyPane*)(m_wndStatusBar.GetElement(1)))->m_clrText = RGB(255,0,0); ///return whatever new color you want
		m_wndStatusBar.GetElement(1)->SetText(lpMsg);
		m_wndStatusBar.ForceRecalcLayout();
		m_wndStatusBar.GetElement(1)->Redraw();
		//m_wndStatusBar.GetElement(0)->DrawImage(
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**	
	@brief	
	@author	humkyung
	@date	2014.12.23
	@param	pCmdUI
	@return	none
*/
void CMainFrame::OnUpdateViewProjectExplorer(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndProjectExplorer.IsVisible());
}

/**	
	@brief	show/hide project explorer
	@author	humkyung
	@date	2014.12.23
	@param	
	@return	none
*/
void CMainFrame::OnViewProjectExplorer()
{
	m_wndProjectExplorer.ShowWindow(m_wndProjectExplorer.IsVisible() ? SW_HIDE : SW_NORMAL);
	this->AdjustDockingLayout();
}

/**	
	@brief	
	@author	humkyung
	@date	2014.12.23
	@param	pCmdUI
	@return	none
*/
void CMainFrame::OnUpdateViewPropertyWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndProperties.IsVisible());
}

/**	
	@brief	show/hide property window
	@author	humkyung
	@date	2014.12.23
	@param	
	@return	none
*/
void CMainFrame::OnViewPropertyWindow()
{
	m_wndProperties.ShowWindow(m_wndProperties.IsVisible() ? SW_HIDE : SW_NORMAL);
	this->AdjustDockingLayout();
}

/**	
	@brief	
	@author	humkyung
	@date	2014.12.23
	@param	pCmdUI
	@return	none
*/
void CMainFrame::OnUpdateViewOutputWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
}

/**	
	@brief	show/hide output window
	@author	humkyung
	@date	2014.12.23
	@param	
	@return	none
*/
void CMainFrame::OnViewOutputWindow()
{
	m_wndOutput.ShowWindow(m_wndOutput.IsVisible() ? SW_HIDE : SW_NORMAL);
	this->AdjustDockingLayout();
}