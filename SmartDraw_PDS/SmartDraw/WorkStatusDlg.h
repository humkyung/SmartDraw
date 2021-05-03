#pragma once

#include <gui/BtnST.h>
#include <gui/TextProgressCtrl.h>
#include "resource.h"

// CWorkStatusDlg dialog

class CWorkStatusDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorkStatusDlg)

public:
        static void UpdateWorkStatus(CString& rTitle , int& process);
	static CWorkStatusDlg* GetInstance();

	CWorkStatusDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWorkStatusDlg();

//	virtual void OnFinalRelease();

// Dialog Data
	enum { IDD = IDD_WORK_STATUS };
        int m_process;
        static CString m_rTitle;
	CTextProgressCtrl m_processCtrl;

	CWinThread* m_pThread;
	volatile LONG m_bThreadRunning;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
        virtual BOOL OnInitDialog();
protected:
        virtual void OnOK();
        virtual void OnCancel();

 
};
