#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include <AnnoFilter.h>

// CAnnoFilterDlg dialog

class CAnnoFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnnoFilterDlg)

public:
	CAnnoFilterDlg(const STRING_T& sSettingName,const STRING_T&,AnnoFilters*,CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnnoFilterDlg();

// Dialog Data
	enum { IDD = IDD_ANNO_FILTER };
	CMFCButton m_wndNewFilter, m_wndDeleteFilter;
	CMFCButton m_wndFilterUp, m_wndFilterDown;
	CMFCButton m_wndOKButton , m_wndCancelButton;
	CGridCtrl m_wndGridCtrl;

	AnnoFilters* m_pAnnoFilters;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int FillCellsWithFilter(const int& iRow,CAnnoFilter* pAnnoFilter);
	int UpdateContents();
	STRING_T m_sSettingName,m_sFilterItem;
public:
	afx_msg void OnBnClickedButtonNewFilter();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnBnClickedButtonDeleteFilter();
	afx_msg void OnBnClickedButtonFilterUp();
	afx_msg void OnBnClickedButtonFilterDown();
};
