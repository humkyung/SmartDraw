// SmartDrawAnnoPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawAnnoPropertyPage.h"

using namespace PropertyPage;
// CSmartDrawAnnoPropertyPage dialog

IMPLEMENT_DYNAMIC(CSmartDrawAnnoPropertyPage, CDialog)

CSmartDrawAnnoPropertyPage::CSmartDrawAnnoPropertyPage(UINT nIDTempalte , CWnd* pParent /*=NULL*/)
	: CDialog(nIDTempalte , pParent)
{

}

CSmartDrawAnnoPropertyPage::~CSmartDrawAnnoPropertyPage()
{
}

void CSmartDrawAnnoPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmartDrawAnnoPropertyPage, CDialog)
END_MESSAGE_MAP()


// CSmartDrawAnnoPropertyPage message handlers

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   CSmartDrawAnnoPropertyPage::SaveData
    @return     int
    @param      LPCTSTR sSettingFilePath
    @brief
******************************************************************************/
int PropertyPage::CSmartDrawAnnoPropertyPage::SaveData()
{
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   CSmartDrawAnnoPropertyPage::LoadData
    @return     int
    @param      LPCTSTR sSettingFilePath
    @brief
******************************************************************************/
int PropertyPage::CSmartDrawAnnoPropertyPage::LoadData(LPCTSTR sSettingFilePath)
{
	m_rSettingFilePath = sSettingFilePath;
	return ERROR_SUCCESS;
}