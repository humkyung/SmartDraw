#pragma once

#include <afxtabctrl.h>
#include <SmartDrawPropertyPage.h>

#include "OpeningHoleGeneralDlg.h"
#include "OpeningHoleChartDlg.h"

// COpeningHolePage dialog

class COpeningHolePage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(COpeningHolePage)

public:
	COpeningHolePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpeningHolePage();

	CString GetIniFilePath() const;
	int SaveData();
// Dialog Data
	enum { IDD = IDD_OPENINGHOLE_PAGE };
		
	CMFCTabCtrl m_wndTabs;
	COpeningHoleGeneralDlg m_dlgOpeningHoleGeneral;
	COpeningHoleChartDlg m_dlgOpeningHoleChart;

	static COpeningHolePage* instance;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
