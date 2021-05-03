#pragma once
#include <SmartDrawPropertyPage.h>
#include "GroupCheck.h"

//#import <SmartDrawActiveX.tlb>

// CDatabaseSettingPage dialog
namespace PropertyPage
{
class CDatabaseSettingPage : public CSmartDrawPropertyPage
{
	class CAccessFileBrowseEdit : public CMFCEditBrowseCtrl
	{
		virtual void OnBrowse();
	public:
		CString m_sFilePath;
	};

	DECLARE_DYNAMIC(CDatabaseSettingPage)

public:
	CDatabaseSettingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDatabaseSettingPage();

// Dialog Data
	enum { IDD = IDD_DATABASE_SETTING_PAGE };

	CAccessFileBrowseEdit m_wndAccessFilePath;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedRadioOracle();
	afx_msg void OnBnClickedRadioSqlServer();
	afx_msg void OnBnClickedRadioAccess();
	afx_msg void OnBnClickedRadioDrvFile();					/// 2014.07.29 added by humkyung
	afx_msg void OnBnClickedProjectDatabaseConnection();	/// 2012.09.19 added by humkyung
	afx_msg void OnBnClickedDesignDatabaseConnection();		/// 2012.09.19 added by humkyung
	afx_msg void OnBnClickedButtonDatabaseDump();
	DECLARE_MESSAGE_MAP()

	CWinFormsControl<SmartDrawFramework::SmartDrawUserControl> m_ctrlSmartDraw;
public:
	virtual BOOL OnInitDialog();

	int LoadData();
	int SaveData();
};
};