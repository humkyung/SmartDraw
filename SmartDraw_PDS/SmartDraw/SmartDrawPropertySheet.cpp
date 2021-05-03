// SmartDrawAnnoPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include <util/FileTools.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "PluginManager.h"

#include <fstream>
using namespace std;

IMPLEMENT_DYNAMIC(CPropertyTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CPropertyTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

// CSmartDrawPropertySheet dialog

IMPLEMENT_DYNAMIC(CSmartDrawPropertySheet, CDialog)

CSmartDrawPropertySheet::CSmartDrawPropertySheet(const int& nSelectedPage , const CString& rProjectName , const CString& rSettingFilePath , const STRING_T& sDrawingType ,
														 CWnd* pParent /*=NULL*/)
	: CDialog(CSmartDrawPropertySheet::IDD, pParent) , m_nSelectedPage(nSelectedPage) , m_rProjectName(rProjectName) , m_sIniFilePath(rSettingFilePath)
{
	m_sDrawingType = sDrawingType;
	m_pActivePage = NULL;
}

CSmartDrawPropertySheet::~CSmartDrawPropertySheet()
{
}

void CSmartDrawPropertySheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_TREE_PAGE , m_wndPropertyTreeCtrl);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CSmartDrawPropertySheet, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAGE, &CSmartDrawPropertySheet::OnTvnSelchangedTreePage)
	ON_BN_CLICKED(IDOK, &CSmartDrawPropertySheet::OnBnClickedOk)
	ON_COMMAND(ID_BORDER_OPEN, &CSmartDrawPropertySheet::OnOpenBorder)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PAGE, &CSmartDrawPropertySheet::OnNMRClickTreePage)
	ON_UPDATE_COMMAND_UI(ID_BORDER_OPEN, &CSmartDrawPropertySheet::OnUpdateBorderOpen)
	ON_BN_CLICKED(IDCANCEL, &CSmartDrawPropertySheet::OnBnClickedCancel)
	ON_COMMAND(ID_BORDER_CHANGED , &CSmartDrawPropertySheet::OnBorderChanged)
END_MESSAGE_MAP()


