// MyTreeListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include <IsString.h>
#include "SmartDraw.h"
#include "DraTreeListCtrl.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "SmartDrawView.h"
#include "NewDraFileDlg.h"
#include "DraFileSettingDlg.h"
#include "SmartDrawPropertySheet.h"
#include "PropertiesWnd.h"
#include "StringTable.h"
#include "EditTitleBlockDataDlg.h"

#include <IsTools.h>
#include <Tokenizer.h>

#include <stack>

// CDraTreeListCtrl

IMPLEMENT_DYNAMIC(CDraTreeListCtrl, CTreeListCtrl)

CDraTreeListCtrl::CDraTreeListCtrl()
{

}

CDraTreeListCtrl::~CDraTreeListCtrl()
{
	try
	{
		for(list<CaDraw_DraFile*>::iterator itr = m_oDraFileList.begin();itr != m_oDraFileList.end();++itr)
		{
			SAFE_DELETE( *itr );
		}
		m_oDraFileList.clear();
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CDraTreeListCtrl, CTreeListCtrl)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CDraTreeListCtrl::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CDraTreeListCtrl::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(NM_CLICK, &CDraTreeListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CDraTreeListCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CDraTreeListCtrl::OnTvnSelchanged)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CDraTreeListCtrl::OnNMRClick)

///	ON_UPDATE_COMMAND_UI(ID_DATA_COPYEDIT , &CDraTreeListCtrl::OnUpdateDataCopyEdit)
	ON_COMMAND(ID_DRA_NEW , &CDraTreeListCtrl::OnDraNew)
	ON_COMMAND(ID_DRA_COPYEDIT, &CDraTreeListCtrl::OnDraCopyedit)
	ON_COMMAND(ID_DRA_DELETE, &CDraTreeListCtrl::OnDraDelete)
	ON_UPDATE_COMMAND_UI(ID_DRA_DELETE, &CDraTreeListCtrl::OnUpdateDraDelete)
	ON_UPDATE_COMMAND_UI(ID_DRA_EDIT, &CDraTreeListCtrl::OnUpdateDraEdit)
	ON_COMMAND(ID_DRA_EDIT, &CDraTreeListCtrl::OnDraEdit)
	ON_UPDATE_COMMAND_UI(ID_DRA_NEW, &CDraTreeListCtrl::OnUpdateDraNew)
	ON_COMMAND(ID_DRA_OPEN, &CDraTreeListCtrl::OnDraOpen)
	ON_UPDATE_COMMAND_UI(ID_DRA_OPEN, &CDraTreeListCtrl::OnUpdateDraOpen)
	ON_COMMAND(ID_DRA_EDIT_TITLE_BLOCK_DATA, &CDraTreeListCtrl::OnDraEditTitleBlockData)
	ON_UPDATE_COMMAND_UI(ID_DRA_EDIT_TITLE_BLOCK_DATA, &CDraTreeListCtrl::OnUpdateDraEditTitleBlockData)
END_MESSAGE_MAP()



// CDraTreeListCtrl message handlers
/**
	@brief	
	@author HumKyung.BAEK	
	@date 2010-04-18
	@param	
	@return		
*/
void CDraTreeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CTreeListCtrl::OnLButtonDblClk(nFlags, point);
}

/**
	@brief	
	@author HumKyung.BAEK
	@date 2010-04-19
	@param	
	@return		
**/
void CDraTreeListCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	if(pTVDispInfo)
	{
		if(pTVDispInfo->item.hItem && GetParentItem(pTVDispInfo->item.hItem))
		{
			m_rBeginLabelEditText = pTVDispInfo->item.pszText;
		}
		else
		{
			*pResult = 1;
			return;
		}
	}

	*pResult = 0;
}


