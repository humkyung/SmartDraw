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

#include <util/FileTools.h>
#include <util/SplitPath.h>

#include "SmartDraw.h"
#include "OutputWnd.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "SmartDrawView.h"
#include "Resource.h"
#include "MainFrm.h"
#include "StringTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static COutputWnd* __instance__ = NULL;
/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() : m_hThread(NULL)
{
	__instance__ = this;
}

COutputWnd::~COutputWnd()
{
	try
	{
		m_cImages.DeleteImageList();
		if(NULL != m_hThread) ::TerminateThread(m_hThread , -1);
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   GetInstance
    @return     COutputWnd*
    @brief
******************************************************************************/
COutputWnd* COutputWnd::GetInstance()
{
	return __instance__;
}

DWORD WINAPI FolderFilesWatchDogThread(LPVOID lpParam) // thread procedure
{
	pair<COutputWnd*,CSmartDrawDoc*>* pParam = (pair<COutputWnd*,CSmartDrawDoc*>*)(lpParam);

	HANDLE hFileChange = 
		::FindFirstChangeNotification((LPCTSTR)(pParam->first->m_sOutputFolder.operator LPCTSTR()), // folder path
		FALSE,            // don't look in subfolders
		FILE_NOTIFY_CHANGE_FILE_NAME); 
	// watch for
	// renaming, creating, 
	// or deleting a file
	if(INVALID_HANDLE_VALUE == hFileChange)
	{
		DWORD dwError = ::GetLastError();
		// handle error (see this FAQ)
		return dwError;
	}

	while(TRUE)
	{
		::WaitForSingleObject(hFileChange, INFINITE);
		// Bark, bark!!! A file was renamed, created or deleted.

		pParam->first->FillDrawingWindow();
		
		/// update overlay icons of treelist - 2015.04.25 added by humkyung
		CSmartDrawDoc* pDoc = pParam->second;
		POSITION pos = pDoc->GetFirstViewPosition();
		while(pos)
		{
			CView* pView = pDoc->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
			{
				CSmartDrawView* pSmartDrawView = static_cast<CSmartDrawView*>(pView);
				pSmartDrawView->m_wndDraTreeListCtrl.UpdateOverlayIcons(pDoc);
			}
		}
		/// up to here

		::FindNextChangeNotification(hFileChange);
	}

	return 0;
}

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSmartDrawApp* pApp = (CSmartDrawApp*)AfxGetApp();

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	/// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
	if (!m_wndOutputDrawing.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}
	CStringTable& table = CStringTable::GetInstance();
	m_wndOutputDrawing.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
	m_wndOutputDrawing.InsertColumn(0 , table.GetString(_T("IDS_OUTPUT_DRAWING_COL_NAME")).c_str() , LVCFMT_LEFT , 200);
	m_wndOutputDrawing.InsertColumn(1 , table.GetString(_T("IDS_FILE_SIZE_COL_NAME")).c_str() , LVCFMT_LEFT , 100);
	m_wndOutputDrawing.InsertColumn(2 , table.GetString(_T("IDS_LAST_MODIFIED_COL_NAME")).c_str() , LVCFMT_LEFT , 200);

	if(m_cImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1))
	{
		m_oFileIconIndexMap.insert(make_pair(_T(".DGN") , m_cImages.Add(pApp->LoadIcon(IDI_MSTNJ))));
	}

	m_wndOutputDrawing.SetFont(&m_Font);
	m_wndOutputDrawing.SetImageList(&m_cImages , LVSIL_SMALL);

	CString strTabName;
	/// 2013.06.09 added by humkyung
	if (!m_wndOutputLog.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}
	m_wndOutputLog.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
	
	m_wndOutputLog.InsertColumn(0 , table.GetString(_T("IDS_TIME_COL_NAME")).c_str() , LVCFMT_LEFT , 200);
	m_wndOutputLog.InsertColumn(1 , table.GetString(_T("IDS_MESSAGE_COL_NAME")).c_str() , LVCFMT_LEFT , 500);
	/// up to here


	/// Attach list windows to tab:
	strTabName = table.GetString(_T("IDS_DRAWING_TAB")).c_str();
	m_wndTabs.AddTab(&m_wndOutputDrawing, strTabName, (UINT)0);
	strTabName = table.GetString(_T("IDS_DEBUG_TAB")).c_str();
	m_wndTabs.AddTab(&m_wndOutputLog, strTabName , (UINT)1);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_Font);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

