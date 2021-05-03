// ViewAnnoPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "ViewAnnoPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CViewAnnoPage dialog

IMPLEMENT_DYNAMIC(CViewAnnoPage, CSmartDrawPropertyPage)

CViewAnnoPage::CViewAnnoPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CViewAnnoPage::IDD, pParent)
{

}

CViewAnnoPage::~CViewAnnoPage()
{
}

void CViewAnnoPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
	
	DDX_Control(pDX , IDC_STATIC_VIEW_ANNO , m_wndViewAnnoPicture);
	
	DDX_Text(pDX , IDC_EDIT_VIEW_INFO_DIST , m_sViewInfoDist);
	DDX_Control(pDX , IDC_COMBO_VIEW_INFO_TEXT_STYLE , m_cboViewInfoTextStyle);
	DDX_Control(pDX , IDC_COMBO_VIEW_INFO_LEVEL , m_cboViewInfoLevel);
	DDX_Control(pDX , IDC_BUTTON_VIEW_INFO_COLOR , m_btnViewInfoColor);
	DDX_Control(pDX , IDC_COMBO_VIEW_INFO_STYLE , m_cboViewInfoStyle);
	DDX_CBString(pDX , IDC_COMBO_VIEW_INFO_ALIGNMENT , m_sViewInfoAlignment);

	DDX_Text(pDX , IDC_EDIT_MATCHLINE_DATA_DIST , m_sMatchLineDataDist);
	DDX_Control(pDX , IDC_COMBO_MATCHLINE_DATA_TEXT_STYLE , m_cboMatchLineDataTextStyle);
	DDX_Control(pDX , IDC_COMBO_MATCHLINE_DATA_LEVEL , m_cboMatchLineDataLevel);
	DDX_Control(pDX , IDC_BUTTON_MATCHLINE_DATA_COLOR , m_btnMatchLineDataColor);
	DDX_Control(pDX , IDC_COMBO_MATCHLINE_DATA_STYLE , m_cboMatchLineDataStyle);
	DDX_CBString(pDX , IDC_COMBO_MATCHLINE_DATA_ALIGNMENT , m_sMatchLineDataAlignment);
	
	DDX_CBString(pDX , IDC_COMBO_VIEW_COORD_DISPLAY_STYLE , m_sViewCoordDisplayStyle);
	DDX_Control(pDX , IDC_COMBO_VIEW_COORD_TEXT_STYLE , m_cboViewCoordTextStyle);
	DDX_Control(pDX , IDC_COMBO_VIEW_COORD_LEVEL , m_cboViewCoordLevel);
	DDX_Control(pDX , IDC_BUTTON_VIEW_COORD_COLOR , m_btnViewCoordColor);
	DDX_Control(pDX , IDC_COMBO_VIEW_COORD_STYLE , m_cboViewCoordStyle);
}


BEGIN_MESSAGE_MAP(CViewAnnoPage, CSmartDrawPropertyPage)
	ON_CBN_SELENDOK(IDC_COMBO_VIEW_COORD_DISPLAY_STYLE, &CViewAnnoPage::OnCbnSelendokComboViewCoordStyle)
END_MESSAGE_MAP()


// CViewAnnoPage message handlers

BOOL CViewAnnoPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	LoadData();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	load view annotation options
	@author	humkyung
	@date	2014.11.12
*/
int CViewAnnoPage::LoadData(void)
{
	static const STRING_T sApp(_T("View Annotation"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	m_cboViewInfoTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnViewInfoColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_cboViewInfoStyle.LoadContents(pDoc->GetLineStyleFilePath());

	m_cboMatchLineDataTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnMatchLineDataColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_cboMatchLineDataStyle.LoadContents(pDoc->GetLineStyleFilePath());

	m_cboViewCoordTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnViewCoordColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_cboViewCoordStyle.LoadContents(pDoc->GetLineStyleFilePath());

	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	if(_T("MSTN") != sOutputFormat)
	{
		m_cboViewInfoLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboMatchLineDataLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboViewCoordLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboViewInfoLevel.LoadContents(m_sIniFilePath);
	m_cboMatchLineDataLevel.LoadContents(m_sIniFilePath);
	m_cboViewCoordLevel.LoadContents(m_sIniFilePath);

	/// view information
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Dist") , _T("0") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sViewInfoDist = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_TextStyle") , _T("Standard") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordTextStyle.FindString(-1 , szBuf);
		m_cboViewInfoTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Level") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordLevel.FindString(-1 , szBuf);
		m_cboViewInfoLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Color") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_btnViewInfoColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Style") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewInfoStyle.FindString(-1 , szBuf);
		m_cboViewInfoStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Alignment") , _T("Center") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sViewInfoAlignment = szBuf;
	}
	/// up to here

	/// match line data
	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_Dist") , _T("0") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sMatchLineDataDist = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_TextStyle") , _T("Standard") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordTextStyle.FindString(-1 , szBuf);
		m_cboMatchLineDataTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_Level") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordLevel.FindString(-1 , szBuf);
		m_cboMatchLineDataLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_Color") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_btnMatchLineDataColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_Style") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboMatchLineDataStyle.FindString(-1 , szBuf);
		m_cboMatchLineDataStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("MatchLine_Data_Alignment") , _T("Center") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sMatchLineDataAlignment = szBuf;
	}
	/// up to here

	/// view coordinate
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Display_Box") , _T("ON") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_VIEW_COORD_BOX_DISPLAY);
		pButton->SetCheck(CString(_T("ON")) == szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Display_Style") , _T("All") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_sViewCoordDisplayStyle = szBuf;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_VIEW_COORD_DISPLAY_STYLE);
		if(NULL != pComboBox)
		{
			const int at = pComboBox->FindString(-1 , m_sViewCoordDisplayStyle);
			pComboBox->SetCurSel(at);
		}
		OnCbnSelendokComboViewCoordStyle();
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_TextStyle") , _T("Standard") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordTextStyle.FindString(-1 , szBuf);
		m_cboViewCoordTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Level") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordLevel.FindString(-1 , szBuf);
		m_cboViewCoordLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Color") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_btnViewCoordColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Style") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboViewCoordStyle.FindString(-1 , szBuf);
		m_cboViewCoordStyle.SetCurSel((-1 != at) ? at : 0);
	}
	/// up to here

	return 0;
}

