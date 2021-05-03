#pragma once


// CNewDraFileDlg dialog

class CNewDraFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewDraFileDlg)

public:
	CNewDraFileDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewDraFileDlg();

// Dialog Data
	enum { IDD = IDD_NEW_DRA_FILE };
	CString m_sSdrName;

	CMFCButton m_btnOK , m_btnCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
