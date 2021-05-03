// InsideViewLineNoPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "InsideViewLineNoPage.h"
#include "SmartDrawDoc.h"

#include <fstream>
#include <vector>

using namespace PropertyPage;
// CInsideViewLineNoPage dialog

IMPLEMENT_DYNAMIC(CInsideViewLineNoPage, CSmartDrawPropertyPage)

CInsideViewLineNoPage::CInsideViewLineNoPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CInsideViewLineNoPage::IDD, pParent)
{

}

CInsideViewLineNoPage::~CInsideViewLineNoPage()
{
}

void CInsideViewLineNoPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_LINE_NO_TEXT_STYLE , m_cboLineNoTextStyle);
	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_sLabelOffset);
	DDX_Control(pDX , IDC_COMBO_LINE_NO_LEVEL , m_cboLineNoLevel);
	DDX_Control(pDX , IDC_BUTTON_LINE_NO_COLOR , m_btnLineNoColor);
	DDX_Control(pDX , IDC_COMBO_LINE_NO_STYLE , m_cboLineNoStyle);

	DDX_Control(pDX , IDC_COMBO_ELEV_TEXT_STYLE , m_cboElevTextStyle);
	DDX_Control(pDX , IDC_COMBO_ELEV_LEVEL , m_cboElevLevel);
	DDX_Control(pDX , IDC_BUTTON_ELEV_COLOR , m_btnElevColor);
	DDX_Control(pDX , IDC_COMBO_ELEV_STYLE , m_cboElevStyle);

	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_cboArrowType);
	DDX_Text(pDX , IDC_EDIT_ARROW_SIZE , m_sArrowSize);
	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
	
	DDX_Control(pDX , IDC_STATIC_LINE_NO_1 , m_wndLineNo1Label);
	DDX_Control(pDX , IDC_STATIC_LINE_NO_2 , m_wndLineNo2Label);
	DDX_Control(pDX , IDC_STATIC_ELEV_1 , m_wndElev1Label);
	DDX_Control(pDX , IDC_STATIC_ELEV_2 , m_wndElev2Label);
}


BEGIN_MESSAGE_MAP(CInsideViewLineNoPage, CSmartDrawPropertyPage)
	ON_NOTIFY (UDM_TOOLTIP_DISPLAY, NULL, NotifyDisplayTooltip)
	ON_BN_CLICKED(IDC_RADIO_ELEV, &CInsideViewLineNoPage::OnBnClickedRadioElev)
	ON_BN_CLICKED(IDC_RADIO_ELEV_COORD, &CInsideViewLineNoPage::OnBnClickedRadioElevCoord)
	ON_BN_CLICKED(IDC_RADIO_ONLY_LINE_NO, &CInsideViewLineNoPage::OnBnClickedRadioOnlyLineNo)
	ON_BN_CLICKED(IDC_RADIO_LINE_NO_ELEV, &CInsideViewLineNoPage::OnBnClickedRadioLineNoElev)
	ON_BN_CLICKED(IDC_RADIO_LINE_NO_OVER_ELEV, &CInsideViewLineNoPage::OnBnClickedRadioLineNoOverElev)
END_MESSAGE_MAP()


// CInsideViewLineNoPage message handlers

