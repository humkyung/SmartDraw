// NewProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <util/ado/ADODB.h>
#include "SmartDraw.h"
#include "NewProjectDlg.h"
#include "SmartDrawDocData.h"
#include "PluginManager.h"

/******************************************************************************
    @author     humkyung
    @date       2015-03-31
    @function   OnBrowse
    @return     void
    @brief
******************************************************************************/
void CCADAppllicationBrowseEdit::OnBrowse()
{
	/// ÆÄÀÏ Open 
	CFileDialog dlg(TRUE,_T("*.exe"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,_T("Executable File(*.exe)|*.exe||"));
	if(IDOK == dlg.DoModal())
	{
		SetWindowText(dlg.GetPathName());
	}
}

// CNewProjectDlg dialog

IMPLEMENT_DYNAMIC(CNewProjectDlg, CDialog)

CNewProjectDlg::CNewProjectDlg(const CString& sProjectName , CWnd* pParent /*=NULL*/)
	: CDialog(CNewProjectDlg::IDD, pParent)
{
	m_sProjectName = sProjectName;
	m_sProjectUOR = _T("80");	/// default value
	m_sModelOffsetX = m_sModelOffsetY = m_sModelOffsetZ = _T("0");
}

CNewProjectDlg::~CNewProjectDlg()
{
}

void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_PROJECT_NAME , m_sProjectName);
	DDX_Text(pDX , IDC_EDIT_PROJECT_DESC , m_sProjectDesc);
	DDX_Text(pDX , IDC_EDIT_MODEL_OFFSET_X , m_sModelOffsetX);
	DDX_Text(pDX , IDC_EDIT_MODEL_OFFSET_Y , m_sModelOffsetY);
	DDX_Text(pDX , IDC_EDIT_MODEL_OFFSET_Z , m_sModelOffsetZ);
	DDX_Control(pDX , IDC_COMBO_DRAWING_TYPE , m_wndDrawingTypeComboBox);
	DDX_Control(pDX , IDC_COMBO_OUTPUT_FORMAT , m_wndOutputFormatComboBox);
	DDX_Control(pDX , IDC_EDIT_CAD_APPLICATION , m_wndCADApplication);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	DDX_Text(pDX , IDC_EDIT_PROJECT_UOR , m_sProjectUOR);
}


BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNewProjectDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_DRAWING_TYPE, &CNewProjectDlg::OnCbnSelchangeComboDrawingType)
END_MESSAGE_MAP()

CString GetErrorString(const DWORD dwErrorNo)
{
	CString res;

	LPVOID lpMsgBuf = NULL;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.
		return res;
	}
	res = (LPCTSTR)lpMsgBuf;
	
	// Free the buffer.
	LocalFree( lpMsgBuf ); 

	return res;
}


// CNewProjectDlg message handlers

void CNewProjectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	GetDlgItemText(IDC_EDIT_PROJECT_NAME , m_sProjectName);
	GetDlgItemText(IDC_EDIT_PROJECT_DESC , m_sProjectDesc);
	int iCurSel = m_wndDrawingTypeComboBox.GetCurSel();
	if(-1 != iCurSel) m_wndDrawingTypeComboBox.GetLBText(iCurSel , m_sDrawingType);
	
	CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_GENERATE));
	CString sHsrType = (pButton->GetCheck() ? _T("Generate") : _T("Import"));

	iCurSel = m_wndOutputFormatComboBox.GetCurSel();
	if(-1 != iCurSel) m_wndOutputFormatComboBox.GetLBText(iCurSel , m_sOutputFormat);
	if(!m_sProjectName.IsEmpty())
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString rServerFolderPath = docData.GetServerFolderPath();

		////////////////////////////////////////////////////////////////////////////////
		const CString rMDBFilePath = rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
		CString rConnectionString = CString(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

		CADODB adoDB;
		if(adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
		{
			const BOOL isEditMode = (FALSE == GetDlgItem(IDC_EDIT_PROJECT_NAME)->IsWindowEnabled());

			CString rProjectDesc , rCADApplication;
			GetDlgItemText(IDC_EDIT_PROJECT_DESC , rProjectDesc);
			m_wndCADApplication.GetWindowText(rCADApplication);

			CString rSql;
			if(TRUE == isEditMode)
			{
				rSql.Format(_T("UPDATE %s SET C_PROJECT_DESC='%s',C_PROJECT_UOR='%s',C_MODEL_OFFSET_X='%s',C_MODEL_OFFSET_Y='%s',C_MODEL_OFFSET_Z='%s',C_CAD_APPLICATION='%s' WHERE C_PROJECT_NAME='%s'") ,
					PROJECT_TABLE_NAME , rProjectDesc , m_sProjectUOR , m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ,rCADApplication , m_sProjectName);
				if(TRUE == adoDB.ExecuteQuery(rSql.operator LPCTSTR()))
				{
					/// update ini file
					const CString sPrjIniFilePath = rServerFolderPath + _T("\\") + m_sProjectName + _T("\\Setting\\") + m_sProjectName + _T(".prj");
					WritePrivateProfileString(_T("General") , _T("UOR") , m_sProjectUOR , sPrjIniFilePath);
					CString sModelOffset;
					sModelOffset.Format(_T("%s,%s,%s") , m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ);
					WritePrivateProfileString(_T("General") , _T("Model Offset") , sModelOffset , sPrjIniFilePath);
					const CString sIniFilePath(rServerFolderPath + _T("\\") + m_sProjectName + _T("\\Setting\\Default.ini"));
					WritePrivateProfileString(_T("HLR_OPTIONS") , _T("Type") , sHsrType , sIniFilePath);
					/// up to here
				}
			}
			else
			{
				rSql.Format(_T("SELECT C_PROJECT_NAME FROM %s WHERE C_PROJECT_NAME='%s'") , PROJECT_TABLE_NAME , m_sProjectName);
				adoDB.OpenQuery(rSql.operator LPCTSTR());
				
				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);
				if(0 != lRecordCount)
				{
					AfxMessageBox(_T("There is project with same name."));
					return;
				}

				rSql.Format(_T("INSERT INTO %s (C_PROJECT_NAME,C_PROJECT_DESC,C_PROJECT_UOR,C_MODEL_OFFSET_X,C_MODEL_OFFSET_Y,C_MODEL_OFFSET_Z,C_DRAWING_TYPE,C_OUTPUT_FORMAT,C_CAD_APPLICATION) VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s')") ,
					PROJECT_TABLE_NAME , m_sProjectName , rProjectDesc , m_sProjectUOR , m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ,m_sDrawingType , m_sOutputFormat , rCADApplication);
				adoDB.ExecuteQuery(rSql.operator LPCTSTR());

				const CString rIniFilePath = GetExecPath() + _T("\\Setting\\SmartDraw_Project.ini");
				if((TRUE == ::CreateDirectory(rServerFolderPath + _T("\\") + m_sProjectName , NULL)) || (ERROR_ALREADY_EXISTS == GetLastError()))
				{
					const int nFolderCount = GetPrivateProfileInt(_T("Project_Folder") , _T("Folder_Count") , 0 , rIniFilePath);
					for(int i = 0;i < nFolderCount;++i)
					{
						CString rKeyName;
						rKeyName.Format(_T("Folder%d") , i);

						if(GetPrivateProfileString(_T("Project_Folder") , rKeyName , NULL , szBuf , MAX_PATH , rIniFilePath))
						{
							::CreateDirectory(rServerFolderPath + _T("\\") + m_sProjectName + _T("\\") + szBuf , NULL);
						}
					}

					/// copy default project setting / ini file
					const CString sPrjIniFilePath = rServerFolderPath + _T("\\") + m_sProjectName + _T("\\Setting\\") + m_sProjectName + _T(".prj");
					::CopyFile(GetExecPath() + _T("\\Backup\\Default.prj") , sPrjIniFilePath , TRUE);
					WritePrivateProfileString(_T("General") , _T("PrjName") , m_sProjectName , sPrjIniFilePath);
					WritePrivateProfileString(_T("General") , _T("UOR") , m_sProjectUOR , sPrjIniFilePath);
					CString sModelOffset;
					/// 2014.11.09 added by humkyung
					sModelOffset.Format(_T("%s,%s,%s") , m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ);
					WritePrivateProfileString(_T("General") , _T("Model Offset") , sModelOffset , sPrjIniFilePath);
					/// up to here

					const CString sIniFilePath(rServerFolderPath + _T("\\") + m_sProjectName + _T("\\Setting\\Default.ini"));
					::CopyFile(GetExecPath() + _T("\\Backup\\") + m_sDrawingType + _T("_") + m_sOutputFormat + _T(".ini") , sIniFilePath , TRUE);
					/// update default setting
					WritePrivateProfileString(_T("HLR_OPTIONS") , _T("Type") , sHsrType , sIniFilePath);
					if(_T("AUTOCAD") == m_sOutputFormat)
					{
						WritePrivateProfileString(_T("GENERAL") , _T("Levels") , _T("0") , sIniFilePath);
					}
					/// up to here
				}
				else
				{
					CString sErr = GetErrorString(GetLastError());
					if(!sErr.IsEmpty()) AfxMessageBox(sErr , MB_OK | MB_ICONINFORMATION);
				}
			}
		}

		OnOK();
	}
	else
	{
		AfxMessageBox(_T("Please, set project name.") , MB_ICONEXCLAMATION);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @function   OnInitDialog
    @return     BOOL
    @brief
******************************************************************************/
BOOL CNewProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_PROJECT),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = SetIcon(hIcon , FALSE);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	GetDlgItem(IDC_EDIT_PROJECT_NAME)->EnableWindow(m_sProjectName.IsEmpty());
	m_wndOutputFormatComboBox.AddString(IDB_MSTN , _T("MSTN"));
	m_wndOutputFormatComboBox.AddString(IDB_AUTOCAD , _T("AutoCAD"));
	m_wndOutputFormatComboBox.SetCurSel(0);

	/// edit mode - 2015.04.01 added by humkyung
	if(!m_sProjectName.IsEmpty())
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString rServerFolderPath = docData.GetServerFolderPath();
		const CString rMDBFilePath = rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
		CString rConnectionString = CString(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

		CADODB adoDB;
		if(adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
		{
			CString rSql;
			rSql.Format(_T("SELECT C_PROJECT_DESC,C_PROJECT_UOR,C_MODEL_OFFSET_X,C_MODEL_OFFSET_Y,C_MODEL_OFFSET_Z,C_OUTPUT_FORMAT,C_CAD_APPLICATION FROM %s WHERE C_PROJECT_NAME='%s'") , PROJECT_TABLE_NAME , m_sProjectName);
			adoDB.OpenQuery(rSql.operator LPCTSTR());

			STRING_T sValue;
			adoDB.GetFieldValue(0 , _T("C_PROJECT_DESC") , &sValue);
			m_sProjectDesc = sValue.c_str();
			adoDB.GetFieldValue(0 , _T("C_PROJECT_UOR") , &sValue);
			m_sProjectUOR = sValue.c_str();
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_X") , &sValue);
			m_sModelOffsetX = sValue.c_str();
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_Y") , &sValue);
			m_sModelOffsetY = sValue.c_str();
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_Z") , &sValue);
			m_sModelOffsetZ = sValue.c_str();
			adoDB.GetFieldValue(0 , _T("C_OUTPUT_FORMAT") , &sValue);
			const int at = m_wndOutputFormatComboBox.FindStringExact(-1 , sValue.c_str());
			m_wndOutputFormatComboBox.SetCurSel((-1 != at) ? at : 0);
			adoDB.GetFieldValue(0 , _T("C_CAD_APPLICATION") , &sValue);
			m_wndCADApplication.SetWindowText(sValue.c_str());
		}
	}
	/// up to here

	CPluginManager& inst = CPluginManager::GetInstance();
	for(vector<Plugin*>::const_iterator itr = inst.m_oPluginList.begin();itr != inst.m_oPluginList.end();++itr)
	{
		LPCTSTR pDrawingType = (*itr)->pfGetDrawingName();
		const int iItem = m_wndDrawingTypeComboBox.AddString(pDrawingType);
		m_wndDrawingTypeComboBox.SetItemData(iItem , DWORD_PTR(*itr));
	}
	m_wndDrawingTypeComboBox.SetCurSel(0);
	this->OnCbnSelchangeComboDrawingType();
	m_wndDrawingTypeComboBox.EnableWindow(m_sProjectName.IsEmpty());
	
	m_wndOutputFormatComboBox.EnableWindow(m_sProjectName.IsEmpty());

	m_wndCADApplication.EnableFileBrowseButton(); /// To show file open dialog

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_OK),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndOKButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndCancelButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-14
    @function   OnCbnSelchangeComboDrawingType
    @return     void
    @brief
