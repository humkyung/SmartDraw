#pragma once

#include <gui/GridCtrl/GridCtrl.h>
// CPipeDiaEditorDlg dialog

class CPipeDiaEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CPipeDiaEditorDlg)

public:
	CPipeDiaEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPipeDiaEditorDlg();

// Dialog Data
	enum { IDD = IDD_PIPE_DIA_EDIT };
	CMFCTabCtrl m_wndTabCtrl;
	vector<CGridCtrl*> m_oGridCtrlList;
	CMFCButton m_btnNewPipeDiaData , m_btnDeletePipeDiaData;
	CMFCButton m_btnOK , m_btnCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int InitGridCtrl();
public:
	afx_msg void OnBnClickedButtonNewPipeDiaData();
	afx_msg void OnBnClickedButtonDeletePipeDiaData();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonExport();
};
