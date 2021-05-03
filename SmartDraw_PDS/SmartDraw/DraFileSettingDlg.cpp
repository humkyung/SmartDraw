// DraFileSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include <util/SplitPath.h>
#include "SmartDraw.h"
#include "DraFileSettingDlg.h"
#include "DraViewSettingDlg.h"
#include "SmartDrawDocData.h"
#include "StringTable.h"
#include "..\..\SmartDrawModule\aDraw_DraFile.h"

#include <Tokenizer.h>

#include <list>

#define IDC_TAB_VIEW	1

IMPLEMENT_DYNAMIC(CViewEditTreeCtrl, CEditTreeCtrl)

CViewEditTreeCtrl::CViewEditTreeCtrl()
{
}

CViewEditTreeCtrl::~CViewEditTreeCtrl()
{
	try
	{
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CViewEditTreeCtrl, CEditTreeCtrl)
	ON_COMMAND(ID_VIEW_NEW, &CViewEditTreeCtrl::OnViewNew)
	ON_COMMAND(ID_VIEW_COPYEDIT, &CViewEditTreeCtrl::OnViewCopyEdit)
	ON_COMMAND(ID_VIEW_DELETE , &CViewEditTreeCtrl::OnViewDelete)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewEditTreeCtrl::OnTvnEndlabeledit)
END_MESSAGE_MAP()

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   DisplayContextMenu
    @return     void
    @param      CPoint  &
    @param      point
    @brief
