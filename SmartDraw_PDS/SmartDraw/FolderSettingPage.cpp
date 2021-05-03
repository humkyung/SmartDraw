// FolderSettingPage.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "SmartDraw.h"
#include "FolderSettingPage.h"
#include "SmartDrawDocData.h"

using namespace PropertyPage;

void CaDrawDDLFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	/// 파일 Open 
	CFileDialog dlg(TRUE,_T("ddl,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("DDL File(*.ddl)|*.ddl|All Documents(*.*)|*.*||"));
	if(IDOK == dlg.DoModal())
	{
		m_rFilePath = dlg.GetPathName();
		SetWindowText(m_rFilePath);
	}
}

void CaDrawFolderBrowseEdit::OnBrowse()
{
	TCHAR path[MAX_PATH + 1];
	memset(path,0,sizeof(TCHAR) * MAX_PATH );

	BROWSEINFO bi;

	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.lpszTitle = _T("경로를 입력해주세요");
	bi.pszDisplayName = path;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;

	LPITEMIDLIST idlist = SHBrowseForFolder( &bi );
	if ( idlist )
	{
		SHGetPathFromIDList( idlist, path ); 
		m_rFolderPath = path;
		SetWindowText(m_rFolderPath);
	}
}

// CFolderSettingPage dialog

IMPLEMENT_DYNAMIC(CFolderSettingPage, CSmartDrawPropertyPage)

CFolderSettingPage::CFolderSettingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CFolderSettingPage::IDD, pParent)
{

}

CFolderSettingPage::~CFolderSettingPage()
{
}

void CFolderSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_EDIT_DESIGN_DDL_FILE_PATH , m_wndDesignDDLFilePath);
	DDX_Control(pDX , IDC_EDIT_EQP_DDL_FILE_PATH , m_wndEQPDDLFilePath);

	DDX_Control(pDX , IDC_EDIT_DRAWING_FOLDER , m_wndDrawingFolderPath);	/// 2012.02.12 added by humkyung

	DDX_Control(pDX , IDC_EDIT_PIPE_FOLDER , m_wndPipeFolderPath);
	DDX_Control(pDX , IDC_EDIT_EQUIPMENT_FOLDER , m_wndEquipmentFolderPath);
	DDX_Control(pDX , IDC_EDIT_STRUCTURE_FOLDER , m_wndStructureFolderPath);
	DDX_Control(pDX , IDC_EDIT_CABLE_TRAY_FOLDER , m_wndCableTFolderPath);
	DDX_Control(pDX , IDC_EDIT_MISC_FOLDER , m_wndMiscFolderPath);
}


BEGIN_MESSAGE_MAP(CFolderSettingPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CFolderSettingPage message handlers

BOOL PropertyPage::CFolderSettingPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	m_wndDesignDDLFilePath.EnableFileBrowseButton(); // To show file open dialog
	m_wndEQPDDLFilePath.EnableFileBrowseButton(); // To show file open dialog

	m_wndDrawingFolderPath.EnableFolderBrowseButton();	/// 2012.02.12 added by humkyung

	m_wndPipeFolderPath.EnableFolderBrowseButton();
	m_wndEquipmentFolderPath.EnableFolderBrowseButton();
	m_wndStructureFolderPath.EnableFolderBrowseButton();
	m_wndCableTFolderPath.EnableFolderBrowseButton();
	m_wndMiscFolderPath.EnableFolderBrowseButton();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   SaveData
    @return     int
    @param      const               CString&
    @param      sSettingFilePath
    @brief
******************************************************************************/
int PropertyPage::CFolderSettingPage::LoadData( LPCTSTR pSettingFilePath )
{
	assert(pSettingFilePath && "pSettingFilePath is NULL");

	if(pSettingFilePath)
	{
		m_sIniFilePath = pSettingFilePath;

		TCHAR szBuf[MAX_PATH + 1] = {'\0',};
		CString sApp(_T("Folder"));
		if(GetPrivateProfileString(sApp , _T("Design DDL") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndDesignDDLFilePath.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("EQP DDL") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndEQPDDLFilePath.SetWindowText(szBuf);
		}

		if(GetPrivateProfileString(sApp , _T("Drawing") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndDrawingFolderPath.SetWindowText(szBuf);
		}
		
		if(GetPrivateProfileString(sApp , _T("Pipe") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndPipeFolderPath.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("Equipment") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndEquipmentFolderPath.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("Structure") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndStructureFolderPath.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("CableT") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndCableTFolderPath.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(sApp , _T("Misc") , _T("") , szBuf , MAX_PATH , pSettingFilePath))
		{
			m_wndMiscFolderPath.SetWindowText(szBuf);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CFolderSettingPage::SaveData( )
{
	CString rString , sApp(_T("Folder"));;

	m_wndDesignDDLFilePath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Design DDL") , rString , m_sIniFilePath);
	m_wndEQPDDLFilePath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("EQP DDL") , rString , m_sIniFilePath);

	m_wndDrawingFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Drawing") , rString , m_sIniFilePath);

	m_wndPipeFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Pipe") , rString , m_sIniFilePath);
	m_wndEquipmentFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Equipment") , rString , m_sIniFilePath);
	m_wndStructureFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Structure") , rString , m_sIniFilePath);
	m_wndCableTFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("CableT") , rString , m_sIniFilePath);
	m_wndMiscFolderPath.GetWindowText(rString);
	WritePrivateProfileString(sApp , _T("Misc") , rString , m_sIniFilePath);

	return ERROR_SUCCESS;
}