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

// aDraw_CableTrayView.cpp : implementation of the CSmartDrawView class
//

#include "stdafx.h"
#include <assert.h>
#include "SmartDraw.h"

#include <SmartDrawAnnoTextStyle.h>

#include "SmartDrawDoc.h"
#include "SmartDrawView.h"
#include "SmartDrawDocData.h"
#include "WorkStatusDlg.h"
#include "PluginManager.h"
#include "MainFrm.h"
#include "StringTable.h"
#include "CADInteractive.h"

#include <util/FileTools.h>
#include <util/SplitPath.h>
#include <util/Path.h>
#include <util/FileVersion.h>

#include <DgnV7Lib.h>
#include <SmartDrawPDSModuleImpl.h>
#include <SmartDrawAnno.h>
#include <BackgroundFile.h>

#include  <Tokenizer.h>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmartDrawView

IMPLEMENT_DYNCREATE(CSmartDrawView, CView)

BEGIN_MESSAGE_MAP(CSmartDrawView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSmartDrawView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_OPTION_SETTING , &CSmartDrawView::OnOptionSetting)
	ON_UPDATE_COMMAND_UI(ID_OPTION_SETTING , &CSmartDrawView::OnUpdateOptionSetting)
END_MESSAGE_MAP()

// CSmartDrawView construction/destruction

CSmartDrawView::CSmartDrawView()
{
	// TODO: add construction code here

}

CSmartDrawView::~CSmartDrawView()
{
}

BOOL CSmartDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSmartDrawView drawing

void CSmartDrawView::OnDraw(CDC* /*pDC*/)
{
	CSmartDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CSmartDrawView printing


void CSmartDrawView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CSmartDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSmartDrawView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSmartDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSmartDrawView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSmartDrawView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CSmartDrawView diagnostics

#ifdef _DEBUG
void CSmartDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CSmartDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSmartDrawDoc* CSmartDrawView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmartDrawDoc)));
	return (CSmartDrawDoc*)m_pDocument;
}
#endif //_DEBUG


// CSmartDrawView message handlers

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.10
	@param	
	@return
*/
int CSmartDrawView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	rect.SetRectEmpty();

	DWORD uStyle = TVS_HASBUTTONS|TVS_HASLINES|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT|TVS_CHECKBOXES|TVS_EDITLABELS;
	if(m_wndDraTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER , rect , this , ID_DRA_TREE_LIST_CTRL))
	{
		InitTreeList();

		m_cImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1);
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ROOT));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_SETTING));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_DRA));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_VIEW));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_SHORTCUT_OVERLAY));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ERROR_OVERLAY));
		BOOL res = m_cImages.SetOverlayImage(4,1);
		res = m_cImages.SetOverlayImage(5,2);
		m_wndDraTreeListCtrl.SetImageList(&m_cImages,TVSIL_NORMAL);

		CStringTable& table = CStringTable::GetInstance();
		const STRING_T str = table.GetString(_T("IDS_DRAWINGS")).c_str();
		HTREEITEM hRoot = m_wndDraTreeListCtrl.InsertItem(str.c_str());
	}

	return 0;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.10
	@param	
	@return
