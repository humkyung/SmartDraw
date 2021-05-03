#pragma once

#include "SmartDrawRscImp.h"
// CLevelComboBox

class ON_EXT_SMARTDRAWRSC CLevelComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CLevelComboBox)

public:
	CLevelComboBox(const DWORD& dwStyle=CBS_SORT|CBS_DROPDOWNLIST);
	virtual ~CLevelComboBox();

	static BOOL RecreateComboBox(CComboBox* pCombo, LPVOID lpParam=0);

	/*
		@brief	clear level list
		@author	humkyung
		@date	2014.06.24
	*/
	static int ResetContents();
	int SaveContents(const CString& sIniFilePath);					/// 2014.06.23 added by humkyung
	int LoadContents(const CString& sIniFilePath);					/// 2014.06.23 added by humkyung
	BOOL ModifyStyle(const DWORD& dwRemove , const DWORD& dwAdd);	/// 2014.06.23 added by humkyung
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg UINT OnGetDlgCode();
protected:
	virtual void PreSubclassWindow();
private:
	DWORD m_dwStyle;	/// 2014.06.23 added by humkyung
};


