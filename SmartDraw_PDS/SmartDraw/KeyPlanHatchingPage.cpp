// KeyPlanHatchingPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include <IsString.h>
#include "SmartDrawPropertySheet.h"
#include "KeyPlanHatchingPage.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "CADInteractive.h"

#include <string>
using namespace std;

using namespace PropertyPage;
// CKeyPlanHatchingPage dialog

IMPLEMENT_DYNCREATE(CKeyPlanHatchingPage, CSmartDrawPropertyPage)

CKeyPlanHatchingPage::CKeyPlanHatchingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CKeyPlanHatchingPage::IDD)
{

}

CKeyPlanHatchingPage::~CKeyPlanHatchingPage()
{
}

void CKeyPlanHatchingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_KEY_PLAN_HATCHING , m_wndKeyPlanHatchingPicture);
	DDX_Control(pDX , IDC_COMBO_KEYPLAN_HATCHING_TEXT_STYLE , m_cboTextStyle);
	DDX_Control(pDX , IDC_COMBO_KEYPLAN_NAME_LEVEL , m_cboNameLevel);
	DDX_Control(pDX , IDC_BUTTON_KEYPLAN_NAME_COLOR , m_btnNameColor);
	DDX_Control(pDX , IDC_STATIC_THIS_DRAWING_TEXT , m_wndThisDrawingText);
	DDX_Text(pDX , IDC_EDIT_THIS_DRAWING_TEXT , m_sThisDrawingText);
	DDX_Control(pDX , IDC_BUTTON_THIS_DRAWING_PICK , m_wndThisDrawingPickButton);
}

BOOL CKeyPlanHatchingPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	m_sProjectName = pParent->GetProjectName();

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnNameColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_wndKeyPlanHatchingPicture.Load(GetExecPath() + _T("\\Resource\\key_plan_hatching.bmp"));
	m_wndKeyPlanHatchingPicture.Draw();
	
	{
		LOGFONT LogFont;
		CFont BoldFont;
		BoldFont.CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));
		BoldFont.GetLogFont(&LogFont);
		m_wndThisDrawingText.SetTransparent(TRUE);
		m_wndThisDrawingText.SetFont(LogFont);
		m_wndThisDrawingText.SetTextColor(RGB(0,0,255));
	}

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	if(_T("MSTN") != sOutputFormat) m_cboNameLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	m_cboNameLevel.LoadContents(m_sIniFilePath);
	/// up to here

	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_MOUSE_PICK),IMAGE_ICON,0,0,0);
	HICON hOldIcon = m_wndThisDrawingPickButton.SetIcon(hIcon);
	if(hOldIcon) DestroyIcon(hOldIcon);

	LoadData();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CKeyPlanHatchingPage, CSmartDrawPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_THIS_DRAWING_TEXT, &CKeyPlanHatchingPage::OnEnChangeEditThisDrawingText)
END_MESSAGE_MAP()

void CKeyPlanHatchingPage::OnEnChangeEditThisDrawingText()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);

	m_wndThisDrawingText.SetText(m_sThisDrawingText);
}

