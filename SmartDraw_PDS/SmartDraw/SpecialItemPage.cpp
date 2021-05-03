// SpecialItemPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "SmartDrawDoc.h"
#include "ValvePage.h"

using namespace PropertyPage;

// CSpecialItemPage dialog

IMPLEMENT_DYNAMIC(CSpecialItemPage, CSmartDrawPropertyPage)

CSpecialItemPage::CSpecialItemPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CSpecialItemPage::IDD, pParent)
{

}

CSpecialItemPage::~CSpecialItemPage()
{
}

void CSpecialItemPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_ENCLOSURE , m_wndEnclosureType);

	DDX_Control(pDX , IDC_BUTTON_LEADER_LINE_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_BUTTON_LABEL_TEXT_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_ARROW_TYPE , m_cboArrowType);
	DDX_Control(pDX , IDC_COMBO_LABEL_TEXT_STYLE , m_cboTextStyle);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);

	DDX_Text(pDX , IDC_EDIT_ENCLOSURE_A , m_rEnclosureA);
	DDX_Text(pDX , IDC_EDIT_ENCLOSURE_B , m_rEnclosureB);
	DDX_Text(pDX , IDC_EDIT_LEADER_ARROW_SIZE , m_rLeaderArrowSize);
	DDX_Text(pDX , IDC_EDIT_LABEL_DELIMITER , m_rLabelDelimiter);
}


BEGIN_MESSAGE_MAP(CSpecialItemPage, CSmartDrawPropertyPage)
	ON_CBN_SELENDOK(IDC_COMBO_ENCLOSURE_TYPE, &CSpecialItemPage::OnCbnSelendokComboEnclosureType)
	///ON_CBN_SELENDOK(IDC_COMBO_LABEL_TYPE, &CSpecialItemPage::OnCbnSelendokComboLabelType)
END_MESSAGE_MAP()


// CSpecialItemPage message handlers

BOOL PropertyPage::CSpecialItemPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);

	/// load contents - 2014.06.23 added by humkyung
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

void PropertyPage::CSpecialItemPage::OnCbnSelendokComboEnclosureType()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ENCLOSURE_TYPE);
	const int nSel = pComboBox->GetCurSel();
	switch(nSel)
	{
		case 0: //! circle
			m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\instrument_enclosure_00.bmp"));
			break;
		case 1:	
			m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\instrument_enclosure_01.bmp"));
			break;
		case 2:
			m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\instrument_enclosure_02.bmp"));
			break;
		case 3:
			m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\instrument_enclosure_03.bmp"));
			break;
		case 4:
			m_wndEnclosureType.Load(GetExecPath() + _T("\\Resource\\instrument_enclosure_04.bmp"));
			break;
	}
	m_wndEnclosureType.Draw();
}

