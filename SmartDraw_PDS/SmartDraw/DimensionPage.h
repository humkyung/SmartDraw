#pragma once

#include <LevelComboBox.h>
#include <SmartDrawPropertyPage.h>
#include <SmartDrawColorButton.h>
#include "TextStyleComboBox.h"
#include <ArrowComboBox.h>

// CDimensionPage dialog
namespace PropertyPage
{
class CDimensionPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CDimensionPage)

public:
	CDimensionPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDimensionPage();

// Dialog Data
	enum { IDD = IDD_DIMENSION_PAGE };
	CLevelComboBox m_cboTextLevel;
	CSmartDrawColorButton m_wndTextColorButton , m_wndDimLineColorButton , m_wndExtLineColorButton;
	CTextStyleComboBox m_wndTextStyleComboBox;
	CArrowComboBox m_wndArrowTypeComboBox;

	CColorCheck m_wndAutoSize;	/// 2014.11.16 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int LoadData(void);
protected:
	CString m_sDimStyle;
	CString m_rTextOffset , m_rDimLineDistance;
	CString m_rExtLineExtension , m_rExtLineOffset;
	CString m_rArrowSize;
public:
	virtual BOOL OnInitDialog();
	int SaveData(void);
};

class CIntDimStylePage : public CDimensionPage
{
	DECLARE_DYNAMIC(CIntDimStylePage)
public:
	CIntDimStylePage(CWnd* pParent = NULL);
	~CIntDimStylePage();
};

class CExtDimStylePage : public CDimensionPage
{
	DECLARE_DYNAMIC(CExtDimStylePage)
public:
	CExtDimStylePage(CWnd* pParent = NULL);
	~CExtDimStylePage();
};

};
