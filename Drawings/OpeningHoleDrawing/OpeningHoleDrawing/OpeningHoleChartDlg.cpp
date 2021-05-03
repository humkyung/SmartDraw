// OpeningHoleChartDlg.cpp : implementation file
//

#include "stdafx.h"
#include <IsTools.h>
#include <IsString.h>
#include <util/FileTools.h>
#include <Tokenizer.h>
#include "OpeningHoleDrawing.h"
#include "OpeningHoleChartDlg.h"
#include "OpeningHolePage.h"

// COpeningHoleChartDlg dialog

IMPLEMENT_DYNAMIC(COpeningHoleChartDlg, CDialog)

COpeningHoleChartDlg::COpeningHoleChartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpeningHoleChartDlg::IDD, pParent)
{

}

COpeningHoleChartDlg::~COpeningHoleChartDlg()
{
}

void COpeningHoleChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_STATIC_HOLE_CHART_PICTURE , m_wndHoleChartPicture);
	DDX_Control(pDX , IDC_CHECK_HOLE_CHART , m_btnHoleChartCheck);
	DDX_Control(pDX , IDC_BUTTON_PICK , m_btnPosPick);
	DDX_Control(pDX , IDC_COMBO_CHART_LABEL_TEXT_STYLE , m_wndTextStyleComboBox);
	DDX_Control(pDX , IDC_BUTTON_CHART_COLOR , m_btnChartColor);
	DDX_Control(pDX , IDC_BUTTON_CHART_LABEL_COLOR , m_btnLabelColor);
}


BEGIN_MESSAGE_MAP(COpeningHoleChartDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PICK, &COpeningHoleChartDlg::OnBnClickedButtonPick)
END_MESSAGE_MAP()

/******************************************************************************
    @author     humkyung
    @date       2012-03-14
    @function   SaveData
    @return     int
    @brief
******************************************************************************/
int COpeningHoleChartDlg::SaveData( const CString& sIniFilePath )
{
	static const STRING_T sApp(_T("OpeningHole"));

	WritePrivateProfileString(sApp.c_str() , _T("Chart_OnOff") , ((TRUE == m_btnHoleChartCheck.GetCheck()) ? _T("ON") : _T("OFF")) , sIniFilePath);

	CString rString;
	{
		CString sLocX , sLocY;
		GetDlgItemText(IDC_EDIT_LOC_X , sLocX);
		GetDlgItemText(IDC_EDIT_LOC_Y , sLocY);
		rString.Format(_T("%s,%s") , sLocX , sLocY);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Location") , rString , sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_ROW_HEIGHT , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Chart_RowHeight") , rString , sIniFilePath);

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LEVEL);
	int at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Level") , rString , sIniFilePath);
	}

	at = m_btnChartColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp.c_str() , _T("Chart_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Style") , rString , sIniFilePath);
	}
	
	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Weight") , rString , sIniFilePath);
	}

	{
		CString sWidth[7];

		GetDlgItemText(IDC_EDIT_HOLE_NO_WIDTH , sWidth[0]);
		GetDlgItemText(IDC_EDIT_LINE_NO_WIDTH , sWidth[1]);
		GetDlgItemText(IDC_EDIT_HOLE_SIZE_WIDTH , sWidth[2]);
		GetDlgItemText(IDC_EDIT_PIPE_OD_WIDTH , sWidth[3]);
		GetDlgItemText(IDC_EDIT_INS_THK_WIDTH , sWidth[4]);
		GetDlgItemText(IDC_EDIT_COORDINATE_WIDTH , sWidth[5]);
		GetDlgItemText(IDC_EDIT_ELEVATION_WIDTH , sWidth[6]);

		rString.Format(_T("%s,%s,%s,%s,%s,%s,%s") , sWidth[0] , sWidth[1] , sWidth[2] , sWidth[3] , sWidth[4] , sWidth[5] , sWidth[6]);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Width") , rString , sIniFilePath);
	}

	at = m_wndTextStyleComboBox.GetCurSel();
	if(-1 != at)
	{
		m_wndTextStyleComboBox.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Label_TextStyle") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_LEVEL);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Label_Level") , rString , sIniFilePath);
	}

	at = m_btnLabelColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp.c_str() , _T("Chart_Label_Color") , rString , sIniFilePath);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_STYLE);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Label_Style") , rString , sIniFilePath);
	}

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_WEIGHT);
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Chart_Label_Weight") , rString , sIniFilePath);
	}

	return ERROR_SUCCESS;
}

// COpeningHoleChartDlg message handlers

BOOL COpeningHoleChartDlg::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();

	static const STRING_T sApp(_T("OpeningHole"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);

	COpeningHolePage* pPage = (COpeningHolePage*)(GetParent()->GetParent());
	const CString sIniFilePath = pPage->GetIniFilePath();

	m_wndHoleChartPicture.Load(sExecPath + _T("Resource\\HoleChart.bmp"));
	m_wndHoleChartPicture.Draw();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_MOUSE_PICK),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_btnPosPick.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	m_btnHoleChartCheck.SetLabels(_T("ON") , _T("OFF"));
	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnHoleChartCheck.SetCheck(STRING_T(_T("ON")) == szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Location") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(3 == oResult.size())
		{
			SetDlgItemText(IDC_EDIT_LOC_X , oResult[1].c_str());
			SetDlgItemText(IDC_EDIT_LOC_Y , oResult[2].c_str());
		}
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_RowHeight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ROW_HEIGHT , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnChartColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnChartColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Width") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(7 == oResult.size())
		{
			SetDlgItemText(IDC_EDIT_HOLE_NO_WIDTH , oResult[0].c_str());
			SetDlgItemText(IDC_EDIT_LINE_NO_WIDTH , oResult[1].c_str());
			SetDlgItemText(IDC_EDIT_HOLE_SIZE_WIDTH , oResult[2].c_str());
			SetDlgItemText(IDC_EDIT_PIPE_OD_WIDTH , oResult[3].c_str());
			SetDlgItemText(IDC_EDIT_INS_THK_WIDTH , oResult[4].c_str());
			SetDlgItemText(IDC_EDIT_COORDINATE_WIDTH , oResult[5].c_str());
			SetDlgItemText(IDC_EDIT_ELEVATION_WIDTH , oResult[6].c_str());
		}
	}

	m_wndTextStyleComboBox.FillTextStyleList(sIniFilePath);
	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Label_TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		const int nFound = m_wndTextStyleComboBox.FindString(-1 , szBuf);
		if(-1 != nFound) m_wndTextStyleComboBox.SetCurSel(nFound);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Label_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_LEVEL);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	m_btnLabelColor.FillColors(sIniFilePath);
	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Label_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Label_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_STYLE);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Chart_Label_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_LABEL_WEIGHT);
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-15
    @class      COpeningHoleChartDlg
    @function   OnBnClickedButtonPick
    @return     void
    @brief
******************************************************************************/
void COpeningHoleChartDlg::OnBnClickedButtonPick()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	GetTempPath(MAX_PATH , szBuf);
	CString sCmdFilePath = szBuf + CString(_T("run.cmd"));
	double x = 0.0 , y = 0.0;
	int res = GetInputPointOnMSTNJ(x , y , sCmdFilePath.operator LPCTSTR());
	if(ERROR_SUCCESS == res)
	{
		SetDlgItemText(IDC_EDIT_LOC_X , IsString::TrimedTrailingZero(x).c_str());
		SetDlgItemText(IDC_EDIT_LOC_Y , IsString::TrimedTrailingZero(y).c_str());
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL COpeningHoleChartDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
