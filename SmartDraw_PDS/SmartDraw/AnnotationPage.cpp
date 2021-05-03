// AnnotationPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <IsTools.h>
#include "AnnotationPage.h"
#include "SmartDrawPropertySheet.h"

using namespace PropertyPage;
// CAnnotationPage dialog

IMPLEMENT_DYNAMIC(CAnnotationPage, CSmartDrawPropertyPage)

CAnnotationPage::CAnnotationPage()
	: CSmartDrawPropertyPage(CAnnotationPage::IDD)
{

}

CAnnotationPage::~CAnnotationPage()
{
}

void CAnnotationPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnnotationPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CAnnotationPage message handlers

BOOL CAnnotationPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_rSettingFilePath = pParent->GetSettingFilePath();
	this->LoadData(m_rSettingFilePath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-18
    @function   CAnnotationPage::LoadData
    @return     int
    @param      LPCTSTR sSettingFilePath
    @brief
******************************************************************************/
int PropertyPage::CAnnotationPage::LoadData(LPCTSTR sSettingFilePath)
{
	static const STRING_T sApp=_T("OUTSIDE_OPTIONS");
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_PIPE/EQP") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_PIPE_EQP_DIST , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("INSIDE DIST_PIPE/EQP") , _T("0") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_INSIDE_PIPE_EQP_DIST , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_ELEV") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_ELEV_DIST , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_STR") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_STR_DIST , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_A") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_DIST_A , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_B") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_DIST_B , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_C") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_DIST_C , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_D") , _T("3") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_DIST_D , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_E") , _T("3") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_DIST_E , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_E1") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_E1 , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_E2") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_E2 , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("DIST_O1") , _T("") , szBuf , MAX_PATH , m_rSettingFilePath))
	{
		SetDlgItemText(IDC_EDIT_O1 , szBuf);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-18
    @function   CAnnotationPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CAnnotationPage::SaveData()
{
	static const STRING_T sApp=_T("OUTSIDE_OPTIONS");
	CString rString;
	
	GetDlgItemText(IDC_EDIT_PIPE_EQP_DIST , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_PIPE/EQP") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_INSIDE_PIPE_EQP_DIST , rString);
	WritePrivateProfileString(sApp.c_str() , _T("INSIDE DIST_PIPE/EQP") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_ELEV_DIST , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_ELEV") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_STR_DIST , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_STR") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_DIST_A , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_A") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_DIST_B , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_B") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_DIST_C , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_C") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_DIST_D , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_D") , rString , m_rSettingFilePath);
	
	GetDlgItemText(IDC_EDIT_DIST_E , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_E") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_E1 , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_E1") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_E2 , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_E2") , rString , m_rSettingFilePath);

	GetDlgItemText(IDC_EDIT_O1 , rString);
	WritePrivateProfileString(sApp.c_str() , _T("DIST_O1") , rString , m_rSettingFilePath);

	return ERROR_SUCCESS;
}