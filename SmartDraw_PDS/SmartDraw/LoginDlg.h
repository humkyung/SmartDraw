#pragma once

#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include <gui/GradientStatic.h>
#include <util/ado/ADODB.h>
#include <util/ado/SQLFile.h>
#include "MyBitmapComboBox.h"

#include <map>
using namespace std;

// CLoginDlg dialog

class CServerFolderBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
public:
	CString m_sServerFolder;
};

class CLoginDlg : public CDialog
{
	DECLARE_DYNCREATE(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_LOGIN };

	CGradientStatic		m_wndStaticTitle;
	CGradientStatic		m_wndProjectNameStatic;
	CMyBitmapComboBox	m_wndProjectNameCombo;
	CString				m_rProjectName;
	CGradientStatic		m_wndUserIDStatic;
	CEdit				m_wndUserID;
	CString				m_rID;
	CGradientStatic		m_wndPasswordStatic;
	CBoostBaseFilterEdit	m_wndPassword;
	CGradientStatic		m_wndStaticMsg;
	CServerFolderBrowseEdit m_wndServerFolder;

	///CMFCButton		m_wndSelectServerButton;
	CMFCButton		m_wndLoginButton;
	CMFCButton		m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnReceiveMsg(WPARAM wParam, LPARAM lParam);  /// 2013.11.13 added by humkyung

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectServer();
private:
	CString m_rServerFolderPath;
	map<STRING_T,ProjectInfo> m_ProjectInfoMap;

	///static CClientSocket m_oClientSocket;
public:
	CString GetServerFolderPath() const { return m_rServerFolderPath; }
	afx_msg void OnBnClickedOk();
private:
	int LoadAppSettingFile(void);
private:
	int SaveAppSettingFile(void);
	int ReadProjectNoFromMDB(void);
	int DisplayMessage(const STRING_T& rMsg , COLORREF bgColor , COLORREF fgColor);
	int SyncDatabaseStructure(void);
	int SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile , vector<STRING_T>& FieldNameEntry);
public:
	CString GetProjectNameOf(const STRING_T& rProjectNo);
	CString GetProjectDescOf(const STRING_T& rProjectNo);
	CString GetProjectClientOf(const STRING_T& rProjectNo);
	afx_msg void OnEnChangeEditFolder();
	afx_msg void OnBnClickedButtonRestActivateCode();
};
