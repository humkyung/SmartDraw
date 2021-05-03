// HatchingPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <DgnV7Lib.h>
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "VerticalLineHatchingPage.h"

#include <fstream>
using namespace std;
using namespace PropertyPage;

// CVerticalLineHatchingPage dialog

IMPLEMENT_DYNAMIC(CVerticalLineHatchingPage, CSmartDrawPropertyPage)

CVerticalLineHatchingPage::CVerticalLineHatchingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CVerticalLineHatchingPage::IDD, pParent)
{

}

CVerticalLineHatchingPage::~CVerticalLineHatchingPage()
{
}

void CVerticalLineHatchingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_LESS_THAN , m_wndLessThanPicture);
	DDX_Control(pDX , IDC_STATIC_GREATER_EQUAL , m_wndGreaterEqualPicture);
	DDX_Control(pDX , IDC_STATIC_LESS_THAN_LABEL , m_wndLessThanLabel);
	DDX_Control(pDX , IDC_STATIC_GREATER_EQUAL_LABEL , m_wndGreaterEqualLabel);

	DDX_Control(pDX , IDC_COMBO_VERTICAL_PIPE_HATCHING_LEVEL , m_cboHatchingLevel);
	DDX_Control(pDX , IDC_BUTTON_VERTICAL_PIPE_HATCHING_COLOR , m_btnHatchingColor);
	DDX_Control(pDX , IDC_COMBO_VERTICAL_PIPE_HATCHING_STYLE , m_cboHatchingStyle);
	DDX_Control(pDX , IDC_COMBO_VERTICAL_PIPE_HATCHING_WEIGHT , m_cboHatchingWeight);
}


BEGIN_MESSAGE_MAP(CVerticalLineHatchingPage, CSmartDrawPropertyPage)
	ON_CBN_SELENDOK(IDC_COMBO_LESS_THAN, &CVerticalLineHatchingPage::OnCbnSelendokComboLessThan)
	ON_CBN_SELENDOK(IDC_COMBO_GREATER_EQUAL, &CVerticalLineHatchingPage::OnCbnSelendokComboGreaterEqual)
	ON_BN_CLICKED(IDC_CHECK_DEPEND_ON_SIZE, &CVerticalLineHatchingPage::OnBnClickedCheckDependOnSize)
END_MESSAGE_MAP()


// CVerticalLineHatchingPage message handlers

BOOL PropertyPage::CVerticalLineHatchingPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	{
		LOGFONT LogFont;
		CFont BoldFont;
		BoldFont.CreateFont(40,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));
		BoldFont.GetLogFont(&LogFont);
		m_wndLessThanLabel.SetFont(LogFont);
		m_wndLessThanLabel.SetTextColor(RGB(0,0,255));
		m_wndGreaterEqualLabel.SetFont(LogFont);
		m_wndGreaterEqualLabel.SetTextColor(RGB(0,0,255));
	}

	/// fill size combobox
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SIZE);
	list<CSmartDrawDocData::SizeRecord>* pSizeRecordList = docData.GetSizeRecordList();
	for(list<CSmartDrawDocData::SizeRecord>::const_iterator itr = pSizeRecordList->begin();itr != pSizeRecordList->end();++itr)
	{
		CString rString(itr->inch.c_str() + CString(_T(" - ")) + itr->metric.c_str() + CString(_T("mm")));
		pComboBox->AddString(rString);
	}

	/// load contents - 2014.06.23 added by humkyung
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboHatchingStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat) m_cboHatchingLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN);
	m_cboHatchingLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.23