/**
	@brief	read instrument configuration
	@author	HumKyung.BAEK
	@date	?
*/
int PropertyPage::CSpecialItemPage::LoadData(void)
{
	const CString rApp(_T("Specialty"));

	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(rApp , _T("Leader_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderLevel.FindString(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(rApp , _T("Leader_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(rApp , _T("Leader_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderStyle.FindString(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(rApp , _T("Leader_Weight") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderWeight.FindString(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(rApp , _T("Enclosure_SizeA") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rEnclosureA = szBuf;
	}
	if(GetPrivateProfileString(rApp , _T("Enclosure_SizeB") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rEnclosureB = szBuf;
	}

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ENCLOSURE_TYPE);
	if(GetPrivateProfileString(rApp , _T("EnclosureType") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		int nFound = pComboBox->FindString(-1 , szBuf);
		pComboBox->SetCurSel(nFound);
	}else	pComboBox->SetCurSel(0);
	OnCbnSelendokComboEnclosureType();

	/*pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_TYPE);
	if(GetPrivateProfileString(rApp , _T("INST_LABEL_TYPE") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		int nFound = pComboBox->FindString(-1 , szBuf);
		pComboBox->SetCurSel(nFound);
	}else	pComboBox->SetCurSel(0);
	///OnCbnSelendokComboLabelType();*/
	
	if(GetPrivateProfileString(rApp , _T("Label_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelLevel.FindString(-1 , szBuf);
		m_cboLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(rApp , _T("Label_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(rApp , _T("Label_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelStyle.FindString(-1 , szBuf);
		m_cboLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(rApp , _T("Label_Delimiter") ,  NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rLabelDelimiter = szBuf;
	}
	if(GetPrivateProfileString(rApp , _T("TextStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(rApp , _T("Enclosure_CenterLine") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CString rString(szBuf);
		rString.MakeUpper();
		
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ON);
		pButton->SetCheck(_T("ON") == rString);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_OFF);
		pButton->SetCheck(_T("OFF") == rString);
	}
	
	if(GetPrivateProfileString(rApp , _T("Arrow_Type") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboArrowType.FindString(-1 , szBuf);
		m_cboArrowType.SetCurSel((-1 != at) ? at : 0);
	}else	m_cboArrowType.SetCurSel(0);
	if(GetPrivateProfileString(rApp , _T("Arrow_Size") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rLeaderArrowSize = szBuf;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.25
*/
int PropertyPage::CSpecialItemPage::SaveData(void)
{
	const CString rApp(_T("Specialty"));
	UpdateData();

	CString rString;
	
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ENCLOSURE_TYPE);
	int nSel = pComboBox->GetCurSel();
	if(-1 != nSel)
	{
		pComboBox->GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("EnclosureType") , rString , m_sIniFilePath);
	}

	WritePrivateProfileString(rApp , _T("Enclosure_SizeA") , m_rEnclosureA , m_sIniFilePath);
	WritePrivateProfileString(rApp , _T("Enclosure_SizeB") , m_rEnclosureB , m_sIniFilePath);

	/*pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_TYPE);
	nSel = pComboBox->GetCurSel();
	if(-1 != nSel)
	{
		pComboBox->GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("INST_LABEL_TYPE") , rString , m_sIniFilePath);
	}*/

	nSel = m_cboLeaderLevel.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderLevel.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Leader_Level") , rString , m_sIniFilePath);
	}
	rString.Format(_T("%d") , m_btnLeaderColor.GetColorIndex());
	WritePrivateProfileString(rApp , _T("Leader_Color") , rString , m_sIniFilePath);
	nSel = m_cboLeaderStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Leader_Style") , rString , m_sIniFilePath);
	}
	nSel = m_cboLeaderWeight.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLeaderWeight.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Leader_Weight") , rString , m_sIniFilePath);
	}

	nSel = m_cboLabelLevel.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLabelLevel.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Label_Level") , rString , m_sIniFilePath);
	}
	rString.Format(_T("%d") , m_btnLabelColor.GetColorIndex());
	WritePrivateProfileString(rApp , _T("Label_Color") , rString , m_sIniFilePath);
	nSel = m_cboLabelStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboLabelStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Label_Style") , rString , m_sIniFilePath);
	}

	WritePrivateProfileString(rApp , _T("Label_Delimiter") , m_rLabelDelimiter , m_sIniFilePath);
	
	nSel = m_cboTextStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboTextStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("TextStyle") , rString , m_sIniFilePath);
	}

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ON);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(rApp , _T("Enclosure_CenterLine") , _T("ON") , m_sIniFilePath);
	}else	WritePrivateProfileString(rApp , _T("Enclosure_CenterLine") , _T("OFF") , m_sIniFilePath);
	
	nSel = m_cboArrowType.GetCurSel();
	if(-1 != nSel)
	{
		m_cboArrowType.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("Arrow_Type") , rString , m_sIniFilePath);
	}
	WritePrivateProfileString(rApp , _T("Arrow_Size") , m_rLeaderArrowSize , m_sIniFilePath);
	
	/// 2014.06.23 added by humkyung
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	m_cboLabelLevel.SaveContents(m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}
