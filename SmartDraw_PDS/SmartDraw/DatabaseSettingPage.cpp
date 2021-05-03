// DatabaseSettingPage.cpp : implementation file
//

#include "stdafx.h"
#include <util/FileTools.h>
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "DatabaseSettingPage.h"
#include "ProjectSettingSheet.h"

using namespace PropertyPage;
// CDatabaseSettingPage dialog

/******************************************************************************
    @author     humkyung
    @date       2012-04-10
    @function   OnBrowse
    @return     void
    @brief
******************************************************************************/
void CDatabaseSettingPage::CAccessFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	/// ÆÄÀÏ Open 
	CFileDialog dlg(TRUE,_T("mdb,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Access File(*.mdb)|*.mdb|All Documents(*.*)|*.*||"));
	if(IDOK == dlg.DoModal())
	{
		m_sFilePath = dlg.GetPathName();
		SetWindowText(m_sFilePath);
	}
}

IMPLEMENT_DYNAMIC(CDatabaseSettingPage, CSmartDrawPropertyPage)

CDatabaseSettingPage::CDatabaseSettingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CDatabaseSettingPage::IDD, pParent)
{

}

CDatabaseSettingPage::~CDatabaseSettingPage()
{
}

void CDatabaseSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_EDIT_ACCESS_FILE_PATH , m_wndAccessFilePath);
	DDX_ManagedControl(pDX, IDC_CTRL_PROGRESS, m_ctrlSmartDraw);
}

BEGIN_MESSAGE_MAP(CDatabaseSettingPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_ORACLE, &CDatabaseSettingPage::OnBnClickedRadioOracle)
	ON_BN_CLICKED(IDC_RADIO_SQL_SERVER, &CDatabaseSettingPage::OnBnClickedRadioSqlServer)
	ON_BN_CLICKED(IDC_RADIO_ACCESS, &CDatabaseSettingPage::OnBnClickedRadioAccess)
	ON_BN_CLICKED(IDC_RADIO_DRV_FILE, &CDatabaseSettingPage::OnBnClickedRadioDrvFile)
	ON_BN_CLICKED(IDC_BUTTON_TEST_PROJECT_DATBASE_CONNECTION , &CDatabaseSettingPage::OnBnClickedProjectDatabaseConnection)
	ON_BN_CLICKED(IDC_BUTTON_TEST_DESIGN_DATABASE_CONNECTION , &CDatabaseSettingPage::OnBnClickedDesignDatabaseConnection)
	ON_BN_CLICKED(IDC_BUTTON_DATABASE_DUMP, &CDatabaseSettingPage::OnBnClickedButtonDatabaseDump)
END_MESSAGE_MAP()


// CDatabaseSettingPage message handlers

BOOL PropertyPage::CDatabaseSettingPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	m_wndAccessFilePath.EnableFolderBrowseButton();

	LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-10
    @function   CDatabaseSettingPage::OnBnClickedRadioOracle
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CDatabaseSettingPage::OnBnClickedRadioOracle()
{
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_ORACLE));
	GetDlgItem(IDC_EDIT_ACCESS_FILE_PATH)->EnableWindow(FALSE);
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-09
    @class      PropertyPage
    @function   CDatabaseSettingPage::OnBnClickedRadioSqlServer
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CDatabaseSettingPage::OnBnClickedRadioSqlServer()
{
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_SQL_SERVER));
	const BOOL bCheck = pButton->GetCheck();
	GetDlgItem(IDC_STATIC_SQL_SERVER)->EnableWindow(bCheck);
	GetDlgItem(IDC_RADIO_WINDOWS_USER)->EnableWindow(bCheck);
	GetDlgItem(IDC_RADIO_SQL_SERVER_USER)->EnableWindow(bCheck);
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-10
    @function   CDatabaseSettingPage::OnBnClickedRadioAccess
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CDatabaseSettingPage::OnBnClickedRadioAccess()
{
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_ACCESS));
	const BOOL bCheck = pButton->GetCheck();
	
	GetDlgItem(IDC_BUTTON_DATABASE_DUMP)->EnableWindow(TRUE);
}

