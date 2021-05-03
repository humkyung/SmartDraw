#pragma once

#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>

#include <SmartDrawPropertyPage.h>
#include "TextStyleComboBox.h"
#include <SmartDrawColorButton.h>
#include "CPPToolTip/PPToolTip.h"

// CStructurePage dialog
namespace PropertyPage
{
class CStructurePage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CStructurePage)

public:
	CStructurePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStructurePage();

// Dialog Data
	enum { IDD = IDD_STRUCTURE_PAGE };
	CTextStyleComboBox m_cboTextStyle;
	CLevelComboBox m_cboLabelLevel;
	CSmartDrawColorButton m_btnLabelColor;
	CLineStyleComboBox m_cboLabelStyle;
	
	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;

	CLevelComboBox m_cboGridCircleLevel;	/// 2015.03.23 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void OnBnClickedButtonColumnData();		/// 2014.12.27 added by humkyung
	afx_msg void OnBnClickedButtonGridLineData();	/// 2014.12.27 added by humkyung
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CString m_rDelimiter , m_rBubbleSize;

	CPalette m_Palette;
	CPPToolTip m_tooltip;
public:
	int LoadData(void);
	int SaveData(void);
};
};