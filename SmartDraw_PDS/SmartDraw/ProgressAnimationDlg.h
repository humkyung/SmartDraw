#pragma once

#include <PictureWndEx.h>
#include "Resource.h"

// CProgressAnimationDlg dialog

class CProgressAnimationDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressAnimationDlg)

public:
	CProgressAnimationDlg(CWnd* pParent = NULL);   // standard constructor
	static CProgressAnimationDlg& GetInstance();
	virtual ~CProgressAnimationDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESS_ANIMATION };
	CWinThread* m_pThread;

	bool Create(CWnd * pWndParent);
	volatile LONG m_bThreadRunning;
	static void PassMsgLoop(bool bEnableOnIdleCalls	);
protected:
	virtual BOOL DestroyWindow();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CPictureWndEx m_Picture;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
