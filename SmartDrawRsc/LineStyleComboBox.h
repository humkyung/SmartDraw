#pragma once

#include "SmartDrawRscImp.h"
#include <IsTools.h>
// CLineStyleComboBox

class ON_EXT_SMARTDRAWRSC CLineStyleComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CLineStyleComboBox)

public:
	typedef struct tagLineStyle
	{
		STRING_T sType;
		STRING_T sDesc;
	}LineStyle;

	CLineStyleComboBox();
	virtual ~CLineStyleComboBox();

	static int ResetContents();
	int LoadContents(const CString& sLineStyleFilePath);
protected:
	DECLARE_MESSAGE_MAP()
};