BOOL PropertyPage::CInsideViewLineNoPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_cboLineNoTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLineNoColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_cboElevTextStyle.FillTextStyleList(m_sIniFilePath);
	m_btnElevColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	
	{
		LOGFONT LogFont;
		CFont BoldFont;
		BoldFont.CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));
		BoldFont.GetLogFont(&LogFont);
		m_wndLineNo1Label.SetTransparent(TRUE);
		m_wndLineNo1Label.SetFont(LogFont);
		m_wndLineNo1Label.SetTextColor(RGB(0,0,255));
		m_wndLineNo2Label.SetTransparent(TRUE);
		m_wndLineNo2Label.SetFont(LogFont);
		m_wndLineNo2Label.SetTextColor(RGB(0,0,255));
		m_wndElev1Label.SetTransparent(TRUE);
		m_wndElev1Label.SetFont(LogFont);
		m_wndElev1Label.SetTextColor(RGB(0,0,255));
		m_wndElev2Label.SetTransparent(TRUE);
		m_wndElev2Label.SetFont(LogFont);
		m_wndElev2Label.SetTextColor(RGB(0,0,255));
	}

	m_tooltip[0].Create(this);
	m_tooltip[0].AddTool(GetDlgItem(IDC_COMBO_LINE_NO_TEXT_STYLE), _T("Tooltip to the control IDC_COMBO_LINE_NO_TEXT_STYLE"));
	m_tooltip[0].SetNotify();
	m_tooltip[1].Create(this);
	m_tooltip[1].AddTool(GetDlgItem(IDC_COMBO_ELEV_TEXT_STYLE), _T("Tooltip to the control IDC_COMBO_ELEV_TEXT_STYLE"));
	m_tooltip[1].SetNotify();

	/// load contents - 2014.06.23 added by humkyung
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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CInsideViewLineNoPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CInsideViewLineNoPage::LoadData()
{
	static const CString sApp(_T("Inside Pipe"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ONLY_LINE_NO);
		pButton->SetCheck(STRING_T(_T("Only LineNo")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_ELEV);
		pButton->SetCheck(STRING_T(_T("LineNo+Elev")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_OVER_ELEV);
		pButton->SetCheck(STRING_T(_T("LineNo/Elev")) == szBuf);
	}

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

	UpdateData(FALSE);
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CInsideViewLineNoPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CInsideViewLineNoPage::SaveData()
{
	static const CString sApp(_T("Inside Pipe"));
	UpdateData();

	CString rString;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ONLY_LINE_NO);
	if(pButton->GetCheck()) rString = _T("Only LineNo");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_ELEV);
	if(pButton->GetCheck()) rString = _T("LineNo+Elev");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINE_NO_OVER_ELEV);
	if(pButton->GetCheck()) rString = _T("LineNo/Elev");
	WritePrivateProfileString(sApp , _T("DisplayStyle") , rString , m_sIniFilePath);

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

	return ERROR_SUCCESS;
}

BOOL PropertyPage::CInsideViewLineNoPage::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip[0].RelayEvent(pMsg); 
	m_tooltip[1].RelayEvent(pMsg); 

	return CSmartDrawPropertyPage::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.17
