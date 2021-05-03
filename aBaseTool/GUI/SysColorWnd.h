/////////////////////////////////////////////////////////////////////////////
//
// Disclaimer Notice
// ------------------
// The Author cannot guarantee this software is error free.
// The author also disclaims all responsibility for damages or 
// loss of profits caused by the failure of this software
//. Your usage of these modifications indicates your willingness to 
// accept complete responsibility.
//
//
// Developed by: Norm Almond for www.codeproject.com 
//
// Please use this software freely, if you fix or make any modifications to
// the software you can let me know so I can improve future versions.
//
// Version 1.00 - First Released 03 March 2001
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSCOLORWND_H__401DAB0A_73A7_46FD_86CB_8418F34FA856__INCLUDED_)
#define AFX_SYSCOLORWND_H__401DAB0A_73A7_46FD_86CB_8418F34FA856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysColorWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSysColorWnd window

class CSysColorWnd : public CListBox
{
// Construction
public:
	CSysColorWnd();

// Attributes
public:
	void  SetCurrentColor(COLORREF crCurrent);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysColorWnd)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CPoint pt, CWnd* pWnd, UINT nID);
	virtual ~CSysColorWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSysColorWnd)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSCOLORWND_H__401DAB0A_73A7_46FD_86CB_8418F34FA856__INCLUDED_)
