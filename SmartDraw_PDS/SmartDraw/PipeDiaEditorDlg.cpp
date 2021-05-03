// PipeDiaEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <aDraw_PipeDiaTable.h>
#include "PipeDiaEditorDlg.h"
#include "SmartDrawDoc.h"
#include "ProjectExplorer.h"

#include "soci.h"
#include "soci-sqlite3.h"

// CPipeDiaEditorDlg dialog

IMPLEMENT_DYNAMIC(CPipeDiaEditorDlg, CDialog)

CPipeDiaEditorDlg::CPipeDiaEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPipeDiaEditorDlg::IDD, pParent)
{

}

CPipeDiaEditorDlg::~CPipeDiaEditorDlg()
{
	try
	{
		for(vector<CGridCtrl*>::iterator itr = m_oGridCtrlList.begin();itr != m_oGridCtrlList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
	}
	catch(...)
	{}
}

void CPipeDiaEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_BUTTON_NEW_PIPE_DIA_DATA , m_btnNewPipeDiaData);
	DDX_Control(pDX , IDC_BUTTON_DELETE_PIPE_DIA_DATA , m_btnDeletePipeDiaData);
	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}


BEGIN_MESSAGE_MAP(CPipeDiaEditorDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW_PIPE_DIA_DATA, &CPipeDiaEditorDlg::OnBnClickedButtonNewPipeDiaData)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_PIPE_DIA_DATA, &CPipeDiaEditorDlg::OnBnClickedButtonDeletePipeDiaData)
	ON_BN_CLICKED(IDOK, &CPipeDiaEditorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CPipeDiaEditorDlg::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CPipeDiaEditorDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CPipeDiaEditorDlg message handlers
/******************************************************************************
	@brief		initialize grid control
	@author     humkyung
    @date       2015-07-15
    @function   InitGridCtrl
    @return     int
******************************************************************************/
int CPipeDiaEditorDlg::InitGridCtrl()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	list<STRING_T> oCodeList;
	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	{
		soci::rowset<soci::row> rs(oSession.prepare << _T("select * from codes"));
		for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
		{
			const STRING_T sValue = itr->get<STRING_T>(0);
			oCodeList.push_back(sValue);
		}
	}

	/// remove all tabs
	m_wndTabCtrl.RemoveAllTabs();
	for(vector<CGridCtrl*>::iterator itr = m_oGridCtrlList.begin();itr != m_oGridCtrlList.end();++itr)
	{
		(*itr)->DestroyWindow();
		SAFE_DELETE(*itr);
	}
	m_oGridCtrlList.clear();
	/// up to here

	CRect rect;
	GetDlgItem(IDC_STATIC_TAB_CTRL)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	for(list<STRING_T>::const_iterator itr = oCodeList.begin();itr != oCodeList.end();++itr)
	{
		CGridCtrl* pGridCtrl = new CGridCtrl();
		if(TRUE == pGridCtrl->Create(rect , &m_wndTabCtrl , WM_USER + m_oGridCtrlList.size()))
		{
			const CString COLUMNS[]={_T("") , _T("From Size(mm)") , _T("To Size(mm)") , _T("O.D(mm)")};
			
			/// 그리드 컨트롤 폰트 코드
			CFont font;
			font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
			pGridCtrl->SetFont(&font);
			font.DeleteObject();

			pGridCtrl->SetFixedRowCount(1);
			pGridCtrl->SetFixedColumnCount(1);

			pGridCtrl->SetColumnCount(sizeof(COLUMNS) / sizeof(COLUMNS[0]));
			pGridCtrl->SetItemText(0 , 0 , _T(""));
			pGridCtrl->SetColumnWidth(0 , 60);
			for(int col = 1;col < pGridCtrl->GetColumnCount();++col)
			{
				pGridCtrl->SetColumnWidth(col , 70);
				pGridCtrl->SetItemText(0 , col , COLUMNS[col]);
			}
			
			CString str;
			pGridCtrl->SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);
			OSTRINGSTREAM_T oss;
			oss << _T("select FromSize,ToSize,OD from ") << itr->c_str();
			soci::rowset<soci::row> rs(oSession.prepare << oss.str());
			for(soci::rowset<soci::row>::const_iterator jtr = rs.begin();jtr != rs.end();++jtr)
			{
				const int iRows = pGridCtrl->GetRowCount();
				pGridCtrl->SetRowCount(iRows + 1);
				pGridCtrl->SetRowHeight(iRows , DEFAULT_GRID_ROW_HEIGHT);
				str.Format(_T("%d") , iRows);
				pGridCtrl->SetItemText(iRows , 0 , str);

				const double dFromSize = jtr->get<double>(0);
				oss.str(_T("")); oss << dFromSize;
				pGridCtrl->SetItemText(iRows , 1 , oss.str().c_str());

				const double dToSize = jtr->get<double>(1);
				oss.str(_T("")); oss << dToSize;
				pGridCtrl->SetItemText(iRows , 2 , oss.str().c_str());

				const double dOD = jtr->get<double>(2);
				oss.str(_T("")); oss << dOD;
				pGridCtrl->SetItemText(iRows , 3 , oss.str().c_str());
			}

			/// fill it up with stuff
			pGridCtrl->SetEditable(TRUE);
			pGridCtrl->EnableDragAndDrop(FALSE);
			pGridCtrl->SetTextBkColor(RGB(255, 255, 224));
			pGridCtrl->SetRowResize(FALSE);
			pGridCtrl->SetColumnResize(TRUE);
			pGridCtrl->SetListMode(FALSE);
			pGridCtrl->SetSingleRowSelection(TRUE);
			pGridCtrl->SetHeaderSort(FALSE);
			pGridCtrl->SetSingleColSelection(TRUE);
			///////////////////////////////////////////////////////////////////
			pGridCtrl->AutoSizeColumns();

			pGridCtrl->ShowWindow(SW_SHOW);
			m_wndTabCtrl.AddTab(pGridCtrl,itr->c_str());

			m_oGridCtrlList.push_back(pGridCtrl);
		}
		else
		{
			SAFE_DELETE(pGridCtrl);
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

BOOL CPipeDiaEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/// apply language setting
	CString rString;
	this->GetWindowText(rString);
	this->SetWindowText(_TR(rString));
	/// up to here

	CRect rect;
	GetDlgItem(IDC_STATIC_TAB_CTRL)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	if (!m_wndTabCtrl.Create (CMFCTabCtrl::STYLE_3D_VS2005, rect , this , 0x100 + 1))
	{
		TRACE0("Failed to create Properies Grid \n");
		return ERROR_BAD_ENVIRONMENT;      /// fail to create
	}
	m_wndTabCtrl.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabCtrl.SetActiveTabBoldFont(TRUE);
	m_wndTabCtrl.EnableAutoColor(TRUE);
	m_wndTabCtrl.SetWindowPos(&(CWnd::wndTop) , 0 , 0 , 0 , 0 , SWP_NOSIZE | SWP_NOMOVE);
	m_wndTabCtrl.ShowWindow(SW_SHOW);

	/// apply language setting
	GetDlgItem(IDC_BUTTON_IMPORT)->SetWindowText(_TR("Import"));
	GetDlgItem(IDC_BUTTON_EXPORT)->SetWindowText(_TR("Export"));
	/// up to here

	InitGridCtrl();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_NEW),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_btnNewPipeDiaData.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DELETE),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_btnDeletePipeDiaData.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OK), IMAGE_ICON, 16,16, 0 );
	m_btnOK.SetIcon(hOKIcon);
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON, 16,16, 0 );
	m_btnCancel.SetIcon(hCancelIcon);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	insert a new pipe dia. data
	@author	humkyung
	@date	2015.07.17