*/
int PropertyPage::CVerticalLineHatchingPage::LoadData(void)
{
	static const STRING_T sApp(_T("Hatching"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_00.bmp"));
	m_wndLessThanPicture.Draw();
	m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_01.bmp"));
	m_wndGreaterEqualPicture.Draw();

	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Type") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_DEPEND_ON_SIZE);
		pButton->SetCheck(STRING_T(_T("Depend on Size")) == szBuf);
		this->OnBnClickedCheckDependOnSize();
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Cirital_Size") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SIZE);
		const int at = pCombo->FindString(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkA") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LESS_THAN);
		const int at = pCombo->FindString(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
		this->OnCbnSelendokComboLessThan();
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkB") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GREATER_EQUAL);
		const int at = pCombo->FindString(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
		this->OnCbnSelendokComboGreaterEqual();
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingLevel.FindString(-1 , szBuf);
		m_cboHatchingLevel.SetCurSel((-1 != at) ? at : 0);
	}

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnHatchingColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_btnHatchingColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingStyle.FindString(-1 , szBuf);
		m_cboHatchingStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingWeight.FindString(-1 , szBuf);
		m_cboHatchingWeight.SetCurSel((-1 != at) ? at : 0);
	}

	return 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-14
    @function   CVerticalLineHatchingPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CVerticalLineHatchingPage::SaveData()
{
	static const STRING_T sApp(_T("Hatching"));

	CString rString;

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_DEPEND_ON_SIZE);
	WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Type") , 
		((pButton->GetCheck()) ? _T("Depend on Size") : _T("ALL")) , m_sIniFilePath);
	
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SIZE);
	int at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Cirital_Size") , rString , m_sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LESS_THAN);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkA") , rString , m_sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GREATER_EQUAL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkB") , rString , m_sIniFilePath);
	}

	at = m_cboHatchingLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Level") , rString , m_sIniFilePath);
	}

	at = m_btnHatchingColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Color") , rString , m_sIniFilePath);
	
	at = m_cboHatchingStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Style") , rString , m_sIniFilePath);
	}

	at = m_cboHatchingWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Weight") , rString , m_sIniFilePath);
	}

	m_cboHatchingLevel.SaveContents(m_sIniFilePath);	/// 2014.06.23 added by humkyung

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CVerticalLineHatchingPage::OnCbnSelendokComboLessThan()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LESS_THAN);
	const int nSel = pComboBox->GetCurSel();
	switch(nSel)
	{
		case 0:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_00.bmp"));
			break;
		case 1:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_01.bmp"));
			break;
		case 2:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_02.bmp"));
			break;
		case 3:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_03.bmp"));
			break;
		case 4:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_04.bmp"));
			break;
		case 5:
			m_wndLessThanPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_05.bmp"));
			break;
	}
	m_wndLessThanPicture.Draw();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CVerticalLineHatchingPage::OnCbnSelendokComboGreaterEqual()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_GREATER_EQUAL);
	const int nSel = pComboBox->GetCurSel();
	switch(nSel)
	{
		case 0:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_00.bmp"));
			break;
		case 1:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_01.bmp"));
			break;
		case 2:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_02.bmp"));
			break;
		case 3:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_03.bmp"));
			break;
		case 4:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_04.bmp"));
			break;
		case 5:
			m_wndGreaterEqualPicture.Load(GetExecPath() + _T("\\Resource\\hatching_mark_05.bmp"));
			break;
	}
	m_wndGreaterEqualPicture.Draw();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CVerticalLineHatchingPage::OnBnClickedCheckDependOnSize()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_DEPEND_ON_SIZE);
	if(pButton)
	{
		if(pButton->GetCheck())
		{
			GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_GREATER_EQUAL)->ShowWindow(SW_SHOW);
			m_wndGreaterEqualPicture.ShowWindow(SW_SHOW);
			m_wndLessThanLabel.ShowWindow(SW_SHOW);
			m_wndGreaterEqualLabel.ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_GREATER_EQUAL)->ShowWindow(SW_HIDE);
			m_wndGreaterEqualPicture.ShowWindow(SW_HIDE);
			m_wndLessThanLabel.ShowWindow(SW_HIDE);
			m_wndGreaterEqualLabel.ShowWindow(SW_HIDE);
		}
	}
}