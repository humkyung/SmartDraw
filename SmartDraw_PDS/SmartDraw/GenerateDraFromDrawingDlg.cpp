// GenerateDraFromDrawingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "GenerateDraFromDrawingDlg.h"
#include <util/FileTools.h>
#include <util/SplitPath.h>
#include <util/Path.h>
#include "SmartDrawDoc.h"
#include "../../SmartDrawPDSModule/GenerateDraFromDrawing.h"
#include "../../SmartDrawPDSModule/BackgroundFile.h"

static bool __generated__ = false;

// CGenerateDraFromDrawingDlg dialog

IMPLEMENT_DYNAMIC(CGenerateDraFromDrawingDlg, CDialog)

CGenerateDraFromDrawingDlg::CGenerateDraFromDrawingDlg(const CString& sDraFolderPath , const CString& sDrawingFolderPath , 
													   const CString& sProjectUOR , const CString& sDatabaseFilePath , CWnd* pParent /*=NULL*/)
	: CDialog(CGenerateDraFromDrawingDlg::IDD, pParent)
{
	m_sDraFolderPath = sDraFolderPath;
	m_sDrawingFolderPath = sDrawingFolderPath;
	m_sProjectUOR = sProjectUOR;
	m_sDatabaseFilePath = sDatabaseFilePath;

	m_sVadFolderPath = _T("");	/// 2014.05.06 added by humkyung
}

CGenerateDraFromDrawingDlg::~CGenerateDraFromDrawingDlg()
{
}

void CGenerateDraFromDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_FILTER , m_cboFilter);
	DDX_Control(pDX , IDC_BUTTON_TOGGLE_SELECTION , m_btnToggleSelection);
	DDX_Control(pDX , IDC_LIST_PDS_DRAWING , m_wndDrawingListCtrl);
	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}


BEGIN_MESSAGE_MAP(CGenerateDraFromDrawingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TOGGLE_SELECTION, &CGenerateDraFromDrawingDlg::OnBnClickedButtonToggleSelection)
	ON_BN_CLICKED(IDOK, &CGenerateDraFromDrawingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGenerateDraFromDrawingDlg::OnBnClickedCancel)
	ON_CBN_SELENDOK(IDC_COMBO_FILTER, &CGenerateDraFromDrawingDlg::OnCbnSelendokComboFilter)
	ON_EN_CHANGE(IDC_EDIT_DRAWING_FILE_FOLDER, &CGenerateDraFromDrawingDlg::OnEnChangeEditDrawingFileFolder)
END_MESSAGE_MAP()


// CGenerateDraFromDrawingDlg message handlers

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawingDlg
    @function   OnBnClickedButtonToggleSelection
    @return     void
    @brief
