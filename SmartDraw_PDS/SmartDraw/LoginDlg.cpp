// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <gui/FolderDlg/FolderDlg.h>
#include <util/Registry.h>
#include <util/FileTools.h>
#include <Tokenizer.h>
#include "LoginDlg.h"
#include "enablebuddybutton.h"
#include "SmartDrawDocData.h"
#include "StringTable.h"
#include "soapLicWebServiceSoapProxy.h"
#include "LicWebServiceSoap.nsmap"

#include <fstream>

extern "C" __declspec(dllimport) int __stdcall Encode(LMS_ENCODE* param);
extern "C" __declspec(dllimport) int __stdcall Decode(LMS_ENCODE* param);

CString PrintMACaddress(unsigned char MACData[])
{
	CString rString;
	rString.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
	return rString;
}

CString _GetComputerName()
{
	DWORD dSize = MAX_COMPUTERNAME_LENGTH + 1;
	TCHAR szBuffer[MAX_COMPUTERNAME_LENGTH + 1] = {'\0' ,};
	::GetComputerName(szBuffer , &dSize);
	szBuffer[dSize] = '\0';
	
	return CString(szBuffer);
}

CString GetMACAddress()
{
	CString strMacAddress = _T("");

	IP_ADAPTER_INFO AdapterInfo[16];		// Allocate information for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);	// Save the memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(		// Call GetAdapterInfo
		AdapterInfo,						// [out] buffer to receive data
		&dwBufLen);							// [in] size of receive data buffer
	assert(dwStatus == ERROR_SUCCESS);		// Verify return value is valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
	do 
	{
		strMacAddress += PrintMACaddress(pAdapterInfo->Address);	// Print MAC address
		pAdapterInfo = pAdapterInfo->Next;				// Progress through linked list
	}while(pAdapterInfo);						// Terminate if last adapter

	return strMacAddress;
}

/**
	@brief	encrpyt licese key
	@author	humkyung
	@date	201904.09
**/
CString EncryptDecrypt()
{
	const CString sKey = ::_GetComputerName();
	const CString toEncrypt = ::GetMACAddress();
    CString output = toEncrypt;
    
    for (int i = 0; i < toEncrypt.GetLength(); i++)
	{
		int at = toEncrypt.GetAt(i) ^ sKey.GetAt(i % (sKey.GetLength()));
		(at != 0) ?	output.SetAt(i, toEncrypt.GetAt(i) ^ sKey.GetAt(i % (sKey.GetLength()))) : output.SetAt(i, ' ');
	}
    
    return output;
}

/**
	@brief	generate machine key
	@author	humkyung
	@date	2019.04.09
**/
void GenerateMachineKey()
{
	const CString strComputerName = ::_GetComputerName();
	const CString strMacAddress = ::GetMACAddress();

	const CString strFilePath(CFileTools::GetCommonAppDataPath() + "\\SmartDraw_PDS" + _T("\\machinekey.dat"));
	OFSTREAM_T ofile(strFilePath.operator LPCTSTR());
	if(ofile.is_open())
	{
		ofile << _T("COMPUTER NAME=") << strComputerName.operator LPCTSTR() << std::endl;
		ofile << _T("MAC ADDRESS=") << strMacAddress.operator LPCTSTR();
	}
	ofile.close();

	CString str;
	str.Format(_T("machinekey is successfully generated\n You can find %s."), strFilePath);
	AfxMessageBox(str);
}

CClientSocket CLoginDlg::m_oClientSocket;
/******************************************************************************
    @author     humkyung
    @date       2012-04-22
    @function   OnBrowse
    @return     void
    @brief
******************************************************************************/
void CServerFolderBrowseEdit::OnBrowse()
{
	UINT uFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CFolderDialog dlg(_T( "Select SmartDraw Server Path" ), m_sServerFolder , this , uFlags);
	if(IDOK == dlg.DoModal())
	{
		m_sServerFolder = dlg.GetFolderPath();
		SetWindowText(m_sServerFolder);
	}
}

