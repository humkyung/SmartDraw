#pragma once
#include <SmartDrawPropertyPage.h>
#include "PictureWndEx.h"

// CGridNoPage dialog
namespace PropertyPage
{
class CGridNoPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CGridNoPage)

public:
	CGridNoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGridNoPage();

// Dialog Data
	enum { IDD = IDD_GRID_NO_PAGE };
	CPictureWndEx  m_wndGridNoPicture;
	CMFCButton m_wndBasePositionButtonn , m_wndXAxisDistanceButton , m_wndYAxisDistanceButton;
protected:
	int LoadData(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonBasePosition();			/// 2014.08.25 added by humkyung
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int SaveData();
};
};