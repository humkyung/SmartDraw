#pragma once

#include <SmartDrawPropertyPage.h>
#include <SmartDrawColorButton.h>
#include <TextStyleComboBox.h>
#include <CellLibraryFileBrowseEdit.h>
#include <ArrowComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include "PictureWndEx.h"

// CPlatformHatchingPage dialog
namespace PropertyPage
{
class CPlatformHatchingPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CPlatformHatchingPage)

public:
	CPlatformHatchingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlatformHatchingPage();

// Dialog Data
	enum { IDD = IDD_PLATFORM_HATCHING_PAGE };
	
	CCellLibraryFileBrowseEdit m_wndCellLibraryFileEdit;
	CPictureWndEx m_wndPlatformHatchingPicture;
	CSmartDrawColorButton m_wndPFHatchingTextColorButton , m_wndPFHatchingColorButton;
	CTextStyleComboBox m_wndPFHatchingTextStyleComboBox;
	CLevelComboBox m_cboGratingPFLevel , m_cboConcretePFLevel , m_cboEqpPFLevel;
	CLineStyleComboBox m_cboHatchingStyle;
	CLineWeightComboBox m_cboHatchingWeight;
	CComboBox m_cboHatchingSize;	/// 2014.04.11 added by humkyung

	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;

	CArrowComboBox m_cboArrowType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnEnChangeEditPFCellLibraryFilePath();
	DECLARE_MESSAGE_MAP()
public:
	int SaveData();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};
};