*/
void CSmartDrawView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_wndDraTreeListCtrl.GetSafeHwnd())
	{
		m_wndDraTreeListCtrl.SetWindowPos(NULL , 0 , 0 , cx , cy , SWP_NOZORDER);
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.10
	@param	
	@return
*/
int CSmartDrawView::InitTreeList(void)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	DWORD uExStyle	= TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK|TVS_EX_ITEMLINES;
	CStringTable& table = CStringTable::GetInstance();

	const CString rIniFilePath = GetExecPath() + _T("\\Setting\\SmartDraw_Project.ini");

	int nColumnCount = 0;
	if(GetPrivateProfileString(_T("Dra_Tree_List") , _T("Column_Count") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		nColumnCount = ATOI_T(szBuf);
	}
	for(int i = 0;i < nColumnCount;++i)
	{
		CString rKeyName;
		rKeyName.Format(_T("Column%d") , i);

		if(GetPrivateProfileString(_T("Dra_Tree_List") , rKeyName , NULL , szBuf , MAX_PATH , rIniFilePath))
		{
			vector<STRING_T> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(3 == oResult.size())
			{
				CDraTreeListCtrl::ColumnData columnData;
				columnData.rColumnName  = table.GetString(oResult[0].c_str()).c_str();
				columnData.rAlignment = oResult[1].c_str();
				columnData.nColumnWidth  = ATOI_T(oResult[2].c_str());
				m_wndDraTreeListCtrl.m_ColumnDataMap.insert(make_pair(i , columnData));

				int nFormat = LVCFMT_LEFT;
				if(_T("CENTER") == columnData.rAlignment)
				{
					nFormat = LVCFMT_CENTER;
				}
				else if(_T("RIGHT") == columnData.rAlignment)
				{
					nFormat = LVCFMT_RIGHT;
				}
				m_wndDraTreeListCtrl.InsertColumn(i, columnData.rColumnName, nFormat, columnData.nColumnWidth);
			}
		}
	}

	m_wndDraTreeListCtrl.SetExtendedStyle(uExStyle);

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.10
	@param	
	@return
*/
void CSmartDrawView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	///static BOOL bFirstTime = TRUE;

	CSmartDrawDoc* pDoc = (CSmartDrawDoc*)GetDocument();
	const CString rProjectName = pDoc->GetProjectName();
	if(pDoc && !rProjectName.IsEmpty())
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString rServerFolderPath = docData.GetServerFolderPath();

		const CString rSettingFolderPath = rServerFolderPath + _T("\\") + rProjectName + _T("\\Setting");
		const CString rDraFolderPath = rServerFolderPath + _T("\\") + rProjectName + _T("\\Dra");
		m_wndDraTreeListCtrl.FillDraView(pDoc , rSettingFolderPath , rDraFolderPath);
		///if(bFirstTime)
		{
			m_wndDraTreeListCtrl.SortTree(0 , TRUE , m_wndDraTreeListCtrl.GetRootItem());
			///bFirstTime = FALSE;
		}
	}
}

