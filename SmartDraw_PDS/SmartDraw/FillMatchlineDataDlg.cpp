// FillMatchlineDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "FillMatchlineDataDlg.h"
#include "afxdialogex.h"

#include "soci.h"
#include "soci-sqlite3.h"

// CFillMatchlineDataDlg dialog

IMPLEMENT_DYNAMIC(CFillMatchlineDataDlg, CDialog)

CFillMatchlineDataDlg::CFillMatchlineDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFillMatchlineDataDlg::IDD, pParent)
{

}

CFillMatchlineDataDlg::~CFillMatchlineDataDlg()
{
}

void CFillMatchlineDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_MATCHLINE_DRAWING_PREFIX,m_sMatchLineDwgPrefix);
	DDX_Text(pDX,IDC_EDIT_NO_MATCHLINE_DRAWING,m_sNoMatchLineDwg);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CFillMatchlineDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFillMatchlineDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFillMatchlineDataDlg message handlers

/**
	@brief	
	@author	humkyung
*/
BOOL CFillMatchlineDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	{
		{
			soci::rowset<soci::row> rs(oSession.prepare << _T("select Value from PrjConfigs where Name='MatchLine Drawing Prefix'"));
			for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
			{
				if(soci::indicator::i_ok == itr->get_indicator(0))
				{
					SetDlgItemText(IDC_EDIT_MATCHLINE_DRAWING_PREFIX,itr->get<STRING_T>(0).c_str());
				}
			}
		}

		{
			soci::rowset<soci::row> rs(oSession.prepare << _T("select Value from PrjConfigs where Name='No MatchLine Drawing'"));
			for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
			{
				if(soci::indicator::i_ok == itr->get_indicator(0))
				{
					SetDlgItemText(IDC_EDIT_NO_MATCHLINE_DRAWING,itr->get<STRING_T>(0).c_str());
				}
			}
		}
	}

	HICON hDlgIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16,16, 0 );
	HICON hOldIcon = this->SetIcon(hDlgIcon , FALSE);
	if(hOldIcon) DestroyIcon(hOldIcon);

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK),
		IMAGE_ICON,
		16,16, 0 );
	m_wndOKButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL),
		IMAGE_ICON,
		16,16, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);

	return TRUE;
}

void CFillMatchlineDataDlg::OnBnClickedOk()
{
	UpdateData();

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	{
		CString rString;

		oSession << _T("delete from PrjConfigs where Name='MatchLine Drawing Prefix'");
		rString = m_sMatchLineDwgPrefix;
		rString.Replace(_T("'") , _T("''"));
		oSession << _T("insert into PrjConfigs(Name,Value) Values('MatchLine Drawing Prefix','") << rString.operator LPCTSTR() << _T("')");

		oSession << _T("delete from PrjConfigs where Name='No MatchLine Drawing'");
		rString = m_sNoMatchLineDwg;
		rString.Replace(_T("'") , _T("''"));
		oSession << _T("insert into PrjConfigs(Name,Value) Values('No MatchLine Drawing','") << rString.operator LPCTSTR() << _T("')");
	}

	CDialog::OnOK();
}
