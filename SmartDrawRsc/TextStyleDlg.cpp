// TextStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDrawRsc.h"
#include "TextStyleDlg.h"
#include "CADFontGridCell.h"
#include "FilterGridCell.h"
#include <Tokenizer.h>

#include "Resource.h"

#define	ROW_HEIGHT	25

// CTextStyleDlg dialog

IMPLEMENT_DYNAMIC(CTextStyleDlg, CDialog)

CTextStyleDlg::CTextStyleDlg(const CString& rIniFilePath , CWnd* pParent /*=NULL*/)
	: CDialog(CTextStyleDlg::IDD, pParent) , m_rIniFilePath(rIniFilePath)
{

}

CTextStyleDlg::~CTextStyleDlg()
{
}

void CTextStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_NEW_STYLE , m_wndNewTextStyle);
	DDX_Control(pDX , IDC_BUTTON_DELETE_STYLE , m_wndDeleteTextStyle);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CTextStyleDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW_STYLE, &CTextStyleDlg::OnBnClickedButtonNewStyle)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_STYLE, &CTextStyleDlg::OnBnClickedButtonDeleteStyle)
	ON_BN_CLICKED(IDOK, &CTextStyleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTextStyleDlg message handlers

BOOL CTextStyleDlg::OnInitDialog()
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_TEXT_STYLE),IMAGE_ICON,0,0,0);
		HICON hOldIcon = SetIcon(hIcon , FALSE);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_NEW),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndNewTextStyle.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndNewTextStyle.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_DELETE),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndDeleteTextStyle.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndDeleteTextStyle.SetIcon(hIcon);
	}

	CCADFontGridCell::m_rIniFilePath = m_rIniFilePath;

	CRect rect;
	GetDlgItem(IDC_STATIC_TEXT_STYLE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.InflateRect(5 , 5);
	BOOL b = m_wndGridCtrl.Create(rect , this , 0x100);
	if(TRUE == b)
	{
		//그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		m_wndGridCtrl.SetFont(&font);
		font.DeleteObject();

		m_ImageList.Create(IDB_TEXT_STYLE , 16 , 16 , RGB(255 , 255 , 255));
		m_wndGridCtrl.SetImageList(&m_ImageList);

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetColumnCount(6);
		m_wndGridCtrl.SetRowHeight(0 , ROW_HEIGHT);

		m_wndGridCtrl.SetItemText(0 , 0 , _T("Text Style"));
		m_wndGridCtrl.SetItemText(0 , 1 , _T("Font Name"));
		m_wndGridCtrl.SetItemText(0 , 2 , _T("Font Height"));
		m_wndGridCtrl.SetItemText(0 , 3 , _T("Width Factor"));
		m_wndGridCtrl.SetItemText(0 , 4 , _T("Oblique"));
		m_wndGridCtrl.SetItemText(0 , 5 , _T("Weight"));

		// fill it up with stuff
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

		UpdateContents();
		m_wndGridCtrl.AutoSizeColumns();
		m_wndGridCtrl.SetColumnWidth(1 , 120);

		m_wndGridCtrl.SetColumnWidth(0 , 180);
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

	@date	2010.05.31
*/
int CTextStyleDlg::UpdateContents()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	CString rTextStyleAppName;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rTextStyleAppName = CString(szBuf) + _T("_TEXT_STYLE");
	}else	return ERROR_BAD_ENVIRONMENT;

	vector<STRING_T> oTextStyleResult;
	if(GetPrivateProfileString(rTextStyleAppName , _T("TextStyle List") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oTextStyleResult , szBuf , CIsComma());
		m_wndGridCtrl.SetRowCount(oTextStyleResult.size() + 1);
	}

	vector<STRING_T> oResult;
	for(int i = 0;i < int(oTextStyleResult.size());++i)
	{
		if(GetPrivateProfileString(rTextStyleAppName , oTextStyleResult[i].c_str() , NULL , szBuf , MAX_PATH , m_rIniFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(5 == oResult.size())
			{
				m_wndGridCtrl.SetRowHeight(i + 1 , ROW_HEIGHT);
				m_wndGridCtrl.SetItemImage(i + 1 , 0 , 0);
				m_wndGridCtrl.SetItemText(i + 1,0,oTextStyleResult[i].c_str());
				m_wndGridCtrl.SetItemText(i + 1,1,oResult[0].c_str());
				m_wndGridCtrl.SetCellType(i + 1,1,RUNTIME_CLASS(CCADFontGridCell));
				m_wndGridCtrl.SetItemText(i + 1,2,oResult[1].c_str());
				m_wndGridCtrl.SetCellType(i + 1,2,RUNTIME_CLASS(CFilterGridCell));
				m_wndGridCtrl.SetItemText(i + 1,3,oResult[2].c_str());
				m_wndGridCtrl.SetCellType(i + 1,3,RUNTIME_CLASS(CFilterGridCell));
				m_wndGridCtrl.SetItemText(i + 1,4,oResult[3].c_str());
				m_wndGridCtrl.SetCellType(i + 1,4,RUNTIME_CLASS(CFilterGridCell));
				m_wndGridCtrl.SetItemText(i + 1,5,oResult[4].c_str());
				m_wndGridCtrl.SetCellType(i + 1,5,RUNTIME_CLASS(CFilterGridCell));
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
*/
void CTextStyleDlg::OnBnClickedButtonNewStyle()
{
	m_wndGridCtrl.SetRowCount(m_wndGridCtrl.GetRowCount() + 1);
	const int nRowCount = m_wndGridCtrl.GetRowCount();
	m_wndGridCtrl.SetRowHeight(nRowCount - 1 , ROW_HEIGHT);
	m_wndGridCtrl.SetItemImage(nRowCount - 1 , 0 , 0);

	m_wndGridCtrl.SetCellType(nRowCount - 1,1,RUNTIME_CLASS(CCADFontGridCell));
	m_wndGridCtrl.SetCellType(nRowCount - 1,2,RUNTIME_CLASS(CFilterGridCell));
	m_wndGridCtrl.SetCellType(nRowCount - 1,3,RUNTIME_CLASS(CFilterGridCell));
	m_wndGridCtrl.SetCellType(nRowCount - 1,4,RUNTIME_CLASS(CFilterGridCell));
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
*/
void CTextStyleDlg::OnBnClickedButtonDeleteStyle()
{
	CCellRange range = m_wndGridCtrl.GetSelectedCellRange();
	const int nMinRow = range.GetMinRow();
	if(-1 != nMinRow)
	{
		m_wndGridCtrl.DeleteRow(nMinRow);
		WriteTextStyles();
	}
}

/**
	@brief	사용자가 EDIT를 끝냈을 때...

	@author	HumKyung.BAEK

	@date	2010.05.31
**/
BOOL CTextStyleDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if(wParam == m_wndGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if(GVN_ENDLABELEDIT == pGridView->hdr.code)
		{
			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			CString rTextStyleAppName;
			if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
			{
				rTextStyleAppName = CString(szBuf) + _T("_TEXT_STYLE");
			}

			CString rValue;
			const int nColumnCount = m_wndGridCtrl.GetColumnCount();
			for(int i = 1;i < nColumnCount;++i)
			{
				rValue += m_wndGridCtrl.GetItemText(pGridView->iRow , i);
				if(i < (nColumnCount - 1)) rValue += _T(",");
			}
			CString rKeyName(m_wndGridCtrl.GetItemText(pGridView->iRow , 0));
			WritePrivateProfileString(rTextStyleAppName , rKeyName , rValue , m_rIniFilePath);

			CString rTextStyleList;
			const int nRowCount = m_wndGridCtrl.GetRowCount();
			for(int i = 1;i < nRowCount;++i)
			{
				rTextStyleList += m_wndGridCtrl.GetItemText(i , 0);
				if(i < nRowCount - 1) rTextStyleList += _T(",");
			}
			WritePrivateProfileString(rTextStyleAppName , _T("TextStyle List") , rTextStyleList , m_rIniFilePath);
		}
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	TextStyle들을 파일에 저장합니다.

	@author	HumKyung.BAEK

	@date	2010.05.31
**/
int CTextStyleDlg::WriteTextStyles(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CString rTextStyleAppName;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rTextStyleAppName = CString(szBuf) + _T("_TEXT_STYLE");
	}

	CString rTextStyleList;
	const int nRowCount = m_wndGridCtrl.GetRowCount();
	for(int i = 1;i < nRowCount;++i)
	{
		CString rValue;
		const int nColumnCount = m_wndGridCtrl.GetColumnCount();
		for(int j = 1;j < nColumnCount;++j)
		{
			rValue += m_wndGridCtrl.GetItemText(i , j);
			if(j < (nColumnCount - 1)) rValue += _T(",");
		}
		CString rKeyName(m_wndGridCtrl.GetItemText(i , 0));
		WritePrivateProfileString(rTextStyleAppName , rKeyName , rValue , m_rIniFilePath);

		rTextStyleList += m_wndGridCtrl.GetItemText(i , 0);
		if(i < nRowCount - 1) rTextStyleList += _T(",");
	}

	WritePrivateProfileString(rTextStyleAppName , _T("TextStyle List") , rTextStyleList , m_rIniFilePath);

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
**/
void CTextStyleDlg::OnBnClickedOk()
{
	WriteTextStyles();

	OnOK();
}
