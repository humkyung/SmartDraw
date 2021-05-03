#pragma once

#include "PictureWndEx.h"
#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <SmartDrawColorButton.h>
#include <ColorCheck.h>

// COutsideEquipmentPage dialog
namespace PropertyPage
{
class COutsideEquipmentPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(COutsideEquipmentPage)

public:
	COutsideEquipmentPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutsideEquipmentPage();

// Dialog Data
	enum { IDD = IDD_OUTSIDE_EQUIPMENT_PAGE };
	CPictureWndEx m_wndPicture;

	CString m_sDisplayStyle;	/// 2014.09.12 added by humkyung

	CTextStyleComboBox m_cboNameTextStyle;
	CString m_sLabelOffset;
	CLevelComboBox m_cboNameLevel;
	CSmartDrawColorButton m_btnNameColor;
	CLineStyleComboBox m_cboNameStyle;

	CTextStyleComboBox m_cboCoordTextStyle;
	CLevelComboBox m_cboCoordLevel;
	CSmartDrawColorButton m_btnCoordColor;
	CLineStyleComboBox m_cboCoordStyle;
	
	CColorCheck m_chkLeaderDisplay;
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