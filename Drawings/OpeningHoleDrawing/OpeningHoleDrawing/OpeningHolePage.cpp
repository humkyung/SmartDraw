// OpeningHolePage.cpp : implementation file
//

#include "stdafx.h"
#include "OpeningHoleDrawing.h"
#include "OpeningHolePage.h"

COpeningHolePage* COpeningHolePage::instance = NULL;
// COpeningHolePage dialog

IMPLEMENT_DYNAMIC(COpeningHolePage, CSmartDrawPropertyPage)

COpeningHolePage::COpeningHolePage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(COpeningHolePage::IDD, pParent)
{

}

COpeningHolePage::~COpeningHolePage()
{
}

void COpeningHolePage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COpeningHolePage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDOK, &COpeningHolePage::OnBnClickedOk)
END_MESSAGE_MAP()


// COpeningHolePage message handlers
/******************************************************************************
    @author     humkyung
    @date       2012-03-12
    @function   SaveData
    @return     void
    @brief
******************************************************************************/
int COpeningHolePage::SaveData()
{
	m_dlgOpeningHoleGeneral.SaveData( m_sIniFilePath );
	m_dlgOpeningHoleChart.SaveData( m_sIniFilePath );

	return ERROR_SUCCESS;
}

void COpeningHolePage::OnBnClickedOk()
{
}

BOOL COpeningHolePage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	CRect rect;
	this->GetClientRect(&rect);
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D , rect , this, 1))
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;	// fail to create
	}
	m_wndTabs.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabs.ShowWindow(SW_SHOW);

	if(TRUE == m_dlgOpeningHoleGeneral.Create(COpeningHoleGeneralDlg::IDD , &m_wndTabs))
	{
		m_wndTabs.AddTab(&m_dlgOpeningHoleGeneral , _T("General"));
	}
	if(TRUE == m_dlgOpeningHoleChart.Create(COpeningHoleChartDlg::IDD , &m_wndTabs))
	{
		m_wndTabs.AddTab(&m_dlgOpeningHoleChart , _T("Chart"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-12
    @function   GetIniFilePath
    @return     CString
    @brief
******************************************************************************/
CString COpeningHolePage::GetIniFilePath() const
{
	return m_sIniFilePath;
}