*/
void PropertyPage::CInsideViewLineNoPage::NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	*result = 0;	
	NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)pNMHDR;

	switch (CWnd::FromHandle(pNotify->hwndTool)->GetDlgCtrlID())
	{
		case IDC_COMBO_LINE_NO_TEXT_STYLE:
		{
		//Changes the tooltip's text
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_LINE_NO_TEXT_STYLE);
			const int nSel = pComboBox->GetCurSel();
			if(-1 != nSel)
			{
				CString rTextStyle , rToolTipString;
				pComboBox->GetLBText(nSel , rTextStyle);

				CString rTextStyleKeyName;
				if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
				{
					rTextStyleKeyName = CString(szBuf) + _T("_TEXT_STYLE");

					vector<STRING_T> oResult;
					if(GetPrivateProfileString(rTextStyleKeyName , rTextStyle , NULL , szBuf , MAX_PATH , m_sIniFilePath))
					{
						CTokenizer<CIsComma>::Tokenize(oResult , szBuf  , CIsComma());

						rToolTipString.Format(_T("<table><tr><td valign=vcenter><icon idres=141 width=32 height=32></td>")\
							_T("<td><center><h2>%s Text Style</h2><br><hr color=blue></center>")\
							_T("<br>Font = %s")\
							_T("<br>Text Height = %s")\
							_T("<br>Width Factor= %s")\
							_T("<br>Oblique = %s")\
							_T("<br>Weight = %s")\
							_T("</td></tr></table>") ,
							rTextStyle , 
							oResult[0].c_str() , oResult[1].c_str() , oResult[2].c_str() , oResult[3].c_str() , oResult[4].c_str()
							);
						pNotify->ti->sTooltip = rToolTipString;
					}
				}
			}
			break;
		}
		case IDC_COMBO_COORDINATE_TEXT_STYLE:
		{
		//Changes the tooltip's text
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_COORDINATE_TEXT_STYLE);
			const int nSel = pComboBox->GetCurSel();
			if(-1 != nSel)
			{
				CString rTextStyle , rToolTipString;
				pComboBox->GetLBText(nSel , rTextStyle);

				CString rTextStyleKeyName;
				if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
				{
					rTextStyleKeyName = CString(szBuf) + _T("_TEXT_STYLE");

					vector<STRING_T> oResult;
					if(GetPrivateProfileString(rTextStyleKeyName , rTextStyle , NULL , szBuf , MAX_PATH , m_sIniFilePath))
					{
						CTokenizer<CIsComma>::Tokenize(oResult , szBuf  , CIsComma());

						rToolTipString.Format(_T("<table><tr><td valign=vcenter><icon idres=141 width=32 height=32></td>")\
							_T("<td><center><h2>%s Text Style</h2><br><hr color=blue></center>")\
							_T("<br>Font = %s")\
							_T("<br>Text Height = %s")\
							_T("<br>Width Factor= %s")\
							_T("<br>Oblique = %s")\
							_T("<br>Weight = %s")\
							_T("</td></tr></table>") ,
							rTextStyle , 
							oResult[0].c_str() , oResult[1].c_str() , oResult[2].c_str() , oResult[3].c_str() , oResult[4].c_str()
							);
						pNotify->ti->sTooltip = rToolTipString;
					}
				}
			}
		}
			break;
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CInsideViewLineNoPage::OnBnClickedRadioElev()
{
	m_wndElev1Label.SetText(_T("CL. 745"));
	m_wndElev2Label.SetText(_T("CL. 757"));
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CInsideViewLineNoPage::OnBnClickedRadioElevCoord()
{
	m_wndElev1Label.SetText(_T("CL. 745 N. 900"));
	m_wndElev2Label.SetText(_T("CL. 757 N. 500"));
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CInsideViewLineNoPage::OnBnClickedRadioOnlyLineNo()
{
	CString rString;
	m_wndLineNo1Label.GetWindowText(rString);
	int nFound = rString.Find('(');
	if(-1 != nFound) rString = rString.Left(nFound);
	m_wndLineNo1Label.SetText(rString);
	m_wndLineNo2Label.GetWindowText(rString);
	nFound = rString.Find('(');
	if(-1 != nFound) rString = rString.Left(nFound);
	m_wndLineNo2Label.SetText(rString);

	m_wndElev1Label.ShowWindow(SW_HIDE);
	m_wndElev2Label.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CInsideViewLineNoPage::OnBnClickedRadioLineNoElev()
{
	CString rString[2];

	m_wndLineNo1Label.GetWindowText(rString[0]);
	m_wndElev1Label.GetWindowText(rString[1]);
	m_wndLineNo1Label.SetText(rString[0] + _T("(") + rString[1] + _T(")"));
	m_wndElev1Label.ShowWindow(SW_HIDE);
	m_wndLineNo2Label.GetWindowText(rString[0]);
	m_wndElev2Label.GetWindowText(rString[1]);
	m_wndLineNo2Label.SetText(rString[0] + _T("(") + rString[1] + _T(")"));
	m_wndElev2Label.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.23
*/
void PropertyPage::CInsideViewLineNoPage::OnBnClickedRadioLineNoOverElev()
{
	CString rString;
	m_wndLineNo1Label.GetWindowText(rString);
	int nFound = rString.Find('(');
	if(-1 != nFound) rString = rString.Left(nFound);
	m_wndLineNo1Label.SetText(rString);
	m_wndLineNo2Label.GetWindowText(rString);
	nFound = rString.Find('(');
	if(-1 != nFound) rString = rString.Left(nFound);
	m_wndLineNo2Label.SetText(rString);

	m_wndElev1Label.ShowWindow(SW_SHOW);
	m_wndElev2Label.ShowWindow(SW_SHOW);
}