// CLoginDlg dialog

IMPLEMENT_DYNCREATE(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_TITLE , m_wndStaticTitle);
	DDX_Control(pDX , IDC_STATIC_PROJECT , m_wndProjectNameStatic);
	DDX_Control(pDX , IDC_COMBO_PROJECT , m_wndProjectNameCombo);
	DDX_Text(pDX , IDC_COMBO_PROJECT , m_rServerFolderPath);
	DDX_Control(pDX , IDC_STATIC_ID , m_wndUserIDStatic);
	DDX_Control(pDX , IDC_EDIT_ID , m_wndUserID);
	DDX_Text(pDX , IDC_EDIT_ID , m_rID);
	DDX_Control(pDX , IDC_STATIC_PASSWORD , m_wndPasswordStatic);
	DDX_Control(pDX , IDC_EDIT_PASSWORD , m_wndPassword);
	DDX_Text(pDX , IDC_EDIT_FOLDER , m_rServerFolderPath);
	DDX_Control(pDX , IDC_STATIC_MSG , m_wndStaticMsg);

	DDX_Control(pDX , IDOK , m_wndLoginButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	DDX_Control(pDX , IDC_EDIT_FOLDER , m_wndServerFolder);
}

/**
	@brief	

	@author	humkyung
*/
BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	/// Set Visual Style
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	auto_ptr<CFont> m_pBoldFont(new CFont);
	m_pBoldFont->CreateFont(20,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));

	m_wndStaticTitle.SetColor( RGB(0, 0, 0) );
	m_wndStaticTitle.SetGradientColor( RGB(74, 84, 103) );
	m_wndStaticTitle.SetFont(m_pBoldFont.get());
	m_wndStaticTitle.SetVerticalGradient(TRUE);
	m_wndStaticTitle.SetTextAlign(DT_CENTER);
	m_wndStaticTitle.SetTextColor(RGB(255 , 255 , 255));

	m_wndProjectNameStatic.SetColor(RGB(245,245,245));
	m_wndProjectNameStatic.SetGradientColor(RGB(245,245,245));
	m_wndProjectNameStatic.SetTextColor(RGB(0, 0, 0));

	m_wndUserIDStatic.SetColor(RGB(245,245,245));
	m_wndUserIDStatic.SetGradientColor(RGB(245,245,245));
	m_wndUserIDStatic.SetTextColor(RGB(0, 0, 0));

	CString rRegEx=_T("([a-zA-Z0-9])*");
	/*m_wndUserID.SetRegEx(rRegEx);
	m_wndUserID.CreateToolTip(this , _T("아이디로 사용할 수 없는 글자입니다."));
	m_wndUserID.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_wndUserID.SetForegroundColourError (RGB (255, 255, 255));
	m_wndUserID.SetBackgroundColourError (RGB (255, 0, 0));*/
	
	m_wndPasswordStatic.SetColor(RGB(245,245,245));
	m_wndPasswordStatic.SetGradientColor(RGB(245,245,245));
	m_wndPasswordStatic.SetTextColor(RGB(0, 0, 0));

	rRegEx = _T("([a-zA-Z0-9!@#$%^&*()_+])*");
	m_wndPassword.SetRegEx(rRegEx);

	CStringTable& table = CStringTable::GetInstance();
	const STRING_T str = table.GetString(_T("IDS_INVALID_STRING"));
	m_wndPassword.CreateToolTip(this , str.c_str());
	m_wndPassword.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_wndPassword.SetForegroundColourError (RGB (255, 255, 255));
	m_wndPassword.SetBackgroundColourError (RGB (255, 0, 0));

	m_wndServerFolder.EnableFileBrowseButton(); /// To show file open dialog

	m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
	m_wndStaticMsg.SetColor( RGB(255,0,0) );
	m_wndStaticMsg.SetTextColor(RGB(0 , 0 , 0));

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndLoginButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	
	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK),
		IMAGE_ICON,
		16,16, 0 );
	m_wndLoginButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL),
		IMAGE_ICON,
		16,16, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);

	HICON hIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_RESET_ACTIVATE_CODE),IMAGE_ICON,16,16, 0 );
	((CButton*)GetDlgItem(IDC_BUTTON_RESET_ACTIVATE_CODE))->SetIcon(hIcon);

	LoadAppSettingFile();
	SyncDatabaseStructure();
	ReadProjectNoFromMDB();
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SERVER, &CLoginDlg::OnBnClickedButtonSelectServer)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_FOLDER, &CLoginDlg::OnEnChangeEditFolder)
	ON_MESSAGE(SMARTLMS_MSG, &CLoginDlg::OnReceiveMsg)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ACTIVATE_CODE, &CLoginDlg::OnBnClickedButtonRestActivateCode)
