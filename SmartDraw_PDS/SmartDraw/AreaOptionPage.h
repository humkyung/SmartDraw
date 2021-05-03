#pragma once

#include <SmartDrawPropertyPage.h>
#include <ColorCheck.h>

// CAreaOptionPage dialog
namespace PropertyPage
{
class CAreaOptionPage : public CSmartDrawPropertyPage
{
	typedef struct tagArea
	{
		int Top,Bottom,Left,Right;
	};

	DECLARE_DYNAMIC(CAreaOptionPage)
public:
	CAreaOptionPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAreaOptionPage();

	int SaveData();
// Dialog Data
	enum { IDD = IDD_AREA_OPTION };
	CComboBox m_wndTagRangeComboBox;	/// 2014.09.11 added by humkyung
	CSpinButtonCtrl m_wndTopSpinCtrl , m_wndBottomSpinCtrl;
	CSpinButtonCtrl m_wndLeftSpinCtrl , m_wndRightSpinCtrl;
	CColorCheck m_btnTopSideCheck , m_btnBottomSideCheck;
	CColorCheck m_btnLeftSideCheck, m_btnRightSideCheck;
	CColorCheck m_btnSecTopSideCheck , m_btnSecBottomSideCheck;
	CColorCheck m_btnSecLeftSideCheck, m_btnSecRightSideCheck;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDeltaposTopRange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposBottomRange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposLeftRange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRightRange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboTagRange();
	afx_msg void OnEnChangeEditTopRange();
	afx_msg void OnEnChangeEditBottomRange();
	afx_msg void OnEnChangeEditLeftRange();
	afx_msg void OnEnChangeEditRightRange();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
private:
	CString m_rIniFilePath;

	struct tagArea m_oLineNoArea , m_oOtherArea;
};
};
