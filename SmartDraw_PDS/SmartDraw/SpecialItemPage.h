#pragma once

#include <SmartDrawPropertyPage.h>
#include "PictureWndEx.h"
#include <ArrowComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawColorButton.h>

// CSpecialItemPage dialog
namespace PropertyPage
{
class CSpecialItemPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CSpecialItemPage)

public:
	CSpecialItemPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecialItemPage();

// Dialog Data
	enum { IDD = IDD_SPECIAL_ITEM_PAGE };
	CPictureWndEx  m_wndEnclosureType;
	CSmartDrawColorButton  m_btnLabelColor , m_btnLeaderColor;
	CArrowComboBox m_cboArrowType;
	CTextStyleComboBox m_cboTextStyle;

	CLevelComboBox m_cboLeaderLevel;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
	CLevelComboBox m_cboLabelLevel;
	CLineStyleComboBox m_cboLabelStyle;

	CString m_rEnclosureA , m_rEnclosureB , m_rLeaderArrowSize , m_rLabelDelimiter;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCbnSelendokComboEnclosureType();
	///afx_msg void OnCbnSelendokComboLabelType();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};