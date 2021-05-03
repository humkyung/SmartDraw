#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include <aDraw_DraFile.h>

#include <map>
using namespace std;

// CEditTitleBlockDataDlg dialog

class CEditTitleBlockDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditTitleBlockDataDlg)

public:
	CEditTitleBlockDataDlg(list<CaDraw_DraFile*> oDraFileList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditTitleBlockDataDlg();

	int LoadData();
	int SaveData();
// Dialog Data
	enum { IDD = IDD_EDIT_TITLE_BLOCK_DATA };
	CMFCTabCtrl m_wndTabCtrl;
	CGridCtrl m_wndGridCtrl;
	CMFCButton m_btnOK , m_btnCancel;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int InitGridCtrl(const CString& sBorderFileName);

	list<CaDraw_DraFile*> m_oDraFileList;
	map<CString,CGridCtrl*> m_oBorderGridCtrlList;
public:
	afx_msg void OnBnClickedOk();
};
