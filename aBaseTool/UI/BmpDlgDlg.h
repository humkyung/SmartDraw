// BmpDlgDlg.h : header file
//

#if !defined(AFX_BMPDLGDLG_H__75486C76_32EB_11D2_A762_000000000000__INCLUDED_)
#define AFX_BMPDLGDLG_H__75486C76_32EB_11D2_A762_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BitmapDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgDlg dialog

class CBmpDlgDlg : public CBitmapDialog
{
// Construction
public:
	CBmpDlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBmpDlgDlg)
	enum { IDD = IDD_BMPDLG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpDlgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBmpDlgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCenter();
	afx_msg void OnStretch();
	afx_msg void OnTile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPDLGDLG_H__75486C76_32EB_11D2_A762_000000000000__INCLUDED_)
