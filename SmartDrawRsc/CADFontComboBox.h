#pragma once

#include "BitmapComboBox.h"
#include "SmartDrawRscImp.h"

// CCADFontComboBox

class ON_EXT_SMARTDRAWRSC CCADFontComboBox : public CBitmapComboBox
{
	DECLARE_DYNAMIC(CCADFontComboBox)

public:
	CCADFontComboBox(const CString& rIniFilePath);
	virtual ~CCADFontComboBox();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
private:
	int FillFontList(const CString& rFontFolder , const CString& sOutputFormat);
	CString m_rIniFilePath;
public:
};
