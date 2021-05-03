// OutsideViewLineNoPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "OutsideViewLineNoPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// COutsideViewLineNoPage dialog

IMPLEMENT_DYNAMIC(COutsideViewLineNoPage, CSmartDrawPropertyPage)

COutsideViewLineNoPage::COutsideViewLineNoPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(COutsideViewLineNoPage::IDD, pParent)
{

}

COutsideViewLineNoPage::~COutsideViewLineNoPage()
{
}

void COutsideViewLineNoPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_PICTURE , m_wndPicture);

	DDX_Control(pDX , IDC_COMBO_LINE_NO_TEXT_STYLE , m_cboLineNoTextStyle);
	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_sLabelOffset);
	DDX_Control(pDX , IDC_COMBO_LINE_NO_LEVEL , m_cboLineNoLevel);
	DDX_Control(pDX , IDC_BUTTON_LINE_NO_COLOR , m_btnLineNoColor);
	DDX_Control(pDX , IDC_COMBO_LINE_NO_STYLE , m_cboLineNoStyle);

	DDX_Control(pDX , IDC_COMBO_ELEV_TEXT_STYLE , m_cboElevTextStyle);
	DDX_Control(pDX , IDC_COMBO_ELEV_LEVEL , m_cboElevLevel);
	DDX_Control(pDX , IDC_BUTTON_ELEV_COLOR , m_btnElevColor);
	DDX_Control(pDX , IDC_COMBO_ELEV_STYLE , m_cboElevStyle);

	DDX_Control(pDX , IDC_CHECK_DISPLAY_LEADER_LINE , m_chkLeaderDisplay); 
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_cboArrowType);
	DDX_Text(pDX , IDC_EDIT_ARROW_SIZE , m_sArrowSize);
	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
}

