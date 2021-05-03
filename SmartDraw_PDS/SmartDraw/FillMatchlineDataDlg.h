#pragma once


// CFillMatchlineDataDlg dialog

class CFillMatchlineDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CFillMatchlineDataDlg)

public:
	CFillMatchlineDataDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFillMatchlineDataDlg();

// Dialog Data
	enum { IDD = IDD_FILL_MATCHLINE_DATA };

	CString m_sMatchLineDwgPrefix;
	CString m_sNoMatchLineDwg;

	CMFCButton m_wndOKButton;
	CMFCButton m_wndCancelButton;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
