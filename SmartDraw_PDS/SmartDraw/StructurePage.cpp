// StructurePage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "StructurePage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CStructurePage dialog

IMPLEMENT_DYNAMIC(CStructurePage, CSmartDrawPropertyPage)

CStructurePage::CStructurePage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CStructurePage::IDD, pParent)
{

}

CStructurePage::~CStructurePage()
{
}

void CStructurePage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_LINE_COLOR , m_btnLeaderColor);
	
	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE  , m_cboTextStyle);
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);

	DDX_Text(pDX , IDC_EDIT_DELIMITER , m_rDelimiter);

	DDX_Text(pDX , IDC_EDIT_BUBBLE_SIZE , m_rBubbleSize);
	DDX_Control(pDX , IDC_COMBO_LINE_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LINE_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LINE_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LINE_WEIGHT, m_cboLeaderWeight);

	DDX_Control(pDX , IDC_COMBO_GRID_CIRCLE_LEVEL , m_cboGridCircleLevel);	/// 2015.03.23 added by humkyung
}


BEGIN_MESSAGE_MAP(CStructurePage, CSmartDrawPropertyPage)
	ON_NOTIFY (UDM_TOOLTIP_DISPLAY, NULL, NotifyDisplayTooltip)
	ON_BN_CLICKED(IDC_RADIO_COLUMN_DATA, &CStructurePage::OnBnClickedButtonColumnData)
	ON_BN_CLICKED(IDC_RADIO_GRID_LINE_DATA, &CStructurePage::OnBnClickedButtonGridLineData)
END_MESSAGE_MAP()


// CStructurePage message handlers

BOOL PropertyPage::CStructurePage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_tooltip.Create(this);
	m_tooltip.AddTool(GetDlgItem(IDC_COMBO_TEXT_STYLE), _T("Tooltip to the control IDC_COMBO_TEXT_STYLE"));
	m_tooltip.SetNotify();

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboGridCircleLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboLabelLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	m_cboGridCircleLevel.LoadContents(m_sIniFilePath);
	/// up to here

	((CButton*)GetDlgItem(IDC_RADIO_COLUMN_DATA))->SetCheck(TRUE);

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL PropertyPage::CStructurePage::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg); 

	return CSmartDrawPropertyPage::PreTranslateMessage(pMsg);
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.17
*/
void PropertyPage::CStructurePage::NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	*result = 0;	
	NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)pNMHDR;

	switch (CWnd::FromHandle(pNotify->hwndTool)->GetDlgCtrlID())
	{
		case IDC_COMBO_TEXT_STYLE:
		{
		//Changes the tooltip's text
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TEXT_STYLE);
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
	}
}