*/
void CPipeDiaEditorDlg::OnBnClickedButtonNewPipeDiaData()
{	
	const int iTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = static_cast<CGridCtrl*>(m_wndTabCtrl.GetTabWnd(iTab));
	if(NULL != pGridCtrl)
	{
		CCellRange rng = pGridCtrl->GetSelectedCellRange();
		if(rng.Count() > 0)
		{
			CString str;
			str.Format(_T("%d") , pGridCtrl->GetRowCount() - 1);
			const int at = rng.GetMinRow();
			int iRow = pGridCtrl->InsertRow(str , at);
			pGridCtrl->SetRowHeight(iRow , DEFAULT_GRID_ROW_HEIGHT);
		}
		else
		{
			pGridCtrl->SetRowCount(pGridCtrl->GetRowCount() + 1);
			pGridCtrl->SetRowHeight(pGridCtrl->GetRowCount() - 1 , DEFAULT_GRID_ROW_HEIGHT);

			CString str;
			str.Format(_T("%d") , pGridCtrl->GetRowCount() - 1);
			pGridCtrl->SetItemText(pGridCtrl->GetRowCount() - 1 , 0 , str);
		}
		pGridCtrl->Invalidate();

		CString str;
		for(int i = 1;i < pGridCtrl->GetRowCount();++i)
		{
			str.Format(_T("%d") , i);
			pGridCtrl->SetItemText(i , 0 , str);
		}
	}
}