END_MESSAGE_MAP()

// CLoginDlg message handlers

/**	
	@brief	select server path
	@author	humkyung
	@date	????.??.??
	@param	
	@return
*/
void CLoginDlg::OnBnClickedButtonSelectServer()
{
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CFolderDialog dlg(_T( "Select SmartDraw Server Path" ), NULL , this , uFlags);
	if(IDOK == dlg.DoModal())
	{
		m_rServerFolderPath = dlg.GetFolderPath();
		CString rServerMDB = m_rServerFolderPath;
		if(_T("\\") != rServerMDB.Right(1)) rServerMDB += _T("\\");
		rServerMDB += PROJECT_MDB_NAME;

		/// copy template mdb to aDraw_Project.MDB as server database if server database doesn't exist.
		if(FALSE == FileExist(rServerMDB))
		{
			CString rExecPath = GetExecPath();
			CString rTemplateMDB = (_T("\\") != rExecPath.Right(1)) ? (rExecPath + _T("\\")) : rExecPath;
			rTemplateMDB += CString(_T("Backup\\")) + PROJECT_MDB_NAME;
			::CopyFile(rTemplateMDB , rServerMDB , TRUE);

			this->SyncDatabaseStructure();	/// 2012.04.27 added by humkyung
		}

		ReadProjectNoFromMDB();

		UpdateData(FALSE);
	}
}

/**
	@brief	사용자가 입력한 값들을 ini 파일로 저장한다.
	@author	humkyung
	@date	????.??.??
	@param
	@return
*/
void CLoginDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_rServerFolderPath.IsEmpty())
	{
		DisplayMessage(_T("Please, Select a server folder") , RGB(255,0,0) , RGB(0,0,0));
		return;
	}

	/// get selected Project No
	const int nSelItem = m_wndProjectNameCombo.GetCurSel();
	if(-1 != nSelItem)
		m_wndProjectNameCombo.GetLBText(nSelItem, m_rProjectName);
	else	m_rProjectName = _T("");

	STRING_T rServerMDB = m_rServerFolderPath;
	if('\\' != rServerMDB.at( rServerMDB.length() - 1)) rServerMDB += _T("\\");
	rServerMDB += PROJECT_MDB_NAME;

	if(FALSE == FileExist(rServerMDB.c_str()))
	{
		CString rExecPath = GetExecPath();
		STRING_T rTemplateMDB = (_T("\\") != rExecPath.Right(1)) ? (rExecPath + _T("\\")).operator LPCTSTR() : rExecPath.operator LPCTSTR();
		rTemplateMDB += CString(_T("Backup\\")) + PROJECT_MDB_NAME;
		CopyFile(rTemplateMDB.c_str() ,rServerMDB.c_str() , TRUE);
	}

	SaveAppSettingFile();
	CDialog::OnOK();
}

