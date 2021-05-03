// PDSDataCopyDlg.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "SmartDraw.h"
#include "ProjectSettingSheet.h"
#include "FileBrowseGridCell.h"

// CProjectSettingSheet dialog

IMPLEMENT_DYNAMIC(CProjectSettingSheet, CDialog)

CProjectSettingSheet::CProjectSettingSheet(const int& nSelectedPage , const CString& rProjectName , const CString& rSettingFilePath , CWnd* pParent /*=NULL*/)
	: CDialog(CProjectSettingSheet::IDD, pParent) , m_nSelectedPage(nSelectedPage) , m_rProjectName(rProjectName) , m_sIniFilePath(rSettingFilePath)
{
	m_pActivePage = NULL;
}

CProjectSettingSheet::~CProjectSettingSheet()
{
}

void CProjectSettingSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CProjectSettingSheet, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAGE, &CProjectSettingSheet::OnTvnSelchangedTreePage)
	ON_BN_CLICKED(IDOK, &CProjectSettingSheet::OnBnClickedOk)
END_MESSAGE_MAP()


// CProjectSettingSheet message handlers
/**
	@brief	

	@author	HumKyung.BAEK

	@date	
*/
BOOL CProjectSettingSheet::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString sText;
	GetWindowText(sText);
	SetWindowText(sText + _T(" - ") + m_rProjectName);

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	if(pTreeCtrl)
	{
		CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();
		pTreeCtrl->SetImageList(&(pApp->m_aDrawImages) , TVSIL_NORMAL);
		HTREEITEM hRoot = pTreeCtrl->InsertItem(_T("Project Settings"));
		
		HTREEITEM hItem = AddPage(CDatabaseSettingPage::IDD , _T("Database Setting") , &m_wndDatabaseSettingPage , DATABASE_ICON , hRoot);
		m_PageIndexMap.insert(make_pair(0 , &m_wndDatabaseSettingPage));

		hItem = AddPage(CFolderSettingPage::IDD , _T("Folder Setting") , &m_wndFolderSettingPage , FOLDER_ICON , hRoot);
		m_PageIndexMap.insert(make_pair(1 , &m_wndFolderSettingPage));
		m_wndFolderSettingPage.LoadData(m_sIniFilePath);
		
		///m_wndDatabaseSettingPage.LoadData(m_rSettingFilePath);

		pTreeCtrl->Expand(hRoot , TVE_EXPAND);
	}

	map<int,CSmartDrawPropertyPage*>::iterator where = m_PageIndexMap.find(m_nSelectedPage);
	if(where != m_PageIndexMap.end())
	{
		m_pActivePage = where->second;
		for(map<HTREEITEM,CSmartDrawPropertyPage*>::iterator itr = m_PageItemMap.begin();itr != m_PageItemMap.end();++itr)
		{
			if(m_pActivePage == itr->second)
			{
				pTreeCtrl->SetItemState(itr->first , TVIS_SELECTED, TVIS_SELECTED);
				pTreeCtrl->SelectItem(itr->first);
				break;
			}
		}

		m_pActivePage->ShowWindow(SW_SHOWNORMAL);
		pTreeCtrl->SetFocus();
	}

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
		m_wndCancelButton.SetIcon(hIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	?
*/
HTREEITEM CProjectSettingSheet::AddPage(const UINT& nID , const CString& rLabel , CSmartDrawPropertyPage* pPage , const int& nImage , HTREEITEM hParent)
{
	assert(pPage && "pPage is NULL");
	HTREEITEM hItem = NULL;

	if(pPage)
	{
		if(TRUE == pPage->Create(m_sIniFilePath , nID , this))
		{
			CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
			hItem = pTreeCtrl->InsertItem(rLabel , nImage , nImage , hParent);
			m_PageItemMap.insert(make_pair(hItem , pPage));
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
		m_PageItemMap.insert(make_pair(hItem , pPage));
	}
	
	return hItem;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.07.09
*/
void CProjectSettingSheet::OnTvnSelchangedTreePage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	SetActivePage(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

/**
	@brief	

	@author	humkyung

	@date	2010.07.09
*/
int CProjectSettingSheet::SetActivePage(HTREEITEM hItem)
{
	assert(hItem && "hItem is NULL");

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PAGE);
	if(hItem && pTreeCtrl)
	{
		map<HTREEITEM , CSmartDrawPropertyPage*>::iterator where = m_PageItemMap.find(hItem);
		if((where != m_PageItemMap.end()) && (NULL != where->second))
		{
			if(m_pActivePage != where->second)
			{
				if(NULL != m_pActivePage) m_pActivePage->ShowWindow(SW_HIDE);
				m_pActivePage = where->second;
				m_pActivePage->ShowWindow(SW_SHOWNORMAL);
				
				/// read project ini file to display model folder - 2012.04.18 added by humkyung
				if(m_pActivePage == &m_wndFolderSettingPage)
				{
					m_wndFolderSettingPage.LoadData(m_sIniFilePath);
				}
				/// up to here
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
    @date       2012-02-12
    @function   OnBnClickedOk
    @return     void
    @brief
******************************************************************************/
void CProjectSettingSheet::OnBnClickedOk()
{
	for(map<int , CSmartDrawPropertyPage*>::iterator itr = m_PageIndexMap.begin();itr != m_PageIndexMap.end();++itr)
	{
		itr->second->SaveData(/*m_rSettingFilePath*/);
	}

	OnOK();
}