/**
	@brief	start thread watching output folder
	@author	humkyung
*/
void COutputWnd::StartFolderWatcher(const CString& sOutputFolder,CSmartDrawDoc* pDoc)
{
	m_sOutputFolder = sOutputFolder;
	if(NULL != m_hThread) ::TerminateThread(m_hThread , -1);

	static pair<COutputWnd*,CSmartDrawDoc*> param;
	{
		param.first = this;
		param.second = pDoc;
	}
	m_hThread = ::CreateThread(NULL, 0, FolderFilesWatchDogThread, LPVOID(&param) , 0, NULL);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   FillDrawingWindow
    @return     void
    @brief
******************************************************************************/
void COutputWnd::FillDrawingWindow()
{
	m_wndOutputDrawing.DeleteAllItems();

	vector<CString> oFoundFiles;
	CFileTools::FindFiles(oFoundFiles , m_sOutputFolder , 	_T("*.*") , false);
	for(vector<CString>::iterator itr = oFoundFiles.begin();itr != oFoundFiles.end();++itr)
	{
		CSplitPath splitter(*itr);
		const CString sFileExt = splitter.GetExtension().MakeUpper();
		map<CString , int>::iterator where = m_oFileIconIndexMap.find(sFileExt);
		if(where == m_oFileIconIndexMap.end())
		{
			SHFILEINFO info;
			::SHGetFileInfo(*itr , FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
			m_oFileIconIndexMap.insert(make_pair(sFileExt , m_cImages.Add(info.hIcon)));
		}
		const int iFileIconIndex = m_oFileIconIndexMap[sFileExt];
		
		CString rString;
		int nItem = m_wndOutputDrawing.InsertItem(m_wndOutputDrawing.GetItemCount() , splitter.GetFileName() + splitter.GetExtension() , iFileIconIndex);
		if(-1 != nItem)
		{
			struct stat fileInfo;
			if(0 == stat(CStringA((*itr)).operator const char*() , &fileInfo))
			{
				int nFormat = 0;
				__int64 size = fileInfo.st_size;
				if (size<1024)
					nFormat=1;
				else if (size<(1024*1024))
					nFormat=2;
				else
					nFormat=3;
				
				CString tmp, sizestr;
				switch (nFormat)
				{
					case 1:
						rString.Format(_T("%I64d"), size);
						break;
					case 2:
						rString.Format(_T("%I64d %s"), (size + 1023) / 1024, _T("KB"));
						break;
					case 3:
						rString.Format(_T("%I64d %s"), (size + 1024 * 1024 - 1) / 1024 / 1024, _T("MB"));
						break;
				}

				m_wndOutputDrawing.SetItemText(nItem , 1 , rString);
				
				CTime time(fileInfo.st_mtime);
				SYSTEMTIME timeDest;
				time.GetAsSystemTime( timeDest ) ;

				TCHAR text[512];
				if (!GetDateFormat(
					LOCALE_USER_DEFAULT,	// locale for which date is to be formatted
					DATE_SHORTDATE,         // flags specifying function options
					&timeDest,				// date to be formatted
					0,						// date format string
					text,					// buffer for storing formatted string
					512						// size of buffer
					))
					continue;
				rString=text;

				if (!GetTimeFormat(
					LOCALE_USER_DEFAULT,					// locale for which date is to be formatted
					TIME_NOSECONDS|TIME_FORCE24HOURFORMAT,  // flags specifying function options
					&timeDest,  // date to be formatted
					0,          // date format string
					text,       // buffer for storing formatted string
					512         // size of buffer
					))
					continue;
				
				rString+=_T(" ");
				rString+=text;
				m_wndOutputDrawing.SetItemText(nItem , 2 , rString);
			}
		}
	}
}

/**
	@brief	display message
	@author	humkyung
	@date	2013.06.09
*/
int COutputWnd::DisplayMessage(WPARAM wParam , LPARAM lParam)
{
	if(m_wndOutputLog.GetSafeHwnd())
	{
		if(-1 == lParam)
		{
			m_wndOutputLog.DeleteAllItems();
		}
		else
		{
			CTime t = CTime::GetCurrentTime();
			CString str;
			str.Format(_T("%d.%d.%d %d-%d-%d") , t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
			const int iItem = m_wndOutputLog.InsertItem(m_wndOutputLog.GetItemCount() , str);
			m_wndOutputLog.SetItemText(iItem , 1 , LPCTSTR(wParam));
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CMFCListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &COutputList::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &COutputList::OnNMRClick)
	ON_COMMAND(ID_OUTPUT_DELETE, &COutputList::OnDeleteDrawing)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers
void COutputList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;
		
		const CString sItemText = this->GetItemText(pNMItemActivate->iItem , 0);
		const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sItemText;
		pDoc->OpenDrawing(sOutputFilePath);
	}
	
	*pResult = 0;
}

/**
	@brief	show output popup
	@author	humkyung
	@date	2017.07.04
*/
void COutputList::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if(-1 != pNMItemActivate->iItem)
	{
		CPoint pt(pNMItemActivate->ptAction);

		UINT uFlags;
		const int nItem = this->HitTest(pt , &uFlags);
		if(uFlags & LVHT_ONITEMLABEL)
		{
			this->ClientToScreen(&pt);
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_OUTPUT_POPUP, pt.x, pt.y, this , TRUE);
		}
	}
	*pResult = 0;
}

/**
	@brief	delete selected drawing
	@author	humkyung
	@date	2017.07.04
*/
void COutputList::OnDeleteDrawing()
{
	POSITION pos = this->GetFirstSelectedItemPosition();
	int selected=-1;
	if (NULL != pos)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		const CString sProjectName(pDoc->GetProjectName());
		const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;

		const int nItem = this->GetNextSelectedItem(pos);
		const CString sItemText = this->GetItemText(nItem , 0);
		const CString sOutputFilePath = sPrjFolderPath + _T("\\Output\\") + sItemText;
		if(IDYES == AfxMessageBox(_TR("Do you want to selected drawing?"), MB_YESNO | MB_ICONQUESTION))
		{
			try
			{
				::DeleteFile(sOutputFilePath);
			}
			catch(exception& ex)
			{
				AfxMessageBox(ex.what());
			}
		}
	}
}

/*
	@brief	prevent system popup menu
*/
void COutputWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
