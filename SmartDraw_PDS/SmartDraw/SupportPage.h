#pragma once

#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawColorButton.h>
#include <SmartDrawPropertyPage.h>
// CSupportPage dialog
namespace PropertyPage
{
class CSupportPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CSupportPage)

public:
	CSupportPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSupportPage();

// Dialog Data
	enum { IDD = IDD_SUPPORT_PAGE };
	CArrowComboBox m_cboArrowType;
	CSmartDrawColorButton m_btnLeaderColor , m_btnLabelColor;
	CTextStyleComboBox m_cboTextStyle;

	CLevelComboBox m_cboLeaderLevel;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
	CLevelComboBox m_cboLabelLevel;
	CLineStyleComboBox m_cboLabelStyle;

	CString m_rLeaderArrowSize , m_rLabelOffset;
	CString m_sDisplayName;	/// 2014.12.31 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};
