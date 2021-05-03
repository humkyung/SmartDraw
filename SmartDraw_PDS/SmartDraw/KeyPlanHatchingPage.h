#pragma once

#include <SmartDrawPropertyPage.h>
#include "TextStyleComboBox.h"
#include "PictureWndEx.h"
#include "LabelStatic.h"

// CKeyPlanHatchingPage dialog
namespace PropertyPage
{
class CKeyPlanHatchingPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNCREATE(CKeyPlanHatchingPage)

public:
	int LoadData(void);
	int SaveData(void);

	CKeyPlanHatchingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyPlanHatchingPage();
// Overrides

// Dialog Data
	enum { IDD = IDD_KEY_PLAN_HATCHING_PAGE };
	CPictureWndEx  m_wndKeyPlanHatchingPicture;
	CTextStyleComboBox m_cboTextStyle;;
	CLevelComboBox m_cboNameLevel;
	CSmartDrawColorButton m_btnNameColor;
	CLabelStatic  m_wndThisDrawingText;
	CString m_sThisDrawingText;
	CMFCButton m_wndThisDrawingPickButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditThisDrawingText();
	afx_msg void OnBnClickedButtonThisDrawingPick();
	DECLARE_MESSAGE_MAP()
private:
	CString m_sBorderConfigFilePath;
};
};