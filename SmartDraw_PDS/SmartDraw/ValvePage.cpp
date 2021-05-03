// ValvePage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "ValvePage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CValvePage dialog

IMPLEMENT_DYNAMIC(CValvePage, CSmartDrawPropertyPage)

CValvePage::CValvePage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CValvePage::IDD, pParent)
{

}

CValvePage::~CValvePage()
{
}

void CValvePage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_ENCLOSURE , m_wndEnclosureType);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_cboArrowType);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_cboTextStyle);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);

	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_rLabelOffset);
	DDX_Text(pDX , IDC_EDIT_ARROW_SIZE , m_rLeaderArrowSize);

	DDX_CBString(pDX , IDC_COMBO_DISPLAY_NAME , m_sDisplayName);	/// 2014.12.31 added by humkyung
}


BEGIN_MESSAGE_MAP(CValvePage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CValvePage message handlers

BOOL PropertyPage::CValvePage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\valve.bmp"));
	m_wndEnclosureType.Draw();

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	
	if(_T("MSTN") != sOutputFormat)
	{
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	m_cboLabelLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int PropertyPage::CValvePage::LoadData(void)
{
	const CString sApp(_T("Valve"));

	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	((CButton*)GetDlgItem(IDC_RADIO_ONLY_NAME))->SetCheck(TRUE);
	if(GetPrivateProfileString(sApp , _T("DisplayType") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		((CButton*)GetDlgItem(IDC_RADIO_ONLY_NAME))->SetCheck(_T("Only Name") == CString(szBuf));
		((CButton*)GetDlgItem(IDC_RADIO_NAME_ELEV))->SetCheck(_T("Name+Elevation") == CString(szBuf));
	}

	((CButton*)GetDlgItem(IDC_RADIO_ENCLOSURE_RECTANGLE))->SetCheck(TRUE);
	GetPrivateProfileString(sApp , _T("EnclosureType") , _T("None") , szBuf , MAX_PATH , m_sIniFilePath);
	{
		((CButton*)GetDlgItem(IDC_RADIO_ENCLOSURE_NONE))->SetCheck(_T("None") == CString(szBuf));
		((CButton*)GetDlgItem(IDC_RADIO_ENCLOSURE_RECTANGLE))->SetCheck(_T("Rectangle") == CString(szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderLevel.FindString(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderStyle.FindString(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Weight") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderWeight.FindString(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}
	
	if(GetPrivateProfileString(sApp , _T("Label_Offset") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rLabelOffset = szBuf;
	}

	if(GetPrivateProfileString(sApp , _T("Label_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelLevel.FindString(-1 , szBuf);
		m_cboLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Label_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelStyle.FindString(-1 , szBuf);
		m_cboLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("TextStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
		
	if(GetPrivateProfileString(sApp , _T("Arrow_Type") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboArrowType.FindString(-1 , szBuf);
		m_cboArrowType.SetCurSel((-1 != at) ? at : 0);
	}else	m_cboArrowType.SetCurSel(0);
	if(GetPrivateProfileString(sApp , _T("Arrow_Size") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rLeaderArrowSize = szBuf;
	}

	if(GetPrivateProfileString(sApp , _T("DisplayName") , _T("piping_comp_no") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sDisplayName = szBuf;
	}

	return 0;
}

/**
	@brief	
	@author	humkyung
	@date	2015.09.01
*/
int PropertyPage::CValvePage::SaveData(void)
{
	const CString sApp(_T("Valve"));
	UpdateData();

	CString rString;

	/// 2014.12.22 added by humkyung
	const bool bOnlyName = (TRUE == ((CButton*)GetDlgItem(IDC_RADIO_ONLY_NAME))->GetCheck());
	WritePrivateProfileString(sApp , _T("DisplayType") , (bOnlyName ? _T("Only Name") : _T("Name+Elevation")) , m_sIniFilePath);

	CString sEnclosureType = ((CButton*)GetDlgItem(IDC_RADIO_ENCLOSURE_NONE))->GetCheck() ? _T("None") : _T("Rectangle");
	WritePrivateProfileString(sApp , _T("EnclosureType") , sEnclosureType , m_sIniFilePath);
	/// up to here

	int nSel = m_cboLeaderLevel.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderLevel.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Leader_Level") , rString , m_sIniFilePath);
	}
	rString.Format(_T("%d") , m_btnLeaderColor.GetColorIndex());
	WritePrivateProfileString(sApp , _T("Leader_Color") , rString , m_sIniFilePath);
	nSel = m_cboLeaderStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Leader_Style") , rString , m_sIniFilePath);
	}
	nSel = m_cboLeaderWeight.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderWeight.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Leader_Weight") , rString , m_sIniFilePath);
	}

	WritePrivateProfileString(sApp , _T("Label_Offset") , m_rLabelOffset , m_sIniFilePath);

	nSel = m_cboLabelLevel.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLabelLevel.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Label_Level") , rString , m_sIniFilePath);
	}
	rString.Format(_T("%d") , m_btnLabelColor.GetColorIndex());
	WritePrivateProfileString(sApp , _T("Label_Color") , rString , m_sIniFilePath);
	nSel = m_cboLabelStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLabelStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Label_Style") , rString , m_sIniFilePath);
	}
	
	nSel = m_cboTextStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboTextStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("TextStyle") , rString , m_sIniFilePath);
	}
	
	nSel = m_cboArrowType.GetCurSel();
	if(-1 != nSel)
	{
		m_cboArrowType.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("Arrow_Type") , rString , m_sIniFilePath);
	}
	WritePrivateProfileString(sApp , _T("Arrow_Size") , m_rLeaderArrowSize , m_sIniFilePath);
	
	/// 2014.06.23 added by humkyung
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	m_cboLabelLevel.SaveContents(m_sIniFilePath);
	/// up to here

	/// 2014.12.31 added by humkyung
	WritePrivateProfileString(sApp , _T("DisplayName") , m_sDisplayName , m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}