/******************************************************************************
    @brief		
	@author     humkyung
    @date       2014-07-29
    @function   CDatabaseSettingPage::OnBnClickedRadioDrvFile
    @return     void
******************************************************************************/
void PropertyPage::CDatabaseSettingPage::OnBnClickedRadioDrvFile()
{
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_DRV_FILE));
	const BOOL bCheck = pButton->GetCheck();

	GetDlgItem(IDC_BUTTON_DATABASE_DUMP)->EnableWindow(FALSE);
}

/**
	@brief	connection test for project database
	@author	humkyung
	@date	2012.09.19
*/
void PropertyPage::CDatabaseSettingPage::OnBnClickedProjectDatabaseConnection()
{
	CSmartDrawApp* pApp = static_cast<CSmartDrawApp*>(AfxGetApp());
	
	CString sHost , sService , sId , sPassword;
	GetDlgItemText(IDC_EDIT_HOST , sHost);
	GetDlgItemText(IDC_EDIT_SERVICE , sService);
	GetDlgItemText(IDC_EDIT_PROJECT_USER_ID , sId);
	GetDlgItemText(IDC_EDIT_PROJECT_PASSWORD , sPassword);

	if(true == m_ctrlSmartDraw.GetControl()->DoOracleConnectionTest(gcnew System::String(sHost) , gcnew System::String(sService) , gcnew System::String(sId) , gcnew System::String(sPassword)))
	{
		AfxMessageBox(_T("Connection is success") , MB_OK|MB_ICONINFORMATION);
	}
}