******************************************************************************/
void CGenerateDraFromDrawingDlg::OnBnClickedButtonToggleSelection()
{
	const int iItemCount = m_wndDrawingListCtrl.GetItemCount();
	for(int i = 0;i < iItemCount;++i)
	{
		m_wndDrawingListCtrl.SetCheck(i , !m_wndDrawingListCtrl.GetCheck(i));
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawingDlg
    @function   OnInitDialog
    @return     BOOL
    @brief
******************************************************************************/
BOOL CGenerateDraFromDrawingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_cImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1))
	{
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_MSTNJ));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_NEW));
	}

	m_wndDrawingListCtrl.SetExtendedStyle(m_wndDrawingListCtrl.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

	m_wndDrawingListCtrl.InsertColumn(0 , _T("File Name") , LVCFMT_LEFT , 250);
	m_wndDrawingListCtrl.SetImageList(&m_cImages , LVSIL_SMALL);

	m_cboFilter.SetCurSel(0);	/// select hidden drawing
	OnCbnSelendokComboFilter();

	__generated__ = false;

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_btnCancel.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawingDlg
    @function   OnBnClickedOk
    @return     void
    @brief
******************************************************************************/
void CGenerateDraFromDrawingDlg::OnBnClickedOk()
{
	this->BeginWaitCursor();

	const int iFilter = m_cboFilter.GetCurSel();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	const int iItemCount = m_wndDrawingListCtrl.GetItemCount();
	int iCount = 0;
	for(int i = 0;i < iItemCount;++i)
	{
		const BOOL bChecked = m_wndDrawingListCtrl.GetCheck(i);
		if(FALSE == bChecked) continue;
		
		const CString sItemText = m_wndDrawingListCtrl.GetItemText(i , 0);
		CSplitPath path(sItemText);

		if(CGenerateDraFromDrawingDlg::PDS_HIDDEN == iFilter)			/// generate dra file from .dgn file
		{
			const double uor(ATOF_T(m_sProjectUOR.operator LPCTSTR()));
			CGenerateDraFromDrawing generator(uor);
			const CString sDrawingFilePath = IsTools::CPath::Combine(m_sDrawingFolderPath , sItemText , 0);
			if(ERROR_SUCCESS == generator.Generate(
				CString(m_sDraFolderPath + _T("\\") + path.GetFileName() + _T(".dra")).operator LPCTSTR() , 
				sDrawingFilePath.operator LPCTSTR(),
				m_sDatabaseFilePath.operator LPCTSTR()))
			{
				++iCount;
			}
		}
	}
	__generated__ = true;

	/// show import result - 2013.05.01 added by humkyung
	CString sMsg;
	sMsg.Format(_T("%d file(s) is imported") , iCount);
	AfxMessageBox(sMsg , MB_OK|MB_ICONINFORMATION);
	/// up to here

	this->EndWaitCursor();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawingDlg
    @function   OnBnClickedCancel
    @return     void
    @brief
******************************************************************************/
void CGenerateDraFromDrawingDlg::OnBnClickedCancel()
{
	(true == __generated__) ? OnOK() : OnCancel();
}

/**
	@brief	change folder for selected filter
	@author	humkyung
	@date	2014.05.06
*/
void CGenerateDraFromDrawingDlg::OnCbnSelendokComboFilter()
{
	const int iCurSel = m_cboFilter.GetCurSel();
	if(0 == iCurSel)
	{
		SetDlgItemText(IDC_EDIT_DRAWING_FILE_FOLDER , m_sDrawingFolderPath);
		((CEdit*)GetDlgItem(IDC_EDIT_DRAWING_FILE_FOLDER))->SetReadOnly(TRUE);
	}
	else if(1 == iCurSel)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_DRAWING_FILE_FOLDER))->SetReadOnly(FALSE);
		SetDlgItemText(IDC_EDIT_DRAWING_FILE_FOLDER , m_sVadFolderPath);
		GetDlgItem(IDC_EDIT_DRAWING_FILE_FOLDER)->SetFocus();
	}
}

/**
	@brief	will display contents in selected folder
	@author	humkyung
	@date	2014.05.06
*/
void CGenerateDraFromDrawingDlg::OnEnChangeEditDrawingFileFolder()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	m_wndDrawingListCtrl.DeleteAllItems();
	vector<CString> oFoundFiles;

	const int iCurSel = m_cboFilter.GetCurSel();
	CString sFilter;
	int iImage = 0;
	if(0 == iCurSel)
	{
		sFilter = _T("*.dgn");
		CFileTools::FindFiles(oFoundFiles , m_sDrawingFolderPath , sFilter , false);
		iImage = 0;
	}
	else
	{
		sFilter = _T("*.vad");
		GetDlgItemText(IDC_EDIT_DRAWING_FILE_FOLDER , m_sVadFolderPath);
		if('\\' == m_sVadFolderPath.Right(1)) m_sVadFolderPath.Left(m_sVadFolderPath.GetLength() - 1);
		CFileTools::FindFiles(oFoundFiles , m_sVadFolderPath , sFilter , false);
		iImage = 1;
	}
	
	for(vector<CString>::iterator itr = oFoundFiles.begin();itr != oFoundFiles.end();++itr)
	{
		CSplitPath path(*itr);
		const CString sFileName = path.GetFileName() + path.GetExtension();
		m_wndDrawingListCtrl.InsertItem(m_wndDrawingListCtrl.GetItemCount() , sFileName , iImage);
	}
}
