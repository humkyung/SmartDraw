// BmpDlg.h : main header file for the BMPDLG application
//

#if !defined(AFX_BMPDLG_H__75486C74_32EB_11D2_A762_000000000000__INCLUDED_)
#define AFX_BMPDLG_H__75486C74_32EB_11D2_A762_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgApp:
// See BmpDlg.cpp for the implementation of this class
//

class CBmpDlgApp : public CWinApp
{
public:
	CBmpDlgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpDlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBmpDlgApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPDLG_H__75486C74_32EB_11D2_A762_000000000000__INCLUDED_)
