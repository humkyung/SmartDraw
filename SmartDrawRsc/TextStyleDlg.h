#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include "Resource.h"
#include "SmartDrawRscImp.h"

// CTextStyleDlg dialog

class ON_EXT_SMARTDRAWRSC CTextStyleDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextStyleDlg)

public:
	CTextStyleDlg(const CString& rIniFilePath , CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextStyleDlg();

// Dialog Data
	enum { IDD = IDD_TEXT_STYLE };
	CMFCButton m_wndNewTextStyle , m_wndDeleteTextStyle;
	CGridCtrl m_wndGridCtrl;
	CImageList m_ImageList;
	CMFCButton m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int UpdateContents();
private:
	CString m_rIniFilePath;
public:
	afx_msg void OnBnClickedButtonNewStyle();
	afx_msg void OnBnClickedButtonDeleteStyle();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	int WriteTextStyles(void);
public:
	afx_msg void OnBnClickedOk();
};
