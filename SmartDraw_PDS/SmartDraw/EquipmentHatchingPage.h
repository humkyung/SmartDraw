#pragma once

#include <SmartDrawPropertyPage.h>
#include <SmartDrawColorButton.h>
#include <TextStyleComboBox.h>
#include <CellLibraryFileBrowseEdit.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include "PictureWndEx.h"
// CEquipmentHatchingPage dialog

namespace PropertyPage
{
class CEquipmentHatchingPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CEquipmentHatchingPage)

public:
	CEquipmentHatchingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEquipmentHatchingPage();

	int SaveData();
// Dialog Data
	enum { IDD = IDD_EQUIPMENT_HATCHING_PAGE };
	CCellLibraryFileBrowseEdit m_wndCellLibraryFileEdit;

	CPictureWndEx m_wndEquipmentHatchingPicture;
	CLevelComboBox m_cboHatchingLevel;
	CSmartDrawColorButton m_btnHatchingColor;
	CLineStyleComboBox m_cboHatchingStyle;
	CLineWeightComboBox m_cboHatchingWeight;
protected:
	int m_iHatchingScale;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnEnChangeEditEqpCellLibraryFilePath();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
};