******************************************************************************/
void CNewProjectDlg::OnCbnSelchangeComboDrawingType()
{
	const int at = m_wndDrawingTypeComboBox.GetCurSel();
	if(-1 != at)
	{
		Plugin* pPlugin = reinterpret_cast<Plugin*>(m_wndDrawingTypeComboBox.GetItemData(at));
		if(NULL != pPlugin)
		{
			STRING_T sHsrType = pPlugin->pfGetHsrType();
			
			CButton* pGenerateButton = (CButton*)(GetDlgItem(IDC_RADIO_GENERATE));
			pGenerateButton->EnableWindow(TRUE);
			CButton* pImportButton = (CButton*)(GetDlgItem(IDC_RADIO_IMPORT));
			pImportButton->EnableWindow(TRUE);

			vector<STRING_T> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , sHsrType , CIsComma());
			for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
			{
				CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_GENERATE));
				pButton->SetCheck(_T("Generate") == (*itr));				
				pButton = (CButton*)(GetDlgItem(IDC_RADIO_IMPORT));
				pButton->SetCheck(_T("Import") == (*itr));
				break;
			}

			if(1 == oResult.size())
			{
				CButton* pButton = (CButton*)(GetDlgItem(IDC_RADIO_GENERATE));
				pButton->EnableWindow(pButton->GetCheck());
				pButton = (CButton*)(GetDlgItem(IDC_RADIO_IMPORT));
				pButton->EnableWindow(pButton->GetCheck());
			}
		}
	}
}
