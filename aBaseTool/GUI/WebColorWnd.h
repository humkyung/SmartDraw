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

// WebColorWnd.h: interface for the CWebColorWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEBCOLORWND_H__793DEE24_0E7D_477B_948C_D1B0B1657186__INCLUDED_)
#define AFX_WEBCOLORWND_H__793DEE24_0E7D_477B_948C_D1B0B1657186__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// WebColorWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebColorWnd window

class CWebColorWnd : public CListBox
{
// Construction
public:
	CWebColorWnd();

// Attributes
public:
	void  SetCurrentColor(COLORREF crCurrent);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebColorWnd)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CPoint pt, CWnd* pWnd, UINT nID);
	virtual ~CWebColorWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWebColorWnd)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_WEBCOLORWND_H__793DEE24_0E7D_477B_948C_D1B0B1657186__INCLUDED_)
