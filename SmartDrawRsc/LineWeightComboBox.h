#pragma once

#include "SmartDrawRscImp.h"
// CLineWeightComboBox

class ON_EXT_SMARTDRAWRSC CLineWeightComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CLineWeightComboBox)

public:
	CLineWeightComboBox();
	virtual ~CLineWeightComboBox();

	/*
		@brief	clear line weight list
		@author	humkyung
		@date	2014.06.24
	*/
	static int ResetContents();
	int LoadContents(const CString& sLineWeightFilePath);	/// 2014.06.24 added by humkyung
protected:
	DECLARE_MESSAGE_MAP()
};
