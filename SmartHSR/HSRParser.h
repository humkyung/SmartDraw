// HSRParse.h: interface for the CHSRParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRPARSE_H__5885955D_9DD6_4686_A556_EFC0187B6F54__INCLUDED_)
#define AFX_HSRPARSE_H__5885955D_9DD6_4686_A556_EFC0187B6F54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "TemplateElement.h"

class CHSRReader;
class CHSRParser
{
public:
	CHSRParser(CHSRReader* pHSRReader);
	virtual ~CHSRParser();
public:
	static CHSRElement* CreateElement(const STRING_T& rTypeString);
	void Parse(LPCTSTR pFilePath , const CHSRElement::MODEL_T& modelT);
private:
	void ParseLine(const CHSRElement::MODEL_T& modelT);
private:
	CHSRReader* m_pHSRReader;

	CHSRScanner* m_pHSRScanner;
	CTemplateElement*  m_pTemplateElement;
};

#endif // !defined(AFX_HSRPARSE_H__5885955D_9DD6_4686_A556_EFC0187B6F54__INCLUDED_)
