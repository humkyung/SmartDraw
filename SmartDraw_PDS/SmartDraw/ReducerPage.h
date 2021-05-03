#pragma once

#include "PictureWndEx.h"
#include <SmartDrawColorButton.h>
#include "TextStyleComboBox.h"
#include <ArrowComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawPropertyPage.h>

// CReducerPage dialog
namespace PropertyPage
{
class CReducerPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CReducerPage)

public:
	CReducerPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReducerPage();

// Dialog Data
	enum { IDD = IDD_REDUCER_PAGE };
	CPictureWndEx m_wndReducerPicture;
	CSmartDrawColorButton m_btnLabelColor , m_btnLeaderColor;
	CTextStyleComboBox m_cboTextStyle;
	CArrowComboBox m_wndArrowComboBox;

	CLevelComboBox m_cboLabelLevel;
	CLineStyleComboBox m_cboLabelStyle;
	CLineWeightComboBox m_cboLabelWeight;

	CLevelComboBox m_cboLeaderLevel;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData();
};
};