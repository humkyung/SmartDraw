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
#include <assert.h>
#include "mainfrm.h"
#include "ProjectExplorer.h"
#include "Resource.h"
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "NewProjectDlg.h"
#include "StringTable.h"
#include <aDraw_PipeDiaTable.h>

#include <util/ado/ADODB.h>
#include <Tokenizer.h>

#include "soci.h"
#include "soci-sqlite3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CProjectExplorer* __instance__ = NULL;
/////////////////////////////////////////////////////////////////////////////
// CProjectExplorer

CProjectExplorer::CProjectExplorer() : m_wndResize(FALSE)
{
	__instance__ = this;
}

CProjectExplorer::~CProjectExplorer()
{
}

BEGIN_MESSAGE_MAP(CProjectExplorer, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_PROJECT_NEW, &CProjectExplorer::OnProjectNew)
	ON_COMMAND(ID_PROJECT_DELETE, &CProjectExplorer::OnProjectDelete)
	ON_COMMAND(ID_PROJECT_EDIT , &CProjectExplorer::OnProjectEdit)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT , &CProjectExplorer::OnUpdateProjectEdit)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DELETE, &CProjectExplorer::OnUpdateProjectDelete)

	ON_COMMAND(ID_HORZ_RESIZE, OnHorzResize)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CProjectExplorer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	/// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	m_wndResize.Create( NULL, _T("") , WS_VISIBLE | WS_CHILD,CRect(0,0,0,0), this , ID_RESIZE );

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}
	InitPropList();

	/// Fill in some static tree view data (dummy code, nothing magic here)
	FillProjectTree();
	AdjustLayout();

	return 0;
}

