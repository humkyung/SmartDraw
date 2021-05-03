// ReducerPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "ReducerPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CReducerPage dialog

IMPLEMENT_DYNAMIC(CReducerPage, CSmartDrawPropertyPage)

CReducerPage::CReducerPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CReducerPage::IDD, pParent)
{

}

CReducerPage::~CReducerPage()
{
}

void CReducerPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
	
	DDX_Control(pDX , IDC_STATIC_REDUCER , m_wndReducerPicture);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);
	DDX_Control(pDX , IDC_COMBO_LABEL_WEIGHT , m_cboLabelWeight);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT , m_cboLeaderWeight);

	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_cboTextStyle);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_wndArrowComboBox);
}


BEGIN_MESSAGE_MAP(CReducerPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CReducerPage message handlers

BOOL PropertyPage::CReducerPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboLabelLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CReducerPage::LoadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CReducerPage::LoadData(void)
{
	static const CString sApp(_T("Reducer"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	m_wndReducerPicture.Load(GetExecPath() + _T("\\Resource\\reducer.bmp"));
	m_wndReducerPicture.Draw();

	if(GetPrivateProfileString(sApp , _T("Name_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_MAIN_RUN);
		pButton->SetCheck(STRING_T(_T("Type1")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_RUN_MAIN);
		pButton->SetCheck(STRING_T(_T("Type2")) == szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("ECC_Labeling") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ECC_LABELING , szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("CON_Labeling") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_CON_LABELING , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("Name_Unit") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_METRIC);
		pButton->SetCheck(STRING_T(_T("Metric")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_INCH);
		pButton->SetCheck(STRING_T(_T("Inch")) == szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_LABEL_OFFSET , szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelLevel.FindString(-1 , szBuf);
		m_cboLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Label_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelStyle.FindString(-1 , szBuf);
		m_cboLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelWeight.FindString(-1 , szBuf);
		m_cboLabelWeight.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Arrow_Type") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_wndArrowComboBox.FindString(-1 , szBuf);
		m_wndArrowComboBox.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Arrow_Size") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ARROW_SIZE , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderLevel.FindString(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderStyle.FindString(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderWeight.FindString(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}

	return 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CReducerPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CReducerPage::SaveData(void)
{
	static const CString sApp(_T("Reducer"));
	CString rString;

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_MAIN_RUN);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("Name_Style") , _T("Type1") , m_sIniFilePath);
	}
	pButton = (CButton*)GetDlgItem(IDC_RADIO_RUN_MAIN);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("Name_Style") , _T("Type2") , m_sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_ECC_LABELING , rString);
	WritePrivateProfileString(sApp , _T("ECC_Labeling") , rString , m_sIniFilePath);
	
	GetDlgItemText(IDC_EDIT_CON_LABELING , rString);
	WritePrivateProfileString(sApp , _T("CON_Labeling") , rString , m_sIniFilePath);

	/*pButton = (CButton*)GetDlgItem(IDC_RADIO_METRIC);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("Name_Unit") , _T("Metric") , m_sIniFilePath);
	}
	pButton = (CButton*)GetDlgItem(IDC_RADIO_INCH);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("Name_Unit") , _T("Inch") , m_sIniFilePath);
	}*/

	/// write project unit - 2016.02.11 added by humkyung
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const STRING_T sPrjUnit = pDoc->GetProjectUnit();
	WritePrivateProfileString(sApp , _T("SizeUnit") , sPrjUnit.c_str() , m_sIniFilePath);
	/// up to here


	int at = m_cboTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("TextStyle") , rString , m_sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_LABEL_OFFSET , rString);
	WritePrivateProfileString(sApp , _T("Label_Offset") , rString , m_sIniFilePath);

	at = m_cboLabelLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Level") , rString , m_sIniFilePath);
	}
	
	at = m_btnLabelColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Label_Color") , rString , m_sIniFilePath);

	at = m_cboLabelStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Style") , rString , m_sIniFilePath);
	}

	at = m_cboLabelWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Weight") , rString , m_sIniFilePath);
	}

	at = m_wndArrowComboBox.GetCurSel();
	if(-1 != at)
	{
		m_wndArrowComboBox.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Arrow_Type") , rString , m_sIniFilePath);
	}
	GetDlgItemText(IDC_EDIT_ARROW_SIZE , rString);
	WritePrivateProfileString(sApp , _T("Arrow_Size") , rString , m_sIniFilePath);

	at = m_cboLeaderLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Level") , rString , m_sIniFilePath);
	}

	at = m_btnLeaderColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Leader_Color") , rString , m_sIniFilePath);

	at = m_cboLeaderStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Style") , rString , m_sIniFilePath);
	}

	at = m_cboLeaderWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Weight") , rString , m_sIniFilePath);
	}

	/// 2014.06.23 added by humkyung
	m_cboLabelLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}