/**	
	@brief	generate drawings
	@author	HumKyung.BAEK
	@date	2010.05.17
	@param	
	@return
*/
int CSmartDrawView::OnGenerate()
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetDocument();

		/// check cad executable location - 2015.08.29 added by humkyung
		if(pDoc->IsMSTNOutput())
		{
			const CString sMSTNJPath = docData.GetMSTNJFilePath();
			if(sMSTNJPath.IsEmpty())
			{
				AfxMessageBox(_TR("Can't find USTN file") , MB_OK , MB_ICONERROR);
				return ERROR_BAD_ENVIRONMENT;
			}
		}
		else if(pDoc->IsAutoCADOutput())
		{
			const CString sCADAppPath = pDoc->GetCADAppPath();
			if(FALSE == ::FileExist(sCADAppPath))
			{
				AfxMessageBox(_TR("Can't find AutoCAD file") , MB_OK , MB_ICONERROR);
				return ERROR_BAD_ENVIRONMENT;
			}
		}
		/// up to here

		CWorkStatusDlg dlg(this);
		dlg.m_pThread = AfxBeginThread(CSmartDrawView::StatusThreadEntry , this , THREAD_PRIORITY_NORMAL , 0 , CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();	

			const CString sCmdFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->GetProjectName() + _T("\\Working\\pds.cmd");
			if(FALSE == ::FileExist(sCmdFilePath))
			{
				AfxMessageBox(_TR("Can't find command file") , MB_OK , MB_ICONERROR);
				return ERROR_BAD_ENVIRONMENT;
			}

			if(GetDocument()->IsMSTNOutput())
			{
				/// open mstn j and write outputs
				const CString sMSTNJPath = docData.GetMSTNJFilePath();
				if(!sMSTNJPath.IsEmpty())
				{
					const CString sCommonAppDataPath = CFileTools::GetCommonAppDataPath();
					const CString sSeedFilePath = sCommonAppDataPath + _T("\\SmartDraw_PDS\\Seed\\seed3d.dgn");

					if(CFileTools::DoesFileExist(sSeedFilePath))
					{
						/// write keyin text file - 2015.06.11 added by humkyung
						const CString sKeyinFilePath = CFileTools::GetTempPath() + _T("keyin.txt");
						OFSTREAM_T ofile(sKeyinFilePath.operator LPCTSTR());
						if(ofile.is_open())
						{
							ofile << _T("mdl load aDgnWriter \"") << sCmdFilePath.operator LPCTSTR() << _T("\"") << std::endl;
							ofile << _T("QUIT");
							ofile.close();
						}
						/// up to here
						const long length = GetShortPathName(CFileTools::GetTempPath(), NULL, 0);
						if (length > 0)
						{
							TCHAR* buffer = new TCHAR[length];
							if(GetShortPathName(CFileTools::GetTempPath(), buffer, length) > 0)
							{
								const CString sArguments(_T("\"") + sSeedFilePath + _T("\" -S") + CString(buffer) + _T("keyin.txt"));
								::ShellExecute(NULL , _T("open") , sMSTNJPath , sArguments , NULL , SW_SHOW);
								delete []buffer;
							}
						}
					}
				}
				/// up to here
			}
			else if(GetDocument()->IsAutoCADOutput())
			{
				const CString sCADAppPath = pDoc->GetCADAppPath();
				CFileVersion version;
				version.Open(sCADAppPath);
				int pos = 0;
				CString sVerNo = version.GetProductVersion().Tokenize(_T(".") , pos).TrimLeft('R');
				
				STRING_T sAutoCADPluginsPath = CFileTools::GetCommonAppDataPath() + _T("\\Autodesk\\ApplicationPlugins");
				STRING_T sPluginPath = sAutoCADPluginsPath + _T("\\SmartDraw\\") + STRING_T(sVerNo.operator LPCTSTR());
				STRING_T sStartUpFileName = sPluginPath + _T("\\StartUp.scr");
				
				OFSTREAM_T oFile(sStartUpFileName.c_str());
				{
					STRING_T sDllFilePath = sAutoCADPluginsPath + _T("\\SmartDraw\\") + STRING_T(sVerNo.operator LPCTSTR()) + _T("\\DwgWriter.NET.dll");
					IsString::ReplaceOf(sDllFilePath , _T("\\") , _T("/"));
					STRING_T sDataFilePath(sCmdFilePath.operator LPCTSTR());
					IsString::ReplaceOf(sDataFilePath , _T("\\") , _T("/"));

					oFile << _T("(command \"netload\" \"") << sDllFilePath << _T("\") ");
					oFile << _T("(command \"DwgWriter\" \"") << sDataFilePath << _T("\") ");
					oFile.close();

					STRING_T sParam;
					sParam = _T(" /b \"") + sStartUpFileName + _T("\"");
					::ShellExecute(NULL , _T("open") , sCADAppPath , sParam.c_str() , NULL , SW_SHOW);
				}
			}

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	thread entry point
	@author	HumKyung.BAEK
	@date	2010.05.17
	@param	
	@return
*/
UINT CSmartDrawView::StatusThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CSmartDrawView* p = (CSmartDrawView*)pVoid;
		if(p) p->StatusThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   GetSelectedDrawings
    @return     int
    @param      list<CaDraw_DraFile*>&  oSelectedDrawingList
    @brief
******************************************************************************/
int CSmartDrawView::GetSelectedDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList)
{
	for(map<CString,HTREEITEM>::iterator itr = m_wndDraTreeListCtrl.m_OptionTreeItemMap.begin();itr != m_wndDraTreeListCtrl.m_OptionTreeItemMap.end();++itr)
	{
		HTREEITEM hItem = m_wndDraTreeListCtrl.GetChildItem(itr->second);
		while(hItem)
		{
			if(m_wndDraTreeListCtrl.GetCheck(hItem))
			{
				CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)(m_wndDraTreeListCtrl.GetItemData(hItem));
				oSelectedDrawingList.push_back(pDraFile);
			}
			hItem = m_wndDraTreeListCtrl.GetNextSiblingItem(hItem);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief		get all drawings
    @author     humkyung
    @date       2016.03.07
    @return     int
******************************************************************************/
int CSmartDrawView::GetAllDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList)
{
	for(map<CString,HTREEITEM>::const_iterator itr = m_wndDraTreeListCtrl.m_OptionTreeItemMap.begin();itr != m_wndDraTreeListCtrl.m_OptionTreeItemMap.end();++itr)
	{
		HTREEITEM hItem = m_wndDraTreeListCtrl.GetChildItem(itr->second);
		while(hItem)
		{
			CaDraw_DraFile* pDraFile = (CaDraw_DraFile*)(m_wndDraTreeListCtrl.GetItemData(hItem));
			if(NULL != pDraFile) oSelectedDrawingList.push_back(pDraFile);
			hItem = m_wndDraTreeListCtrl.GetNextSiblingItem(hItem);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	thread function
	@author	HumKyung.BAEK
	@date	2010.05.17
	@param	
	@return
*/
int CSmartDrawView::StatusThread(void)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawDoc* pDoc = GetDocument();

	list<CaDraw_DraFile*> oSelectedDraFileEntry;
	this->GetSelectedDrawings( oSelectedDraFileEntry );
	
	const CString sCmdFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->GetProjectName() + _T("\\Working\\pds.cmd");
	::DeleteFile(sCmdFilePath);

	CString rTitle = _T("Generating...");
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	long nTotalCount = oSelectedDraFileEntry.size() , nIndex = 0;
	for(list<CaDraw_DraFile*>::const_iterator itr = oSelectedDraFileEntry.begin();itr != oSelectedDraFileEntry.end();++itr)
	{
		//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(CString(CString((*itr)->GetFileNameWithoutExt().c_str()) + _T(" ") + rTitle).operator LPCTSTR()) , 0);
		this->GenerateOutput(*itr);
		
		int progress = int((double(++nIndex) / double(nTotalCount))*100.);
		if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	return drawing scale which is used for 1:1 scale drawing
	@author humkyung
	@date 2014.05.10
	@param	
	@return	drawing scale
*/
static double GetDrawingScale(CaDraw_DraFile* pDraFile , const STRING_T& sIniFilePath)
{
	assert(pDraFile && "pDraFile is NULL");
	double res = 1.0;

	if(pDraFile)
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		
		vector<STRING_T> oResult;
		UINT iCount = GetPrivateProfileInt(_T("GENERATION_OPTIONS") , _T("Generation Item Count") , 0 , sIniFilePath.c_str());
		for(int i = 0;i < iCount;++i)
		{
			OSTRINGSTREAM_T oss;
			oss << _T("Generation Item") << i;
			if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , oss.str().c_str() , _T("False") , szBuf , MAX_PATH , sIniFilePath.c_str()))
			{
				CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
				if((oResult.size() >= 2) && (STRING_T(_T("1:1 Scale Drawing")) == oResult[0]) && (STRING_T(_T("True")) == oResult[1]))
				{
					list<CaDraw_View*> oViewList;
					pDraFile->GetViewList(&oViewList);
					for(list<CaDraw_View*>::const_iterator itr = oViewList.begin();itr != oViewList.end();++itr)
					{
						if(1.0 == res)
						{
							res = (*itr)->GetScale();
						}
						else if(res != (*itr)->GetScale())
						{
							res = 1.0;
							break;
						}
					}
					break;
				}
			}
		}
	}

	return res;
}

/**
	@brief	return output file name for drawing
	@author humkyung
	@date 2014.05.10
	@param	
	@return	output file name
*/
static CString GetOutputFileName(CaDraw_DraFile* pDraFile , const STRING_T& sIniFilePath)
{
	assert(pDraFile && "pDraFile is NULL");
	CString res;

	if(pDraFile)
	{
		res = pDraFile->GetFileNameWithoutExt().c_str();

		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Generation Item19") , _T("False") , szBuf , MAX_PATH , sIniFilePath.c_str());
		vector<STRING_T> oResult;
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if((oResult.size() >= 2) && (STRING_T(_T("True")) == oResult[1]))
		{
			const CString sDraIniFilePath = CString(STRING_T(pDraFile->GetFilePath() + _T(".ini")).c_str());
			const UINT uiCount = GetPrivateProfileInt(_T("Revision Data") , _T("Count") , 0 , sDraIniFilePath);
			if(uiCount > 0)
			{
				CString sKey;
				sKey.Format(_T("Data%d") , uiCount - 1);
				if(GetPrivateProfileString(_T("Revision Data") , sKey , _T("") , szBuf , MAX_PATH , sDraIniFilePath))
				{
					CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
					if(!oResult.empty()) res += CString(_T("_")) + oResult[0].c_str();
				}
			}
		}
	}

	return res;
}

/**
	@brief	generate command file
	@author HumKyung.BAEK
	@date 2010.05.28
	@param	
	@return		
*/
int CSmartDrawView::GenerateCmdFile(const CString& rFilePath , const CString& sCadFilePath , const CString& sHsrFilePath , const CString& sAnnoFilePath , 
									const CString& sOutputSettingFilePath , const double& dDrawingScale) const
{
	OFSTREAM_T ofile(rFilePath.operator LPCTSTR() , fstream::out | fstream::app);	/// open file for append mode
	if(ofile.is_open())
	{
		ofile << _T("NEWFILE ") << sCadFilePath.operator LPCTSTR() << std::endl;
		if(GetDocument()->IsMSTNOutput())
		{
			if(CFileTools::DoesFileExist(sHsrFilePath))
			{
				ofile << sHsrFilePath.operator LPCTSTR() << std::endl;
			}
			/// scale hidden drawing - 2014.05.10 added by humkyung
			if(1.0 != dDrawingScale)
			{
				ofile << _T("SCALE ") << dDrawingScale << std::endl;
			}
			/// up to here

			if(CFileTools::DoesFileExist(sAnnoFilePath))
			{
				ofile << sAnnoFilePath.operator LPCTSTR() << std::endl;
			}
		}
		else if(GetDocument()->IsAutoCADOutput())
		{
			if(!sOutputSettingFilePath.IsEmpty())
			{
				ofile << sOutputSettingFilePath.operator LPCTSTR() << std::endl;
			}

			if(!sHsrFilePath.IsEmpty())
			{
				ofile << sHsrFilePath.operator LPCTSTR() << std::endl;
			}
			/// scale hidden drawing - 2014.05.10 added by humkyung
			if(1.0 != dDrawingScale)
			{
				ofile << _T("scaleup ") << dDrawingScale << std::endl;
			}
			/// up to here

			ofile << sAnnoFilePath.operator LPCTSTR() << std::endl;
			ofile << _T("SAVE DESIGN") << std::endl;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	generate output for setting
	@author	humkyung
	@date	2014.06.29
*/
CString CSmartDrawView::GenerateSettingOutput(const CString& sIniFilePath)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	CString sTextStyleKeyName;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , sIniFilePath))
	{
		sTextStyleKeyName = CString(szBuf) + _T("_TEXT_STYLE");
	}else	return CString();

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(sTextStyleKeyName , _T("TextStyle List") , NULL , szBuf , MAX_PATH , sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
	}

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawDoc* pDoc = GetDocument();

	const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + pDoc->GetProjectName();
	CSplitPath path(sIniFilePath);
	const CString sOutputSettingFilePath = sPrjFolderPath + _T("\\Working\\") + path.GetFileName() + _T(".m2d");
	OFSTREAM_T ofile(sOutputSettingFilePath.operator LPCTSTR() , fstream::out | fstream::trunc);
	if(ofile.is_open())
	{
		for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			CSmartDrawAnnoTextStyle oTextStyle;
			oTextStyle.Parse(itr->c_str() , sIniFilePath.operator LPCTSTR());
			ofile << _T("<TEXTSTYLE>|") << oTextStyle.GetName() << _T(",") << oTextStyle.GetFontName() << _T(",") << oTextStyle.height() << _T(",");
			ofile << oTextStyle.GetWidthFactor() << _T(",") << oTextStyle.GetOblique() <<  _T(",False") << std::endl; 
		}

		return sOutputSettingFilePath;
	}

	return CString();
}

/**	
	@brief	generate output drawing from a dra file.
	@author	humkyung
	@date	2010.05.17
	@param	
	@return
*/
int CSmartDrawView::GenerateOutput(CaDraw_DraFile* pDraFile)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CSmartDrawDoc* pDoc = GetDocument();
		const int iGenerationType = pDoc->GetGenerationType();

		CPluginManager& inst = CPluginManager::GetInstance();
		const STRING_T& sDrawingType = pDoc->GetDrawingType();
		Plugin* pPlugin = inst.GetPluginOf(sDrawingType);
		if(NULL != pPlugin)
		{
			const CString sProjectName(pDoc->GetProjectName());
			const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + sProjectName;
			const CString sFileName(pDraFile->GetFileNameWithoutExt().c_str());

			CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
			CString sTitle(_T("Generate HSR"));
			int progress = 0;

			const STRING_T sHsrFilePath = STRING_T(sPrjFolderPath.operator LPCTSTR()) + _T("\\Working\\") + STRING_T(sFileName.operator LPCTSTR()) + _T(".hsr");
			/// generate .hsr file
			if(CSmartDrawDoc::HLR == (CSmartDrawDoc::HLR & iGenerationType))
			{
				if(pDlg) pDlg->UpdateWorkStatus(sTitle , progress);
				CSmartDrawPDSModuleImpl oCadModule;
				//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(sTitle.operator LPCTSTR()) );
				::DeleteFile(sHsrFilePath.c_str());
				CIsPoint3d ptModelOffset(ATOF_T(pDoc->GetModelOffsetX().c_str()) , ATOF_T(pDoc->GetModelOffsetY().c_str()) , ATOF_T(pDoc->GetModelOffsetZ().c_str()));
				oCadModule.GenerateHSR(sHsrFilePath , pDraFile , sPrjFolderPath.operator LPCTSTR() , sProjectName.operator LPCTSTR() , ptModelOffset);
			}

			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			const STRING_T sIniFilePath = sPrjFolderPath.operator LPCTSTR() + STRING_T(_T("\\Setting\\")) + pDraFile->GetSettingFileName() + STRING_T(_T(".ini"));
			const double dDrawingScale = GetDrawingScale(pDraFile , sIniFilePath);	/// 2014.05.10 added by humkyung

			CString sAnnoFilePath;
			if(CSmartDrawDoc::ANNOTATION == (CSmartDrawDoc::ANNOTATION & iGenerationType))
			{
				progress = 50;
				sTitle = _T("Generate Anno");
				if(pDlg) pDlg->UpdateWorkStatus(sTitle , progress);

				/// generate annotation file
				const CString sOutputFilePath = docData.GetServerFolderPath() + _T("\\") + sProjectName + _T("\\Output\\") + GetOutputFileName(pDraFile , sIniFilePath) + pDoc->GetDefaultExt();
				sAnnoFilePath = docData.GetServerFolderPath() + _T("\\") + sProjectName + _T("\\Working\\") + CString(pDraFile->GetFileNameWithoutExt().c_str()) + _T(".anno");
				//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(sTitle.operator LPCTSTR()) );
				/// up to here

				::DeleteFile(sAnnoFilePath);
				double dModelOffset[3]={ATOF_T(pDoc->GetModelOffsetX().c_str()) , ATOF_T(pDoc->GetModelOffsetY().c_str()) , ATOF_T(pDoc->GetModelOffsetZ().c_str())};
				int res = pPlugin->pfAnnotate(sOutputFilePath , pDraFile->GetFilePath().c_str() , sProjectName , sPrjFolderPath , dDrawingScale , dModelOffset);
			}

			if(CFileTools::DoesFileExist(sHsrFilePath.c_str()) || CFileTools::DoesFileExist(sAnnoFilePath))
			{
				const CString sOutputSettingFilePath = GenerateSettingOutput(sIniFilePath.c_str());

				/// generate cmd file and write on dgn file
				STRING_T sHsrType(_T("Import"));	/// convert pds hidden file to hsr file
				if(GetPrivateProfileString(_T("HLR_OPTIONS") , _T("Type") , _T("Import") , szBuf , MAX_PATH , sIniFilePath.c_str()))
				{
					sHsrType = szBuf;
				}

				STRING_T sDrawingFolder;
				if(GetPrivateProfileString(_T("Folder") , _T("Drawing") , _T("") , szBuf , MAX_PATH , pDoc->GetPrjSettingFilePath()))
				{
					sDrawingFolder = szBuf;
				}
				CString sOrgCadFilePath = CString(sDrawingFolder.c_str()) + _T("\\") + sFileName + pDoc->GetDefaultExt();
				if(_T("Generate") == sHsrType)
				{
					if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
					{
						sOrgCadFilePath = docData.GetServerFolderPath() + _T("\\") + sProjectName + _T("\\Border\\") + CString(szBuf);
					}
				}
				
				const CString sDestCadFilePath = docData.GetServerFolderPath() + _T("\\") + sProjectName + _T("\\Output\\") + GetOutputFileName(pDraFile , sIniFilePath) + pDoc->GetDefaultExt();
				::CopyFile(sOrgCadFilePath , sDestCadFilePath , FALSE);
				const CString sCmdFilePath = docData.GetServerFolderPath() + _T("\\") + pDoc->GetProjectName() + _T("\\Working\\pds.cmd");
				GenerateCmdFile(sCmdFilePath , sDestCadFilePath , ((_T("Generate") == sHsrType) ? sHsrFilePath.c_str() : _T("")) , sAnnoFilePath , sOutputSettingFilePath , dDrawingScale);
				///

				progress = 100;
				sTitle = _T("Complete");
				if(pDlg) pDlg->UpdateWorkStatus(sTitle , progress);

				//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(sTitle.operator LPCTSTR()) );
			}
			else
			{
				//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(CString(_T("Error on ") + CString(pDraFile->GetFileNameWithoutExt().c_str())).operator LPCTSTR()) );
				///TODO: show error log
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	popup option setting dialog
	@author	humkyung
	@date	2014.05.30
*/
void CSmartDrawView::OnOptionSetting()
{
	m_wndDraTreeListCtrl.OnDraEdit();
}

/**
	@brief	disable when there is no selected option,dra item
	@author	humkyung
	@date	2014.05.30
*/
void CSmartDrawView::OnUpdateOptionSetting(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = m_wndDraTreeListCtrl.GetSelectedItem();
	if(NULL != hItem)
	{
		int iImage = -1,iSelectedImage = -1;
		m_wndDraTreeListCtrl.GetItemImage(hItem , iImage , iSelectedImage);
		pCmdUI->Enable(ICON_ROOT != iImage);
		return;
	}

	pCmdUI->Enable(FALSE);
}