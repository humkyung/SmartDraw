#pragma once

#include <ArrowComboBox.h>
#include <TextStyleComboBox.h>
#include <SmartDrawColorButton.h>
#include <SmartDrawPropertyPage.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include "LabelStatic.h"
#include "CPPToolTip/PPToolTip.h"

#include <vector>
using namespace std;

// CInsideViewLineNoPage dialog
namespace PropertyPage
{
class CInsideViewLineNoPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CInsideViewLineNoPage)

public:
	CInsideViewLineNoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInsideViewLineNoPage();

// Dialog Data
	enum { IDD = IDD_INSIDE_VIEW_LINE_NO_PAGE };
	CLabelStatic  m_wndLineNo1Label , m_wndLineNo2Label;
	CLabelStatic  m_wndElev1Label , m_wndElev2Label;
	
	CTextStyleComboBox m_cboLineNoTextStyle;
	CString m_sLabelOffset;
	CLevelComboBox m_cboLineNoLevel;
	CSmartDrawColorButton m_btnLineNoColor;
	CLineStyleComboBox m_cboLineNoStyle;

	CTextStyleComboBox m_cboElevTextStyle;
	CLevelComboBox m_cboElevLevel;
	CSmartDrawColorButton m_btnElevColor;
	CLineStyleComboBox m_cboElevStyle;
	
	CArrowComboBox m_cboArrowType;
	CString m_sArrowSize;
	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void OnBnClickedRadioElev();
	afx_msg void OnBnClickedRadioElevCoord();
	afx_msg void OnBnClickedRadioOnlyLineNo();
	afx_msg void OnBnClickedRadioLineNoElev();
	afx_msg void OnBnClickedRadioLineNoOverElev();
	DECLARE_MESSAGE_MAP()
public:
	int LoadData();
	int SaveData();

	virtual BOOL OnInitDialog();
private:
	CPPToolTip m_tooltip[2];
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
};