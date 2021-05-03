// RevisionManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "RevisionManagerDlg.h"


// CRevisionManagerDlg dialog

IMPLEMENT_DYNAMIC(CRevisionManagerDlg, CDialog)

CRevisionManagerDlg::CRevisionManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRevisionManagerDlg::IDD, pParent)
{

}

CRevisionManagerDlg::~CRevisionManagerDlg()
{
}

void CRevisionManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_NEW_REV_DATA , m_btnNewRevData);
	DDX_Control(pDX , IDC_BUTTON_DELETE_REV_DATA , m_btnDeleteRevData);
	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}

BEGIN_MESSAGE_MAP(CRevisionManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW_REV_DATA, &CRevisionManagerDlg::OnBnClickedButtonNewRevData)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_REV_DATA, &CRevisionManagerDlg::OnBnClickedButtonDeleteRevData)
	ON_BN_CLICKED(IDOK, &CRevisionManagerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRevisionManagerDlg message handlers

BOOL CRevisionManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_NEW),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_btnNewRevData.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_DELETE),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_btnDeleteRevData.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	InitGridCtrl();

	this->SetIcon((HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16,16, 0 ) , FALSE);

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK), IMAGE_ICON, 16,16, 0 );
	m_btnOK.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON, 16,16, 0 );
	m_btnCancel.SetIcon(hCancelIcon);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
	@brief		initialize grid control
	@author     humkyung
    @date       2014-11-17
    @function   InitGridCtrl
    @return     int
******************************************************************************/
int CRevisionManagerDlg::InitGridCtrl()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	CRect rect;
	GetDlgItem(IDC_STATIC_REV_DATA_INFO)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(0 , 0 , 0 , 0);
	BOOL b = m_wndGridCtrl.Create(rect , this , 0x100);
	if(TRUE == b)
	{
		vector<CRevisionData*>* pRevDataList = pDoc->m_oRevisionSource.GetRevisionDataList();

		/// 그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		m_wndGridCtrl.SetFont(&font);
		font.DeleteObject();

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetFixedColumnCount(1);

		m_wndGridCtrl.SetColumnCount(sizeof(CRevisionChart::COLUMNS)/sizeof(STRING_T) + 1);
		m_wndGridCtrl.SetItemText(0 , 0 , _T(""));
		m_wndGridCtrl.SetColumnWidth(0 , 60);
		for(int col = 1;col < m_wndGridCtrl.GetColumnCount();++col)
		{
			m_wndGridCtrl.SetColumnWidth(col , 70);
			m_wndGridCtrl.SetItemText(0 , col , CRevisionChart::COLUMNS[col - 1].c_str());
		}
		m_wndGridCtrl.SetRowCount(pRevDataList->size() + 1);

		CString str;
		int i = 0;
		m_wndGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);
		for(vector<CRevisionData*>::iterator itr = pRevDataList->begin();itr != pRevDataList->end();++itr,++i)
		{
			m_wndGridCtrl.SetRowHeight(i + 1 , DEFAULT_GRID_ROW_HEIGHT);
			str.Format(_T("%d") , i + 1);
			m_wndGridCtrl.SetItemText(i + 1 , 0 , str);

			for(int j = 1;j < m_wndGridCtrl.GetColumnCount();++j)
			{
				const CString sColumn = m_wndGridCtrl.GetItemText(0 , j);
				const STRING_T sValue = (*itr)->Value(sColumn.operator LPCTSTR());
				m_wndGridCtrl.SetItemText(i + 1 , j , sValue.c_str());
			}
		}

		/// fill it up with stuff
		m_wndGridCtrl.SetEditable(TRUE);
		m_wndGridCtrl.EnableDragAndDrop(FALSE);
		m_wndGridCtrl.SetTextBkColor(RGB(255, 255, 224));
		m_wndGridCtrl.SetRowResize(FALSE);
		m_wndGridCtrl.SetColumnResize(TRUE);
		m_wndGridCtrl.SetListMode(FALSE);
		m_wndGridCtrl.SetSingleRowSelection(TRUE);
		m_wndGridCtrl.SetHeaderSort(FALSE);
		m_wndGridCtrl.SetSingleColSelection(TRUE);
		/////////////////////////////////////////////////////////////////

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/******************************************************************************
    @brief		add a new revision data
	@author     humkyung
    @date       2012-03-31
    @function   OnBnClickedButtonNewRevData
    @return     void
******************************************************************************/
void CRevisionManagerDlg::OnBnClickedButtonNewRevData()
{
	m_wndGridCtrl.SetRowCount(m_wndGridCtrl.GetRowCount() + 1);
	m_wndGridCtrl.SetRowHeight(m_wndGridCtrl.GetRowCount() - 1 , DEFAULT_GRID_ROW_HEIGHT);
	
	CString str;
	str.Format(_T("%d") , m_wndGridCtrl.GetRowCount() - 1);
	m_wndGridCtrl.SetItemText(m_wndGridCtrl.GetRowCount() - 1 , 0 , str);
}