BEGIN_MESSAGE_MAP(COutsideViewLineNoPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// COutsideViewLineNoPage message handlers

/**
	@brief	
	@author	HumKyung.BAEK
	@date	
*/
BOOL PropertyPage::COutsideViewLineNoPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_wndPicture.Load(GetExecPath() + _T("\\Resource\\LineNo_External_01.bmp"));
	m_wndPicture.Draw();

	m_cboLineNoTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLineNoColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_cboElevTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnElevColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboLineNoStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboElevStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboLineNoLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboElevLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboLineNoLevel.LoadContents(m_sIniFilePath);
	m_cboElevLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   COutsideViewLineNoPage::LoadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::COutsideViewLineNoPage::LoadData(void)
{
	static const CString sApp("Outside Pipe");
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	
	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ONLY_LINE_NO);
		pButton->SetCheck(STRING_T(_T("Only LineNo")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_ELEV);
		pButton->SetCheck(STRING_T(_T("LineNo+Elev")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_OVER_ELEV);
		pButton->SetCheck(STRING_T(_T("LineNo/Elev")) == szBuf);
	}

	/// load alignment type for view side - 2014.11.11 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Top Alignment_Style") , _T("Outside") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TOP_ALIGNMENT_TYPE);
		const int at = pComboBox->FindStringExact(0,szBuf);
		(-1 != at) ? pComboBox->SetCurSel(at) : pComboBox->SetCurSel(0);
	}
	if(GetPrivateProfileString(sApp , _T("Bottom Alignment_Style") , _T("Outside") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_BOTTOM_ALIGNMENT_TYPE);
		const int at = pComboBox->FindStringExact(0,szBuf);
		(-1 != at) ? pComboBox->SetCurSel(at) : pComboBox->SetCurSel(0);
	}
	if(GetPrivateProfileString(sApp , _T("Left Alignment_Style") , _T("Outside") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LEFT_ALIGNMENT_TYPE);
		const int at = pComboBox->FindStringExact(0,szBuf);
		(-1 != at) ? pComboBox->SetCurSel(at) : pComboBox->SetCurSel(0);
	}
	if(GetPrivateProfileString(sApp , _T("Right Alignment_Style") , _T("Outside") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_RIGHT_ALIGNMENT_TYPE);
		const int at = pComboBox->FindStringExact(0,szBuf);
		(-1 != at) ? pComboBox->SetCurSel(at) : pComboBox->SetCurSel(0);
	}
	/// up to here

	if(GetPrivateProfileString(sApp , _T("LineNo_TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLineNoTextStyle.FindString(-1 , szBuf);
		m_cboLineNoTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sLabelOffset = szBuf;
	}
	if(GetPrivateProfileString(sApp , _T("LineNo_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLineNoLevel.FindString(-1 , szBuf);
		m_cboLineNoLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("LineNo_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLineNoColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("LineNo_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLineNoStyle.FindString(-1 , szBuf);
		m_cboLineNoStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Elev_Type") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_ELEV_TYPE);
		const int at = pCombo->FindString(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Elev_TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboElevTextStyle.FindString(-1 , szBuf);
		m_cboElevTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Elev_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboElevLevel.FindString(-1 , szBuf);
		m_cboElevLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Elev_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnElevColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Elev_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboElevStyle.FindString(-1 , szBuf);
		m_cboElevStyle.SetCurSel((-1 != at) ? at : 0);
	}

	/// 2013.06.16 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Leader") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_chkLeaderDisplay.SetCheck(_T("ON") == STRING_T(szBuf));
	}else	m_chkLeaderDisplay.SetCheck(FALSE);
	/// up to here

	if(GetPrivateProfileString(sApp , _T("Arrow_Type") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboArrowType.FindString(-1 , szBuf);
		m_cboArrowType.SetCurSel((-1 != at) ? at : 0);
	}else	m_cboArrowType.SetCurSel(0);
	if(GetPrivateProfileString(sApp , _T("Arrow_Size") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sArrowSize = szBuf;
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

	/// 2015.02.12 added by humkyung
	if(GetPrivateProfileString(sApp , _T("LineNo_ExtGrpLevel"), _T("LEVEL_0"),szBuf,MAX_PATH,m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_GROUPING_LEVEL);
		const int at = pComboBox->FindStringExact(0 , szBuf);
		(-1 != at) ? pComboBox->SetCurSel(at) : pComboBox->SetCurSel(0);
	}
	/// up to here

	UpdateData(FALSE);
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   COutsideViewLineNoPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::COutsideViewLineNoPage::SaveData(void)
{
	static const CString sApp(_T("Outside Pipe"));
	UpdateData();

	CString rString;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ONLY_LINE_NO);
	if(pButton->GetCheck()) rString = _T("Only LineNo");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_ELEV);
	if(pButton->GetCheck()) rString = _T("LineNo+Elev");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_OVER_ELEV);
	if(pButton->GetCheck()) rString = _T("LineNo/Elev");
	WritePrivateProfileString(sApp , _T("DisplayStyle") , rString , m_sIniFilePath);

	/// save alignment type for view side - 2014.11.11 added by humkyung
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TOP_ALIGNMENT_TYPE);
	if(NULL != pComboBox)
	{
		pComboBox->GetWindowText(rString);
		WritePrivateProfileString(sApp , _T("Top Alignment_Style") , rString , m_sIniFilePath);
	}
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_BOTTOM_ALIGNMENT_TYPE);
	if(NULL != pComboBox)
	{
		pComboBox->GetWindowText(rString);
		WritePrivateProfileString(sApp , _T("Bottom Alignment_Style") , rString , m_sIniFilePath);
	}
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LEFT_ALIGNMENT_TYPE);
	if(NULL != pComboBox)
	{
		pComboBox->GetWindowText(rString);
		WritePrivateProfileString(sApp , _T("Left Alignment_Style") , rString , m_sIniFilePath);
	}
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_RIGHT_ALIGNMENT_TYPE);
	if(NULL != pComboBox)
	{
		pComboBox->GetWindowText(rString);
		WritePrivateProfileString(sApp , _T("Right Alignment_Style") , rString , m_sIniFilePath);
	}
	/// up to here

	int at = m_cboLineNoTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLineNoTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("LineNo_TextStyle") , rString , m_sIniFilePath);
	}
	WritePrivateProfileString(sApp , _T("Label_Offset") , m_sLabelOffset , m_sIniFilePath);
	at = m_cboLineNoLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLineNoLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("LineNo_Level") , rString , m_sIniFilePath);
	}
	
	at = m_btnLineNoColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("LineNo_Color") , rString , m_sIniFilePath);
	at = m_cboLineNoStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLineNoStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("LineNo_Style") , rString , m_sIniFilePath);
	}

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_ELEV_TYPE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Elev_Type") , rString , m_sIniFilePath);
	}

	at = m_cboElevTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboElevTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Elev_TextStyle") , rString , m_sIniFilePath);
	}
	at = m_cboElevLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboElevLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Elev_Level") , rString , m_sIniFilePath);
	}
	at = m_btnElevColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Elev_Color") , rString , m_sIniFilePath);
	at = m_cboElevStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboElevStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Elev_Style") , rString , m_sIniFilePath);
	}

	/// 2013.06.16 added by humkyung
	WritePrivateProfileString(sApp , _T("Leader") , (TRUE == m_chkLeaderDisplay.GetCheck()) ? _T("ON") : _T("OFF") , m_sIniFilePath);
	/// up to here

	at = m_cboArrowType.GetCurSel();
	if(-1 != at)
	{
		m_cboArrowType.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Arrow_Type") , rString , m_sIniFilePath);
	}
	WritePrivateProfileString(sApp , _T("Arrow_Size") , m_sArrowSize , m_sIniFilePath);
	
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
	m_cboLineNoLevel.SaveContents(m_sIniFilePath);
	m_cboElevLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	/// 2015.02.12 added by humkyung
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_GROUPING_LEVEL);
	if(NULL != pComboBox)
	{
		pComboBox->GetWindowText(rString);
		WritePrivateProfileString(sApp , _T("LineNo_ExtGrpLevel"), rString ,m_sIniFilePath);
	}
	/// up to here

	return ERROR_SUCCESS;
}
