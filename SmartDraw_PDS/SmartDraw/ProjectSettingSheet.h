#pragma once
#include <gui/GridCtrl/GridCtrl.h>
#include "FolderSettingPage.h"
#include "DatabaseSettingPage.h"

#include <list>
#include <map>
using namespace std;
using namespace PropertyPage;
// CProjectSettingSheet dialog

class CProjectSettingSheet : public CDialog
{
	DECLARE_DYNAMIC(CProjectSettingSheet)

public:
	CProjectSettingSheet(const int& nSelectedPage , const CString& rProjectName , const CString& rSettingFilePath , CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectSettingSheet();

// Dialog Data
	enum { IDD = IDD_PROJECT_SETTING };

	CFolderSettingPage m_wndFolderSettingPage;
	CDatabaseSettingPage m_wndDatabaseSettingPage;
	CMFCButton m_wndOKButton , m_wndCancelButton;

	CImageList m_cImages;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTvnSelchangedTreePage(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	map<int , CSmartDrawPropertyPage*> m_PageIndexMap;
	map<HTREEITEM , CSmartDrawPropertyPage*> m_PageItemMap;

	CDialog* m_pActivePage;
	int m_nSelectedPage;
	CString m_rProjectName , m_sIniFilePath;
	list<CDialog*> m_PageList;

	HTREEITEM AddPage(const UINT& nID , const CString& rLabel , CSmartDrawPropertyPage* pPage , const int& nImage , HTREEITEM hParent);
	int SetActivePage(HTREEITEM hItem);
public:
	afx_msg void OnBnClickedOk();
};
