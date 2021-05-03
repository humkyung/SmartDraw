// SmartDrawAnnoPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SmartDrawPropertyPage.h"
#include "LevelComboBox.h"

// CSmartDrawPropertyPage dialog

IMPLEMENT_DYNAMIC(CSmartDrawPropertyPage, CDialog)

CSmartDrawPropertyPage::CSmartDrawPropertyPage(UINT nIDTempalte , CWnd* pParent /*=NULL*/)
	: CDialog(nIDTempalte , pParent)
{

}

CSmartDrawPropertyPage::~CSmartDrawPropertyPage()
{
}

void CSmartDrawPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmartDrawPropertyPage, CDialog)
END_MESSAGE_MAP()


// CSmartDrawPropertyPage message handlers

/******************************************************************************
    @author     humkyung
    @date       2012-03-12
    @class      CSmartDrawPropertyPage
    @function   Create
    @return     BOOL
    @param      LPCTSTR pIniFilePath
    @param      UINT    nIDTemplate
    @param      CWnd*   pParentWnd
    @brief
******************************************************************************/
BOOL CSmartDrawPropertyPage::Create(LPCTSTR pIniFilePath , UINT nIDTemplate , CWnd* pParentWnd)
{
	m_sIniFilePath = pIniFilePath;

	return CDialog::Create(nIDTemplate , pParentWnd);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   CSmartDrawPropertyPage::SaveData
    @return     int
    @param      LPCTSTR sSettingFilePath
    @brief
******************************************************************************/
int CSmartDrawPropertyPage::SaveData()
{
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   CSmartDrawPropertyPage::LoadData
    @return     int
    @param      LPCTSTR sSettingFilePath
    @brief
******************************************************************************/
int CSmartDrawPropertyPage::LoadData(LPCTSTR sIniFilePath)
{
	m_sIniFilePath = sIniFilePath;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   GetIniFilePath
    @return     CString
    @brief
******************************************************************************/
CString CSmartDrawPropertyPage::GetIniFilePath() const
{
	return m_sIniFilePath;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL CSmartDrawPropertyPage::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
