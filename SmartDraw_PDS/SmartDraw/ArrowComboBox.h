#pragma once

#include <BitmapComboBox.h>

// CArrowComboBox

class CArrowComboBox : public CBitmapComboBox
{
	DECLARE_DYNAMIC(CArrowComboBox)

public:
	CArrowComboBox();
	virtual ~CArrowComboBox();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PreSubclassWindow();
private:
	int FillArrowList(void);
};


