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

#include "stdafx.h"
#include "ViewTree.h"
#include "ProjectExplorer.h"
#include "SmartDraw.h"

#include <Tokenizer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @function   ActiveTreeItem
    @return     int
    @param      HTREEITEM   hItem
    @brief
******************************************************************************/
int CViewTree::UnActivateAllProjectTreeItem(HTREEITEM hItem)
{
	while(hItem)
	{
		int iImage = -1 , iSelectedImage = -1;
		this->GetItemImage(hItem , iImage , iSelectedImage);
		if((2 == iImage) || (3 == iImage)) this->SetItemImage(hItem , 2 , 2);

		HTREEITEM hChild = this->GetChildItem(hItem);
		if(hChild)
		{
			this->UnActivateAllProjectTreeItem(hChild);
		}

		hItem = this->GetNextSiblingItem(hItem);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	open new database(*.mdb) file

	@author	HumKyung.BAEK

	@date	2010.05.10
*/
void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	
	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point , &uFlags);
	if(hItem && (uFlags & TVHT_ONITEM))	/// ITEM TEXT를 클릭 했을때...
	{
		const CString rItemText = GetItemText(hItem);
		
		int nImage = -1 , nSelectedImage = -1;
		this->GetItemImage(hItem , nImage , nSelectedImage);
		if(((2 == nImage) || (3 == nImage)) && GetParentItem(hItem) && (GetParent()->IsKindOf(RUNTIME_CLASS(CProjectExplorer))))
		{
			vector<STRING_T> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult , rItemText.operator LPCTSTR() , CIsFromString(_T("/")));
			CString rProjectName(oResult[0].c_str());
			rProjectName.Trim();

			/// open new project
			CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();
			if(pApp)
			{
				pApp->OpenNewDocument(rProjectName);
				
				this->UnActivateAllProjectTreeItem(this->GetRootItem());
				this->SetItemImage(hItem , 3 , 3);
			}
		}
	}

	*pResult = 1;	//! ?
}
