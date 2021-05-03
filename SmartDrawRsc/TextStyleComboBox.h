#pragma once

#include "BitmapComboBox.h"
#include "SmartDrawRscImp.h"

class ON_EXT_SMARTDRAWRSC CTextStyleComboBox : public CBitmapComboBox
{
	DECLARE_DYNAMIC(CTextStyleComboBox)
public:
	CTextStyleComboBox();
	~CTextStyleComboBox(void);

	int FillTextStyleList(const CString& rIniFilePath);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
private:
	CString m_rIniFilePath;	
	int m_nPrevSel;
public:
	afx_msg void OnCbnSelendok();
	int SetCurSel(int nSelect);
};
