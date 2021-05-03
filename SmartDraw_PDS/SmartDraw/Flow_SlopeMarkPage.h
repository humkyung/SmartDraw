#pragma once

#include <LabelStatic.h>
#include "PictureWndEx.h"
#include <SmartDrawColorButton.h>
#include <SmartDrawPropertyPage.h>

// Flow_SlopeMarkPage dialog
namespace PropertyPage
{
class CFlow_SlopeMarkPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CFlow_SlopeMarkPage)

public:
	CFlow_SlopeMarkPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlow_SlopeMarkPage();

// Dialog Data
	enum { IDD = IDD_FLOW_SLOPE_MARK_PAGE };
	CPictureWndEx m_wndFlowMarkPicture , m_wndSlopeMarkPicture;
	CSmartDrawColorButton m_wndFlowMarkColorButton , m_wndSlopeMarkTextColorButton;
	CTextStyleComboBox m_wndTextStyleComboBox;

	CLabelStatic m_wndDenominatorStatic;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
};
};
