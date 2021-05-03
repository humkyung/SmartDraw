#pragma once

#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <SmartDrawColorButton.h>
#include <ColorCheck.h>
// CInsideEquipmentPage dialog
namespace PropertyPage
{
class CInsideEquipmentPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CInsideEquipmentPage)

public:
	CInsideEquipmentPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInsideEquipmentPage();

// Dialog Data
	enum { IDD = IDD_EQUIPMENT_PAGE };
	CTextStyleComboBox m_cboNameTextStyle;
	CString m_sLabelOffset;
	CLevelComboBox m_cboNameLevel;
	CSmartDrawColorButton m_btnNameColor;
	CLineStyleComboBox m_cboNameStyle;

	CColorCheck m_chkCoordDisplay;
	CTextStyleComboBox m_cboCoordTextStyle;
	CLevelComboBox m_cboCoordLevel;
	CSmartDrawColorButton m_btnCoordColor;
	CLineStyleComboBox m_cboCoordStyle;
	
	CColorCheck m_chkCenterlineDisplay;
	CLevelComboBox m_cboCenterlineLevel;
	CSmartDrawColorButton m_btnCenterlineColor;
	CLineStyleComboBox m_cboCenterlineStyle;
	CLineWeightComboBox m_cboCenterlineWeight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int LoadData(void);
	int SaveData(void);

	virtual BOOL OnInitDialog();
};
};