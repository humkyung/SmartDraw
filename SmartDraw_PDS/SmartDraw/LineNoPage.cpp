// LineNoPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <gui/enablebuddybutton.h>
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h."
#include "LineNoPage.h"
#include "LineNoLabelFormatDlg.h"

using namespace PropertyPage;
// CLineNoPage dialog

IMPLEMENT_DYNAMIC(CLineNoPage, CSmartDrawPropertyPage)

CLineNoPage::CLineNoPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CLineNoPage::IDD, pParent)
{

}

CLineNoPage::~CLineNoPage()
{
}

void CLineNoPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_ELEVATION_TYPE_BY_SPEC , m_wndElevationTypeBySpec);
	DDX_Control(pDX , IDC_BUTTON_SET_SIZE_BY_SPEC , m_wndSetSizeBySpecButton);
	DDX_Control(pDX , IDC_CHECK_TRIM_END , m_btnTrimEnd);
}


BEGIN_MESSAGE_MAP(CLineNoPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_LINE_NO_DIM_SIZE_ALL, &CLineNoPage::OnBnClickedLineNoDimSizeAll)
	ON_BN_CLICKED(IDC_RADIO_LINE_NO_DIM_OVER_SIZE, &CLineNoPage::OnBnClickedLineNoDimOverSize)
	ON_BN_CLICKED(IDC_BUTTON_ELEVATION_TYPE_BY_SPEC, &CLineNoPage::OnBnClickedButtonElevationTypeBySpec)
END_MESSAGE_MAP()


// CLineNoPage message handlers
/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.25
*/
BOOL PropertyPage::CLineNoPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	/// fill line no over size combobox
	CComboBox* pDimComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO_DIM_OVER_SIZE);
	list<CSmartDrawDocData::SizeRecord>* pSizeRecordList = docData.GetSizeRecordList();
	for(list<CSmartDrawDocData::SizeRecord>::const_iterator itr = pSizeRecordList->begin();itr != pSizeRecordList->end();++itr)
	{
		CString rString(itr->inch.c_str() + CString(_T("-")) + itr->metric.c_str() + CString(_T("mm")));
		pDimComboBox->AddString(rString);
	}

	///#18(http://atools.co.kr:9002/redmine/issues/18) - 2016.03.12 added by humkyung
	CString rString;
	GetDlgItem(IDC_BUTTON_ELEVATION_TYPE_BY_SPEC)->GetWindowText(rString);
	GetDlgItem(IDC_BUTTON_ELEVATION_TYPE_BY_SPEC)->SetWindowText(_TR(rString.operator LPCTSTR()));
	///up to here

	m_btnTrimEnd.SetWindowText(_TR("Trim End '-' in Line No"));

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CLineNoPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CLineNoPage::LoadData()
{
	static const CString sApp(_T("Line Number"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	/// 2014.06.13 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Display Dim. Style") , _T("All") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_DIM_SIZE_ALL);
		pButton->SetCheck(STRING_T(_T("All")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_DIM_OVER_SIZE);
		pButton->SetCheck(STRING_T(_T("All")) != szBuf);
		if(TRUE == pButton->GetCheck())
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO_DIM_OVER_SIZE);
			const int at = pComboBox->FindString(-1 , szBuf);
			pComboBox->SetCurSel((-1 != at) ? at : 0);
		}
	}
	/// up to here

	if(GetPrivateProfileString(sApp , _T("CL_Prefix") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_CENTER , szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("BOP_Prefix") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_BOP , szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("TOP_Prefix") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_TOP , szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("WP_Prefix") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_WP , szBuf);
	}

	///2016.04.01 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Trim End -") , _T("False") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnTrimEnd.SetCheck((_T("True") == STRING_T(szBuf)) ? TRUE : FALSE);
	}
	///up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CLineNoPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CLineNoPage::SaveData()
{
	static const CString sApp(_T("Line Number"));

	CString rString;
	/// 2014.06.13 added by humkyung
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_DIM_SIZE_ALL);
	if(pButton->GetCheck())
	{
		rString = _T("All");
	}
	else
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO_DIM_OVER_SIZE);
		int at = pComboBox->GetCurSel();
		if(-1 != at) pComboBox->GetLBText(at , rString);
	}
	WritePrivateProfileString(sApp , _T("Display Dim. Style") , rString , m_sIniFilePath);
	/// up to here

	GetDlgItemText(IDC_EDIT_CENTER , rString);
	WritePrivateProfileString(sApp , _T("CL_Prefix") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_BOP , rString);
	WritePrivateProfileString(sApp , _T("BOP_Prefix") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_TOP , rString);
	WritePrivateProfileString(sApp , _T("TOP_Prefix") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_WP , rString);
	WritePrivateProfileString(sApp , _T("WP_Prefix") , rString , m_sIniFilePath);

	///2016.04.01 added by humkyung
	WritePrivateProfileString(sApp , _T("Trim End -") , (TRUE == m_btnTrimEnd.GetCheck()) ? _T("True") : _T("False") , m_sIniFilePath);
	///up to here

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	humkyung
	@date	2014.06.14
*/
void PropertyPage::CLineNoPage::OnBnClickedLineNoDimSizeAll()
{
	GetDlgItem(IDC_COMBO_LINE_NO_DIM_OVER_SIZE)->EnableWindow(FALSE);
}

/**
	@brief	
	@author	humkyung
	@date	2014.06.14
*/
void PropertyPage::CLineNoPage::OnBnClickedLineNoDimOverSize()
{
	GetDlgItem(IDC_COMBO_LINE_NO_DIM_OVER_SIZE)->EnableWindow(TRUE);
}

/**
	@brief	execute a program to edit elevation type by spec
	@author	humkyung
	@date	2016.03.12
*/
void PropertyPage::CLineNoPage::OnBnClickedButtonElevationTypeBySpec()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	const CString sExecFilePath = GetExecPath() + _T("\\DataExchange.exe");
	const CString sSqliteFilePath = pDoc->GetRefFilePath().c_str();
	const CString sAccessFilePath = pDoc->GetAccessFilePath().c_str();
	const CString sArgs = (_T("\"") + sExecFilePath + _T("\" \"ElevationTypeBySpec\" \"") + sSqliteFilePath + _T("\" \"") + sAccessFilePath + _T("\""));
	DWORD exitCode = 0;
	executeCommandLine(sArgs , exitCode);
}