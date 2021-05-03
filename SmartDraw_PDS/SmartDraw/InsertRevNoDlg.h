#pragma once


// CInsertRevNoDlg dialog

class CInsertRevNoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInsertRevNoDlg)

public:
	CInsertRevNoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInsertRevNoDlg();

// Dialog Data
	enum { IDD = IDD_INSERT_REV_NO };
	CMFCButton m_btnOK , m_btnCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioAuto();
	afx_msg void OnBnClickedRadioSelect();
	afx_msg void OnBnClickedOk();
};