/**
	@brief	delete selected pipe dia data
	@author	humkyung
	@date	2015.07.17
*/
void CPipeDiaEditorDlg::OnBnClickedButtonDeletePipeDiaData()
{
	const int iTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = static_cast<CGridCtrl*>(m_wndTabCtrl.GetTabWnd(iTab));
	if(NULL != pGridCtrl)
	{
		CCellRange range = pGridCtrl->GetSelectedCellRange();
		for(int i = range.GetMaxRow();i >= range.GetMinRow();--i)
		{
			pGridCtrl->DeleteRow(i);
		}
		pGridCtrl->Invalidate();

		CString str;
		for(int i = 1;i < pGridCtrl->GetRowCount();++i)
		{
			str.Format(_T("%d") , i);
			pGridCtrl->SetItemText(i , 0 , str);
		}
	}
}

/**
	@brief	save pipe dia datas
	@author	humkyung
	@date	2015.07.17
*/
void CPipeDiaEditorDlg::OnBnClickedOk()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	try
	{
		soci::transaction txn(oSession);
		for(int iTab = 0 ;iTab < m_wndTabCtrl.GetTabsNum();++iTab)
		{
			CString rString;
			m_wndTabCtrl.GetTabLabel(iTab,rString);
			oSession << _T("delete from ") << rString.operator LPCTSTR();

			CGridCtrl* pGridCtrl = static_cast<CGridCtrl*>(m_wndTabCtrl.GetTabWnd(iTab));
			if(NULL != pGridCtrl)
			{
				for(int row = 1;row < pGridCtrl->GetRowCount();++row)
				{
					CString sValue;
					sValue = pGridCtrl->GetItemText(row , 1);
					if(sValue.IsEmpty()) continue;
					const double dFromSize = ATOF_T(sValue);
					sValue = pGridCtrl->GetItemText(row , 2);
					if(sValue.IsEmpty()) continue;
					const double dToSize = ATOF_T(sValue);
					sValue = pGridCtrl->GetItemText(row , 3);
					if(sValue.IsEmpty()) continue;
					const double dOD = ATOF_T(sValue);

					oSession << _T("insert into ") << rString.operator LPCTSTR() << _T("(FromSize,ToSize,Inch,OD) values(") 
						<< dFromSize << _T(",") << dToSize << _T(",0,") << dOD;
				}
			}
		}
		txn.commit();

		pDoc->LoadPipeDiaTable();
	}
	catch(...){}

	OnOK();
}

/**
	@brief	import pipe dia. table
	@author	humkyung
	@date	2016.02.18
*/
void CPipeDiaEditorDlg::OnBnClickedButtonImport()
{
	CSmartDrawApp* pApp = static_cast<CSmartDrawApp*>(AfxGetApp());
	try
	{
		const CString sExecPath = GetExecPath();
		CSmartDrawDoc* pDoc = CSmartDrawDoc::GetInstance();
		if(NULL == pDoc) return;

		CWnd* pImportButton = GetDlgItem(IDC_BUTTON_IMPORT);
		pImportButton->EnableWindow(FALSE);
		{
			const CString sExecFilePath = GetExecPath() + _T("\\PipeDiaEditor.exe");
			const CString sArgs = (_T("\"") + sExecFilePath + _T("\" \"Import\" \"") + CString(pDoc->GetRefFilePath().c_str()) + _T("\""));

			DWORD exitCode = 0;
			executeCommandLine(sArgs , exitCode);
		}
		pImportButton->EnableWindow(TRUE);

		InitGridCtrl();
		/// update project code list in project properties
		CProjectExplorer* pProjectExplorer = CProjectExplorer::GetInstance();
		pProjectExplorer->ActivateTreeItem(pDoc);
		/// up to here
	}
	catch(std::exception ex)
	{
		AfxMessageBox(ex.what());
	}
}

/**
	@brief	export pipe dia. table
	@author	humkyung
	@date	2016.02.18
*/
void CPipeDiaEditorDlg::OnBnClickedButtonExport()
{
	CSmartDrawApp* pApp = static_cast<CSmartDrawApp*>(AfxGetApp());
	try
	{
		const CString sExecPath = GetExecPath();
		CSmartDrawDoc* pDoc = CSmartDrawDoc::GetInstance();
		if(NULL == pDoc) return;

		CWnd* pExportButton = GetDlgItem(IDC_BUTTON_IMPORT);
		pExportButton->EnableWindow(FALSE);
		{
			const CString sExecFilePath = GetExecPath() + _T("\\PipeDiaEditor.exe");
			const CString sArgs = (_T("\"") + sExecFilePath + _T("\" \"Export\" \"") + CString(pDoc->GetRefFilePath().c_str()) + _T("\""));

			DWORD exitCode = 0;
			executeCommandLine(sArgs , exitCode);
		}
		pExportButton->EnableWindow(TRUE);
	}
	catch(std::exception ex)
	{
		AfxMessageBox(ex.what());
	}
}