******************************************************************************/
void CViewEditTreeCtrl::DisplayContextMenu(CPoint & point) 
{
	CPoint pt(point);
	ScreenToClient(&pt);
	UINT flags;
	HTREEITEM hItem = HitTest(pt, &flags);
	bool bOnItem = (flags & TVHT_ONITEM) != 0;

	CMenu menu;
	VERIFY(menu.CreatePopupMenu());
	VERIFY(menu.AppendMenu(MF_STRING, ID_VIEW_NEW, _T("New")));
	if(bOnItem) 
	{
		int nImage = -1 , nSelectedImage = -1;
		this->GetItemImage(hItem , nImage , nSelectedImage);
		if(0 != nImage)
		{
			VERIFY(menu.AppendMenu(MF_STRING, ID_VIEW_COPYEDIT, _T("CopyEdit")));
			VERIFY(menu.AppendMenu(MF_STRING, ID_VIEW_DELETE, _T("Delete")));
		}
	}

	// maybe the menu is empty...
	if(menu.GetMenuItemCount() > 0)
	{
		menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnTvnEndlabeledit
    @return     void
    @param      NMHDR   *pNMHDR
    @param      LRESULT *pResult
    @brief
******************************************************************************/
void CViewEditTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	SetFocus();
	LPCTSTR lszText = pTVDispInfo->item.pszText;

	/// 이름이 변경되지 않았을때 종료
	if(NULL == lszText) return;

	HTREEITEM hItem = GetSelectedItem();
	if(NULL != hItem)
	{
		CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());
		CaDraw_DraFile* pDraFile = pParent->GetDraFile();

		CString rItemText = GetItemText(hItem);
		CaDraw_View* pDraView = pDraFile->GetView(lszText);
		if(NULL != pDraView)
		{
			CStringTable& table = CStringTable::GetInstance();
			STRING_T str = table.GetString(_T("IDS_SAME_VIEW_EXISTS"));
			AfxMessageBox(str.c_str() , MB_ICONEXCLAMATION);
			return;
		}
		else
		{
			pDraView = (CaDraw_View*)(GetItemData(hItem));
			pDraView->SetName( lszText );
		}

		SetItemText(hItem , lszText);
	}

	*pResult = 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnViewNew
    @return     void
    @brief
******************************************************************************/
void CViewEditTreeCtrl::OnViewNew()
{
	CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());
	CaDraw_DraFile* pDraFile = pParent->GetDraFile();
	CaDraw_View* pDraView = pDraFile->AddView( _T("New") );

	if(NULL != pDraView)
	{
		HTREEITEM hRoot = this->GetRootItem();
		HTREEITEM hChild = this->GetChildItem(hRoot);
		while(hChild)
		{
			this->SetItemImage(hChild , 1 , 1);
			hChild = this->GetNextSiblingItem(hChild);
		}

		HTREEITEM hItem = this->InsertItem(_T("New") , 2 , 2 , hRoot);
		if(NULL != hItem)
		{
			this->SelectItem(hItem);
			this->SetFocus();
		
			pParent->UpdateViewDlgWith(pDraView);
			this->SetItemData(hItem , DWORD(pDraView));

			/// begin edit label
			this->EditLabel(hItem);
		}
	}
	else
	{
		CStringTable& table = CStringTable::GetInstance();
		STRING_T str = table.GetString(_T("IDS_SAME_VIEW_EXISTS"));
		AfxMessageBox(str.c_str() , MB_ICONEXCLAMATION);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnViewCopyEdit
    @return     void
    @brief
******************************************************************************/
void CViewEditTreeCtrl::OnViewCopyEdit()
{
	HTREEITEM hItem = GetSelectedItem();
	if(NULL != hItem)
	{
		CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());
		CaDraw_DraFile* pDraFile = pParent->GetDraFile();
		
		CString rItemText = this->GetItemText(hItem);
		CaDraw_View* pDraView = pDraFile->CopyView( (rItemText + _T("_Copy")).operator LPCTSTR() , rItemText.operator LPCTSTR() );

		if(NULL != pDraView)
		{
			HTREEITEM hRoot = this->GetRootItem();
			HTREEITEM hChild = this->GetChildItem(hRoot);
			while(hChild)
			{
				this->SetItemImage(hChild , 1 , 1);
				hChild = this->GetNextSiblingItem(hChild);
			}

			HTREEITEM hItem = this->InsertItem(rItemText + _T("_Copy") , 2 , 2 , hRoot);
			if(NULL != hItem)
			{
				this->SelectItem(hItem);
				this->SetFocus();

				pParent->UpdateViewDlgWith(pDraView);
				this->SetItemData(hItem , DWORD(pDraView));

				/// begin edit label
				this->EditLabel(hItem);
			}
		}
		else
		{
			CStringTable& table = CStringTable::GetInstance();
			STRING_T str = table.GetString(_T("IDS_SAME_VIEW_EXISTS"));
			AfxMessageBox(str.c_str() , MB_ICONEXCLAMATION);
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnViewDelete
    @return     void
    @brief
******************************************************************************/
void CViewEditTreeCtrl::OnViewDelete()
{
	const int res = AfxMessageBox(_T("Do you want to delete this data?") , MB_YESNO);
	if(IDYES != res) return;
	{
		HTREEITEM hItem = GetSelectedItem();
		if(NULL != hItem)
		{
			CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());
			CaDraw_DraFile* pDraFile = pParent->GetDraFile();

			CString rItemText = GetItemText(hItem);
			if(ERROR_SUCCESS == pDraFile->RemoveView(rItemText.operator LPCTSTR()))
			{
				int nImage = -1 , nSelectedImage = -1;
				this->GetItemImage(hItem , nImage , nSelectedImage);
				
				if(2 == nImage)
				{
					HTREEITEM hNextItem = this->GetNextSiblingItem(hItem);
					if(NULL != hNextItem)
					{
						this->SetItemImage(hNextItem , 2 , 2);
						CaDraw_View* pDraView = (CaDraw_View*)(this->GetItemData(hNextItem));
						pParent->UpdateViewDlgWith( pDraView );
					}
					else if(NULL != (hNextItem = this->GetPrevSiblingItem(hItem)))
					{
						this->SetItemImage(hNextItem , 2 , 2);
						CaDraw_View* pDraView = (CaDraw_View*)(this->GetItemData(hNextItem));
						pParent->UpdateViewDlgWith( pDraView );
					}
				}

				this->DeleteItem(hItem);
			}
		}
	}
}

// CDraFileSettingDlg dialog

IMPLEMENT_DYNAMIC(CDraFileSettingDlg, CDialog)

CDraFileSettingDlg::CDraFileSettingDlg(const CString& rDraFilePath , CSmartDrawDoc* pDoc , const CString& sSettingFileName , CWnd* pParent /*=NULL*/)
	: CDialog(CDraFileSettingDlg::IDD, pParent) , m_rDraFilePath(rDraFilePath) , m_pDoc(pDoc) , m_sSettingFileName(sSettingFileName)
{
	m_pDraFile = NULL;
	m_pViewDlg = NULL;
}

CDraFileSettingDlg::~CDraFileSettingDlg()
{
	try
	{
		SAFE_DELETE(m_pDraFile);
		SAFE_DELETE(m_pViewDlg);
	}
	catch(...)
	{
	}
}

void CDraFileSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_SAVE_VIEW , m_btnSaveView);
	DDX_Control(pDX , IDC_TREE_VIEW , m_wndViewTreeCtrl);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CDraFileSettingDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TAB_VIEW, &CDraFileSettingDlg::OnNMRClickTabView)
	ON_BN_CLICKED(IDOK, &CDraFileSettingDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_VIEW, &CDraFileSettingDlg::OnNMDblclkTreeView)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_VIEW, &CDraFileSettingDlg::OnBnClickedButtonSaveView)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_VIEW, &CDraFileSettingDlg::OnNMRClickTreeView)
	ON_COMMAND(ID_VIEW_NEW, &CDraFileSettingDlg::OnViewNew)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEW, &CDraFileSettingDlg::OnUpdateViewNew)