/**
	@brief	TREELISTCTRL END LABEL EDIT
	@author HumKyung.BAEK	
	@date 2010-04-19
	@param	
	@return		
**/
void CDraTreeListCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawView* pView = (CSmartDrawView*)GetParent();
	CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();

	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	if(pTVDispInfo && GetParentItem(pTVDispInfo->item.hItem)/* && GetParentItem(GetParentItem(pTVDispInfo->item.hItem))*/)
	{
		const CString rItemText = GetItemText(pTVDispInfo->item.hItem , 0);
		const CString rValue = pTVDispInfo->item.pszText;

		/// return if no change
		if(rItemText == rValue) return;

		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(pTVDispInfo->item.hItem , nImage , nSelectedImage);
		if(ICON_VIEW == nImage)
		{
			HTREEITEM hParent = GetParentItem(pTVDispInfo->item.hItem);
			const CString rDraName = GetItemText(hParent , 0);
			const CString rViewName= GetItemText(pTVDispInfo->item.hItem);
			
			const CString rDraFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T(".dra");

			map<int,ColumnData>::iterator where = m_ColumnDataMap.find(pTVDispInfo->item.cChildren);
			if(where != m_ColumnDataMap.end())
			{
				if(0 == pTVDispInfo->item.cChildren)
				{
					CaDraw_DraFile draFile(rDraFilePath.operator LPCTSTR());
					if(ERROR_SUCCESS != draFile.ChangeViewName(rValue.operator LPCTSTR() , rViewName.operator LPCTSTR()))
					{
						AfxMessageBox(_T("Fail to change view name."));
						*pResult = 1;
						return;
					}
				}
			}
		}
		else if((ICON_SETTING == nImage) && (0 == pTVDispInfo->item.cChildren))
		{
			const CString sSettingFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rItemText + _T(".ini");
			const CString sNewSettingFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rValue + _T(".ini");
			if(TRUE != FileExist(sNewSettingFilePath))
			{
				::MoveFile(sSettingFilePath , sNewSettingFilePath);

				map<CString , HTREEITEM>::iterator where = m_OptionTreeItemMap.find(CString(rItemText).MakeUpper());
				if(where != m_OptionTreeItemMap.end()) m_OptionTreeItemMap.erase(where);
				m_OptionTreeItemMap.insert(make_pair(CString(rValue).MakeUpper() , pTVDispInfo->item.hItem));

				/// change sdr's setting name - 2012.04.24 added by humkyung
				HTREEITEM hChildItem = this->GetChildItem(pTVDispInfo->item.hItem);
				while(hChildItem)
				{
					CaDraw_DraFile* pSdrFile = (CaDraw_DraFile*)(this->GetItemData(hChildItem));
					if(NULL != pSdrFile)
					{
						pSdrFile->SetSettingFileName(rValue);
						pSdrFile->Save();
					}

					hChildItem = this->GetNextSiblingItem(hChildItem);
				}
				/// up to here
			}
			else
			{
				AfxMessageBox(_T("There is same setting file.") , MB_OK | MB_ICONEXCLAMATION);
				*pResult = 1;
				return;
			}
		}
		else if((ICON_DRA == nImage) && (0 == pTVDispInfo->item.cChildren))
		{
			const CString rDraName = GetItemText(pTVDispInfo->item.hItem , 0);
			const CString sSdrFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T(".dra");
			const CString sNewSdrFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rValue + _T(".dra");
			if(TRUE != FileExist(sNewSdrFilePath))
			{
				::MoveFile(sSdrFilePath , sNewSdrFilePath);
			}
			else
			{
				AfxMessageBox(_T("There is same sdr file.") , MB_OK | MB_ICONEXCLAMATION);
				*pResult = 1;
				return;
			}
		}
	}

	*pResult = 0;
}

/**
	@brief	
	@author humkyung	
	@date 2009-11-02 오전 10:23:55	
	@param	
	@return		
**/
void CDraTreeListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTREEVIEW* pNMTV = (NMTREEVIEW*)pNMHDR;

	CPoint ptTree= pNMTV->ptDrag;

	TVHITTESTINFO htinfo;
	htinfo.pt    = ptTree;
	htinfo.flags = TVHT_ONITEMICON;
	HTREEITEM hItem = HitTest (&htinfo);
	if(hItem)
	{
		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		if(ICON_VIEW == nImage)
		{
			HTREEITEM hParentItem = GetParentItem(hItem);
			SetCheck(hItem , (BST_UNCHECKED == GetCheck(hParentItem)) ? FALSE : TRUE);
		}
		else
		{
			/// 체크가 안되있으면 모두 체크
			if(BST_UNCHECKED == GetCheck(hItem))
			{
				FindTreeItemAndCheck(hItem, FALSE);
			}
			else /// 체크 되있으면 모두 언체크
			{
				FindTreeItemAndCheck(hItem, TRUE);
			}
		}

		*pResult = 0;
	}
	else
	{
		*pResult = 0;
	}
}

/**
	@brief	
	@author HumKyung.BAEK	
	@date 2009-11-03 오전 10:48:45	
	@param	
	@return		
**/
void CDraTreeListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTREEVIEW* pNMTV = (NMTREEVIEW*)pNMHDR;

	CPoint ptTree= pNMTV->ptDrag;

	TVHITTESTINFO htinfo;
	htinfo.pt    = ptTree;
	htinfo.flags = TVHT_ONITEMICON;
	HTREEITEM hItem = HitTest (&htinfo);
	if(hItem)
	{
		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		if(ICON_VIEW == nImage)
		{
			HTREEITEM hParentItem = GetParentItem(hItem);
			SetCheck(hItem , (BST_UNCHECKED == GetCheck(hParentItem)) ? FALSE : TRUE);
		}
		else
		{
			//! 체크가 안되있으면 모두 체크
			if(BST_UNCHECKED == GetCheck(hItem))
			{
				FindTreeItemAndCheck(hItem, FALSE);
			}
			else //! 체크 되있으면 모두 언체크
			{
				FindTreeItemAndCheck(hItem, TRUE);
			}
		}

		*pResult = 1;
	}
	else
	{
		*pResult = 0;
	}
}

/**
	@brief	
	@author KHS	
	@date 2009-11-03 오전 10:32:49	
	@param	
	@return		
**/
int CDraTreeListCtrl::FindTreeItemAndCheck(HTREEITEM hItem, BOOL bCheck)
{
	if(hItem)
	{
		//! Revision 도면은 체크 하지 않는다.
		///if(NULL == GetParentItem(GetParentItem(GetParentItem(hItem))))
		{
			SetCheck(hItem, bCheck);
		}
	
		HTREEITEM hChild = GetChildItem(hItem);
		do
		{
			if(1 == FindTreeItemAndCheck(hChild, bCheck))
			{
				return 1;
			}

		}while((hChild = GetNextSiblingItem(hChild)) != NULL);
	}
	return 0;
}