/******************************************************************************
	@brief		delete selected rows
	@author     humkyung
    @date       2012-03-31
    @function   OnBnClickedButtonDeleteRevData
    @return     void
******************************************************************************/
void CRevisionManagerDlg::OnBnClickedButtonDeleteRevData()
{
	CCellRange range = m_wndGridCtrl.GetSelectedCellRange();
	for(int i = range.GetMaxRow();i >= range.GetMinRow();--i)
	{
		m_wndGridCtrl.DeleteRow(i);
	}
	m_wndGridCtrl.Invalidate();

	CString str;
	for(int i = 1;i < m_wndGridCtrl.GetRowCount();++i)
	{
		str.Format(_T("%d") , i);
		m_wndGridCtrl.SetItemText(i , 0 , str);
	}
}

/******************************************************************************
    @brief		save revision data to xml file
	@author     humkyung
    @date       2012-03-31
    @function   OnBnClickedOk
    @return     void
******************************************************************************/
void CRevisionManagerDlg::OnBnClickedOk()
{
	const int iCount = m_wndGridCtrl.GetRowCount() - 1;
	
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	vector<CRevisionData*>* pRevDataList = pDoc->m_oRevisionSource.GetRevisionDataList();
	if(NULL != pRevDataList)
	{
		CString rString;

		int iIndex = 0;
		for(vector<CRevisionData*>::iterator itr = pRevDataList->begin();itr != pRevDataList->end();++iIndex)
		{
			if(iIndex < iCount)
			{
				for(int col = 1;col < m_wndGridCtrl.GetColumnCount();++col)
				{
					const CString sColumn = m_wndGridCtrl.GetItemText(0 , col);
					rString = m_wndGridCtrl.GetItemText(iIndex + 1 , col);
					(*itr)->Value(sColumn.operator LPCTSTR()) = rString.operator LPCTSTR();
				}
			}
			else
			{
				SAFE_DELETE( *itr );
				itr = pRevDataList->erase(itr);
				continue;
			}
			++itr;
		}

		/// add new revision datas to list
		if(iIndex < iCount)
		{
			for(int i = iIndex;i < iCount;++i)
			{
				CRevisionData* pRevData = new CRevisionData;
				if(NULL != pRevData)
				{
					for(int col = 1;col < m_wndGridCtrl.GetColumnCount();++col)
					{
						const CString sColumn = m_wndGridCtrl.GetItemText(0 , col);
						rString = m_wndGridCtrl.GetItemText(i + 1 , col);
						pRevData->Value(sColumn.operator LPCTSTR()) = rString.operator LPCTSTR();
					}
					pRevDataList->push_back( pRevData );
				}
			}
		}
	}
	pDoc->m_oRevisionSource.Save();

	OnOK();
}