/**
	@brief	load key plan hatching data
	@author	humkyung
	@date	2014.06.06
*/
int CKeyPlanHatchingPage::LoadData(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CString rBorderFileName(szBuf);
		m_sBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + m_sProjectName + _T("\\Border\\") + rBorderFileName + _T(".cfg");
		
		CString sApp(_T("KeyPlan Hatching"));
		vector<STRING_T> oResult;
		if(GetPrivateProfileString(sApp , _T("Model Position") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_MODEL_X , oResult[0].c_str());
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_MODEL_Y , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(sApp , _T("X Scale") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_X_SCALE, szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("Y Scale") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_Y_SCALE, szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("Angle") , _T("0") , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_KEYPLAN_HATCHING_ANGLE);
			const int iFound = pComboBox->FindStringExact(0 , szBuf);
			(-1 != iFound) ? pComboBox->SetCurSel(iFound) : pComboBox->SetCurSel(0);
		}

		if(GetPrivateProfileString(sApp , _T("This Drawing Text") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			m_sThisDrawingText = szBuf;
		}
		if(GetPrivateProfileString(sApp , _T("Text Position") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_X , oResult[0].c_str());
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_Y , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(sApp , _T("Text Style") , NULL , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			const int iFound = m_cboTextStyle.FindStringExact(0 , szBuf);
			(-1 != iFound) ? m_cboTextStyle.SetCurSel(iFound) : m_cboTextStyle.SetCurSel(0);
		}
		if(GetPrivateProfileString(sApp , _T("Text Level") , _T("") , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			const int at = m_cboNameLevel.FindString(-1 , szBuf);
			m_cboNameLevel.SetCurSel((-1 != at) ? at : 0);
		}
		if(GetPrivateProfileString(sApp , _T("Text Color") , _T("") , szBuf , MAX_PATH , m_sBorderConfigFilePath))
		{
			m_btnNameColor.SetColorIndex(ATOI_T(szBuf));
		}

		UpdateData(FALSE);

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-06
    @function   CKeyPlanHatchingPage::SaveData
    @return     int
    @param      void
    @brief		save keyplan hatching data
******************************************************************************/
int CKeyPlanHatchingPage::SaveData(void)
{
	UpdateData();

	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CString sApp(_T("KeyPlan Hatching"));
	
	vector<STRING_T> oResult;
	CString sValue[2];
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_MODEL_X , sValue[0]);
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_MODEL_Y , sValue[1]);
	WritePrivateProfileString(sApp , _T("Model Position") , sValue[0] + _T(",") + sValue[1] , m_sBorderConfigFilePath);
	
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_X_SCALE , sValue[0]);
	WritePrivateProfileString(sApp , _T("X Scale") , sValue[0] , m_sBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_Y_SCALE , sValue[0]);
	WritePrivateProfileString(sApp , _T("Y Scale") , sValue[0] , m_sBorderConfigFilePath);
	
	GetDlgItem(IDC_COMBO_KEYPLAN_HATCHING_ANGLE)->GetWindowText(sValue[0]);
	WritePrivateProfileString(sApp , _T("Angle") , sValue[0] , m_sBorderConfigFilePath);

	WritePrivateProfileString(sApp , _T("This Drawing Text") , m_sThisDrawingText , m_sBorderConfigFilePath);
	
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_X , sValue[0]);
	GetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_Y , sValue[1]);
	WritePrivateProfileString(sApp , _T("Text Position") , sValue[0] + _T(",") + sValue[1] , m_sBorderConfigFilePath);

	m_cboTextStyle.GetWindowText(sValue[0]);
	WritePrivateProfileString(sApp , _T("Text Style") , sValue[0], m_sBorderConfigFilePath);
	
	CString rString;
	int at = m_cboNameLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboNameLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Text Level") , rString , m_sBorderConfigFilePath);
	}

	at = m_btnNameColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Text Color") , rString , m_sBorderConfigFilePath);

	m_cboNameLevel.SaveContents(m_sIniFilePath);	/// 2014.06.23 added by humkyung

	return ERROR_SUCCESS;
}

/**
	@brief	read text position for "This Drawing"
	@author	humkyung
	@date	2014.12.23
*/
void CKeyPlanHatchingPage::OnBnClickedButtonThisDrawingPick()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
		{
			CSmartDrawDoc* pDoc = GetMDIActiveDocument();
			const CString sPrjName = pDoc->GetProjectName();
			const CString rBorderFilePath = docData.GetServerFolderPath() + _T("\\") + sPrjName + _T("\\Border\\") + szBuf;

			GetTempPath(MAX_PATH , szBuf);
			CString rCmdFilePath = szBuf + CString(_T("run.cmd"));
			double x = 0.0 , y = 0.0;
			const int res = CCADInteractive::GetInputPoint(x , y , rBorderFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
			if(ERROR_SUCCESS == res)
			{
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_X , IsString::TrimedTrailingZero(x).c_str());
				SetDlgItemText(IDC_EDIT_KEYPLAN_HATCHING_TEXT_POS_Y , IsString::TrimedTrailingZero(y).c_str());
			}
		}
	}
}