/**
	@brief	connection test for design database
	@author	humkyung
	@date	2012.09.19
*/
void PropertyPage::CDatabaseSettingPage::OnBnClickedDesignDatabaseConnection()
{
	CSmartDrawApp* pApp = static_cast<CSmartDrawApp*>(AfxGetApp());
	
	CString sHost , sService , sId , sPassword;
	GetDlgItemText(IDC_EDIT_HOST , sHost);
	GetDlgItemText(IDC_EDIT_SERVICE , sService);
	GetDlgItemText(IDC_EDIT_DESIGN_USER_ID , sId);
	GetDlgItemText(IDC_EDIT_DESIGN_PASSWORD , sPassword);

	if(true == m_ctrlSmartDraw.GetControl()->DoOracleConnectionTest(gcnew System::String(sHost) , gcnew System::String(sService) , gcnew System::String(sId) , gcnew System::String(sPassword)))
	{
		AfxMessageBox(_T("Connection is success") , MB_OK|MB_ICONINFORMATION);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-09
    @class      PropertyPage
    @function   CDatabaseSettingPage::OnBnClickedButtonDatabaseDump
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CDatabaseSettingPage::OnBnClickedButtonDatabaseDump()
{
	CSmartDrawApp* pApp = static_cast<CSmartDrawApp*>(AfxGetApp());
	try
	{
		const CString sExecPath = GetExecPath();
		CSmartDrawDoc* pDoc = CSmartDrawDoc::GetInstance();///GetMDIActiveDocument();
		if(NULL == pDoc) return;
		const CString sPrjSettingFilePath = pDoc->GetPrjSettingFilePath();

		CString sHost , sService , sProjectId , sProjectPassword , sAccessFilePath(pDoc->GetAccessFilePath().c_str());
		CString sDesignId , sDesignPassword;
		
		/// copy empty mdb file - 2012.09.25 added by humkyung
		::CopyFile(sExecPath + _T("\\Backup\\PDS.mdb") , sAccessFilePath , FALSE);
		/// up to here

		SaveData();

		
		CWnd* pDumpButton = GetDlgItem(IDC_BUTTON_DATABASE_DUMP);
		pDumpButton->EnableWindow(FALSE);
		{
			const CString sExecFilePath = GetExecPath() + _T("\\ExtractDatabase.exe");
			CString sArgs = (_T("\"") + sExecFilePath + _T("\" \"") + m_sIniFilePath + _T("\""));

			DWORD exitCode = 0;
			executeCommandLine(sArgs , exitCode);

			/*CProjectSettingSheet* pParent = (CProjectSettingSheet*)GetParent();
			if(NULL != pParent)
			{
				pParent->m_wndFolderSettingPage.LoadData();
			}*/
		}
		pDumpButton->EnableWindow(TRUE);
	}
	catch(std::exception ex)
	{
		AfxMessageBox(ex.what());
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-09
    @class      PropertyPage
    @function   CDatabaseSettingPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CDatabaseSettingPage::LoadData()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	static const CString sApp(_T("Database"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	::GetPrivateProfileString(sApp , _T("Type") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_ORACLE));
	pButton->SetCheck(CString(_T("Oracle")) == szBuf);
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_SQL_SERVER));
	pButton->SetCheck(CString(_T("Sql Server")) == szBuf);
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_ACCESS));
	pButton->SetCheck((CString(_T("Access")) == szBuf));
	/// 2014.07.29 added by humkyung
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_DRV_FILE));
	pButton->SetCheck((CString(_T("DRV File")) == szBuf));
	/// up to here

	SetDlgItemText(IDC_EDIT_ACCESS_FILE_PATH , pDoc->GetAccessFilePath().c_str());

	CString rString;
	::GetPrivateProfileString(sApp , _T("Host") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_HOST , szBuf);
	::GetPrivateProfileString(sApp , _T("Service") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_SERVICE , szBuf);
	::GetPrivateProfileString(sApp , _T("Project_Database_UserID") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_PROJECT_USER_ID , szBuf);
	::GetPrivateProfileString(sApp , _T("Project_Database_Password") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_PROJECT_PASSWORD , szBuf);
	
	::GetPrivateProfileString(sApp , _T("Design_Database_UserID") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_DESIGN_USER_ID , szBuf);
	::GetPrivateProfileString(sApp , _T("Design_Database_Password") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_DESIGN_PASSWORD , szBuf);

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-09
    @class      PropertyPage
    @function   CDatabaseSettingPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CDatabaseSettingPage::SaveData()
{
	static const CString sApp(_T("Database"));

	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_ORACLE));
	BOOL bCheck = pButton->GetCheck();
	if(TRUE == bCheck)
	{
		::WritePrivateProfileString(sApp , _T("Type") , _T("Oracle") , m_sIniFilePath);
	}
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_SQL_SERVER));
	if(TRUE == pButton->GetCheck())
	{
		::WritePrivateProfileString(sApp , _T("Type") , _T("Sql Server") , m_sIniFilePath);
	}
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_ACCESS));
	if(TRUE == pButton->GetCheck())
	{
		::WritePrivateProfileString(sApp , _T("Type") , _T("Access") , m_sIniFilePath);
		
		CString rString;
		GetDlgItemText(IDC_EDIT_ACCESS_FILE_PATH , rString);
		::WritePrivateProfileString(sApp , _T("Access File") , rString , m_sIniFilePath);
	}
	/// 2014.07.29 added by humkyung
	pButton = (CButton*)(GetDlgItem(IDC_RADIO_DRV_FILE));
	if(TRUE == pButton->GetCheck())
	{
		::WritePrivateProfileString(sApp , _T("Type") , _T("DRV File") , m_sIniFilePath);
	}
	/// up to here

	CString rString;
	GetDlgItemText(IDC_EDIT_HOST , rString);
	::WritePrivateProfileString(sApp , _T("Host") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_SERVICE , rString);
	::WritePrivateProfileString(sApp , _T("Service") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_PROJECT_USER_ID , rString);
	::WritePrivateProfileString(sApp , _T("Project_Database_UserID") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_PROJECT_PASSWORD , rString);
	::WritePrivateProfileString(sApp , _T("Project_Database_Password") , rString , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_DESIGN_USER_ID , rString);
	::WritePrivateProfileString(sApp , _T("Design_Database_UserID") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_DESIGN_PASSWORD , rString);
	::WritePrivateProfileString(sApp , _T("Design_Database_Password") , rString , m_sIniFilePath);

	return ERROR_SUCCESS;
}