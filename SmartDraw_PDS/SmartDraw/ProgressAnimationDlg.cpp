// ProgressAnimationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "ProgressAnimationDlg.h"

// CProgressAnimationDlg dialog

IMPLEMENT_DYNAMIC(CProgressAnimationDlg, CDialog)

CProgressAnimationDlg::CProgressAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressAnimationDlg::IDD, pParent)
{

}

/**
	@brief	
	@author humkyung
	@date 2014.06.21
	@param	
	@return		
*/
CProgressAnimationDlg& CProgressAnimationDlg::GetInstance()
{
	static CProgressAnimationDlg __INSTANCE__;

	return __INSTANCE__;
}

CProgressAnimationDlg::~CProgressAnimationDlg()
{
	try
	{
		if(NULL != m_pThread)
		{
			/*delete m_pThread;
			m_pThread = NULL;*/
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
		}
	}
	catch(...)
	{
		AfxMessageBox( _T("Thread error ") );
	}
}

void CProgressAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MYPICTURE, m_Picture);
}


BEGIN_MESSAGE_MAP(CProgressAnimationDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CProgressAnimationDlg message handlers

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
bool CProgressAnimationDlg::Create( CWnd * pWndParent )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() == NULL );

	CRect rcDesktop;
	VERIFY(
		::SystemParametersInfo(
			SPI_GETWORKAREA,
			0,
			PVOID(&rcDesktop),
			0
			)
		);
	
	if(!CDialog::Create(MAKEINTRESOURCE(CProgressAnimationDlg::IDD) , pWndParent))
	{
		ASSERT( FALSE );
		return false;
	}

	if(NULL != pWndParent) pWndParent->GetWindowRect(&rcDesktop);
	CRect rcWnd( 0, 0, 0, 0);
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(
		( rcDesktop.Width() -  rcWnd.Width() ) * 0.5,
		( rcDesktop.Height() - rcWnd.Height() ) * 0.5
		);

	VERIFY(
		::SetWindowPos(m_hWnd , (NULL != pWndParent) ? pWndParent->GetSafeHwnd() : HWND_TOP , rcWnd.left , rcWnd.top , rcWnd.Width() , rcWnd.Height() , SWP_NOSIZE | SWP_SHOWWINDOW)
		);

	return true;
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
BOOL CProgressAnimationDlg::DestroyWindow() 
{
	ShowWindow( SW_HIDE );
	
	return CDialog::DestroyWindow();
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
void CProgressAnimationDlg::PassMsgLoop(bool bEnableOnIdleCalls)
{
	//__PROF_UIS_MANAGE_STATE;
	AFX_MANAGE_STATE( ::AfxGetAppModuleState() );

	MSG msg;
	// Process all the messages in the message queue
	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		if( !AfxGetThread()->PumpMessage() )
		{
			PostQuitMessage(0);
			return;
		} // if( !AfxGetThread()->PumpMessage() )
	} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	if( bEnableOnIdleCalls )
	{
		for(	LONG nIdleCounter = 0L;
			::AfxGetThread()->OnIdle(nIdleCounter);
			nIdleCounter ++
			);
	}
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
BOOL CProgressAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/// animate control functions...
	if(m_Picture.Load(GetExecPath() + _T("\\Resource\\Loading.gif")))
	{
		m_Picture.Draw();
	}
	
	if(NULL != m_pThread)
	{
		m_pThread->m_bAutoDelete = FALSE;
		m_pThread->ResumeThread();
	}
        else
        {
                return FALSE;
        }

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
void CProgressAnimationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		///dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
void CProgressAnimationDlg::OnOK()
{
	if (m_bThreadRunning)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		if(m_bThreadRunning)
		{
			// we gave the thread a chance to quit. Since the thread didn't
			// listen to us we have to kill it.
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
			InterlockedExchange((LONG*)(&m_bThreadRunning) , FALSE);
		}
	}

	CDialog::OnOK();
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
void CProgressAnimationDlg::OnCancel()
{
	if (m_bThreadRunning)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		if(m_bThreadRunning)
		{
			// we gave the thread a chance to quit. Since the thread didn't
			// listen to us we have to kill it.
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
			InterlockedExchange((LONG*)(&m_bThreadRunning) , FALSE);
		}
	}

	CDialog::OnCancel();
}

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
BOOL CProgressAnimationDlg::OnEraseBkgnd(CDC* pDC)
{
	CBitmap m_bitmap;
	BOOL rVal = FALSE;	//CWizardPage_Base::OnEraseBkgnd(pDC);

	if( m_bitmap.LoadBitmap( IDB_LOADING_AUTOCAD ) )
	{
		CRect rect;
		GetClientRect( &rect );

		CDC dc;
		dc.CreateCompatibleDC( pDC );
		CBitmap* pOldBitmap = dc.SelectObject( &m_bitmap );

		pDC->BitBlt( 0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

		dc.SelectObject(pOldBitmap);
		rVal = TRUE;
	}

	return rVal;
}
