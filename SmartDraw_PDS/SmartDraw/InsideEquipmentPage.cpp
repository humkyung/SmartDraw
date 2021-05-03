// EquipmentPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "InsideEquipmentPage.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CInsideEquipmentPage dialog

IMPLEMENT_DYNAMIC(CInsideEquipmentPage, CSmartDrawPropertyPage)

CInsideEquipmentPage::CInsideEquipmentPage(CWnd* pParent /*=NULL*/) : CSmartDrawPropertyPage(CInsideEquipmentPage::IDD, pParent)
{

}

CInsideEquipmentPage::~CInsideEquipmentPage()
{
}

void CInsideEquipmentPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_NAME_TEXT_STYLE , m_cboNameTextStyle);
	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_sLabelOffset);
	DDX_Control(pDX , IDC_COMBO_NAME_LEVEL , m_cboNameLevel);
	DDX_Control(pDX , IDC_BUTTON_NAME_COLOR , m_btnNameColor);
	DDX_Control(pDX , IDC_COMBO_NAME_STYLE , m_cboNameStyle);

	DDX_Control(pDX , IDC_CHECK_COORD_DISPLAY , m_chkCoordDisplay);
	DDX_Control(pDX , IDC_COMBO_COORD_TEXT_STYLE , m_cboCoordTextStyle);
	DDX_Control(pDX , IDC_COMBO_COORD_LEVEL , m_cboCoordLevel);
	DDX_Control(pDX , IDC_BUTTON_COORD_COLOR , m_btnCoordColor);
	DDX_Control(pDX , IDC_COMBO_COORD_STYLE , m_cboCoordStyle);

	DDX_Control(pDX , IDC_CHECK_CENTER_LINE_DISPLAY , m_chkCenterlineDisplay);
	DDX_Control(pDX , IDC_COMBO_CENTER_LINE_LEVEL , m_cboCenterlineLevel);
	DDX_Control(pDX , IDC_BUTTON_CENTER_LINE_COLOR , m_btnCenterlineColor);
	DDX_Control(pDX , IDC_COMBO_CENTER_LINE_STYLE , m_cboCenterlineStyle);
	DDX_Control(pDX , IDC_COMBO_CENTER_LINE_WEIGHT, m_cboCenterlineWeight);
}


BEGIN_MESSAGE_MAP(CInsideEquipmentPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CInsideEquipmentPage message handlers

BOOL PropertyPage::CInsideEquipmentPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_chkCenterlineDisplay.SetLabels(_T("ON") , _T("OFF"));
	m_chkCoordDisplay.SetLabels(_T("ON") , _T("OFF"));

	m_cboNameTextStyle.FillTextStyleList(m_sIniFilePath);
	m_cboCoordTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnCenterlineColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnNameColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnCoordColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	/// update contents - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboNameStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboCoordStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboCenterlineStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboNameLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboCoordLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboCenterlineLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboNameLevel.LoadContents(m_sIniFilePath);
	m_cboCoordLevel.LoadContents(m_sIniFilePath);
	m_cboCenterlineLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CInsideEquipmentPage::LoadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CInsideEquipmentPage::LoadData(void)
{
	static const CString sApp("Inside Equipment");
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(sApp , _T("TagStyle") , _T("Outside View") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_INSIDE_VIEW);
		pButton->SetCheck(STRING_T(_T("Inside View")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_OUTSIDE_VIEW);
		pButton->SetCheck(STRING_T(_T("Outside View")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DEPEND_ON_AREA);
		pButton->SetCheck(STRING_T(_T("Depend on area")) == szBuf);
	}

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

	if(GetPrivateProfileString(sApp , _T("Coord_Display") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_chkCoordDisplay.SetCheck(STRING_T(_T("ON")) == szBuf);
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

	if(GetPrivateProfileString(sApp , _T("Centerline_Display") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_chkCenterlineDisplay.SetCheck(STRING_T(_T("ON")) == szBuf);
	}
	if(GetPrivateProfileString(sApp , _T("Centerline_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCenterlineLevel.FindString(-1 , szBuf);
		m_cboCenterlineLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Centerline_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnCenterlineColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Centerline_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCenterlineStyle.FindString(-1 , szBuf);
		m_cboCenterlineStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Centerline_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboCenterlineWeight.FindString(-1 , szBuf);
		m_cboCenterlineWeight.SetCurSel((-1 != at) ? at : 0);
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CInsideEquipmentPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CInsideEquipmentPage::SaveData(void)
{
	static const CString sApp(_T("Inside Equipment"));
	UpdateData();

	CString rString;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_INSIDE_VIEW);
	if(pButton->GetCheck()) rString = _T("Inside View");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_OUTSIDE_VIEW);
	if(pButton->GetCheck()) rString = _T("Outside View");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_DEPEND_ON_AREA);
	if(pButton->GetCheck()) rString = _T("Depend on area");
	WritePrivateProfileString(sApp , _T("TagStyle") , rString , m_sIniFilePath);

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

	rString = (m_chkCoordDisplay.GetCheck()) ? _T("ON") : _T("OFF");
	WritePrivateProfileString(sApp , _T("Coord_Display") , rString , m_sIniFilePath);

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
	
	rString = (m_chkCenterlineDisplay.GetCheck()) ? _T("ON") : _T("OFF");
	WritePrivateProfileString(sApp , _T("Centerline_Display") , rString , m_sIniFilePath);

	at = m_cboCenterlineLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboCenterlineLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Centerline_Level") , rString , m_sIniFilePath);
	}
	at = m_btnCenterlineColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Centerline_Color") , rString , m_sIniFilePath);
	at = m_cboCenterlineStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboCenterlineStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Centerline_Style") , rString , m_sIniFilePath);
	}
	at = m_cboCenterlineWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboCenterlineWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Centerline_Weight") , rString , m_sIniFilePath);
	}

	/// 2014.06.23 added by humkyung
	m_cboNameLevel.SaveContents(m_sIniFilePath);
	m_cboCoordLevel.SaveContents(m_sIniFilePath);
	m_cboCenterlineLevel.SaveContents(m_sIniFilePath);
	/// up to here
	
	return ERROR_SUCCESS;
}