/**
	@brief	load column setting
	@author	HumKyung.BAEK
	@date	2010.06.18
*/
int PropertyPage::CStructurePage::LoadData(void)
{
	static const CString sApp("Structure");

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = NULL;

		CTokenizer<CIsFromString>::Tokenize(oResult , szBuf , CIsFromString(_T("/")));
		if(2 == oResult.size())
		{
			pButton = (CButton*)GetDlgItem(IDC_RADIO_UP);
			pButton->SetCheck(_T("UP") == oResult[0]);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_DOWN);
			pButton->SetCheck(_T("DOWN") == oResult[0]);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_UP_DOWN);
			pButton->SetCheck(_T("BOTH") == oResult[0]);

			pButton = (CButton*)GetDlgItem(IDC_RADIO_LEFT);
			pButton->SetCheck(_T("LEFT") == oResult[1]);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_RIGHT);
			pButton->SetCheck(_T("RIGHT") == oResult[1]);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_LEFT_RIGHT);
			pButton->SetCheck(_T("BOTH") == oResult[1]);
		}
	}
	
	if(GetPrivateProfileString(sApp , _T("Delimiter") , _T("-") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rDelimiter = szBuf;
	}
	if(GetPrivateProfileString(sApp , _T("BubbleSize") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rBubbleSize = szBuf;
	}
	if(GetPrivateProfileString(sApp , _T("TextStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	
	if(GetPrivateProfileString(sApp , _T("Label_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelLevel.FindStringExact(-1 , szBuf);
		m_cboLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Label_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLabelStyle.FindStringExact(-1 , szBuf);
		m_cboLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}
	
	if(GetPrivateProfileString(sApp , _T("Leader_Level") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderLevel.FindStringExact(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Color") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Style") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderStyle.FindStringExact(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Weight") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderWeight.FindStringExact(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}

	/// read column name type - 2014.12.26 added by humkyung
	GetPrivateProfileString(sApp , _T("Column Name Type") , _T("Column Data") , szBuf , MAX_PATH , m_sIniFilePath);
	((CButton*)GetDlgItem(IDC_RADIO_COLUMN_DATA))->SetCheck(CString(_T("Column Data")) == szBuf);
	((CButton*)GetDlgItem(IDC_RADIO_GRID_LINE_DATA))->SetCheck(CString(_T("Grid Line Data")) == szBuf);
	/// up to here

	if(GetPrivateProfileString(sApp , _T("Grid_Circle_Level") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboGridCircleLevel.FindStringExact(-1 , szBuf);
		m_cboGridCircleLevel.SetCurSel((-1 != at) ? at : 0);
	}

	/// read column name format - 2014.11.17 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Column Name Direction") , _T("East West-North South") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT1);
		pButton->SetCheck(CString(_T("North South-East West")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT2);
		pButton->SetCheck(CString(_T("East West-North South")) == szBuf);
	}
	/// up to here

	if(TRUE == ((CButton*)GetDlgItem(IDC_RADIO_GRID_LINE_DATA))->GetCheck())
	{
		OnBnClickedButtonGridLineData();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	save structure settings
	@author	HumKyung.BAEK
	@date	2010.06.18
*/
int PropertyPage::CStructurePage::SaveData(void)
{
	static const CString sApp(_T("Structure"));

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	UpdateData();

	CString rString;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UP);
	if(pButton->GetCheck()) rString = _T("UP");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_DOWN);
	if(pButton->GetCheck()) rString = _T("DOWN");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_UP_DOWN);
	if(pButton->GetCheck()) rString = _T("BOTH");

	pButton = (CButton*)GetDlgItem(IDC_RADIO_LEFT);
	if(pButton->GetCheck()) rString += _T("/LEFT");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_RIGHT);
	if(pButton->GetCheck()) rString += _T("/RIGHT");
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LEFT_RIGHT);
	if(pButton->GetCheck()) rString += _T("/BOTH");
	WritePrivateProfileString(sApp , _T("DisplayStyle") , rString , m_sIniFilePath);

	WritePrivateProfileString(sApp , _T("Delimiter") , m_rDelimiter , m_sIniFilePath);
	WritePrivateProfileString(sApp , _T("BubbleSize") , m_rBubbleSize , m_sIniFilePath);
	
	const int nSel = m_cboTextStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboTextStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp , _T("TextStyle") , rString , m_sIniFilePath);
	}
	int at = m_cboLabelLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Level") , rString , m_sIniFilePath);
	}

	rString.Format(_T("%d") , m_btnLabelColor.GetColorIndex());
	WritePrivateProfileString(sApp , _T("Label_Color") , rString , m_sIniFilePath);
	at = m_cboLabelStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Style") , rString , m_sIniFilePath);
	}

	at = m_cboLeaderLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Level") , rString , m_sIniFilePath);
	}

	rString.Format(_T("%d") , m_btnLeaderColor.GetColorIndex());
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

	/// save column name type - 2014.12.26 added by humkyung
	rString = (TRUE == ((CButton*)GetDlgItem(IDC_RADIO_COLUMN_DATA))->GetCheck()) ? _T("Column Data") : _T("Grid Line Data");
	WritePrivateProfileString(sApp , _T("Column Name Type") , rString , m_sIniFilePath);
	/// up to here

	at = m_cboGridCircleLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboGridCircleLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Grid_Circle_Level") , rString , m_sIniFilePath);
	}

	/// save column name format - 2014.11.17 added by humkyung
	pButton = (CButton*)GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT1);
	rString = (TRUE == pButton->GetCheck()) ? _T("North South-East West") : _T("East West-North South");
	WritePrivateProfileString(sApp , _T("Column Name Direction") , rString , m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}

/**
	@brief	check north south-east west option
	@author	humkyung
	@date	2014.12.27
*/
void PropertyPage::CStructurePage::OnBnClickedButtonGridLineData()
{
	((CButton*)GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT1))->SetCheck(TRUE);
	GetDlgItem(IDC_COMBO_GRID_CIRCLE_LEVEL)->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT2))->SetCheck(FALSE);
	GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DELIMITER)->EnableWindow(FALSE);
}

/**
	@brief	
	@author	humkyung
	@date	2014.12.27
*/
void PropertyPage::CStructurePage::OnBnClickedButtonColumnData()
{
	GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_COLUMN_NAME_FORMAT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_DELIMITER)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_GRID_CIRCLE_LEVEL)->EnableWindow(FALSE);
}