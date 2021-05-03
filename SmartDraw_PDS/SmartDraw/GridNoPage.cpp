// GridNoPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <IsString.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "GridNoPage.h"
#include "SmartDrawDoc.h"
#include "SmartDrawPropertySheet.h"
#include "CADInteractive.h"

using namespace PropertyPage;
// CGridNoPage dialog

IMPLEMENT_DYNAMIC(CGridNoPage, CSmartDrawPropertyPage)

CGridNoPage::CGridNoPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CGridNoPage::IDD, pParent)
{

}

CGridNoPage::~CGridNoPage()
{
}

void CGridNoPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_GRID_NO , m_wndGridNoPicture);
	DDX_Control(pDX , IDC_BUTTON_BASE_POSITION , m_wndBasePositionButtonn);
	DDX_Control(pDX , IDC_BUTTON_X_AXIS_DISTANCE , m_wndXAxisDistanceButton);
	DDX_Control(pDX , IDC_BUTTON_Y_AXIS_DISTANCE , m_wndYAxisDistanceButton);
}


BEGIN_MESSAGE_MAP(CGridNoPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BASE_POSITION, &CGridNoPage::OnBnClickedButtonBasePosition)
END_MESSAGE_MAP()


// CGridNoPage message handlers

BOOL PropertyPage::CGridNoPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	m_wndGridNoPicture.Load(GetExecPath() + _T("\\Resource\\grid_no.bmp"));
	m_wndGridNoPicture.Draw();

	LoadData();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_MOUSE_PICK),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndBasePositionButtonn.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	load data and display
	@author HumKyung.BAEK	
	@date	2010.07.28
	@return		
**/
int PropertyPage::CGridNoPage::LoadData(void)
{
	static const CString sApp(_T("Grid No"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
		const CString rProjectName(pParent->GetProjectName());

		CString rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Border\\") + CString(szBuf) + _T(".cfg");

		vector<STRING_T> oResult;
		if(GetPrivateProfileString(sApp , _T("Origin Pos") , NULL , szBuf , MAX_PATH , rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(3 == oResult.size())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPPER_LEFT);
				pButton->SetCheck(0 == STRICMP_T(_T("UpperLeft") , oResult[0].c_str()));
				pButton = (CButton*)GetDlgItem(IDC_RADIO_UPPER_RIGHT);
				pButton->SetCheck(0 == STRICMP_T(_T("UpperRight") , oResult[0].c_str()));
				pButton = (CButton*)GetDlgItem(IDC_RADIO_LOWER_LEFT);
				pButton->SetCheck(0 == STRICMP_T(_T("LowerLeft") , oResult[0].c_str()));
				pButton = (CButton*)GetDlgItem(IDC_RADIO_LOWER_RIGHT);
				pButton->SetCheck(0 == STRICMP_T(_T("LowerRight") , oResult[0].c_str()));
				
				SetDlgItemText(IDC_EDIT_X , oResult[1].c_str());
				SetDlgItemText(IDC_EDIT_Y , oResult[2].c_str());
			}
		}

		if(GetPrivateProfileString(sApp , _T("X-Axis") , NULL , szBuf , MAX_PATH , rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_XAXIS_NUMERIC);
				pButton->SetCheck(0 == STRICMP_T(_T("1-2") , oResult[0].c_str()));
				pButton = (CButton*)GetDlgItem(IDC_RADIO_XAXIS_ALPHABETIC);
				pButton->SetCheck(0 == STRICMP_T(_T("A-B") , oResult[0].c_str()));

				SetDlgItemText(IDC_EDIT_XAXIS_DIST , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(sApp , _T("Y-Axis") , NULL , szBuf , MAX_PATH , rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_YAXIS_NUMERIC);
				pButton->SetCheck(0 == STRICMP_T(_T("1-2") , oResult[0].c_str()));
				pButton = (CButton*)GetDlgItem(IDC_RADIO_YAXIS_ALPHABETIC);
				pButton->SetCheck(0 == STRICMP_T(_T("A-B") , oResult[0].c_str()));

				SetDlgItemText(IDC_EDIT_YAXIS_DIST , oResult[1].c_str());
			}
		}
	}
	else
	{
		AfxMessageBox(_T("There is no border file") , MB_OK | MB_ICONWARNING);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	save data

	@author HumKyung.BAEK	

	@date	2010.07.28

	@return		
**/
int PropertyPage::CGridNoPage::SaveData()
{
	static const CString sApp(_T("Grid No"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
		const CString rProjectName(pParent->GetProjectName());

		CString rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Border\\") + CString(szBuf) + _T(".cfg");

		CString rJustify;
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPPER_LEFT);
			if(pButton && pButton->GetCheck()) rJustify = _T("UpperLeft");
			pButton = (CButton*)GetDlgItem(IDC_RADIO_UPPER_RIGHT);
			if(pButton && pButton->GetCheck()) rJustify = _T("UpperRight");
			pButton = (CButton*)GetDlgItem(IDC_RADIO_LOWER_LEFT);
			if(pButton && pButton->GetCheck()) rJustify = _T("LowerLeft");
			pButton = (CButton*)GetDlgItem(IDC_RADIO_LOWER_RIGHT);
			if(pButton && pButton->GetCheck()) rJustify = _T("LowerRight");
		}

		CString rLocX , rLocY;
		GetDlgItemText(IDC_EDIT_X , rLocX);
		GetDlgItemText(IDC_EDIT_Y , rLocY);
		WritePrivateProfileString(sApp , _T("Origin Pos") , rJustify + _T(",") + rLocX + _T(",") + rLocY , rBorderConfigFilePath);

		CString rAxisNumbering;
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_XAXIS_NUMERIC);
			if(pButton && pButton->GetCheck()) rAxisNumbering = _T("1-2");
			pButton = (CButton*)GetDlgItem(IDC_RADIO_XAXIS_ALPHABETIC);
			if(pButton && pButton->GetCheck()) rAxisNumbering = _T("A-B");
		}
		CString rAxisDist;
		GetDlgItemText(IDC_EDIT_XAXIS_DIST , rAxisDist);
		WritePrivateProfileString(sApp , _T("X-Axis") , rAxisNumbering + _T(",") + rAxisDist , rBorderConfigFilePath);

		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_YAXIS_NUMERIC);
			if(pButton && pButton->GetCheck()) rAxisNumbering = _T("1-2");
			pButton = (CButton*)GetDlgItem(IDC_RADIO_YAXIS_ALPHABETIC);
			if(pButton && pButton->GetCheck()) rAxisNumbering = _T("A-B");
		}
		GetDlgItemText(IDC_EDIT_YAXIS_DIST , rAxisDist);
		WritePrivateProfileString(sApp , _T("Y-Axis") , rAxisNumbering + _T(",") + rAxisDist , rBorderConfigFilePath);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	get origin point for grid no
	@author	humkyung
	@date	2014.08.25
*/
void PropertyPage::CGridNoPage::OnBnClickedButtonBasePosition()
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
			int res = CCADInteractive::GetInputPoint(x , y , rBorderFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
			if(ERROR_SUCCESS == res)
			{
				SetDlgItemText(IDC_EDIT_X , IsString::TrimedTrailingZero(x).c_str());
				SetDlgItemText(IDC_EDIT_Y , IsString::TrimedTrailingZero(y).c_str());
			}
		}
	}
}