// CSmartDrawPropertySheet message handlers
/**
	@brief	

	@author	humkyung

	@date	????.??.??
*/
HTREEITEM CSmartDrawPropertySheet::AddPage(const UINT& nID , const CString& rLabel , CSmartDrawPropertyPage* pPage , const int& nImage , HTREEITEM hParent)
{
	assert(pPage && "pPage is NULL");
	HTREEITEM hItem = NULL;

	if(pPage)
	{
		if(TRUE == pPage->Create(m_sIniFilePath , nID , this))
		{
			CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
			hItem = pTreeCtrl->InsertItem(rLabel , nImage , nImage , hParent);
			m_oPageItemMap.insert(make_pair(hItem , pPage));
			pPage->ShowWindow(SW_HIDE);

			CRect rect;
			CWnd* pWnd = (CWnd*)GetDlgItem(IDC_STATIC_PAGE);
			if(pWnd)
			{
				pWnd->GetWindowRect(&rect);
				ScreenToClient(&rect);
			}
			else
			{
				rect.SetRect(0,0,100,100);
			}

			pPage->SetWindowPos(NULL , rect.left , rect.top , rect.Width() , rect.Height() , SWP_NOZORDER);
			m_PageList.push_back(pPage);
		}
	}
	else
	{
		CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
		hItem = pTreeCtrl->InsertItem(rLabel , nImage , nImage , hParent);
		m_oPageItemMap.insert(make_pair(hItem , pPage));
	}
	
	return hItem;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @function   CreatePageOf
    @return     HTREEITEM
    @param      const   STRING_T&
    @param      sTitle  HTREEITEM
    @param      hParent
    @brief
******************************************************************************/
HTREEITEM CSmartDrawPropertySheet::CreatePageOf(const STRING_T& sTitle , const STRING_T& sDisplay , HTREEITEM hParent)
{
	HTREEITEM hItem = NULL;

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	
	if(_T("Border") == sTitle)
	{
		hItem = AddPage(CBorderPage::IDD , sDisplay.c_str() , &m_wndBorderPage , BORDER_ICON , hParent);
	}
	else if(_T("Title Block") == sTitle)
	{
		hItem = AddPage(CTitleBlockPage::IDD , sDisplay.c_str() , &m_wndTitleBlockPage , TITLE_BLOCK_ICON , hParent);
	}
	else if(_T("Grid No.") == sTitle)
	{
		hItem = AddPage(CGridNoPage::IDD , sDisplay.c_str() , &m_wndGridNoPage , GRID_NO_ICON , hParent);
	}
	else if(_T("KeyPlan Hatching") == sTitle)
	{
		hItem = AddPage(CKeyPlanHatchingPage::IDD , sDisplay.c_str() , &m_wndKeyPlanHatchingPage , KEY_PLAN_ICON , hParent);
	}
	else if(_T("Area") == sTitle)
	{
		hItem = AddPage(CAreaOptionPage::IDD , sDisplay.c_str() , &m_wndAreaOptionPage , AREA_ICON , hParent);
	}
	else if(_T("Dimension Style") == sTitle)
	{
		hItem = AddPage(-1 , sDisplay.c_str() , NULL , OTHER_ICON , hParent);
	}
	else if(_T("Int Dim Style") == sTitle)
	{
		hItem = AddPage(CDimensionPage::IDD , sDisplay.c_str() , &m_wndIntDimStylePage , DIMENSION_ICON , hParent);
	}
	else if(_T("Ext Dim Style") == sTitle)
	{
		hItem = AddPage(CDimensionPage::IDD , sDisplay.c_str() , &m_wndExtDimStylePage , DIMENSION_ICON , hParent);
	}
	else if(_T("HLR") == sTitle)
	{
		hItem = AddPage(CHLRPage::IDD , sDisplay.c_str() , &m_wndHLRPage , HLR_ICON , hParent);
	}
	else if(_T("Annotation") == sTitle)
	{
		hItem = AddPage(CAnnotationPage::IDD , sDisplay.c_str() , &m_wndAnnotationPage , TREE_ICON , hParent);
	}
	else if(_T("Line No") == sTitle)
	{
		hItem = AddPage(CLineNoPage::IDD , sDisplay.c_str() , &m_wndLineNoPage , LINE_NO_ICON , hParent);
	}
	else if((_T("Inside View") == sTitle) && (NULL == m_wndInsideViewLineNoPage.GetSafeHwnd()))
	{
		hItem = AddPage(CInsideViewLineNoPage::IDD , sDisplay.c_str() , &m_wndInsideViewLineNoPage , INSIDE_VIEW_ICON , hParent);
	}
	else if((_T("Outside View") == sTitle) && (NULL == m_wndOutsideViewLineNoPage.GetSafeHwnd()))
	{
		hItem = AddPage(COutsideViewLineNoPage::IDD , sDisplay.c_str() , &m_wndOutsideViewLineNoPage , OUTSIDE_VIEW_ICON , hParent);
	}
	else if((_T("Instrument") == sTitle) && (NULL == m_wndInstrumentPage.GetSafeHwnd()))
	{
		hItem = AddPage(CInstrumentPage::IDD , sDisplay.c_str() , &m_wndInstrumentPage , 4 , hParent);
	}
	else if((_T("Specialty") == sTitle) && (NULL == m_wndSpecialItemPage.GetSafeHwnd()))
	{
		hItem = AddPage(CSpecialItemPage::IDD , sDisplay.c_str() , &m_wndSpecialItemPage , SPECIAL_ITEM_ICON , hParent);
	}
	else if((_T("Support") == sTitle) && (NULL == m_wndSupportPage.GetSafeHwnd()))
	{
		hItem = AddPage(CSupportPage::IDD , sDisplay.c_str() , &m_wndSupportPage , SUPPORT_ICON , hParent);
	}
	else if(_T("Equipment") == sTitle)
	{
		hItem = AddPage(-1 , sDisplay.c_str() , NULL , OTHER_ICON , hParent);
	}
	else if((_T("Inside Eqp") == sTitle) && (NULL == m_wndInsideEqpPage.GetSafeHwnd()))
	{
		hItem = AddPage(CInsideEquipmentPage::IDD , sDisplay.c_str() , &m_wndInsideEqpPage , EQUIPMENT_ICON , hParent);
	}
	else if((_T("Outside Eqp") == sTitle) && (NULL == m_wndOutsideEqpPage.GetSafeHwnd()))
	{
		hItem = AddPage(COutsideEquipmentPage::IDD , sDisplay.c_str() , &m_wndOutsideEqpPage , EQUIPMENT_ICON , hParent);
	}
	else if((_T("Nozzle") == sTitle) && (NULL == m_wndNozzlePage.GetSafeHwnd()))
	{
		hItem = AddPage(CNozzlePage::IDD , sDisplay.c_str() , &m_wndNozzlePage , 2 , hParent);
	}
	else if((_T("Reducer") == sTitle) && (NULL == m_wndReducerPage.GetSafeHwnd()))
	{
		hItem = AddPage(CReducerPage::IDD , sDisplay.c_str() , &m_wndReducerPage , 3 , hParent);
	}
	else if((_T("Structure") == sTitle) && (NULL == m_wndStructurePage.GetSafeHwnd()))
	{
		hItem = AddPage(CStructurePage::IDD , sDisplay.c_str() , &m_wndStructurePage , STRUCTURE_ICON , hParent);
	}
	else if((_T("Valve") == sTitle) && (NULL == m_wndValvePage.GetSafeHwnd()))
	{
		hItem = AddPage(CValvePage::IDD , sDisplay.c_str() , &m_wndValvePage , VALVE_ICON , hParent);
	}
	else if((_T("Hatching") == sTitle))
	{
		hItem = AddPage(-1 , sDisplay.c_str() , NULL , OTHER_ICON , hParent);
	}
	else if((_T("Vertical Line Hatching") == sTitle) && (NULL == m_wndVerticalLineHatchingPage.GetSafeHwnd()))
	{
		hItem = AddPage(CVerticalLineHatchingPage::IDD , sDisplay.c_str() , &m_wndVerticalLineHatchingPage , HATCHING_ICON , hParent);
	}
	else if((_T("Equipment Hatching") == sTitle) && (NULL == m_wndEquipmentHatchingPage.GetSafeHwnd()))
	{
		hItem = AddPage(CEquipmentHatchingPage::IDD , sDisplay.c_str() , &m_wndEquipmentHatchingPage , EQP_HATCHING_ICON , hParent);
	}
	else if((_T("Platform Hatching") == sTitle) && (NULL == m_wndPlatformHatchingPage.GetSafeHwnd()))
	{
		hItem = AddPage(CPlatformHatchingPage::IDD , sDisplay.c_str() , &m_wndPlatformHatchingPage , PF_HATCHING_ICON , hParent);
	}
	else if((_T("Flow / Slope Mark") == sTitle) && (NULL == m_wndFlow_SlopeMarkPage.GetSafeHwnd()))
	{
		hItem = AddPage(CFlow_SlopeMarkPage::IDD , sDisplay.c_str() , &m_wndFlow_SlopeMarkPage , MARK_ICON , hParent);
	}
	else if(_T("Chart") == sTitle)
	{
		CSmartDrawPropertyPage* NullPtr=NULL;
		hItem = AddPage(-1 , sTitle.c_str() , NullPtr , TREE_ICON , hParent);
	}
	else if((_T("Revision Chart") == sTitle) && (NULL == m_wndRevisionChartPage.GetSafeHwnd()))
	{
		hItem = AddPage(CRevisionChartPage::IDD , sDisplay.c_str() , &m_wndRevisionChartPage , REVISION_CHART_ICON , hParent);
	}
	else if((_T("Nozzle Chart") == sTitle) && (NULL == m_wndNozzleChartPage.GetSafeHwnd()))
	{
		hItem = AddPage(CNozzleChartPage::IDD , sDisplay.c_str() , &m_wndNozzleChartPage , NOZZLE_CHART_ICON , hParent);
	}
	else if((_T("Generation") == sTitle) && (NULL == m_wndGenerationPage.GetSafeHwnd()))
	{
		hItem = AddPage(CGenerationPage::IDD , sDisplay.c_str() , &m_wndGenerationPage , OTHER_GENERATION , hParent);
	}
	else if((_T("View Annotation") == sTitle) && (NULL == m_wndViewAnnoPage.GetSafeHwnd()))
	{
		hItem = AddPage(CViewAnnoPage::IDD , sDisplay.c_str() , &m_wndViewAnnoPage , OTHER_ICON , hParent);
	}
	else
	{
		CPluginManager& inst = CPluginManager::GetInstance();
		for(vector<Plugin*>::iterator itr = inst.m_oPluginList.begin();itr != inst.m_oPluginList.end();++itr)
		{
			if((NULL != (*itr)->pfGetDrawingName) && (sTitle == (*itr)->pfGetDrawingName()))
			{
				HWND hPageWnd = (*itr)->pfCreatePropertyPage(sTitle.c_str() , m_sIniFilePath.operator LPCTSTR() , this->GetSafeHwnd());
				CSmartDrawPropertyPage* pPage = static_cast<CSmartDrawPropertyPage*>(CSmartDrawPropertyPage::FromHandle(hPageWnd));

				hItem = pTreeCtrl->InsertItem(sTitle.c_str() , PLUGIN_ICON , PLUGIN_ICON , hParent);
				pTreeCtrl->SetItemData(hItem , DWORD_PTR(*itr));
				m_oPageItemMap.insert(make_pair(hItem , pPage));
				pPage->ShowWindow(SW_HIDE);

				CRect rect;
				CWnd* pWnd = (CWnd*)GetDlgItem(IDC_STATIC_PAGE);
				if(pWnd)
				{
					pWnd->GetWindowRect(&rect);
					ScreenToClient(&rect);
				}
				else
				{
					rect.SetRect(0,0,100,100);
				}

				pPage->SetWindowPos(NULL , rect.left , rect.top , rect.Width() , rect.Height() , SWP_NOZORDER);
				m_PageList.push_back(pPage);
			}
		}
	}

	return hItem;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @class      CSmartDrawPropertySheet
    @function   CreatePages
    @return     int
    @param      CMakrup&    xml
    @param      HTREEITEM   hParent
    @brief
******************************************************************************/
int CSmartDrawPropertySheet::CreatePages(CMarkup& xml , HTREEITEM hParent)
{
	HTREEITEM hItem = NULL;

	int iCount = 0;
	while(1)
	{
		STRING_T sName = xml.GetAttribName(iCount++);
		if ( sName.empty() ) break;
		STRING_T sValue = xml.GetAttrib( sName.c_str() );
		
		if(0 == STRICMP_T(sName.c_str() , _T("Name")))
		{
			sName = xml.GetAttribName(iCount++);
			STRING_T sDisplay = (0 == STRICMP_T(sName.c_str() , _T("Display"))) ? xml.GetAttrib( sName.c_str() ) : sValue;

			hItem = this->CreatePageOf(sValue , sDisplay , hParent);
		}
	}
	
	/// try to create child pages
	xml.IntoElem();
	while(true == xml.FindElem(MCD_T("Page")))
	{
		this->CreatePages(xml , hItem);
	}
	xml.OutOfElem();
	/// up to here

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	pTreeCtrl->Expand(hItem , TVE_EXPAND);

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @class      CSmartDrawPropertySheet
    @function   OnInitDialog
    @return     BOOL
    @brief
******************************************************************************/
BOOL CSmartDrawPropertySheet::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_SETTING),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = SetIcon(hIcon , FALSE);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();
	pTreeCtrl->SetImageList(&(pApp->m_aDrawImages) , TVSIL_NORMAL);
	
	pApp->BeginWaitCursor();
	/// reset combobox contents - 2014.06.24 added by humkyung
	CLevelComboBox::ResetContents();
	CLineStyleComboBox::ResetContents();
	CLineWeightComboBox::ResetContents();
	/// up to here

	const CString sCommonAppDataPath = CFileTools::GetCommonAppDataPath();
	const CString sConfigFilePath = sCommonAppDataPath + _T("\\SmartDraw_PDS\\Drawings\\") + CString(m_sDrawingType.c_str()) + _T(".xml");	///TODO: get drawing name
	CMarkup xml;
	if(true == xml.Load(sConfigFilePath.operator LPCTSTR()))
	{
		HTREEITEM hRoot = NULL;
		MCD_STR sLoadResult = xml.GetResult();

		if(true == xml.FindElem(MCD_T("Option")))
		{
			int iCount = 0;
			while ( 1 )
			{
				STRING_T sName = xml.GetAttribName(iCount++);
				if ( sName.empty() ) break;
				STRING_T sValue = xml.GetAttrib( sName.c_str() );

				if(0 == STRICMP_T(sName.c_str() , _T("Name")))
				{
					m_sTitle = sValue;
					hRoot = pTreeCtrl->InsertItem(m_sTitle.c_str());
				}
			}
			
			xml.IntoElem();
			while(true == xml.FindElem(MCD_T("Page")))
			{
				this->CreatePages(xml , hRoot);
			}
			xml.OutOfElem();
		}
	}
	pTreeCtrl->Expand(pTreeCtrl->GetRootItem() , TVE_EXPAND);
		
	HTREEITEM hChildItem = pTreeCtrl->GetChildItem(pTreeCtrl->GetRootItem());
	if(NULL != hChildItem)
	{
		pTreeCtrl->SetItemState(hChildItem , TVIS_SELECTED, TVIS_SELECTED);
		pTreeCtrl->SelectItem(hChildItem);

		m_pActivePage->ShowWindow(SW_SHOWNORMAL);
		pTreeCtrl->SetFocus();
	}
	
	pApp->EndWaitCursor();

	/// set button icon
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_OK),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndOKButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndOKButton.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndCancelButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	return FALSE;///TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	????.??.??
*/
void CSmartDrawPropertySheet::OnTvnSelchangedTreePage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	SetActivePage(pNMTreeView->itemNew.hItem);
	/// 2013.06.09 added by humkyung
	if(NULL != m_pActivePage)
	{
		((CSmartDrawPropertyPage*)m_pActivePage)->LoadData(m_sIniFilePath);
	}
	/// up to here

	*pResult = 0;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.14
*/
void CSmartDrawPropertySheet::OnBnClickedOk()
{
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	for(map<HTREEITEM , CSmartDrawPropertyPage*>::iterator itr = m_oPageItemMap.begin();itr != m_oPageItemMap.end();++itr)
	{
		const CString sItemText = pTreeCtrl->GetItemText(itr->first);

		if(NULL == itr->second) continue;
		if(itr->second->IsKindOf(RUNTIME_CLASS(CSmartDrawPropertyPage)))
		{
			CSmartDrawPropertyPage* pPage = (CSmartDrawPropertyPage*)(itr->second);
			if(pPage) pPage->SaveData();
		}
		else
		{
			Plugin* pPlugin = (Plugin*)(pTreeCtrl->GetItemData(itr->first));
			if(NULL != pPlugin)
			{
				pPlugin->pfSaveSetting();
				pPlugin->pfClosePropertyPage(_T(""));
			}
		}
	}

	OnOK();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.29
*/
void CSmartDrawPropertySheet::OnNMRClickTreePage(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt , ptClient;
	::GetCursorPos(&pt);
	ptClient = pt;
	m_wndPropertyTreeCtrl.ScreenToClient(&ptClient);
	
	TVHITTESTINFO htinfo;
	htinfo.pt       = ptClient;
	htinfo.flags    = TVHT_ONITEMLABEL;
	HTREEITEM hItem = m_wndPropertyTreeCtrl.HitTest(&htinfo);
	if(NULL != hItem)
	{
		///SetFocus();
		///m_wndPropertyTreeCtrl.Select(hItem , TVGN_CARET);
		const CString rItemText = m_wndPropertyTreeCtrl.GetItemText(hItem).MakeUpper();
		if(_T("BORDER") == rItemText)
		{
			CMenu menu;
			menu.LoadMenu(IDR_POPUP_BORDER);
			CMenu* pMenu = menu.GetSubMenu(0);
			if(pMenu) pMenu->TrackPopupMenu(TPM_LEFTALIGN , pt.x , pt.y , this);
		}
	}

	*pResult = 0;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.29
*/
void CSmartDrawPropertySheet::OnOpenBorder()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CString rMSTNJFilePath = docData.GetMSTNJFilePath();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CString rString(szBuf);
		const CString rDestDgnFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\") + rString;

		CString rCmdFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Working\\pds.cmd");
		{
			ofstream ofile(rCmdFilePath);
			if(ofile.is_open())
			{
				ofile << _T("MACRO getinputdatapoint") << std::endl;
				ofile.close();
			}
		}

		::ShellExecute(NULL , _T("open") , rMSTNJFilePath , rDestDgnFilePath , NULL , SW_SHOW);
	}
}

/**
	@brief	call page's LoadData func related to border
	@author	humkyung
	@date	2014.06.06
*/
void CSmartDrawPropertySheet::OnBorderChanged()
{
	m_wndKeyPlanHatchingPage.LoadData();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.29
*/
void CSmartDrawPropertySheet::OnUpdateBorderOpen(CCmdUI *pCmdUI)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		pCmdUI->Enable(TRUE);
	}else	pCmdUI->Enable(FALSE);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   GetDrawingType
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CSmartDrawPropertySheet::GetDrawingType() const
{
	return m_sDrawingType;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.07.09
*/
int CSmartDrawPropertySheet::SetActivePage(HTREEITEM hItem)
{
	assert(hItem && "hItem is NULL");

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	if(hItem && pTreeCtrl)
	{
		map<HTREEITEM , CSmartDrawPropertyPage*>::iterator where = m_oPageItemMap.find(hItem);
		if((where != m_oPageItemMap.end()) && (NULL != where->second))
		{
			if(m_pActivePage != where->second)
			{
				if(NULL != m_pActivePage) m_pActivePage->ShowWindow(SW_HIDE);
				m_pActivePage = where->second;
				m_pActivePage->ShowWindow(SW_SHOWNORMAL);
			}
		}
		else
		{
			HTREEITEM hChild = pTreeCtrl->GetChildItem(hItem);
			SetActivePage(hChild);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   OnBnClickedCancel
    @return     void
    @brief
******************************************************************************/
void CSmartDrawPropertySheet::OnBnClickedCancel()
{
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	for(map<HTREEITEM , CSmartDrawPropertyPage*>::iterator itr = m_oPageItemMap.begin();itr != m_oPageItemMap.end();++itr)
	{
		const CString sItemText = pTreeCtrl->GetItemText(itr->first);

		if(NULL == itr->second) continue;
		if(itr->second->IsKindOf(RUNTIME_CLASS(CSmartDrawPropertyPage)))
		{
		}
		else
		{
			Plugin* pPlugin = (Plugin*)(pTreeCtrl->GetItemData(itr->first));
			if(NULL != pPlugin)
			{
				pPlugin->pfClosePropertyPage(_T(""));
			}
		}
	}

	OnCancel();
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL CSmartDrawPropertySheet::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}