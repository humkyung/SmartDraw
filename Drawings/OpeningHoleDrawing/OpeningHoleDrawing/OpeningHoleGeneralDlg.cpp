// OpeningHoleGeneralDlg.cpp : implementation file
//

#include "stdafx.h"
#include <IsTools.h>
#include <util/FileTools.h>
#include "OpeningHoleDrawing.h"
#include "OpeningHoleGeneralDlg.h"
#include "OpeningHolePage.h"

// COpeningHoleGeneralDlg dialog

IMPLEMENT_DYNAMIC(COpeningHoleGeneralDlg, CDialog)

COpeningHoleGeneralDlg::COpeningHoleGeneralDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpeningHoleGeneralDlg::IDD, pParent)
{

}

COpeningHoleGeneralDlg::~COpeningHoleGeneralDlg()
{
}

void COpeningHoleGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_HOLE_TYPE , m_wndHoleGroupingType);
	DDX_Control(pDX , IDC_COMBO_LABEL_TEXT_STYLE , m_wndTextStyleComboBox);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_BUTTON_HOLE_COLOR , m_btnHoleColor);
	DDX_Control(pDX , IDC_BUTTON_HOLE_GROUP_COLOR , m_btnHoleGroupColor);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_wndArrowComboBox);
}


BEGIN_MESSAGE_MAP(COpeningHoleGeneralDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_HOLE_GROUP_BOX, &COpeningHoleGeneralDlg::OnBnClickedRadioHoleGroupBox)
	ON_BN_CLICKED(IDC_RADIO_HOLE_GROUP_POLYLINE, &COpeningHoleGeneralDlg::OnBnClickedRadioHoleGroupPolyline)
END_MESSAGE_MAP()

/******************************************************************************
    @author     humkyung
    @date       2012-03-14
    @function   SaveData
    @return     int
    @brief
******************************************************************************/
int COpeningHoleGeneralDlg::SaveData( const CString& sIniFilePath )
{
	static const CString sApp(_T("OpeningHole"));

	CString rString;
	GetDlgItemText(IDC_EDIT_GROUP_NO_PREFIX , rString);
	WritePrivateProfileString(sApp , _T("GroupNo_Prefix") , rString , sIniFilePath);
	
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_NO_HOLE_SIZE);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("DisplayStyle") , _T("HoleNo+HoleSize") , sIniFilePath);
	}
	pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_NO);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("DisplayStyle") , _T("HoleNo") , sIniFilePath);
	}
	pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_SIZE);
	if(pButton->GetCheck())
	{
		WritePrivateProfileString(sApp , _T("DisplayStyle") , _T("HoleSize") , sIniFilePath);
	}

	int at = m_wndTextStyleComboBox.GetCurSel();
	if(-1 != at)
	{
		m_wndTextStyleComboBox.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("TextStyle") , rString , sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_LABEL_OFFSET , rString);
	WritePrivateProfileString(sApp , _T("Label_Offset") , rString , sIniFilePath);

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_LEVEL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Level") , rString , sIniFilePath);
	}

	int iColorIdx = m_btnLabelColor.GetColorIndex();
	rString.Format(_T("%d") , iColorIdx);
	WritePrivateProfileString(sApp , _T("Label_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Style") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Weight") , rString , sIniFilePath);
	}

	/// Leader
	at = m_wndArrowComboBox.GetCurSel();
	if(-1 != at)
	{
		m_wndArrowComboBox.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_ArrowType") , rString , sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_ARROW_SIZE , rString);
	WritePrivateProfileString(sApp , _T("Leader_ArrowSize") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_LEVEL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Level") , rString , sIniFilePath);
	}

	iColorIdx = m_btnLeaderColor.GetColorIndex();
	WritePrivateProfileString(sApp , _T("Leader_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Style") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Weight") , rString , sIniFilePath);
	}
	/// up to here

	/// Hole
	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_LEVEL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Hole_Level") , rString , sIniFilePath);
	}

	iColorIdx = m_btnHoleColor.GetColorIndex();
	rString.Format(_T("%d") , iColorIdx);
	WritePrivateProfileString(sApp , _T("Hole_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Hole_Style") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Hole_Weight") , rString , sIniFilePath);
	}
	/// up to here

	/// Hole Group
	pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_GROUP_BOX);
	WritePrivateProfileString(sApp , _T("HoleGroup_Type") , (TRUE == pButton->GetCheck() ? _T("Box") : _T("Polyline")) , sIniFilePath);
	
	GetDlgItemText(IDC_EDIT_CLEARANCE1 , rString);
	WritePrivateProfileString(sApp , _T("HoleGroup_ClearanceA") , rString , sIniFilePath);

	GetDlgItemText(IDC_EDIT_CLEARANCE2 , rString);
	WritePrivateProfileString(sApp , _T("HoleGroup_ClearanceB") , rString , sIniFilePath);

	GetDlgItemText(IDC_EDIT_MIN_DIST , rString);
	WritePrivateProfileString(sApp , _T("HoleGroup_MinDist") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_LEVEL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("HoleGroup_Level") , rString , sIniFilePath);
	}

	iColorIdx = m_btnHoleGroupColor.GetColorIndex();
	rString.Format(_T("%d") , iColorIdx);
	WritePrivateProfileString(sApp , _T("HoleGroup_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("HoleGroup_Style") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("HoleGroup_Weight") , rString , sIniFilePath);
	}
	/// up to here

	return ERROR_SUCCESS;
}