/**
	@brief	application setting file을 읽어 UI.에 뿌려준다.
	@author	humkyung
*/
int CLoginDlg::LoadAppSettingFile(void)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString rAppSettingFilePath = docData.GetAppSettingFilePath();
	TCHAR szBuf[MAX_PATH + 1] = {'\0'};

	if(GetPrivateProfileString(PRODUCT_NAME , _T("Server") , _T("") , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rServerFolderPath = szBuf;
		m_wndServerFolder.m_sServerFolder = m_rServerFolderPath;
	}
	if(GetPrivateProfileString(PRODUCT_NAME , _T("CurPrjName") , _T("") , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rProjectName = szBuf;
	}
	if(GetPrivateProfileString(PRODUCT_NAME , _T("CurID") , _T("") , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rID = szBuf;		
	}

	/// check licese key
	/*
	const CString strLicenseFilePath(CFileTools::GetCommonAppDataPath() + "\\SmartDraw_PDS" + _T("\\license.dat"));
	if(CFileTools::DoesFileExist(strLicenseFilePath.operator LPCTSTR()))
	{
		IFSTREAM_T ifile(strLicenseFilePath.operator LPCTSTR());
		if(ifile.is_open())
		{
			STRING_T sLicense;
			::getline(ifile, sLicense);
			if(sLicense != STRING_T(::EncryptDecrypt().operator LPCTSTR()))
			{
				GetDlgItem(IDOK)->EnableWindow(FALSE);
				AfxMessageBox(_T("invalid license key!!!"));
			}
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			AfxMessageBox(_T("Can't find license key"));
		}
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		AfxMessageBox(_T("Can't find license key"));
	}
	*/
	/// up to here

	UpdateData(FALSE);

	return ERROR_SUCCESS;
}

/**
	@brief	UI의 내용을 application setting file로 저장한다.
	@author	humkyung
*/
int CLoginDlg::SaveAppSettingFile(void)
{
	UpdateData(TRUE);

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString rAppSettingFilePath = docData.GetAppSettingFilePath();
	
	WritePrivateProfileString(PRODUCT_NAME , _T("Server") , m_rServerFolderPath , rAppSettingFilePath);
	WritePrivateProfileString(PRODUCT_NAME , _T("CurPrjName") , m_rProjectName , rAppSettingFilePath);
	WritePrivateProfileString(PRODUCT_NAME , _T("CurID") , m_rID , rAppSettingFilePath);

	/// 2012.12.27 added by humkyung
	docData.SetUser(m_rID);
	CString sPassword;
	m_wndPassword.GetWindowText(sPassword);
	docData.SetPassword(sPassword);
	/// up to here

	return ERROR_SUCCESS;
}

/**
	@brief	read project no from project setup database and add string to combo box.
	@author	humkyung
	@return	
*/
int CLoginDlg::ReadProjectNoFromMDB(void)
{
	CString str;

	if(m_rServerFolderPath.IsEmpty()) return ERROR_BAD_ENVIRONMENT;

	CString rMDBFilePath = m_rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
	CString rConnectionString = CString(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

	CADODB adoDB;
	if(!adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
	{
		m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
		m_wndStaticMsg.SetColor( RGB(255,0,0) );
		m_wndStaticMsg.SetWindowText( _T("Fail to Connect SmartDraw_Project Database.") );
		
		return ERROR_BAD_ENVIRONMENT;
	}
	else
	{
		m_wndStaticMsg.SetGradientColor( GetSysColor(COLOR_BTNFACE) );
		m_wndStaticMsg.SetColor( GetSysColor(COLOR_BTNFACE) );
		m_wndStaticMsg.SetWindowText( _T("Connect SmartDraw_Project Database Success.") );
	}

	if(!adoDB.OpenQuery(_T("SELECT * FROM T_SMARTDRAW_PROJECT")))
		return ERROR_BAD_ENVIRONMENT;

	CString tmp;
	LONG lRecordCount = 0;
	if(adoDB.GetRecordCount(&lRecordCount))
	{
		/// 기존에 들어 있는 내용을 지운다.
		m_wndProjectNameCombo.ResetContent();
		m_ProjectInfoMap.clear();

		STRING_T strValue;
		for(int i = 0;i < lRecordCount;++i)
		{
			adoDB.GetFieldValue(i , _T("C_PROJECT_NAME") , &strValue);
			if(strValue.empty()) continue;

			if(-1 == m_wndProjectNameCombo.FindString(0 , strValue.c_str()))
			{
				m_wndProjectNameCombo.AddString(IDB_PROJECT , strValue.c_str());
				
				STRING_T rValue;
				adoDB.GetFieldValue(i , _T("C_PROJECT_DESC") , &rValue);
				m_ProjectInfoMap[strValue].m_rProjectDesc = rValue;

				/// 2012.12.27 added by humkyung
				adoDB.GetFieldValue(i , _T("C_PASSWORD") , &rValue);
				m_ProjectInfoMap[strValue].m_sPassword = rValue;
				/// up to here
			}
		}
		
		const int nFound = m_wndProjectNameCombo.FindString(0 , m_rProjectName);
		(-1 != nFound) ? m_wndProjectNameCombo.SetCurSel(nFound) : m_wndProjectNameCombo.SetCurSel(0);
		
		const int nItem = m_wndProjectNameCombo.GetCurSel();
		if(-1 != nItem) m_wndProjectNameCombo.GetLBText(nItem , m_rProjectName);

		if(!adoDB.DBDisConnect())
		{
			m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
			m_wndStaticMsg.SetColor( RGB(255,0,0) );///GetSysColor(COLOR_BTNFACE) );
			m_wndStaticMsg.SetWindowText(_T("Fail to connect SmartDraw project database."));

			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	humkyung
*/
int CLoginDlg::DisplayMessage(const STRING_T& rMsg , COLORREF bgColor , COLORREF fgColor)
{
	m_wndStaticMsg.SetGradientColor( bgColor );
	m_wndStaticMsg.SetColor( bgColor );
	m_wndStaticMsg.SetTextColor(fgColor);
	m_wndStaticMsg.SetWindowText( rMsg.c_str() );

	return ERROR_SUCCESS;
}

/**
	@brief	PROJECT NO에 해당하는 PROJECT NAME을 구한다.

	@author	humkyung
*/
CString CLoginDlg::GetProjectNameOf(const STRING_T& rProjectNo)
{
	map<STRING_T,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectName.c_str());

	return CString(_T(""));
}

/**
	@brief	
	@author BHK	
	@date 2009-06-25 오후 2:44:24	
	@param	
	@return		
*/
CString CLoginDlg::GetProjectDescOf(const STRING_T& rProjectNo)
{
	map<STRING_T,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectDesc.c_str());

	return CString("");
}

/**
	@brief	
	@author BHK	
	@date 2009-06-25 오후 2:44:27	
	@param	
	@return		
*/
CString CLoginDlg::GetProjectClientOf(const STRING_T& rProjectNo)
{
	map<STRING_T,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectClient.c_str());

	return CString("");
}
/**
	@brief	MDB database 구조를 SQL파일에 기술한 내용대로 맞춘다.
	@author humkyung	
	@date 2009-06-25 오후 1:42:36	
	@param	
	@return		
*/
int CLoginDlg::SyncDatabaseStructure(void)
{
	CADODB adoDB;
	const CString rMDBFilePath = m_rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
	const CString rConnectionString = CString(PROVIDER) + CString(_T("Data Source=")) + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
	{
		CString rExecPath = GetExecPath();
		if(_T("\\") != rExecPath.Right(1)) rExecPath += _T("\\");
		const CString rSqlFilePath = rExecPath + _T("Backup\\T_SMARTDRAW_PROJECT.XML");
		
		CSQLFile sqlFile;
		if(ERROR_SUCCESS != sqlFile.ParseXML(rSqlFilePath.operator LPCTSTR())) return ERROR_BAD_ENVIRONMENT;
		sqlFile.SyncToTable(adoDB, PROJECT_TABLE_NAME);
		
		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox(_T("Fail to connect Server Database"));
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SQL 파일과 형식을 일치시킨다.
	@author BHK	
	@date 2009-06-25 오후 1:47:01	
	@param	
	@return		
*/
int CLoginDlg::SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile , vector<STRING_T>& FieldNameEntry)
{
	const int nFieldCount = sqlFile.GetColumnCount();
	for(int i = 0;i < nFieldCount;++i)
	{
		const STRING_T rFieldName = sqlFile.GetFieldNameAt(i);
		if(rFieldName.empty()) continue;

		vector<STRING_T>::iterator where = find(FieldNameEntry.begin() , FieldNameEntry.end() , rFieldName);
		if(where == FieldNameEntry.end())
		{
			/// 필드가 존재하지 않으면 테이블에 필드를 생성한다.
			adoDB.ExecuteQuery(STRING_T(_T("ALTER TABLE ") + STRING_T(PROJECT_TABLE_NAME) + _T(" ADD ") + sqlFile.GetFieldDataAt(i)).c_str());
			FieldNameEntry.push_back(rFieldName);
		}
	}

	for(vector<STRING_T>::iterator itr = FieldNameEntry.begin();itr != FieldNameEntry.end();)
	{
		if(false == sqlFile.ExistColumnName(*itr))
		{
			/// 불필요한 필드는 테이블에서 삭제한다.
			adoDB.ExecuteQuery(STRING_T(_T("ALTER TABLE ") + STRING_T(PROJECT_TABLE_NAME) + _T(" DROP [") + *itr + _T("]")).c_str());
			itr = FieldNameEntry.erase(itr);
		}else	itr++;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-27
    @class      CLoginDlg
    @function   OnEnChangeEditFolder
    @return     void
    @brief
******************************************************************************/
void CLoginDlg::OnEnChangeEditFolder()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UpdateData();

	CString rServerMDB = m_rServerFolderPath;
	if(_T("\\") != rServerMDB.Right(1)) rServerMDB += _T("\\");
	rServerMDB += PROJECT_MDB_NAME;

	//! copy template mdb to aDraw_Project.MDB as server database if server database doesn't exist.
	if(FALSE == FileExist(rServerMDB))
	{
		CString rExecPath = GetExecPath();
		CString rTemplateMDB = (_T("\\") != rExecPath.Right(1)) ? (rExecPath + _T("\\")) : rExecPath;
		rTemplateMDB += CString(_T("Backup\\")) + PROJECT_MDB_NAME;
		::CopyFile(rTemplateMDB , rServerMDB , TRUE);

		this->SyncDatabaseStructure();	/// 2012.04.27 added by humkyung
	}

	ReadProjectNoFromMDB();

	UpdateData(FALSE);
}

/**
	@brief  receive message form server
	@author humkyung
	@date   2013.11.13
*/
LRESULT CLoginDlg::OnReceiveMsg(WPARAM wParam, LPARAM lParam)
{
	CCommandObject* pObj = (CCommandObject*)(wParam);
	
	if(GetDlgItem(IDC_BUTTON_RESET_ACTIVATE_CODE)->IsWindowEnabled())
	{
		if(RES_SUCCESS == pObj->m_Packet.Code)
		{
			m_wndStaticMsg.SetTextColor( RGB(0,0,0) );
			SaveAppSettingFile();
			CDialog::OnOK();
		}
		else
		{
			m_wndStaticMsg.SetTextColor( RGB(255,0,0) );
			m_oClientSocket.Close();
		}
		m_wndStaticMsg.SetWindowText(StringHelper(pObj->m_Packet.Code));

		m_wndLoginButton.EnableWindow(TRUE);
	}
	/// called from reset activate code
	else
	{
		GetDlgItem(IDC_BUTTON_RESET_ACTIVATE_CODE)->EnableWindow(TRUE);
		m_wndLoginButton.EnableWindow(TRUE);
		m_oClientSocket.Close();
	}

	return 0L;
}

/**
	@brief		reset activate code
	@author		humkyung
	@date		2014.05.15
	@history	generate machine key 2019.04.09 by humkyung
*/
void CLoginDlg::OnBnClickedButtonRestActivateCode()
{
	::GenerateMachineKey();
}
