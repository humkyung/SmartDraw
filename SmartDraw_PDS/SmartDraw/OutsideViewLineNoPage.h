#pragma once

#include "PictureWndEx.h"
#include <SmartDrawPropertyPage.h>
#include "TextStyleComboBox.h"
#include <SmartDrawColorButton.h>

// COutsideViewLineNoPage dialog
namespace PropertyPage
{
class COutsideViewLineNoPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(COutsideViewLineNoPage)

public:
	COutsideViewLineNoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutsideViewLineNoPage();

// Dialog Data
	enum { IDD = IDD_OUTSIDE_VIEW_LINE_NO_PAGE };
	CPictureWndEx m_wndPicture;

	CTextStyleComboBox m_cboLineNoTextStyle;
	CString m_sLabelOffset;
	CLevelComboBox m_cboLineNoLevel;
	CSmartDrawColorButton m_btnLineNoColor;
	CLineStyleComboBox m_cboLineNoStyle;

	CTextStyleComboBox m_cboElevTextStyle;
	CLevelComboBox m_cboElevLevel;
	CSmartDrawColorButton m_btnElevColor;
	CLineStyleComboBox m_cboElevStyle;
	
	CColorCheck m_chkLeaderDisplay;
	CArrowComboBox m_cboArrowType;
	CString m_sArrowSize;
	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};