// NozzlePage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "NozzlePage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CNozzlePage dialog

IMPLEMENT_DYNAMIC(CNozzlePage, CSmartDrawPropertyPage)

CNozzlePage::CNozzlePage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CNozzlePage::IDD, pParent)
{

}

CNozzlePage::~CNozzlePage()
{
}

void CNozzlePage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_NOZZLE , m_wndNozzlePicture);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_cboArrowType);
	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_cboTextStyle);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT, m_cboLeaderWeight);
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);

	DDX_Text(pDX , IDC_EDIT_LABEL_OFFSET , m_rLabelOffset);
	DDX_Text(pDX , IDC_EDIT_ARROW_SIZE , m_rLeaderArrowSize);
}


BEGIN_MESSAGE_MAP(CNozzlePage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CNozzlePage message handlers

BOOL PropertyPage::CNozzlePage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

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

int PropertyPage::CNozzlePage::LoadData(void)
{
	const CString rApp(_T("Nozzle"));

	m_wndNozzlePicture.Load(GetExecPath() + _T("\\Resource\\nozzle.bmp"));
	m_wndNozzlePicture.Draw();

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

	if(GetPrivateProfileString(rApp , _T("TextStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(rApp , _T("Label_Offset") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_rLabelOffset = szBuf;
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

/******************************************************************************
    @author     humkyung
    @date       2012-03-26
    @function   CNozzlePage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CNozzlePage::SaveData(void)
{
	const CString rApp(_T("Nozzle"));
	UpdateData();

	CString rString;
	
	int nSel = m_cboLeaderLevel.GetCurSel();
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

	WritePrivateProfileString(rApp , _T("Label_Offset") , m_rLabelOffset , m_sIniFilePath);
	
	nSel = m_cboTextStyle.GetCurSel();
	if(-1 != nSel)
	{
		m_cboTextStyle.GetLBText(nSel , rString);
		WritePrivateProfileString(rApp , _T("TextStyle") , rString , m_sIniFilePath);
	}

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
