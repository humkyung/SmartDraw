// OutsideViewLineNoPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "OutsideEquipmentPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// COutsideEquipmentPage dialog

IMPLEMENT_DYNAMIC(COutsideEquipmentPage, CSmartDrawPropertyPage)

COutsideEquipmentPage::COutsideEquipmentPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(COutsideEquipmentPage::IDD, pParent)
{
	m_sDisplayStyle = _T("Name/Coord");
}

COutsideEquipmentPage::~COutsideEquipmentPage()
{
}

void COutsideEquipmentPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_PICTURE , m_wndPicture);

	DDX_CBString(pDX , IDC_COMBO_OUTSIDE_EQP_DISPLAY_STYLE , m_sDisplayStyle);

	DDX_Control(pDX , IDC_COMBO_NAME_TEXT_STYLE , m_cboNameTextStyle);
	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_sLabelOffset);
	DDX_Control(pDX , IDC_COMBO_NAME_LEVEL , m_cboNameLevel);
	DDX_Control(pDX , IDC_BUTTON_NAME_COLOR , m_btnNameColor);
	DDX_Control(pDX , IDC_COMBO_NAME_STYLE , m_cboNameStyle);

	DDX_Control(pDX , IDC_COMBO_COORD_TEXT_STYLE , m_cboCoordTextStyle);
	DDX_Control(pDX , IDC_COMBO_COORD_LEVEL , m_cboCoordLevel);
	DDX_Control(pDX , IDC_BUTTON_COORD_COLOR , m_btnCoordColor);
	DDX_Control(pDX , IDC_COMBO_COORD_STYLE , m_cboCoordStyle);

	DDX_Control(pDX , IDC_CHECK_LEADER_DISPLAY , m_chkLeaderDisplay);
	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
}

BEGIN_MESSAGE_MAP(COutsideEquipmentPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// COutsideEquipmentPage message handlers

/**
	@brief	
	@author	HumKyung.BAEK
	@date	
*/
BOOL PropertyPage::COutsideEquipmentPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_wndPicture.Load(GetExecPath() + _T("\\Resource\\outside_equipment.bmp"));
	m_wndPicture.Draw();

	m_chkLeaderDisplay.SetLabels(_T("ON") , _T("OFF"));

	m_cboNameTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnNameColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_cboCoordTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnCoordColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	/// load contents - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboNameStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboCoordStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboNameLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboCoordLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboNameLevel.LoadContents(m_sIniFilePath);
	m_cboCoordLevel.LoadContents(m_sIniFilePath);
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
    @function   COutsideEquipmentPage::LoadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::COutsideEquipmentPage::LoadData(void)
{
	static const CString sApp("Outside Equipment");
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	/// get display style - 2014.09.12 added by humkyung
	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , _T("Name/Coord") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sDisplayStyle = szBuf;
	}
	/// up to here

	/// load alignment style for view side - 2014.11.11 added by humkyung
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

	if(GetPrivateProfileString(sApp , _T("Name_TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboNameTextStyle.FindString(-1 , szBuf);
		m_cboNameTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sLabelOffset = szBuf;
	}
	if(GetPrivateProfileString(sApp , _T("Name_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboNameLevel.FindString(-1 , szBuf);
		m_cboNameLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Name_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnNameColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Name_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboNameStyle.FindString(-1 , szBuf);
		m_cboNameStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Coord_TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCoordTextStyle.FindString(-1 , szBuf);
		m_cboCoordTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Coord_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCoordLevel.FindString(-1 , szBuf);
		m_cboCoordLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Coord_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnCoordColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Coord_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCoordStyle.FindString(-1 , szBuf);
		m_cboCoordStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Display") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_chkLeaderDisplay.SetCheck(STRING_T(_T("ON")) == szBuf);
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

	UpdateData(FALSE);
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   COutsideEquipmentPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::COutsideEquipmentPage::SaveData(void)
{
	static const CString sApp(_T("Outside Equipment"));
	UpdateData();

	/// save display style - 2014.09.12 added by humkyung
	WritePrivateProfileString(sApp , _T("DisplayStyle") , m_sDisplayStyle , m_sIniFilePath);
	/// up to here

	CString rString;
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

	int at = m_cboNameTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboNameTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Name_TextStyle") , rString , m_sIniFilePath);
	}
	WritePrivateProfileString(sApp , _T("Label_Offset") , m_sLabelOffset , m_sIniFilePath);
	at = m_cboNameLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboNameLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Name_Level") , rString , m_sIniFilePath);
	}
	
	at = m_btnNameColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Name_Color") , rString , m_sIniFilePath);
	at = m_cboNameStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboNameStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Name_Style") , rString , m_sIniFilePath);
	}

	at = m_cboCoordTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboCoordTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Coord_TextStyle") , rString , m_sIniFilePath);
	}
	at = m_cboCoordLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboCoordLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Coord_Level") , rString , m_sIniFilePath);
	}
	at = m_btnCoordColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Coord_Color") , rString , m_sIniFilePath);
	at = m_cboCoordStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboCoordStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Coord_Style") , rString , m_sIniFilePath);
	}

	rString = (m_chkLeaderDisplay.GetCheck()) ? _T("ON") : _T("OFF");
	WritePrivateProfileString(sApp , _T("Leader_Display") , rString , m_sIniFilePath);
	
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
	m_cboNameLevel.SaveContents(m_sIniFilePath);
	m_cboCoordLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}
