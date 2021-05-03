#pragma once

#include <SmartDrawPropertyPage.h>
#include "PictureWndEx.h"
#include <SmartDrawColorButton.h>

// CValvePage dialog
namespace PropertyPage
{
class CValvePage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CValvePage)

public:
	CValvePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CValvePage();

// Dialog Data
	enum { IDD = IDD_VALVE_PAGE };
	CPictureWndEx m_wndEnclosureType;
	CArrowComboBox m_cboArrowType;
	CSmartDrawColorButton m_btnLeaderColor , m_btnLabelColor;
	CTextStyleComboBox m_cboTextStyle;

	CLevelComboBox m_cboLeaderLevel;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
	CLevelComboBox m_cboLabelLevel;
	CLineStyleComboBox m_cboLabelStyle;

	CString m_rLeaderArrowSize , m_rLabelOffset;
	CString m_sDisplayName;	/// 2015.09.01 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};