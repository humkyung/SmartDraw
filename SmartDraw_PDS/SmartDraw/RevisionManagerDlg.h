#pragma once

#include <gui/GridCtrl/GridCtrl.h>

// CRevisionManagerDlg dialog

class CRevisionManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CRevisionManagerDlg)

public:
	CRevisionManagerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRevisionManagerDlg();

// Dialog Data
	enum { IDD = IDD_REVISION_MANAGER };
	CMFCButton m_btnNewRevData , m_btnDeleteRevData;
	CGridCtrl m_wndGridCtrl;
	CMFCButton m_btnOK , m_btnCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int InitGridCtrl();
	int DisplayRevisionData(const int& iItem);
public:
	afx_msg void OnBnClickedButtonNewRevData();
	afx_msg void OnBnClickedButtonDeleteRevData();
	afx_msg void OnBnClickedOk();
};