void CProjectExplorer::OnSize(UINT nType, int cx, int cy)
{
	static bool __init_size__ = true;

	CDockablePane::OnSize(nType, cx, cy);

	/*if(true == __init_size__)
	{*/
		if(m_wndResize.GetSafeHwnd())
		{
			m_wndResize.SetWindowPos(NULL , 0 , int(cy*2./3.) , cx , 4 , SWP_NOZORDER);
			__init_size__ = false;
		}
	/*}
	else if(m_wndResize.GetSafeHwnd())
	{
		CRect rect;
		m_wndResize.GetWindowRect( rect );
		ScreenToClient( rect );

		m_wndResize.SetWindowPos(NULL ,  rect.left , rect.top , cx , rect.Height() , SWP_NOZORDER );
	}*/

	AdjustLayout();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @function   GetInstance
    @return     CProjectExplorer*
    @brief
******************************************************************************/
CProjectExplorer* CProjectExplorer::GetInstance()
{
	return __instance__;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-04
    @function   UpdateTreeItem
    @return     int
    @param      
    @brief		update tree item associated with given parameters
******************************************************************************/
int CProjectExplorer::UpdateTreeItem(const STRING_T& sPrjName , const STRING_T& sPrjDesc)
{
	map<STRING_T , HTREEITEM>::iterator where = m_oProjectNameMap.find(sPrjName);
	if(where != m_oProjectNameMap.end())
	{
		m_wndFileView.SetItemText(where->second , STRING_T(sPrjName + _T(" / ") + sPrjDesc).c_str());
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @function   ActivateTreeItem
    @return     int
    @param      CSmartDrawDoc*
    @brief
******************************************************************************/
int CProjectExplorer::ActivateTreeItem(CSmartDrawDoc* pDoc)
{
	assert(pDoc && "pDoc is NULL");

	if(pDoc)
	{
		try
		{
			m_wndFileView.UnActivateAllProjectTreeItem(m_wndFileView.GetRootItem());

			map<STRING_T , HTREEITEM>::iterator where = m_oDrawingTypeMap.find(pDoc->GetDrawingType());
			if(where != m_oDrawingTypeMap.end())
			{
				///#75: get project code list from reference database - 2016.02.13 added by humkyung
				CMFCPropertyGridProperty* pGroupProp = m_wndPropList.GetProperty(0);
				for(int i = 0;i < pGroupProp->GetSubItemsCount();++i)
				{
					CMFCPropertyGridProperty* pSubItem = pGroupProp->GetSubItem(i);
					CString sPropName = pSubItem->GetName();
					CString sPropValue= pSubItem->GetValue();
					if(_T("Code") == sPropName)
					{
						pSubItem->RemoveAllOptions();

						CSmartDrawDoc* pDoc = CSmartDrawDoc::GetInstance();
						if(NULL != pDoc)
						{
							try
							{
								CString sDesc;
								soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
								{
									soci::rowset<soci::row> rs(oSession.prepare << _T("select * from codes"));
									for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
									{
										const STRING_T sValue = itr->get<STRING_T>(0);
										pSubItem->AddOption(sValue.c_str());
										sDesc += CString(sValue.c_str()) + _T(",");
									}
									pSubItem->SetDescription(_T("(") + sDesc.TrimRight(',') + _T(")"));
								}

								pDoc->LoadPipeDiaTable();
							}
							catch(const std::exception& ex)
							{
								AfxMessageBox(ex.what(),MB_OK);
							}
						}
						break;
					}
				}
				/// up to here

				STRING_T sProjectName = pDoc->GetProjectName().operator LPCTSTR();
				STRING_T sProjectDesc = pDoc->GetProjectDesc();
				STRING_T sPassword = pDoc->GetPassword();
				STRING_T sModelOffsetX = pDoc->GetModelOffsetX();
				STRING_T sModelOffsetY = pDoc->GetModelOffsetY();
				STRING_T sModelOffsetZ = pDoc->GetModelOffsetZ();
				const STRING_T sProjUnit = pDoc->GetProjectUnit();
				const STRING_T sProjCode = pDoc->GetProjectCode();	///#57: get project code
				const STRING_T sCADAppPath = pDoc->GetCADAppPath();	///2016.03.10 added by humkyung

				HTREEITEM hChild = m_wndFileView.GetChildItem(where->second);
				while(hChild)
				{
					const CString sItemText = m_wndFileView.GetItemText(hChild);
					
					if(sItemText == CString(STRING_T(sProjectName + _T(" / ") + sProjectDesc).c_str()))
					{
						m_wndFileView.SetItemImage(hChild , 3 , 3);
						{
							CMFCPropertyGridProperty* pGroupProperty = m_wndPropList.GetProperty(0);
							pGroupProperty->GetSubItem(0)->SetValue(COleVariant(sProjectName.c_str()));
							pGroupProperty->GetSubItem(1)->SetValue(COleVariant(sProjectDesc.c_str()));
							pGroupProperty->GetSubItem(2)->SetValue(COleVariant(sModelOffsetX.c_str()));
							pGroupProperty->GetSubItem(3)->SetValue(COleVariant(sModelOffsetY.c_str()));
							pGroupProperty->GetSubItem(4)->SetValue(COleVariant(sModelOffsetZ.c_str()));
							pGroupProperty->GetSubItem(5)->SetValue(COleVariant(sProjUnit.c_str()));	/// 2016.01.27 added by humkyung
							pGroupProperty->GetSubItem(6)->SetValue(COleVariant(sProjCode.c_str()));	///#57: 2016.02.13 added by humkyung
							pGroupProperty->GetSubItem(7)->SetValue(COleVariant(sCADAppPath.c_str()));	///2016.03.10 added by humkyung
						}
						return ERROR_SUCCESS;
					}
					hChild = m_wndFileView.GetNextSiblingItem(hChild);
				}
			}
		}
		catch(std::exception& ex)
		{
			AfxMessageBox(ex.what() , MB_OK);
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2011-??-??
    @function   FillProjectTree
    @return     void
    @brief
******************************************************************************/
void CProjectExplorer::FillProjectTree()
{
	m_oDrawingTypeMap.clear();
	m_wndFileView.DeleteAllItems();

	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("SmartDraw Project(s)") , 0 , 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	
	CString str;
	CString rServerFolderPath = docData.GetServerFolderPath();
	const CString rMDBFilePath = rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
	CString sConnectionString = CString(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

	CADODB adoDB;
	if(TRUE == adoDB.DBConnect(sConnectionString.operator LPCTSTR()))
	{
		if(!adoDB.OpenQuery(_T("SELECT * FROM T_SMARTDRAW_PROJECT")))
			return;

		CString tmp;
		LONG lRecordCount = 0;
		if(adoDB.GetRecordCount(&lRecordCount))
		{
			STRING_T strValue;
			for(int i = 0;i < lRecordCount;++i)
			{
				adoDB.GetFieldValue(i , _T("C_DRAWING_TYPE") , &strValue);
				map<STRING_T , HTREEITEM>::iterator where = m_oDrawingTypeMap.find(strValue);
				if(where == m_oDrawingTypeMap.end())
				{
					HTREEITEM hItem = m_wndFileView.InsertItem(strValue.c_str() , 1 , 1 , hRoot);
					m_oDrawingTypeMap.insert(make_pair(strValue , hItem));
				}
				HTREEITEM hParentItem = m_oDrawingTypeMap[strValue];

				adoDB.GetFieldValue(i , _T("C_PROJECT_NAME") , &strValue);
				if(strValue.empty()) continue;

				STRING_T rDesc;
				adoDB.GetFieldValue(i , _T("C_PROJECT_DESC") , &rDesc);

				HTREEITEM hItem = m_wndFileView.InsertItem(STRING_T(strValue + _T(" / ") + rDesc).c_str() , 2 , 2, hParentItem);
				/// 2014.06.03 added by humkyung
				if(NULL != hItem)
				{
					m_oProjectNameMap.insert(make_pair(strValue , hItem));
				}
				/// up to here

				m_wndFileView.Expand(hParentItem, TVE_EXPAND);
			}
		}
	}

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
}

void CProjectExplorer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	/*if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}*/

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_PROJECT, point.x, point.y, this, TRUE);
}

/**
	@brief	adjust layout

	@author	humkyung
*/
void CProjectExplorer::AdjustLayout()
{
	if (NULL == GetSafeHwnd())
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	
	CRect rectResize;
	m_wndResize.GetWindowRect(&rectResize);
	ScreenToClient(&rectResize);

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, /*rectClient.Height()*/rectResize.top - cyTlb - 1, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndPropList.SetWindowPos(NULL , rectClient.left + 1 , rectResize.bottom , rectClient.Width() - 2 , rectClient.Height() - rectResize.bottom - 2 , SWP_NOACTIVATE | SWP_NOZORDER);
}

void CProjectExplorer::OnProperties()
{
	AfxMessageBox(_T("Properties...."));

}

void CProjectExplorer::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CProjectExplorer::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CProjectExplorer::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CProjectExplorer::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

/**	
	@brief	create a new project
	@author	HumKyung.BAEK
	@date	2010.05.09
	@param	
	@return
*/
void CProjectExplorer::OnProjectNew()
{
	CNewProjectDlg dlg(false);
	if(IDOK == dlg.DoModal())
	{
		map<STRING_T , HTREEITEM>::iterator where = m_oDrawingTypeMap.find(dlg.m_sDrawingType.operator LPCTSTR());
		if(where == m_oDrawingTypeMap.end())
		{
			const HTREEITEM hRootItem = m_wndFileView.GetRootItem();
			const HTREEITEM hChildItem = m_wndFileView.GetChildItem(hRootItem);

			HTREEITEM hItem = m_wndFileView.InsertItem(dlg.m_sDrawingType , 1 , 1 , hRootItem);
			m_oDrawingTypeMap.insert(make_pair(dlg.m_sDrawingType.operator LPCTSTR() , hItem));
			if(NULL == hChildItem)
			{
				m_wndFileView.Expand(hRootItem , TVE_EXPAND);
			}
		}

		HTREEITEM hParentItem = m_oDrawingTypeMap[dlg.m_sDrawingType.operator LPCTSTR()];
		const CString rProjectName(dlg.m_sProjectName + _T(" / ") + dlg.m_sProjectDesc);
		HTREEITEM hItem = m_wndFileView.InsertItem(rProjectName , 2 , 3 , hParentItem);
		if(NULL != hItem)
		{
			m_wndFileView.Expand(hParentItem , TVE_EXPAND);
		}

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CString sServerFolderPath = docData.GetServerFolderPath();

		CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();
		if(pApp) pApp->OpenNewDocument(sServerFolderPath + _T("\\") + dlg.m_sProjectName);
	}
}

/**	
	@brief	delete a project from project list and rename project folder
	@author	HumKyung.BAEK
	@date	2010.05.09
	@param	
	@return
*/
void CProjectExplorer::OnProjectDelete()
{
	const int res = AfxMessageBox(_T("Do you want to delete this project?") , MB_YESNO|MB_ICONQUESTION);
	if(IDYES == res)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

		const CString rMDBFilePath = docData.GetServerFolderPath() + CString(_T("\\")) + PROJECT_MDB_NAME;
		CString rConnectionString = CString(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

		CADODB adoDB;
		if(adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
		{
			HTREEITEM hItem = m_wndFileView.GetSelectedItem();
			CString rItemText = m_wndFileView.GetItemText(hItem);

			vector<STRING_T> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult , rItemText.operator LPCTSTR() , CIsFromString(_T("/")));
			rItemText = oResult[0].c_str();
			rItemText.Trim();

			CString rSql;
			rSql.Format(_T("DELETE * FROM %s WHERE C_PROJECT_NAME='%s'") , PROJECT_TABLE_NAME , rItemText);
			if(TRUE == adoDB.ExecuteQuery(rSql.operator LPCTSTR()))
			{
				int nImage = -1 , nSelectedImage = -1;
				m_wndFileView.GetItemImage(hItem , nImage , nSelectedImage);

				m_wndFileView.DeleteItem(hItem);

				/// close document when project which is open is deleted - 2012.04.28 added by humkyung
				if(3 == nImage)
				{
					CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();
					if(pApp)
					{
						pApp->CloseAllDocuments(FALSE);
					}
				}
				/// up to here
			}
		}
	}
}

/**	
	@brief	edit a selected project
	@author	humkyung
	@date	2015.03.31
	@param	
	@return
*/
void CProjectExplorer::OnProjectEdit()
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString rItemText = m_wndFileView.GetItemText(hItem);
	vector<STRING_T> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , rItemText.operator LPCTSTR() , CIsFromString(_T("/")));
	if(oResult.size() > 0)
	{
		rItemText = oResult[0].c_str();
		rItemText.Trim();
		CNewProjectDlg dlg(rItemText);
		if(IDOK == dlg.DoModal())
		{
		}
	}
}

/**	
	@brief	
	@author	humkyung
	@date	2015.04.01
	@param	
	@return
*/
void CProjectExplorer::OnUpdateProjectEdit(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	if(NULL != hItem)
	{
		int nImage = -1 , nSelectedImage = -1;
		m_wndFileView.GetItemImage(hItem , nImage , nSelectedImage);
		pCmdUI->Enable(2 == nImage);
	}
	else
	{
		pCmdUI->Enable(false);
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.09
	@param	
	@return
*/
void CProjectExplorer::OnUpdateProjectDelete(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	pCmdUI->Enable(hItem && m_wndFileView.GetParentItem(hItem));
}

/**
	@brief	when resize wnd is moved
	@author	humkyung
	@date	2012.12.26
*/
void CProjectExplorer::OnHorzResize() 
{
	CRect rect;
	m_wndResize.GetWindowRect( rect );
	ScreenToClient( rect );

	rect.bottom += m_wndResize.m_adjust_height;
	m_wndResize.SetWindowPos(NULL ,  rect.left , rect.top + m_wndResize.m_adjust_height , rect.Width() , 4 , SWP_NOZORDER );

	this->AdjustLayout();
}

void CProjectExplorer::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

/**
	@brief	init property list
	@author	humkyung
	@date	2012.12.26
*/
void CProjectExplorer::InitPropList()
{
	CStringTable& table = CStringTable::GetInstance();

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(table.GetString(_T("IDS_PROJECT_PROPERTIES")).c_str());
	if(NULL != pGroup)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString sUser = docData.GetUser();
		const CString sPassword = docData.GetPassword();

		m_wndPropList.AddProperty(pGroup);

		CMFCPropertyGridProperty* pSubItem = new CMFCPropEx(_T("Project Name") , (_variant_t) _T("") , _T("Project Name"));
		pSubItem->AllowEdit(FALSE);
		pGroup->AddSubItem( pSubItem );
		pSubItem = new CMFCPropEx(_T("Project Desc") , (_variant_t) _T("") , _T("Project Description"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		pSubItem = new CMFCPropEx(_T("Model Offset X") , (_variant_t) _T("") , _T("Model Offset X"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		pSubItem = new CMFCPropEx(_T("Model Offset Y") , (_variant_t) _T("") , _T("Model Offset Y"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		pSubItem = new CMFCPropEx(_T("Model Offset Z") , (_variant_t) _T("") , _T("Model Offset Z"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		
		/// 2016.01.26 added by humkyung
		pSubItem = new CMFCPropEx(_T("Unit") , (_variant_t) _T("") , _T("Project Unit"));
		pSubItem->AddOption(_T("Inch Feet"));
		pSubItem->AddOption(_T("Metric"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		/// up to here

		/// 2016.02.13 added by humkyung
		pSubItem = new CMFCPropEx(_T("Code") , (_variant_t) _T("") , _T("Pipe O.D"));
		pSubItem->AllowEdit( TRUE );
		pGroup->AddSubItem( pSubItem );
		/// up to here

		/// 2016.03.10 added by humkyung
		pSubItem = new CMFCPropertyGridFileProperty(_TR("CAD File Path") , TRUE , _T("C:\\"));
		pGroup->AddSubItem( pSubItem );
		/// up to here
	}
}

/******************************************************************************
	@brief
	@author     humkyung
    @date       2012-12-27
    @function   OnPropertyChanged
    @return     LRESULT
    @param      WPARAM  wp
    @param      LPARAM  lp
******************************************************************************/
LRESULT CProjectExplorer::OnPropertyChanged(WPARAM wp,  LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)(lp);

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	const CString rServerFolderPath = docData.GetServerFolderPath();
	CString sMDBFilePath = rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
	CString sConnectionString = CString(PROVIDER) + _T("Data Source=") + sMDBFilePath + DB_PASSWORD;

	CADODB adoDB;
	if(adoDB.DBConnect(sConnectionString.operator LPCTSTR()))
	{
		CMFCPropertyGridProperty* pGroupProp = m_wndPropList.GetProperty(0);
		CString sProjectName = pGroupProp->GetSubItem(0)->GetValue();

		CString sSql;
		CString sPropName = pProp->GetName();
		CString sPropValue= pProp->GetValue();
		if(_T("Project Desc") == sPropName)
		{
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET C_PROJECT_DESC='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		else if(_T("Model Offset X") == sPropName)
		{
			pDoc->SetModelOffsetX(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET C_MODEL_OFFSET_X='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		else if(_T("Model Offset Y") == sPropName)
		{
			pDoc->SetModelOffsetY(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET C_MODEL_OFFSET_Y='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		else if(_T("Model Offset Z") == sPropName)
		{
			pDoc->SetModelOffsetZ(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET C_MODEL_OFFSET_Z='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		/// update project unit - 2016.01.27 added by humkyung
		else if(_T("Unit") == sPropName)
		{
			pDoc->SetProjectUnit(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET UNIT='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		/// up to here
		///#57: update code - 2016.02.13 added by humkyung
		else if(_T("Code") == sPropName)
		{
			pDoc->SetProjectCode(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET CODE='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		/// up to here
		///update cad application path - 2016.02.13 added by humkyung
		else if(_TR("CAD File Path") == sPropName)
		{
			pDoc->SetCADAppPath(sPropValue.operator LPCTSTR());
			sSql.Format(_T("UPDATE T_SMARTDRAW_PROJECT SET C_CAD_APPLICATION='%s' WHERE C_PROJECT_NAME='%s'") , sPropValue , sProjectName);
		}
		///up to here

		VARIANT RecordsAffected;
		VariantInit(&RecordsAffected);
		if(TRUE == adoDB.ExecuteQuery(sSql.operator LPCTSTR() , &RecordsAffected))
		{
			pProp->SetOriginalValue( pProp->GetValue() );
			if(_T("Project Desc") == sPropName) this->UpdateTreeItem(sProjectName.operator LPCTSTR() , sPropValue.operator LPCTSTR());
		}
	}

	return 0;
}