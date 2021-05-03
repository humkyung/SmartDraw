// TemplateElement.h: interface for the CTemplateElement class.
//

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPLATEELEMENT_H__05E1D03F_EE83_43D8_A94F_66EF2EA5FA5A__INCLUDED_)
#define AFX_TEMPLATEELEMENT_H__05E1D03F_EE83_43D8_A94F_66EF2EA5FA5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CHSRReader;
class CTemplateElement : public CHSRElement  
{
public:
	CTemplateElement();
	CTemplateElement(CHSRReader* pHSRReader);
	virtual ~CTemplateElement();
public:
	void CreateElbow(CHSRScanner* pHSRScanner);
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume){return true;}
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID){return NULL;}

	DECLARE_HSR_ELEMENT(CTemplateElement);
private:
	CHSRReader* m_pHSRReader;
};



#endif // !defined(AFX_TEMPLATEELEMENT_H__05E1D03F_EE83_43D8_A94F_66EF2EA5FA5A__INCLUDED_)











