// COpeningHoleGeneralDlg message handlers

BOOL COpeningHoleGeneralDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	static const CString sApp(_T("OpeningHole"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);

	COpeningHolePage* pPage = (COpeningHolePage*)(GetParent()->GetParent());
	const CString sIniFilePath = pPage->GetIniFilePath();
	
	m_wndHoleGroupingType.Load(sExecPath + _T("Resource\\OpeningHole_Box.bmp"));
	m_wndHoleGroupingType.Draw();

	if(GetPrivateProfileString(sApp , _T("GroupNo_Prefix") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_GROUP_NO_PREFIX , szBuf);
	}
	
	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_NO_HOLE_SIZE);
		pButton->SetCheck(0 == STRICMP_T(_T("HoleNo+HoleSize") , szBuf));
		pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_NO);
		pButton->SetCheck(0 == STRICMP_T(_T("HoleNo") , szBuf));
		pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_SIZE);
		pButton->SetCheck(0 == STRICMP_T(_T("HoleSize") , szBuf));
	}

	m_wndTextStyleComboBox.FillTextStyleList(sIniFilePath);
	if(GetPrivateProfileString(sApp , _T("TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		const int nFound = m_wndTextStyleComboBox.FindString(-1 , szBuf);
		if(-1 != nFound) m_wndTextStyleComboBox.SetCurSel(nFound);
	}

	if(GetPrivateProfileString(sApp , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_LABEL_OFFSET , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("Label_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnLabelColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp , _T("Label_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("Label_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Label_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LABEL_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_ArrowType") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		const int at = m_wndArrowComboBox.FindStringExact(-1 , szBuf);
		m_wndArrowComboBox.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_ArrowSize") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ARROW_SIZE , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnLeaderColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LEADER_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Hole_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnHoleColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp , _T("Hole_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnHoleColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("Hole_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Hole_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_Type") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_GROUP_BOX);
		pButton->SetCheck(0 == STRICMP_T(_T("BOX") , szBuf));
		pButton = (CButton*)GetDlgItem(IDC_RADIO_HOLE_GROUP_POLYLINE);
		pButton->SetCheck(0 == STRICMP_T(_T("POLYLINE") , szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_ClearanceA") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_CLEARANCE1 , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_ClearanceB") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_CLEARANCE2 , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_MinDist") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_MIN_DIST , szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnHoleGroupColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp , _T("HoleGroup_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnHoleGroupColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("HoleGroup_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_HOLE_GROUP_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COpeningHoleGeneralDlg::OnBnClickedRadioHoleGroupBox()
{
	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);

	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_HOLE_GROUP_BOX));
	if(pButton->GetCheck())
	{
		m_wndHoleGroupingType.Load(sExecPath + _T("Resource\\OpeningHole_Box.bmp"));
	}
	else
	{
		m_wndHoleGroupingType.Load(sExecPath + _T("Resource\\OpeningHole_Polyline.bmp"));
	}
	m_wndHoleGroupingType.Draw();
}

void COpeningHoleGeneralDlg::OnBnClickedRadioHoleGroupPolyline()
{
	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);

	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_HOLE_GROUP_POLYLINE));
	if(pButton->GetCheck())
	{
		m_wndHoleGroupingType.Load(sExecPath + _T("Resource\\OpeningHole_Polyline.bmp"));
	}
	else
	{
		m_wndHoleGroupingType.Load(_T("Resource\\OpeningHole_Box.bmp"));
	}
	m_wndHoleGroupingType.Draw();
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL COpeningHoleGeneralDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
