// WorkStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkStatusDlg.h"

using namespace std;

// CWorkStatusDlg dialog

IMPLEMENT_DYNAMIC(CWorkStatusDlg, CDialog)

static CWorkStatusDlg* m_pWorkStatusDlg = NULL;

CWorkStatusDlg::CWorkStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkStatusDlg::IDD, pParent), m_pThread(NULL) , m_process(0) , m_bThreadRunning(FALSE)
{
        m_pWorkStatusDlg = this;
}

CWorkStatusDlg::~CWorkStatusDlg()
{
        try
        {
                if(m_pWorkStatusDlg)
                {
                        m_pWorkStatusDlg = NULL;
                }
                if(m_pThread != NULL)
                {
                        /*delete m_pThread;
                        m_pThread = NULL;*/
                        TerminateThread(m_pThread->m_hThread, (DWORD)-1);
		}
        }
        catch(...)
        {
                AfxMessageBox( _T("Thread error") );
        }
}


void CWorkStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
        DDX_Control(pDX , IDC_PROGRESS , m_processCtrl);
}


BEGIN_MESSAGE_MAP(CWorkStatusDlg, CDialog)
END_MESSAGE_MAP()

// CWorkStatusDlg message handlers

BOOL CWorkStatusDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

	m_processCtrl.SetRange(0 , 100);
	m_processCtrl.SetStep(1);
	m_processCtrl.SetPos(0);
	m_processCtrl.SetShowText(TRUE);
	
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

CWorkStatusDlg* CWorkStatusDlg::GetInstance()
{
	return m_pWorkStatusDlg;
}

void CWorkStatusDlg::UpdateWorkStatus(CString &rTitle, int &process)
{
        if((NULL != m_pWorkStatusDlg) && (::IsWindow(m_pWorkStatusDlg->m_hWnd)))
	{
		m_pWorkStatusDlg->m_processCtrl.SetWindowText(rTitle);
		m_pWorkStatusDlg->m_processCtrl.SetPos(process);
	}
}

void CWorkStatusDlg::OnOK()
{
        // TODO: Add your specialized code here and/or call the base class
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

void CWorkStatusDlg::OnCancel()
{
        // TODO: Add your specialized code here and/or call the base class
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