/******************************************************************************
    @brief		save view annotation options
	@author     humkyung
    @date       2014-11-12
    @function   CViewAnnoPage::SaveData
    @return     int
    @param      void
******************************************************************************/
int CViewAnnoPage::SaveData(void)
{
	static const CString sApp(_T("View Annotation"));
	UpdateData();

	WritePrivateProfileString(sApp , _T("View_Info_Dist") , m_sViewInfoDist , m_sIniFilePath);

	CString rString;
	m_cboViewInfoTextStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Info_TextStyle") , rString , m_sIniFilePath);
	m_cboViewInfoLevel.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Info_Level") , rString , m_sIniFilePath);
	int at = m_btnViewInfoColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("View_Info_Color") , rString , m_sIniFilePath);
	m_cboViewInfoStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Info_Style") , rString , m_sIniFilePath);
	WritePrivateProfileString(sApp , _T("View_Info_Alignment") , m_sViewInfoAlignment , m_sIniFilePath);
	/// up to here

	WritePrivateProfileString(sApp , _T("MatchLine_Data_Dist") , m_sMatchLineDataDist , m_sIniFilePath);

	m_cboMatchLineDataTextStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("MatchLine_Data_TextStyle") , rString , m_sIniFilePath);
	m_cboMatchLineDataLevel.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("MatchLine_Data_Level") , rString , m_sIniFilePath);
	at = m_btnMatchLineDataColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("MatchLine_Data_Color") , rString , m_sIniFilePath);
	m_cboMatchLineDataStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("MatchLine_Data_Style") , rString , m_sIniFilePath);
	WritePrivateProfileString(sApp , _T("MatchLine_Data_Alignment") , m_sMatchLineDataAlignment , m_sIniFilePath);

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_VIEW_COORD_BOX_DISPLAY);
	if(NULL != pButton)
	{
		BOOL b = pButton->GetCheck();
		WritePrivateProfileString(sApp , _T("View_Coord_Display_Box") , ((TRUE == b) ? _T("ON") : _T("OFF")) , m_sIniFilePath);
	}
	
	WritePrivateProfileString(sApp , _T("View_Coord_Display_Style") , m_sViewCoordDisplayStyle , m_sIniFilePath);
	m_cboViewCoordTextStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Coord_TextStyle") , rString , m_sIniFilePath);
	m_cboViewCoordLevel.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Coord_Level") , rString , m_sIniFilePath);
	at = m_btnViewCoordColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("View_Coord_Color") , rString , m_sIniFilePath);
	m_cboViewCoordStyle.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("View_Coord_Style") , rString , m_sIniFilePath);

	return ERROR_SUCCESS;
}

void CViewAnnoPage::OnCbnSelendokComboViewCoordStyle()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_VIEW_COORD_DISPLAY_STYLE);
	const int nSel = pComboBox->GetCurSel();
	switch(nSel)
	{
		case 0:
			m_wndViewAnnoPicture.Load(GetExecPath() + _T("\\Resource\\view_anno_option1.bmp"));
			break;
		case 1:
			m_wndViewAnnoPicture.Load(GetExecPath() + _T("\\Resource\\view_anno_option2.bmp"));
			break;
		case 2:
			m_wndViewAnnoPicture.Load(GetExecPath() + _T("\\Resource\\view_anno_option3.bmp"));
			break;
	}
	m_wndViewAnnoPicture.Draw();
}
