#pragma once

#include <BitmapComboBox.h>
// CCADFontComboBox

class CCADFontComboBox : public CBitmapComboBox
{
	DECLARE_DYNAMIC(CCADFontComboBox)

public:
	CCADFontComboBox(const CString& rIniFilePath);
	virtual ~CCADFontComboBox();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	int FillFontList(const CString& rFontFolder);
private:
	CString m_rIniFilePath;
public:
};
