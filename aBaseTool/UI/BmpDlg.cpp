// BmpDlg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BmpDlg.h"
#include "BmpDlgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgApp

BEGIN_MESSAGE_MAP(CBmpDlgApp, CWinApp)
	//{{AFX_MSG_MAP(CBmpDlgApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgApp construction

CBmpDlgApp::CBmpDlgApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBmpDlgApp object

CBmpDlgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgApp initialization

BOOL CBmpDlgApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CBmpDlgDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
