#pragma once

#include <SmartDrawPropertyPage.h>
#include "TextStyleComboBox.h"

// CAnnotationPage dialog
namespace PropertyPage
{
class CAnnotationPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CAnnotationPage)

public:
	CAnnotationPage();
	virtual ~CAnnotationPage();

// Dialog Data
	enum { IDD = IDD_ANNOTATION };
protected:
	int LoadData(LPCTSTR sSettingFilePath);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int SaveData();

	virtual BOOL OnInitDialog();
private:
	CString m_rSettingFilePath;
public:
	afx_msg void OnCbnSelendokTextStyleCombo();
};
};
