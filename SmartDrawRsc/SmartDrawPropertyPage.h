#pragma once

#include "SmartDrawRscImp.h"

// CSmartDrawPropertyPage dialog

class ON_EXT_SMARTDRAWRSC CSmartDrawPropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CSmartDrawPropertyPage)

public:
	CSmartDrawPropertyPage(UINT nIDTempalte , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSmartDrawPropertyPage();

// Dialog Data
	///enum { IDD = IDD_PROPERTYPAGE };
	CString GetIniFilePath() const;	/// 2012.04.01 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_sIniFilePath , m_sProjectName;
public:
	BOOL Create(LPCTSTR pIniFilePath , UINT nIDTemplate , CWnd* pParentWnd = 0);
	virtual int SaveData();
	virtual int LoadData(LPCTSTR sIniFilePath);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};