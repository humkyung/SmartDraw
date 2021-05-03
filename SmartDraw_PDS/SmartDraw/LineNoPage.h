#pragma once

#include <ColorCheck.h>
#include <SmartDrawPropertyPage.h>

// CLineNoPage dialog
namespace PropertyPage
{
class CLineNoPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CLineNoPage)

public:
	CLineNoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLineNoPage();

// Dialog Data
	enum { IDD = IDD_LINE_NO_PAGE };
	CMFCButton m_wndElevationTypeBySpec , m_wndSetSizeBySpecButton;
	CColorCheck m_btnTrimEnd;	/// 2016.04.01 added by humkyung
protected:
	int LoadData();

	/// 2014.06.14 added by humkyung
	afx_msg void OnBnClickedLineNoDimSizeAll();
	afx_msg void OnBnClickedLineNoDimOverSize();
	/// up to here
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonElevationTypeBySpec();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	int SaveData();
};
};
