// HSRLayer.h: interface for the CHSRLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRLAYER_H__B75965BC_6965_4634_BAF4_C5B817F37EB4__INCLUDED_)
#define AFX_HSRLAYER_H__B75965BC_6965_4634_BAF4_C5B817F37EB4__INCLUDED_

#include "Enum.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

#include <string>
using namespace std;

class CHSRLayer  
{
public:
	static HSR_LAYER_E GetLayerCode(const STRING_T& rTypeString , const CHSRElement::MODEL_T& modelT , const STRING_T& sLayerName);
	CHSRLayer();
	virtual ~CHSRLayer();
};

#endif // !defined(AFX_HSRLAYER_H__B75965BC_6965_4634_BAF4_C5B817F37EB4__INCLUDED_)
