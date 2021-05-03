#pragma once

#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <SmartDrawColorButton.h>
#include "PictureWndEx.h"
// CViewAnnoPage dialog

namespace PropertyPage
{
class CViewAnnoPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CViewAnnoPage)

public:
	CViewAnnoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewAnnoPage();

// Dialog Data
	enum { IDD = IDD_VIEW_ANNOA_OPTION_PAGE };
	CPictureWndEx m_wndViewAnnoPicture;

	CString m_sViewInfoDist;
	CTextStyleComboBox m_cboViewInfoTextStyle;
	CLevelComboBox m_cboViewInfoLevel;
	CSmartDrawColorButton m_btnViewInfoColor;
	CLineStyleComboBox m_cboViewInfoStyle;
	CString m_sViewInfoAlignment;

	CString m_sMatchLineDataDist;
	CTextStyleComboBox m_cboMatchLineDataTextStyle;
	CLevelComboBox m_cboMatchLineDataLevel;
	CSmartDrawColorButton m_btnMatchLineDataColor;
	CLineStyleComboBox m_cboMatchLineDataStyle;
	CString m_sMatchLineDataAlignment;

	CString m_sViewCoordDisplayStyle;
	CTextStyleComboBox m_cboViewCoordTextStyle;
	CLevelComboBox m_cboViewCoordLevel;
	CSmartDrawColorButton m_btnViewCoordColor;
	CLineStyleComboBox m_cboViewCoordStyle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelendokComboViewCoordStyle();

	int LoadData(void);
	int SaveData(void);
};
};