#pragma once

#include <SmartDrawPropertyPage.h>
#include "PictureWndEx.h"
#include <ArrowComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawColorButton.h>

// CInstrumentPage dialog
namespace PropertyPage
{
class CInstrumentPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CInstrumentPage)

public:
	CInstrumentPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstrumentPage();

// Dialog Data
	enum { IDD = IDD_INSTRUMENT_PAGE };
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