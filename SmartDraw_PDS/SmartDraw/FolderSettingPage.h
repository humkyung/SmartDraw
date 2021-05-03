#pragma once
#include <SmartDrawPropertyPage.h>

// CFolderSettingPage dialog
namespace PropertyPage
{
class CaDrawDDLFileBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
public:
	CString m_rFilePath;
};

class CaDrawFolderBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
public:
	CString m_rFolderPath;
};

class CFolderSettingPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CFolderSettingPage)

public:
	CFolderSettingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFolderSettingPage();

// Dialog Data
	enum { IDD = IDD_FOLDER_SETTING_PAGE };
	CaDrawDDLFileBrowseEdit m_wndDesignDDLFilePath , m_wndEQPDDLFilePath;

	CMFCEditBrowseCtrl m_wndDrawingFolderPath;	/// 2012.02.12 added by humkyung

	CMFCEditBrowseCtrl m_wndPipeFolderPath;
	CMFCEditBrowseCtrl m_wndEquipmentFolderPath;
	CMFCEditBrowseCtrl m_wndStructureFolderPath;
	CMFCEditBrowseCtrl m_wndCableTFolderPath;
	CMFCEditBrowseCtrl m_wndMiscFolderPath;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int SaveData( );							/// 2012.02.12 added by humkyung
	int LoadData( LPCTSTR pSettingFilePath );	/// 2012.02.12 added by humkyung
	virtual BOOL OnInitDialog();
private:
	CString m_sIniFilePath;
};
};