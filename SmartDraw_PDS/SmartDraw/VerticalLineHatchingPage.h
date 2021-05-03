#pragma once

#include <LevelComboBox.h>
#include "PictureWndEx.h"
#include <SmartDrawColorButton.h>
#include <SmartDrawPropertyPage.h>
#include <CellLibraryFileBrowseEdit.h>
#include "LabelStatic.h"
#include "PlatformHatchingPage.h"

// CVerticalLineHatchingPage dialog
namespace PropertyPage
{
class CVerticalLineHatchingPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CVerticalLineHatchingPage)

public:
	CVerticalLineHatchingPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVerticalLineHatchingPage();

// Dialog Data
	enum { IDD = IDD_VERTICAL_LINE_HATCHING_PAGE };
	
	CPictureWndEx m_wndLessThanPicture , m_wndGreaterEqualPicture;
	CLabelStatic  m_wndLessThanLabel , m_wndGreaterEqualLabel;

	CLevelComboBox m_cboHatchingLevel;
	CSmartDrawColorButton m_btnHatchingColor;
	CLineStyleComboBox m_cboHatchingStyle;
	CLineWeightComboBox m_cboHatchingWeight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCbnSelendokComboLessThan();
	afx_msg void OnCbnSelendokComboGreaterEqual();
	afx_msg void OnBnClickedCheckDependOnSize();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData();
};
};