/**
	@brief	
	@author HumKyung.BAEK
	@date 2010-04-18
	@param	
	@return		
**/
void CDraTreeListCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	int iImage = -1 , iSelectedImage = -1;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	GetItemImage(hItem , iImage , iSelectedImage);
	if((ICON_DRA == iImage) || (ICON_VIEW == iImage))
	{
		CaDraw_DraFile* pDraFile = NULL;
		if(ICON_DRA == iImage)
		{
			pDraFile = (CaDraw_DraFile*)(GetItemData(hItem));
		}
		else if(ICON_VIEW == iImage)
		{
			HTREEITEM hParent = GetParentItem(hItem);
			pDraFile = (CaDraw_DraFile*)(GetItemData(hParent));
		}

		CPropertiesWnd* pPropertiesWnd = CPropertiesWnd::GetInstance();
		if(NULL != pPropertiesWnd)
		{
			pPropertiesWnd->DisplayContentOf( pDraFile );
		}
	}
	else
	{
		CPropertiesWnd* pPropertiesWnd = CPropertiesWnd::GetInstance();
		if(NULL != pPropertiesWnd)
		{
			pPropertiesWnd->DisplayContentOf( NULL );
		}
	}

	*pResult = 0;
}

/**
	@brief	get checked sibling items of given item
	@author	humkyung
	@date	2017.07.07
*/
int CDraTreeListCtrl::GetCheckedSiblingItems(list<HTREEITEM>& oCheckedItems, HTREEITEM hItem)
{
	assert(hItem && "hItem is NULL");

	if(NULL != hItem)
	{
		while(hItem)
		{
			if(this->GetCheck(hItem)) oCheckedItems.push_back(hItem);
			hItem = this->GetNextSiblingItem(hItem);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	delete selected item from dra and treelistctrl
	@author HumKyung.BAEK
	@date 2010-05-11
	@param	
	@return		
**/
int CDraTreeListCtrl::OnDeleteItem()
{
	HTREEITEM hItem = GetFirstSelected();
	if(hItem && (NULL != GetParentItem(hItem)))
	{
		const int res = AfxMessageBox(_TR("Do you want to delete this data?") , MB_YESNO);
		if(IDYES != res) return ERROR_BAD_ENVIRONMENT;

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawView* pView = (CSmartDrawView*)GetParent();
		CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();

		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);
		HTREEITEM hParent = GetParentItem(hItem);

		if(ICON_SETTING == nImage)
		{
			map<CString,HTREEITEM>::iterator where = m_OptionTreeItemMap.find(CString(_T("Default")).MakeUpper());
			if(where != m_OptionTreeItemMap.end())
			{
				/// delete setting file
				const CString rSettingName = GetItemText(hItem , 0);
				if(CString(_T("Default")).MakeUpper() != CString(rSettingName).MakeUpper())
				{
					const CString rSettingFilePath    = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rSettingName + _T(".ini");
					::DeleteFile(rSettingFilePath);

					CopyTreeItem(where->second , hItem);
					
					CString sSettingName = this->GetItemText(hItem);
					where = m_OptionTreeItemMap.find(sSettingName.MakeUpper());
					if(where != m_OptionTreeItemMap.end()) m_OptionTreeItemMap.erase(where);
					DeleteItem(hItem);

					return ERROR_SUCCESS;
				}else	AfxMessageBox(_T("Can't delete default setting file."));
			}else	AfxMessageBox(_T("Can't find defalut setting file."));

			return ERROR_BAD_ENVIRONMENT;
		}
		else if(ICON_DRA == nImage)
		{
			list<HTREEITEM> oCheckedSiblingItems;
			this->GetCheckedSiblingItems(oCheckedSiblingItems, this->GetChildItem(hParent));
			for(list<HTREEITEM>::const_iterator itr = oCheckedSiblingItems.begin();itr != oCheckedSiblingItems.end();++itr)
			{
				/// delete dra file
				const CString rDraName = GetItemText(*itr, 0);
				const CString rDraFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T(".dra");

				::DeleteFile(rDraFilePath);
				DeleteItem(*itr);
			}
			this->SelectItem(hParent);
		}
		else if(ICON_VIEW == nImage)
		{
			list<HTREEITEM> oCheckedSiblingItems;
			this->GetCheckedSiblingItems(oCheckedSiblingItems, this->GetChildItem(hParent));

			for(list<HTREEITEM>::const_iterator itr = oCheckedSiblingItems.begin();itr != oCheckedSiblingItems.end();++itr)
			{
				/// remove view from dra file
				const CString sViewName = GetItemText(*itr, 0);

				CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)(GetItemData(hParent));
				if(NULL != pDraFile)
				{
					pDraFile->RemoveView(sViewName.operator LPCTSTR());
					pDraFile->Save();

					DeleteItem(*itr);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	delete selected item from dra and treelistctrl
	@author HumKyung.BAEK
	@date 2010-05-11
	@param	
	@return		
**/
void CDraTreeListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(VK_DELETE == nChar)
	{
		OnDeleteItem();
		return;
	}
	else if(VK_F2 == nChar)
	{
		HTREEITEM hItem = GetFirstSelected();
		if(hItem)
		{
			int nImage = -1 , nSelectedImage = -1;
			this->GetItemImage(hItem , nImage , nSelectedImage);
			if(((ICON_SETTING == nImage) || (ICON_DRA == nImage)) && (0 == this->GetFocusColumn()))
			{
				if((ICON_SETTING == nImage) && (_T("DEFAULT") == CString(this->GetItemText(hItem)).MakeUpper())) return;

				this->EditLabel(hItem);
				return;
			}
		}
	}

	CTreeListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/**
	@brief	show pop-up menu
	@author HumKyung.BAEK
	@date 2010-05-11
	@param	
	@return		
**/
void CDraTreeListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT point , clientPoint;
	::GetCursorPos(&point);
	clientPoint = point;
	ScreenToClient(&clientPoint);

	TVHITTESTINFO htinfo;
	htinfo.pt    = clientPoint;
	htinfo.flags = TVHT_ONITEMLABEL;
	HTREEITEM hItem = HitTest(&htinfo);
	if((NULL != hItem)/* && GetParentItem(hItem)*/)
	{
		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		SelectItem(hItem);
		SetFocus();
		if(ICON_SETTING == nImage)
		{
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_SETTING, point.x, point.y, this, TRUE);
		}
		else
		{
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_DRA, point.x, point.y, this, TRUE);
		}
	}

	*pResult = 0;
}

/**
	@brief	display dra files which are in dra folder.
	@author HumKyung.BAEK
	@date 2010-05-01
	@param	
	@return		
**/
int CDraTreeListCtrl::FillDraView(CSmartDrawDoc* pDoc , const CString& rSettingFolderPath , const CString& rDraFolderPath)
{
	assert(pDoc && "pDoc is NULL");
	if(pDoc)
	{
		SetRedraw(FALSE);

		/// clear already loaded datas
		HTREEITEM hRoot = GetRootItem() , hParent;
		HTREEITEM hChild= GetChildItem(hRoot);
		while(hChild)
		{
			HTREEITEM hNext = GetNextSiblingItem(hChild);
			DeleteItem(hChild);
			hChild = hNext;
		}
		for(list<CaDraw_DraFile*>::iterator itr = m_oDraFileList.begin();itr != m_oDraFileList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		/// up to here

		/// get setting files
		m_OptionTreeItemMap.clear();
		{
			CFileFind finder;
			BOOL bWorking = finder.FindFile(rSettingFolderPath + _T("\\*.ini"));
			while (bWorking)
			{
				bWorking = finder.FindNextFile();

				CString rFileName(finder.GetFileName());
				const int at = rFileName.ReverseFind('.');
				CString rFileExt = rFileName.Right(rFileName.GetLength() - at - 1);
				rFileName = rFileName.Left(at);
				if(rFileExt.MakeUpper() == _T("INI"))
				{
					hParent = InsertItem(rFileName , ICON_SETTING  , ICON_SETTING , hRoot , TVI_LAST);
					m_OptionTreeItemMap.insert(make_pair(rFileName.MakeUpper() , hParent));
				}
			}
		}
		/// up to here

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;

		TCHAR szBuf[MAX_PATH + 1] = {'\0',};
		CFileFind finder;
		BOOL bWorking = finder.FindFile(rDraFolderPath + _T("\\*.dra"));
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			CString rFileName(finder.GetFileName());
			const int at = rFileName.ReverseFind('.');
			CString rFileExt = rFileName.Right(rFileName.GetLength() - at - 1);
			if(rFileExt.MakeUpper() == _T("DRA"))
			{
				const CString sFilePath = finder.GetFilePath();
				CaDraw_DraFile* pDraFile = new CaDraw_DraFile(sFilePath.operator LPCTSTR());
				if(ERROR_SUCCESS == pDraFile->Parse())
				{
					m_oDraFileList.push_back(pDraFile);

					CString sSettingFileName = pDraFile->GetSettingFileName();
					map<CString,HTREEITEM>::iterator where = m_OptionTreeItemMap.find(sSettingFileName.MakeUpper());
					if(where == m_OptionTreeItemMap.end())
					{
						///rSetting = _T("Default");
						map<CString,HTREEITEM>::iterator where = m_OptionTreeItemMap.find(CString(_T("Default")).MakeUpper());
						if(m_OptionTreeItemMap.end() == where)
						{
							hParent = InsertItem(_T("Default") , ICON_SETTING , ICON_SETTING , hRoot , TVI_LAST);
							m_OptionTreeItemMap.insert(make_pair(CString(_T("Default")).MakeUpper() , hParent));
						}else	hParent = where->second;
					}else	hParent = where->second;
				}

				HTREEITEM hItem = InsertDraItem(pDraFile , ICON_DRA , ICON_DRA , hParent , TVI_LAST);
				/// set overlay icon if output file exists - 2015.04.25 added by humkyung
				if(NULL != hItem)
				{
					const CString sDraName = this->GetItemText(hItem , 0);
					const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sDraName + pDoc->GetDefaultExt();
					if(_T("") != STRING_T(pDraFile->status()))
					{
						SetItemState(hItem , INDEXTOOVERLAYMASK(2) , TVIS_OVERLAYMASK);
					}
					else
					{
						SetItemState(hItem , (FileExist(sOutputFilePath)) ? INDEXTOOVERLAYMASK(1) : INDEXTOOVERLAYMASK(0) , TVIS_OVERLAYMASK);
					}
				}
				/// up to here
			}
		}
		Expand(hRoot , TVE_EXPAND);
		for(map<CString,HTREEITEM>::iterator itr = m_OptionTreeItemMap.begin();itr != m_OptionTreeItemMap.end();++itr)
		{
			Expand(itr->second , TVE_EXPAND);
		}

		SetBackgroundItemColor(GetRootItem());

		SetRedraw(TRUE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	update overlay icons indicating if output file exists
	@author humkyung
	@date	2015-04-25
	@param	
	@return	int
**/
int CDraTreeListCtrl::UpdateOverlayIcons(CSmartDrawDoc* pDoc)
{
	assert(pDoc && "pDoc is NULL");
	if(NULL != pDoc)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawView* pView = (CSmartDrawView*)GetParent();
		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;
		
		HTREEITEM hRoot = GetRootItem();
		HTREEITEM hSetting= GetChildItem(hRoot);
		while(hSetting)
		{
			HTREEITEM hNext = GetNextSiblingItem(hSetting);
			HTREEITEM hDra = GetChildItem(hSetting);
			while(hDra)
			{
				HTREEITEM hNextDra = GetNextSiblingItem(hDra);
				/// update overlay icon depend on output file - 2015.04.25 added by humkyung
				if(NULL != hDra)
				{
					const CString sDraName = this->GetItemText(hDra , 0);
					CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)this->GetItemData(hDra);
					if(NULL != pDraFile)
					{
						LPCTSTR lpStatus = pDraFile->status();
						if(_T("") != STRING_T(lpStatus))
						{
							SetItemState(hDra , INDEXTOOVERLAYMASK(2) , TVIS_OVERLAYMASK);
						}
						else
						{
							const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sDraName + pDoc->GetDefaultExt();
							SetItemState(hDra , (FileExist(sOutputFilePath) ? INDEXTOOVERLAYMASK(1) : INDEXTOOVERLAYMASK(0)) , TVIS_OVERLAYMASK);
						}
					}
				}
				/// up to here
				hDra = hNextDra;
			}
		hSetting = hNext;
		}
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	insert a tree item for dra file
	@author humkyung
	@date 2014-06-16
	@param	
	@return	HTREEITEM
**/
HTREEITEM CDraTreeListCtrl::InsertDraItem(CaDraw_DraFile* pDraFile , const int& iImage , const int& iSelectedImage , const HTREEITEM& hParent , const HTREEITEM& at)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		HTREEITEM hItem = InsertItem(pDraFile->GetFileNameWithoutExt().c_str() , iImage , iSelectedImage, hParent , at);
		if(NULL != hItem)
		{
			this->SetItemData(hItem , DWORD(pDraFile));
			/// fill status of drawing file - 2014.06.06 added by humkyung
			LPCTSTR lpStatus = pDraFile->status();
			if(_T("") != STRING_T(lpStatus))
			{
				CString sStatus;

				CStringTable& table = CStringTable::GetInstance();
				vector<STRING_T> oResult;
				CTokenizer<CIsComma>::Tokenize(oResult , lpStatus , CIsComma());
				for(vector<STRING_T>::const_iterator itr = oResult.begin();itr != oResult.end();++itr)
				{
					if(itr->empty()) continue;
					if(!sStatus.IsEmpty()) sStatus += _T(",");
					sStatus += table.GetString(*itr).c_str();
				}
				SetItemText(hItem , sStatus , 13);
				SetItemTextColor(hItem , 13 , RGB(255,0,0));

				SetItemState(hItem , INDEXTOOVERLAYMASK(2) , TVIS_OVERLAYMASK);
			}
			/// up to here
			this->FillViewInfo(pDraFile , hItem);
			SetBackgroundItemColor(hItem);
		}
		
		return hItem;
	}

	return NULL;
}

/**
	@brief	insert a item for view
	@author humkyung
	@date	2011-11-11
	@param	
	@return		
**/
HTREEITEM CDraTreeListCtrl::InsertViewItem(CaDraw_View* pDraView , const HTREEITEM& hParent)
{
	assert(pDraView && "pDraView is NULL");
	HTREEITEM hItem = NULL;
	if(NULL != pDraView)
	{
		hItem = InsertItem(pDraView->name() , ICON_VIEW , ICON_VIEW , hParent , TVI_LAST);
		if(NULL != hItem)
		{
			this->SetItemData(hItem , DWORD(pDraView));

			STRING_T sValue = pDraView->GetViewDirString();
			SetItemText(hItem , sValue.c_str() , 1);

			CIsVolume volume = pDraView->GetVolume();
			OSTRINGSTREAM_T oss;
			oss.setf(ios_base::fixed, ios_base::floatfield);
			oss << IsString::TrimedTrailingZero(volume.minx()) << _T(",") << IsString::TrimedTrailingZero(volume.miny()) << _T(",") << IsString::TrimedTrailingZero(volume.minz()) << _T(",");
			oss << IsString::TrimedTrailingZero(volume.maxx()) << _T(",") << IsString::TrimedTrailingZero(volume.maxy()) << _T(",") << IsString::TrimedTrailingZero(volume.maxz());
			SetItemText(hItem , oss.str().c_str() , 2);

			CIsPoint3d origin = pDraView->origin();
			oss.str(_T(""));
			oss << IsString::TrimedTrailingZero(origin.x()) << _T(",") << IsString::TrimedTrailingZero(origin.y());
			SetItemText(hItem , oss.str().c_str() , 3);

			const double dScale = pDraView->GetScale();
			oss.str(_T(""));
			oss << _T("1:") << IsString::TrimedTrailingZero(dScale);
			SetItemText(hItem , oss.str().c_str() , 4);

			sValue = pDraView->GetViewAngleString();
			SetItemText(hItem , sValue.c_str() , 5);

			/// fill match line data
			sValue = pDraView->GetMatchLineData(CaDraw_View::UP_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 6);

			sValue = pDraView->GetMatchLineData(CaDraw_View::DOWN_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 7);

			sValue = pDraView->GetMatchLineData(CaDraw_View::EAST_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 8);

			sValue = pDraView->GetMatchLineData(CaDraw_View::WEST_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 9);

			sValue = pDraView->GetMatchLineData(CaDraw_View::NORTH_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 10);

			sValue = pDraView->GetMatchLineData(CaDraw_View::SOUTH_SIDE_T);
			SetItemText(hItem , sValue.c_str() , 11);
			/// up to here

			sValue = pDraView->GetViewInfo();
			SetItemText(hItem , sValue.c_str() , 12);
		}
	}

	return hItem;
}

/**
	@brief	fill view information of a drawing file.
	@author HumKyung.BAEK
	@date 2010-05-10
	@param	
	@return		
**/
int CDraTreeListCtrl::FillViewInfo(CaDraw_DraFile* pDraFile , const HTREEITEM& hParent)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		TCHAR szBuf[MAX_PATH + 1] = {'\0',};

		list<CaDraw_View*> oViewList;
		pDraFile->GetViewList(&oViewList);
		for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
		{
			InsertViewItem(*itr , hParent);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   OnDraNew
    @return     void
    @brief
******************************************************************************/
void CDraTreeListCtrl::OnDraNew()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if((NULL != hItem))
	{
		int nImage = -1 , nSelectedImage = -1;
		this->GetItemImage(hItem , nImage , nSelectedImage);
		if(ICON_SETTING == nImage)
		{
			CNewDraFileDlg dlg;
			if(IDOK == dlg.DoModal())
			{
				CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

				CSmartDrawView* pView = (CSmartDrawView*)GetParent();
				CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();

				CString sSdrName = dlg.m_sSdrName;
				CString sSdrFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->GetProjectName() + _T("\\Dra\\") + sSdrName + _T(".dra");
				if(TRUE == FileExist(sSdrFilePath))
				{
					AfxMessageBox(_T("There is same dra file. please use another name") , MB_OK|MB_ICONEXCLAMATION);
					return;
				}

				const CString sSettingFileName = this->GetItemText(hItem);
				CDraFileSettingDlg oSettingDlg(sSdrFilePath , pDoc , sSettingFileName);
				if(IDOK == oSettingDlg.DoModal())
				{
					CaDraw_DraFile* pDraFile = new CaDraw_DraFile(sSdrFilePath.operator LPCTSTR());
					if(pDraFile)
					{
						pDraFile->Parse();
						map<CString , HTREEITEM>::iterator where = m_OptionTreeItemMap.find(CString(pDraFile->GetSettingFileName()).MakeUpper());
						if(where != m_OptionTreeItemMap.end()) hItem = where->second;
						
						HTREEITEM hDraItem = InsertDraItem(pDraFile , ICON_DRA , ICON_DRA , hItem , TVI_LAST);
						if(NULL != hDraItem)
						{
							m_oDraFileList.push_back( pDraFile );
						}
					}
				}
			}
		}
	}
}

/**
	@brief	copy dra or view and then edit.
	@author HumKyung.BAEK
	@date 2010-05-11
	@param	
	@return		
**/
void CDraTreeListCtrl::OnDraCopyedit()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	HTREEITEM hItem = GetFirstSelected();
	if(hItem)
	{
		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		HTREEITEM hParent = GetParentItem(hItem);

		CSmartDrawView* pView = (CSmartDrawView*)GetParent();
		CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();
		
		if(ICON_SETTING == nImage)
		{
			const CString rSettingName        = GetItemText(hItem , 0);
			const CString rSettingFilePath    = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rSettingName + _T(".ini");
			const CString rNewSettingFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rSettingName + _T("_Copy.ini");

			/// copy setting file
			if(TRUE == CopyFile(rSettingFilePath , rNewSettingFilePath , TRUE))
			{
				HTREEITEM hNewItem = InsertItem(rSettingName + _T("_Copy") , ICON_SETTING , ICON_SETTING , hParent , hItem);
				if(NULL != hNewItem)
				{
					m_OptionTreeItemMap.insert(make_pair(rSettingName + _T("_Copy") , hNewItem));

					SetBackgroundItemColor(hNewItem);

					CTreeListCtrl::SelectItem(hNewItem);
					SetFocus();

					this->EditLabel(hNewItem);
				}
			}
		}
		else if(ICON_DRA == nImage)
		{
			const CString rDraName = GetItemText(hItem , 0);
			const CString rDraFilePath    = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T(".dra");
			const CString rNewDraFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T("_Copy.dra");

			/// copy dra file
			if(TRUE == CopyFile(rDraFilePath , rNewDraFilePath , TRUE))
			{
				CaDraw_DraFile* pDraFile = new CaDraw_DraFile(rNewDraFilePath.operator LPCTSTR());
				if(ERROR_SUCCESS == pDraFile->Parse())
				{
					m_oDraFileList.push_back(pDraFile);

					HTREEITEM hNewItem = InsertDraItem(pDraFile , nImage , nSelectedImage , GetParentItem(hItem) , hItem);
					if(NULL != hNewItem)
					{
						SetFocus();
						this->EditLabel(hNewItem);
					}
				}
			}
			else AfxMessageBox(_T("Same dra file exists.") , MB_ICONEXCLAMATION);
		}
		else if(ICON_VIEW == nImage)
		{
			HTREEITEM hParentItem = GetParentItem(hItem);
			if(NULL != hParentItem)
			{
				CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)(this->GetItemData(hParentItem));
				
				const CString sViewName = GetItemText(hItem , 0);
				CaDraw_View* pDraView = pDraFile->CopyView(CString(sViewName + _T("_Copy")).operator LPCTSTR() , sViewName.operator LPCTSTR());
				if(NULL != pDraView)
				{
					HTREEITEM hItem = InsertViewItem(pDraView , hParentItem);
					SetBackgroundItemColor(hItem);

					pDraFile->Save();
				}
				else
				{
					AfxMessageBox(_T("There is same view!!!") , MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}
}

/**
	@brief	set item's background color
	@author	HumKyung.BAEK
	@date	2010.05.11
*/
int CDraTreeListCtrl::SetBackgroundItemColor(HTREEITEM hItem)
{
	const int nColumnCount = GetColumnCount();

	while(hItem)
	{
		for(int j = 0;j < nColumnCount;++j)
		{
			if((j >= 0) && (j <= 5))
			{
				SetItemBkColor(hItem , j , RGB(0xCC,0xFF,0xFF));
			}
			else
			{
				SetItemBkColor(hItem , j , RGB(0xFF,0xCC,0x99));
			}
		}
		HTREEITEM hChild = GetChildItem(hItem);
		if(hChild) SetBackgroundItemColor(hChild);

		hItem = GetNextSiblingItem(hItem);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	call OnDeleteItem()

	@author	HumKyung.BAEK

	@date	2010.05.11
*/
void CDraTreeListCtrl::OnDraDelete()
{
	OnDeleteItem();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.11
*/
void CDraTreeListCtrl::OnUpdateDraDelete(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = GetFirstSelected();
	if(hItem && (NULL != GetParentItem(hItem)))
	{
		pCmdUI->Enable(TRUE);
	}else	pCmdUI->Enable(FALSE);
}

/**
	@brief	
	@author HumKyung.BAEK
	@date 2010-05-11
	@param	
	@return		
**/
HTREEITEM CDraTreeListCtrl::CopyTreeItem(HTREEITEM hDest , HTREEITEM hParent/* , const bool& bIncludeParent*/)
{
	assert(hDest && "hDest is NULL");
	assert(hParent && "hParent is NULL");
	HTREEITEM hNewItem = NULL;

	if(hDest && hParent)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawView* pView = (CSmartDrawView*)GetParent();
		CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();

		const CString sNewSettingName = this->GetItemText(hDest);
		HTREEITEM hItem = GetChildItem(hParent);
		while(hItem)
		{
			CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)this->GetItemData(hItem);
			pDraFile->SetSettingFileName(sNewSettingName);
			pDraFile->Save();

			HTREEITEM hDraItem = InsertDraItem(pDraFile , ICON_DRA , ICON_DRA , hDest , TVI_LAST);
			hItem = GetNextSiblingItem(hItem);
		}
	}

	return hNewItem;
}

/**
	@brief	can drop only dra item
	@author humkyung
	@date 2015-07-14
	@param	
	@return		
**/
bool CDraTreeListCtrl::CanDragItem(TVITEM & item)
{
	int nImage = -1, nSelImage = -1;
	GetItemImage(item.hItem , nImage , nSelImage);
	return (ICON_DRA == nImage);
}

/**
	@brief	drop is possible only on setting item
	@author humkyung
	@date 2015-07-14
	@param	
	@return		
**/
bool CDraTreeListCtrl::CanDropItem(HTREEITEM hDrag, HTREEITEM hDrop)
{
	int nImage = -1, nSelImage = -1;
	GetItemImage(hDrop , nImage , nSelImage);
	return (ICON_SETTING == nImage);
}

/**
	@brief	move item after drag end
	@author humkyung
	@date 2015-07-14
	@param	
	@return		
**/
void CDraTreeListCtrl::DragMoveItem(HTREEITEM hDrag, HTREEITEM hDrop, bool bCopy)
{
	if(IsAncestor(hDrag, hDrop) || !CanDropItem(hDrag, hDrop)) return;

	CString sSettingFileName = GetItemText(hDrop);
	CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)this->GetItemData(hDrag);
	if(NULL != hDrag)
	{
		pDraFile->SetSettingFileName(sSettingFileName);
		pDraFile->Save();
	}

	/// Setup insertion parameters
	HTREEITEM hInsertAfter = TVI_LAST;
	HTREEITEM hParent = hDrop;

	HTREEITEM hNew = CTreeListCtrl::CopyItem(hDrag, hParent, hInsertAfter);
	SetBackgroundItemColor(hNew);
	SelectItem(hNew);

	// If the control-key ist down, we copy the data, otherwise we move
	// it, thus we have to delete the dragged item.
	if(!bCopy)
	{
		std::stack<HTREEITEM> oItemStack;

		CTreeListCtrlItemData *pItemData = (CTreeListCtrlItemData *)CTreeCtrl::GetItemData(hDrag);
		if(NULL != pItemData) delete pItemData;
		if(ItemHasChildren(hDrag)) oItemStack.push(hDrag);

		while(oItemStack.size() > 0)
		{
			HTREEITEM hItem = oItemStack.top();
			oItemStack.pop();

			for(HTREEITEM hChild = GetChildItem(hItem); hChild != 0; hChild = GetNextSiblingItem(hChild))
			{
				pItemData = (CTreeListCtrlItemData *)CTreeCtrl::GetItemData(hChild);
				if(NULL != pItemData) delete pItemData;
				if(ItemHasChildren(hChild)) oItemStack.push(hChild);
			}
		}
		
		DeleteItem(hDrag);
	}
}

void CDraTreeListCtrl::OnUpdateDraEdit(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = GetFirstSelected();
	if(hItem)
	{
		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		((ICON_SETTING == nImage) || (ICON_DRA == nImage)) ? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
	}else	pCmdUI->Enable(FALSE);
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-10
    @function   OnDraEdit
    @return     void
    @brief
******************************************************************************/
void CDraTreeListCtrl::OnDraEdit()
{
	HTREEITEM hItem = GetFirstSelected();
	if(hItem)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

		int nImage = -1 , nSelectedImage = -1;
		GetItemImage(hItem , nImage , nSelectedImage);

		if((ICON_SETTING == nImage))
		{
			CSmartDrawView* pView = (CSmartDrawView*)GetParent();
			CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();
			const CString rSettingName = GetItemText(hItem , 0);
			const CString rSettingFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Setting\\") + rSettingName + _T(".ini");

			const STRING_T& sDrawingType = pDoc->GetDrawingType();
			CSmartDrawPropertySheet dlg(0 , pDoc->m_rProjectName , rSettingFilePath , sDrawingType , AfxGetMainWnd());
			if(IDOK == dlg.DoModal())
			{
			}
		}
		else if((ICON_DRA == nImage))
		{
			CSmartDrawView* pView = (CSmartDrawView*)GetParent();
			CSmartDrawDoc* pDoc = (CSmartDrawDoc*)pView->GetDocument();
			const CString rDraName = GetItemText(hItem , 0);
			const CString rDraFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->m_rProjectName + _T("\\Dra\\") + rDraName + _T(".dra");
			const CString sSettingFileName = this->GetItemText(GetParentItem(hItem));

			CDraFileSettingDlg dlg(rDraFilePath , pDoc , sSettingFileName);
			if(IDOK == dlg.DoModal())
			{
				CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)this->GetItemData(hItem);
				pDraFile->Parse();
				map<CString , HTREEITEM>::iterator where = m_OptionTreeItemMap.find(CString(pDraFile->GetSettingFileName()).MakeUpper());
				if(where != m_OptionTreeItemMap.end())
				{
					/// delete all view itmes of selected drawing - 2014.06.03 added by humkyung
					vector<HTREEITEM> oChildItems;
					HTREEITEM hChild = this->GetChildItem(hItem);
					while(NULL != hChild)
					{
						oChildItems.push_back(hChild);
						hChild = this->GetNextSiblingItem(hChild);
					}
					for(vector<HTREEITEM>::iterator itr = oChildItems.begin();itr != oChildItems.end();++itr)
					{
						this->DeleteItem(*itr);
					}
					/// up to here
					/// fill status of drawing file - 2014.06.06 added by humkyung
					LPCTSTR lpStatus = pDraFile->status();
					SetItemText(hItem , (NULL != lpStatus) ? lpStatus : _T("") , 13);
					/// up to here

					this->FillViewInfo(pDraFile , hItem);
					SetBackgroundItemColor(hItem);
					this->SelectItem(hItem);
				}
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   OnUpdateDraNew
    @return     void
    @param      CCmdUI  *pCmdUI
    @brief
******************************************************************************/
void CDraTreeListCtrl::OnUpdateDraNew(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = this->GetSelectedItem();
	if((NULL != hItem))
	{
		int nImage = -1 , nSelectedImage = -1;
		this->GetItemImage(hItem , nImage , nSelectedImage);
		pCmdUI->Enable(ICON_SETTING == nImage);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2015.03.24
    @function   OnDraOpen
    @return     void
    @brief
******************************************************************************/
void CDraTreeListCtrl::OnDraOpen()
{
	HTREEITEM hItem = GetFirstSelected();
	if(hItem)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;
		
		const CString sDraName = this->GetItemText(hItem , 0);
		const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sDraName + pDoc->GetDefaultExt();
		pDoc->OpenDrawing(sOutputFilePath);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2015.03.24
    @function   OnUpdateDraOpen
    @return     void
    @param      CCmdUI  *pCmdUI
    @brief
******************************************************************************/
void CDraTreeListCtrl::OnUpdateDraOpen(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = this->GetSelectedItem();
	if((NULL != hItem))
	{
		int nImage = -1 , nSelectedImage = -1;
		this->GetItemImage(hItem , nImage , nSelectedImage);

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;
		
		const CString sDraName = this->GetItemText(hItem , 0);
		const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sDraName + pDoc->GetDefaultExt();
		const BOOL bFileExist = FileExist(sOutputFilePath);

		pCmdUI->Enable((ICON_DRA == nImage) && (bFileExist));
	}
}

/******************************************************************************
    @brief		show title block data edit dialog
	@author     humkyung
    @date       2015.07.17
    @function   OnDraEditTitleBlockData
    @return     void
******************************************************************************/
void CDraTreeListCtrl::OnDraEditTitleBlockData()
{
	list<CaDraw_DraFile*> oDraFileList;
	for(map<CString,HTREEITEM>::const_iterator itr = m_OptionTreeItemMap.begin();itr != m_OptionTreeItemMap.end();++itr)
	{
		HTREEITEM hItem = GetChildItem(itr->second);
		while(hItem)
		{
			if(GetCheck(hItem))
			{
				CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)this->GetItemData(hItem);
				oDraFileList.push_back(pDraFile);
			}
			hItem = GetNextSiblingItem(hItem);
		}
	}

	CEditTitleBlockDataDlg dlg(oDraFileList);
	dlg.DoModal();
}

/******************************************************************************
    @brief		enable when one or more drawings are selected
	@author     humkyung
    @date       2015.07.17
    @function   OnUpdateDraEditTitleBlockData
    @return     void
    @param      CCmdUI  *pCmdUI
******************************************************************************/
void CDraTreeListCtrl::OnUpdateDraEditTitleBlockData(CCmdUI *pCmdUI)
{
	int iCheckedCount = 0;
	for(map<CString,HTREEITEM>::const_iterator itr = m_OptionTreeItemMap.begin();itr != m_OptionTreeItemMap.end();++itr)
	{
		HTREEITEM hItem = GetChildItem(itr->second);
		while(hItem)
		{
			if(GetCheck(hItem)) ++iCheckedCount;
			hItem = GetNextSiblingItem(hItem);
		}
	}

	pCmdUI->Enable(iCheckedCount > 0);
}