END_MESSAGE_MAP()


// CDraFileSettingDlg message handlers

BOOL CDraFileSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_DRA),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = SetIcon(hIcon , FALSE);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	if(m_cImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1))
	{
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ROOT));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_VIEW_BLUE));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_VIEW));
	}

	m_pViewDlg = new CDraViewSettingDlg(_T("") , this);
	if(m_pViewDlg->Create(CDraViewSettingDlg::IDD , this))
	{
		CRect rect;
		m_wndViewTreeCtrl.GetWindowRect(&rect);
		this->ScreenToClient(&rect);

		CRect rectButton;
		m_btnSaveView.GetWindowRect(&rectButton);
		this->ScreenToClient(&rectButton);

		m_pViewDlg->SetWindowPos(NULL , rectButton.right + 10 , rect.top - 15 , 0 , 0 , SWP_NOZORDER | SWP_NOSIZE);
	}

	m_wndViewTreeCtrl.SetImageList(&m_cImages , TVSIL_NORMAL);
	m_hRootOfView = m_wndViewTreeCtrl.InsertItem(_T("Views") , m_wndViewTreeCtrl.GetRootItem());

	m_pDraFile = new CaDraw_DraFile(m_rDraFilePath.operator LPCTSTR());
	if(ERROR_SUCCESS == m_pDraFile->Parse())	/// always return ERROR_SUCCESS
	{
		CString sSettingFileName = m_pDraFile->GetSettingFileName();
		sSettingFileName = sSettingFileName.IsEmpty() ? m_sSettingFileName : sSettingFileName;
		m_pDraFile->SetSettingFileName( sSettingFileName );

		list<CaDraw_View*> oViewList;
		m_pDraFile->GetViewList(&oViewList);
		for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
		{
			CString sItemText = (*itr)->name();
			HTREEITEM hItem = m_wndViewTreeCtrl.InsertItem(sItemText , 1 , 1 , m_hRootOfView);
			m_wndViewTreeCtrl.SetItemData(hItem , DWORD(*itr));

			if(itr == oViewList.begin())
			{
				m_wndViewTreeCtrl.SelectItem(hItem);
				m_wndViewTreeCtrl.SetItemImage(hItem , 2 , 2);
				m_pViewDlg->SetView( *itr );
			}
		}

		m_wndViewTreeCtrl.Expand(m_hRootOfView , TVE_EXPAND);
	}

	CRect rect;
	rect.SetRectEmpty();
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
	
	CString rString;
	CSplitPath path(m_rDraFilePath);
	this->GetWindowText(rString);
	rString += _T(" - ") + path.GetFileName();
	this->SetWindowText(rString);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDraFileSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void CDraFileSettingDlg::OnNMRClickTabView(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

/**	
	@brief	call SaveViewInformation function
	@author	HumKyung.BAEK
	@date	2010.05.12
	@param	
	@return
*/
void CDraFileSettingDlg::OnBnClickedOk()
{
	if(NULL != m_pDraFile) m_pDraFile->Save();
	OnOK();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   OnNMDblclkTreeView
    @return     void
    @param      NMHDR   *pNMHDR
    @param      LRESULT *pResult
    @brief
******************************************************************************/
void CDraFileSettingDlg::OnNMDblclkTreeView(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_wndViewTreeCtrl.GetSelectedItem();
	if((NULL != hItem) && (NULL != m_wndViewTreeCtrl.GetParentItem(hItem)))
	{
		CaDraw_View* pDraView = (CaDraw_View*)(m_wndViewTreeCtrl.GetItemData(hItem));
		
		HTREEITEM hRoot = m_wndViewTreeCtrl.GetRootItem();
		HTREEITEM hChild = m_wndViewTreeCtrl.GetChildItem(hRoot);
		while(hChild)
		{
			m_wndViewTreeCtrl.SetItemImage(hChild , 1 , 1);
			hChild = m_wndViewTreeCtrl.GetNextSiblingItem(hChild);
		}

		m_wndViewTreeCtrl.SelectItem(hItem);
		m_wndViewTreeCtrl.SetItemImage(hItem , 2 , 2);
		m_pViewDlg->SetView( pDraView );
	}

	*pResult = 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   OnBnClickedButtonSaveView
    @return     void
    @brief
******************************************************************************/
void CDraFileSettingDlg::OnBnClickedButtonSaveView()
{
	if(NULL != m_pViewDlg) m_pViewDlg->SaveViewInformation(m_rDraFilePath);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnNMRClickTreeView
    @return     void
    @param      NMHDR   *pNMHDR
    @param      LRESULT *pResult
    @brief
******************************************************************************/
void CDraFileSettingDlg::OnNMRClickTreeView(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT point , clientPoint;
	::GetCursorPos(&point);
	clientPoint = point;
	m_wndViewTreeCtrl.ScreenToClient(&clientPoint);

	TVHITTESTINFO htinfo;
	htinfo.pt    = clientPoint;
	htinfo.flags = TVHT_ONITEMLABEL;
	HTREEITEM hItem = m_wndViewTreeCtrl.HitTest(&htinfo);
	if((NULL != hItem))
	{
		int nImage = -1 , nSelectedImage = -1;
		m_wndViewTreeCtrl.GetItemImage(hItem , nImage , nSelectedImage);

		m_wndViewTreeCtrl.SelectItem(hItem);
		m_wndViewTreeCtrl.SetFocus();
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_VIEW , point.x, point.y, this , TRUE);
	}

	*pResult = 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   GetDraFile
    @return     CaDraw_DraFile*
    @brief
******************************************************************************/
CaDraw_DraFile* CDraFileSettingDlg::GetDraFile() const
{
	return m_pDraFile;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   UpdateViewDlgWith
    @return     int
    @param      CaDraw_View*    pDraViw
    @brief
******************************************************************************/
int CDraFileSettingDlg::UpdateViewDlgWith(CaDraw_View* pDraView)
{
	assert(pDraView && "pDraView is NULL");

	if(pDraView)
	{
		m_pViewDlg->SetView(pDraView);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnViewNew
    @return     void
    @brief
******************************************************************************/
void CDraFileSettingDlg::OnViewNew()
{
	HTREEITEM hItem = m_wndViewTreeCtrl.InsertItem(_T("New") , 1 , 1 , m_hRootOfView);
	m_wndViewTreeCtrl.SelectItem(hItem);
	m_wndViewTreeCtrl.SetFocus();

	/// begin edit label
	m_wndViewTreeCtrl.SendMessage(WM_KEYDOWN,VK_F2,0);
}

void CDraFileSettingDlg::OnUpdateViewNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}