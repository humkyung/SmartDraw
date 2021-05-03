#pragma once

#include "PictureWndEx.h"
#include <ArrowComboBox.h>
#include <SmartDrawColorButton.h>
#include "TextStyleComboBox.h"
#include <SmartDrawPropertyPage.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>

// CNozzlePage dialog
namespace PropertyPage
{
class CNozzlePage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CNozzlePage)

public:
	CNozzlePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNozzlePage();

// Dialog Data
	enum { IDD = IDD_NOZZLE_PAGE };
	CPictureWndEx m_wndNozzlePicture;
	CSmartDrawColorButton m_btnLabelColor , m_btnLeaderColor;
	CArrowComboBox m_cboArrowType;
	CTextStyleComboBox m_cboTextStyle;

	CLevelComboBox m_cboLeaderLevel;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
	CLevelComboBox m_cboLabelLevel;
	CLineStyleComboBox m_cboLabelStyle;

	CString m_rLabelOffset , m_rLeaderArrowSize;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};
